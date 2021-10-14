#ifndef __DRV_BRIDGEMATH_H__
#define __DRV_BRIDGEMATH_H__

typedef struct
{
    float Max;
    float Min;
} Limit_s;

typedef enum
{
    BRIDGE_STOP,
    BRIDGE_BUCK,
    BRIDGE_BOOST,
    BRIDGE_MIX,
} BridgeMode_e;

typedef struct
{
    BridgeMode_e SwitchMode; // 辅助标记，简化PWM设定函数的实现过程
    float I_L;
    float TGGLA[2]; // A侧两处翻转点比例 未考虑死区
    float TGGLB[2]; // B侧两处翻转点比例 未考虑死区
} BridgeSet_s;

typedef struct
{
    BridgeMode_e ModeNow; // 当前控制算法选择的开关模式：Buck，Boost，Mix
    Limit_s BuckLim;
    Limit_s MixLim;
    Limit_s BoostLim;
} BridgeCtrl_s;

typedef struct
{
    float U_A;
    float U_B;
    float I_A; // 由A到B为正方向
    float I_B; // 由A到B为正方向
} BridgeState_s;

typedef struct
{
    int DeadTime_ON;  // MOS管开通死区设置
    int DeadTime_OFF; // MOS管关断死区设置
    float Duty_Max;   // 最大占空比 可取0.95
    float Inductor;   // 开关电源电感量
} BridgeHW_s;

typedef struct
{
    BridgeHW_s HardwareHandle;
    BridgeState_s SenseHandle; // 传感数据 这里的数据每轮计算之前刷新一次，不能在计算过程中刷新
    BridgeSet_s SetHandle;     // 硬件设定量
    BridgeCtrl_s CtrlHandle;   // 控制量
} BridgeData_s;

// 计算当前状态下用Buck从A到B的B端电流加速度可控范围 出错返回-1，正常返回0
extern int BuckA2B_GetLim(BridgeState_s *SenseHandle, BridgeHW_s *HW_Handle, Limit_s *BuckLim);

// 计算正常返回0，计算到达限幅返回1
// 使用当前给定的电流加速度计算占空比 电流加速度需要由外部保证数值范围
extern int BuckA2B_GetDuty(float Iaccl_Set, BridgeState_s *SenseHandle, BridgeHW_s *HW_Handle, BridgeSet_s *SetHandle);

// 计算当前状态下用Boost从A到B的B端电流加速度可控范围 出错返回-1，正常返回0
extern int BoostA2B_GetLim(BridgeState_s *SenseHandle, BridgeHW_s *HW_Handle, Limit_s *BoostLim);

// 计算正常返回0，计算到达限幅返回1
// 使用当前给定的电流加速度计算占空比 电流加速度需要由外部保证数值范围
extern int BoostA2B_GetDuty(float Iaccl_Set, BridgeState_s *SenseHandle, BridgeHW_s *HW_Handle, BridgeSet_s *SetHandle);

#endif
