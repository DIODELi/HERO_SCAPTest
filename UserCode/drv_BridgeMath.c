#include "drv_BridgeMath.h"

#include <math.h>

// 计算当前状态下用Buck从A到B的B端电流加速度可控范围 出错返回-1，正常返回0
int BuckA2B_GetLim(BridgeState_s* SenseHandle, BridgeHW_s* HW_Handle, Limit_s* BuckLim)
{
    float K, MaxLim_CAL, MinLim_CAL;
    float KDIVIND;
    float Temp;
    // 检查传参是否正常
    if (isnan(SenseHandle->U_A) || isnan(SenseHandle->U_B))
    {
        // 电压传感异常，无法计算
        BuckLim->Max = 0;
        BuckLim->Min = 0;
        return -1;
    }

    // 计算范围
    K = HW_Handle->Duty_Max;
    KDIVIND = K / HW_Handle->Inductor;
    MinLim_CAL = -KDIVIND * SenseHandle->U_B;
    MaxLim_CAL = KDIVIND * (SenseHandle->U_A - SenseHandle->U_B);

    if (MaxLim_CAL < MinLim_CAL)
    {
        Temp = MinLim_CAL;
        MinLim_CAL = MaxLim_CAL;
        MaxLim_CAL = Temp;
    }
    BuckLim->Max = MaxLim_CAL;
    BuckLim->Min = MinLim_CAL;
    return 0;
}

// 计算正常返回0，计算到达限幅返回1
// 使用当前给定的电流加速度计算占空比 电流加速度需要由外部保证数值范围
int BuckA2B_GetDuty(float Iaccl_Set, BridgeState_s* SenseHandle, BridgeHW_s* HW_Handle, BridgeSet_s* SetHandle)
{
    float BuckDuty; // Buck状态下A侧H通的时长/周期
    float L = HW_Handle->Inductor;
    float U_A = SenseHandle->U_A;
    float U_B = SenseHandle->U_B;
    float K = HW_Handle->Duty_Max;

    int ret = 0; // 默认为0

    if (fabsf(U_A) < 0.1f)
    {
        // A侧电压很低，此时开关没有意义
        BuckDuty = 0; // 直接不开关，保持低电平
    }
    else
    {
        // A侧电压正常
        // 计算理论占空比
        BuckDuty = (Iaccl_Set * L + U_B * K) / U_A; // 这里对考虑过最大占空比的表达式进行了化简
    }

    // buck模式下，电感电流=IB/K (考虑最大占空比限制)
    if (!isnan(SenseHandle->I_B))
    {
        SetHandle->I_L = SenseHandle->I_B / K; // 输出电流*K可得电感电流
    }
    else
    {
        SetHandle->I_L = 0.001f; // 异常时按照电流流向正来处理
    }

    if (BuckDuty > K)
    {
        BuckDuty = K;
        ret = -1;
    }
    else if (BuckDuty < 0)
    {
        BuckDuty = 0;
        ret = -1;
    }

    // 换算成输出格式
    SetHandle->TGGLA[0] = BuckDuty;
    SetHandle->TGGLA[1] = 1;
    SetHandle->TGGLB[0] = K;
    SetHandle->TGGLB[1] = 1;
    SetHandle->SwitchMode = BRIDGE_BUCK;
    return ret;
}

// 计算当前状态下用Boost从A到B的B端电流加速度可控范围 出错返回-1，正常返回0
int BoostA2B_GetLim(BridgeState_s* SenseHandle, BridgeHW_s* HW_Handle, Limit_s* BoostLim)
{
    float K, MaxLim_CAL, MinLim_CAL;
    float U_A, U_B;
    float KDIVIND;
    float Temp;

    // 检查传参是否正常
    if (isnan(SenseHandle->U_A) || isnan(SenseHandle->U_B))
    {
        // 电压传感异常，无法计算
        BoostLim->Max = 0;
        BoostLim->Min = 0;
        return -1;
    }
    else
    {
        U_A = SenseHandle->U_A;
        U_B = SenseHandle->U_B;
    }

    // Ub过低时特殊处理
    if (fabsf(U_B) < 0.01f)
    {
        U_B = 0.01f;
    }

    // 计算范围
    K = HW_Handle->Duty_Max;
    KDIVIND = K / HW_Handle->Inductor;
    MaxLim_CAL = KDIVIND * U_A * U_A / (4 * U_B);
    MinLim_CAL = KDIVIND * (U_A - U_B);

    if (MaxLim_CAL < MinLim_CAL)
    {
        Temp = MinLim_CAL;
        MinLim_CAL = MaxLim_CAL;
        MaxLim_CAL = Temp;
    }
    BoostLim->Max = MaxLim_CAL;
    BoostLim->Min = MinLim_CAL;
    return 0;
}

// 计算正常返回0，计算到达限幅返回1
// 使用当前给定的电流加速度计算占空比 电流加速度需要由外部保证数值范围
int BoostA2B_GetDuty(float Iaccl_Set, BridgeState_s* SenseHandle, BridgeHW_s* HW_Handle, BridgeSet_s* SetHandle)
{
    float BoostDuty; // Buck状态下A侧H通的时长/周期
    float L = HW_Handle->Inductor;
    float U_A = SenseHandle->U_A;
    float U_B = SenseHandle->U_B;
    float K = HW_Handle->Duty_Max;
    float Delta; // 计算判别式
    float temp;

    int ret = 0; // 默认为零

    Delta = U_A * U_A - 4 * Iaccl_Set / K * U_B * L;
    if (Delta < 0)
    {
        Delta = 0;
        ret = -1; // 可能输入的设定值过大，导致无解，此时取最大输出电流对应的占空比
    }

    if (fabsf(U_B) < 0.1f)
    {
        // B侧电压很低，此时开关没有意义
        BoostDuty = K; // 直接保持高电平
    }
    else if (fabsf(U_A) < 0.1f)
    {
        // A侧电压很低，此时应避免开关
        BoostDuty = 0;
    }
    else
    {
        // 电压正常
        // 计算理论占空比
        BoostDuty = K / 2 / U_B * (U_A + sqrtf(Delta));
    }

    // Boost模式下，电感电流=IA/K
    if (!isnan(SenseHandle->I_A))
    {
        SetHandle->I_L = SenseHandle->I_A / K; // 输出电流*K可得电感电流
    }
    else
    {
        SetHandle->I_L = 0.001f; // 异常时按照电流流向正来处理
    }

    if (BoostDuty > K)
    {
        BoostDuty = K;
        ret = -1;
    }
    else if (BoostDuty < 0)
    {
        BoostDuty = 0;
    }
    else
    {
        // 当前占空比计算结果范围正常 无需修改ret的值
    }
    // 换算成输出格式
    SetHandle->TGGLA[0] = BoostDuty;
    temp = BoostDuty + 1 - K;
    if (temp > 1)
    { // 舍入误差可能会导致temp>1
        temp = 1;
    }
    SetHandle->TGGLA[1] = temp;
    SetHandle->TGGLB[0] = BoostDuty;
    SetHandle->TGGLB[1] = 1;
    SetHandle->SwitchMode = BRIDGE_BOOST;
    return ret;
}
