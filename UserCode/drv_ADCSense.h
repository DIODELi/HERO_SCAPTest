#ifndef __DRV_ADCSENSE_H__
#define __DRV_ADCSENSE_H__

#include "main.h"

#define ADCSAMPLE_NUM 116

#define ADC_BIAS_IA 1979.4f
#define ADC_BIAS_IB 1979.4f
#define ADC_K_IA 1 / 4095.0f * 3.3f * 12.5f
#define ADC_K_IB 1 / 4095.0f * 3.3f * 12.5f

typedef struct
{
    // ˫����״̬
    char WriteNum;
    // ����˫���������
    uint16_t ADC1_DataTemp1[ADCSAMPLE_NUM * 2];
    uint16_t ADC1_DataTemp2[ADCSAMPLE_NUM * 2];
    // ��������Ƿ����
    char DataValid;
    // ������
    float ADC_IA;
    float ADC_IB;
} ADC1_Data_s;

// ÿ��HRTIM���жϿ�ͷ��Ҫ���ô˺���
extern void ADC_Routine(uint32_t PWMCount);

// ADC����ʼ��
extern void ADCSense_Init(void);

#endif
