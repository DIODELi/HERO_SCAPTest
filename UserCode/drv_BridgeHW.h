#ifndef __DRV_BRIDGEHW_H__
#define __DRV_BRIDGEHW_H__

#include "drv_BridgeMath.h"

typedef enum
{
    BRIDGE_A_CAP, // 以超级电容为A侧 此时程序可以控制向BAT的电流
    BRIDGE_A_BAT, // 以直流总线为A侧 此时程序可以控制向CAP的电流
} BridgeDir_e;

typedef struct
{
    float U_CAP;
    float U_BAT;
    float I_CAP;
    float I_BAT;
} ADC_BridgeData_s;

extern int Bridge_SetPWM(BridgeSet_s *SetHandle); // 主要考虑死区问题和PWM边缘细节

// 初始化H桥硬件
extern int Bridge_Init(void);

#endif
