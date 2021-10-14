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
    BridgeMode_e SwitchMode; // ������ǣ���PWM�趨������ʵ�ֹ���
    float I_L;
    float TGGLA[2]; // A��������ת����� δ��������
    float TGGLB[2]; // B��������ת����� δ��������
} BridgeSet_s;

typedef struct
{
    BridgeMode_e ModeNow; // ��ǰ�����㷨ѡ��Ŀ���ģʽ��Buck��Boost��Mix
    Limit_s BuckLim;
    Limit_s MixLim;
    Limit_s BoostLim;
} BridgeCtrl_s;

typedef struct
{
    float U_A;
    float U_B;
    float I_A; // ��A��BΪ������
    float I_B; // ��A��BΪ������
} BridgeState_s;

typedef struct
{
    int DeadTime_ON;  // MOS�ܿ�ͨ��������
    int DeadTime_OFF; // MOS�ܹض���������
    float Duty_Max;   // ���ռ�ձ� ��ȡ0.95
    float Inductor;   // ���ص�Դ�����
} BridgeHW_s;

typedef struct
{
    BridgeHW_s HardwareHandle;
    BridgeState_s SenseHandle; // �������� ���������ÿ�ּ���֮ǰˢ��һ�Σ������ڼ��������ˢ��
    BridgeSet_s SetHandle;     // Ӳ���趨��
    BridgeCtrl_s CtrlHandle;   // ������
} BridgeData_s;

// ���㵱ǰ״̬����Buck��A��B��B�˵������ٶȿɿط�Χ ������-1����������0
extern int BuckA2B_GetLim(BridgeState_s *SenseHandle, BridgeHW_s *HW_Handle, Limit_s *BuckLim);

// ������������0�����㵽���޷�����1
// ʹ�õ�ǰ�����ĵ������ٶȼ���ռ�ձ� �������ٶ���Ҫ���ⲿ��֤��ֵ��Χ
extern int BuckA2B_GetDuty(float Iaccl_Set, BridgeState_s *SenseHandle, BridgeHW_s *HW_Handle, BridgeSet_s *SetHandle);

// ���㵱ǰ״̬����Boost��A��B��B�˵������ٶȿɿط�Χ ������-1����������0
extern int BoostA2B_GetLim(BridgeState_s *SenseHandle, BridgeHW_s *HW_Handle, Limit_s *BoostLim);

// ������������0�����㵽���޷�����1
// ʹ�õ�ǰ�����ĵ������ٶȼ���ռ�ձ� �������ٶ���Ҫ���ⲿ��֤��ֵ��Χ
extern int BoostA2B_GetDuty(float Iaccl_Set, BridgeState_s *SenseHandle, BridgeHW_s *HW_Handle, BridgeSet_s *SetHandle);

#endif
