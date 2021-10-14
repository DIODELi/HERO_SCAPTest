#include "drv_BridgeMath.h"

#include <math.h>

// ���㵱ǰ״̬����Buck��A��B��B�˵������ٶȿɿط�Χ ������-1����������0
int BuckA2B_GetLim(BridgeState_s* SenseHandle, BridgeHW_s* HW_Handle, Limit_s* BuckLim)
{
    float K, MaxLim_CAL, MinLim_CAL;
    float KDIVIND;
    float Temp;
    // ��鴫���Ƿ�����
    if (isnan(SenseHandle->U_A) || isnan(SenseHandle->U_B))
    {
        // ��ѹ�����쳣���޷�����
        BuckLim->Max = 0;
        BuckLim->Min = 0;
        return -1;
    }

    // ���㷶Χ
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

// ������������0�����㵽���޷�����1
// ʹ�õ�ǰ�����ĵ������ٶȼ���ռ�ձ� �������ٶ���Ҫ���ⲿ��֤��ֵ��Χ
int BuckA2B_GetDuty(float Iaccl_Set, BridgeState_s* SenseHandle, BridgeHW_s* HW_Handle, BridgeSet_s* SetHandle)
{
    float BuckDuty; // Buck״̬��A��Hͨ��ʱ��/����
    float L = HW_Handle->Inductor;
    float U_A = SenseHandle->U_A;
    float U_B = SenseHandle->U_B;
    float K = HW_Handle->Duty_Max;

    int ret = 0; // Ĭ��Ϊ0

    if (fabsf(U_A) < 0.1f)
    {
        // A���ѹ�ܵͣ���ʱ����û������
        BuckDuty = 0; // ֱ�Ӳ����أ����ֵ͵�ƽ
    }
    else
    {
        // A���ѹ����
        // ��������ռ�ձ�
        BuckDuty = (Iaccl_Set * L + U_B * K) / U_A; // ����Կ��ǹ����ռ�ձȵı��ʽ�����˻���
    }

    // buckģʽ�£���е���=IB/K (�������ռ�ձ�����)
    if (!isnan(SenseHandle->I_B))
    {
        SetHandle->I_L = SenseHandle->I_B / K; // �������*K�ɵõ�е���
    }
    else
    {
        SetHandle->I_L = 0.001f; // �쳣ʱ���յ���������������
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

    // ����������ʽ
    SetHandle->TGGLA[0] = BuckDuty;
    SetHandle->TGGLA[1] = 1;
    SetHandle->TGGLB[0] = K;
    SetHandle->TGGLB[1] = 1;
    SetHandle->SwitchMode = BRIDGE_BUCK;
    return ret;
}

// ���㵱ǰ״̬����Boost��A��B��B�˵������ٶȿɿط�Χ ������-1����������0
int BoostA2B_GetLim(BridgeState_s* SenseHandle, BridgeHW_s* HW_Handle, Limit_s* BoostLim)
{
    float K, MaxLim_CAL, MinLim_CAL;
    float U_A, U_B;
    float KDIVIND;
    float Temp;

    // ��鴫���Ƿ�����
    if (isnan(SenseHandle->U_A) || isnan(SenseHandle->U_B))
    {
        // ��ѹ�����쳣���޷�����
        BoostLim->Max = 0;
        BoostLim->Min = 0;
        return -1;
    }
    else
    {
        U_A = SenseHandle->U_A;
        U_B = SenseHandle->U_B;
    }

    // Ub����ʱ���⴦��
    if (fabsf(U_B) < 0.01f)
    {
        U_B = 0.01f;
    }

    // ���㷶Χ
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

// ������������0�����㵽���޷�����1
// ʹ�õ�ǰ�����ĵ������ٶȼ���ռ�ձ� �������ٶ���Ҫ���ⲿ��֤��ֵ��Χ
int BoostA2B_GetDuty(float Iaccl_Set, BridgeState_s* SenseHandle, BridgeHW_s* HW_Handle, BridgeSet_s* SetHandle)
{
    float BoostDuty; // Buck״̬��A��Hͨ��ʱ��/����
    float L = HW_Handle->Inductor;
    float U_A = SenseHandle->U_A;
    float U_B = SenseHandle->U_B;
    float K = HW_Handle->Duty_Max;
    float Delta; // �����б�ʽ
    float temp;

    int ret = 0; // Ĭ��Ϊ��

    Delta = U_A * U_A - 4 * Iaccl_Set / K * U_B * L;
    if (Delta < 0)
    {
        Delta = 0;
        ret = -1; // ����������趨ֵ���󣬵����޽⣬��ʱȡ������������Ӧ��ռ�ձ�
    }

    if (fabsf(U_B) < 0.1f)
    {
        // B���ѹ�ܵͣ���ʱ����û������
        BoostDuty = K; // ֱ�ӱ��ָߵ�ƽ
    }
    else if (fabsf(U_A) < 0.1f)
    {
        // A���ѹ�ܵͣ���ʱӦ���⿪��
        BoostDuty = 0;
    }
    else
    {
        // ��ѹ����
        // ��������ռ�ձ�
        BoostDuty = K / 2 / U_B * (U_A + sqrtf(Delta));
    }

    // Boostģʽ�£���е���=IA/K
    if (!isnan(SenseHandle->I_A))
    {
        SetHandle->I_L = SenseHandle->I_A / K; // �������*K�ɵõ�е���
    }
    else
    {
        SetHandle->I_L = 0.001f; // �쳣ʱ���յ���������������
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
        // ��ǰռ�ձȼ�������Χ���� �����޸�ret��ֵ
    }
    // ����������ʽ
    SetHandle->TGGLA[0] = BoostDuty;
    temp = BoostDuty + 1 - K;
    if (temp > 1)
    { // ���������ܻᵼ��temp>1
        temp = 1;
    }
    SetHandle->TGGLA[1] = temp;
    SetHandle->TGGLB[0] = BoostDuty;
    SetHandle->TGGLB[1] = 1;
    SetHandle->SwitchMode = BRIDGE_BOOST;
    return ret;
}
