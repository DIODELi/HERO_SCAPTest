#ifndef __DRV_BRIDGEHW_H__
#define __DRV_BRIDGEHW_H__

#include "drv_BridgeMath.h"

typedef enum
{
    BRIDGE_A_CAP, // �Գ�������ΪA�� ��ʱ������Կ�����BAT�ĵ���
    BRIDGE_A_BAT, // ��ֱ������ΪA�� ��ʱ������Կ�����CAP�ĵ���
} BridgeDir_e;

typedef struct
{
    float U_CAP;
    float U_BAT;
    float I_CAP;
    float I_BAT;
} ADC_BridgeData_s;

extern int Bridge_SetPWM(BridgeSet_s *SetHandle); // ��Ҫ�������������PWM��Եϸ��

// ��ʼ��H��Ӳ��
extern int Bridge_Init(void);

#endif
