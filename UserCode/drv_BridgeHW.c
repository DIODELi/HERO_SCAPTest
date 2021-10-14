#include "drv_BridgeHW.h"

#include "main.h"
#include "hrtim.h"

BridgeData_s Bridge;

#define DEADTIME_ON_SET 100
#define DEADTIME_OFF_SET 100
#define INDUCTOR_SET 	33e-6f
#define DUTYMAX_SET 	0.95f

// TODO
// ��Ҫ�������������PWM��Եϸ�� ���ո������趨�����ԼĴ������в���
int Bridge_SetPWM(BridgeSet_s *SetHandle)
{
    return 0;
}

// �ӵ�ǰADC����д��Bridge
void BridgeLoadADCData(ADC_BridgeData_s* ADC_Now,BridgeState_s* StateNow_Out,BridgeDir_e Dir_Now)
{
    switch (Dir_Now)
    {
    case BRIDGE_A_CAP:
        StateNow_Out->I_A = ADC_Now->I_CAP;
        StateNow_Out->I_B = ADC_Now->I_BAT;
        StateNow_Out->U_A = ADC_Now->U_CAP;
        StateNow_Out->U_B = ADC_Now->U_BAT;
        break;

    case BRIDGE_A_BAT:
        StateNow_Out->I_B = -ADC_Now->I_CAP;
        StateNow_Out->I_A = -ADC_Now->I_BAT;
        StateNow_Out->U_B = ADC_Now->U_CAP;
        StateNow_Out->U_A = ADC_Now->U_BAT;
        break;

    default:
        StateNow_Out->I_B = 0;
        StateNow_Out->I_A = 0;
        StateNow_Out->U_B = 0;
        StateNow_Out->U_A = 0;
        break;
    }
}

static void Bridge_DataInit(BridgeData_s* Init)
{
    // ���������
    Init->HardwareHandle.DeadTime_OFF = DEADTIME_OFF_SET;
    Init->HardwareHandle.DeadTime_ON = DEADTIME_ON_SET;
    Init->HardwareHandle.Duty_Max = DUTYMAX_SET;
    Init->HardwareHandle.Inductor = INDUCTOR_SET;

    // ������ʼ��
    Init->CtrlHandle.ModeNow = BRIDGE_STOP;
    Init->SenseHandle.I_A = 0;
    Init->SenseHandle.I_B = 0;
    Init->SenseHandle.U_A = 0;
    Init->SenseHandle.U_B = 0;
    Init->SetHandle.I_L = 0;
    Init->SetHandle.SwitchMode = BRIDGE_STOP;
    Init->SetHandle.TGGLA[0] = 0;
    Init->SetHandle.TGGLA[1] = 0;
    Init->SetHandle.TGGLB[0] = 0;
    Init->SetHandle.TGGLB[1] = 0;
}

// TODO
static void Bridge_HWInit(void)
{
    HAL_HRTIM_WaveformCounterStart_IT(&hhrtim1, HRTIM_TIMERID_MASTER);

    HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_B);
    HAL_HRTIM_WaveformCounterStart(&hhrtim1, HRTIM_TIMERID_TIMER_A);

    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2);
    HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2);
}

// ��ʼ��H��Ӳ��
int Bridge_Init()
{
    Bridge_DataInit(&Bridge); // ��ʼ��������Ϣ
    Bridge_HWInit();// ��ʼ��Ӳ�������
	return 0;
}
