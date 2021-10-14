#include "drv_ADCSense.h"

#include "main.h"
#include "hrtim.h"

#include "adc.h"
#include "dma.h"

ADC1_Data_s ADC1Data = {0};

float I_A_Test = 0,I_B_Test = 0;

// ÿ��HRTIM���жϿ�ͷ��Ҫ���ô˺���
void ADC_Routine(uint32_t PWMCount)
{
    uint16_t *DataToAVRNow;
    register int_fast32_t FastSum32; // ����������͵ı���
    register int_fast32_t fori;
    // ����������һ�ֲ���
    switch (ADC1Data.WriteNum)
    {
    case 0:
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)(ADC1Data.ADC1_DataTemp1), ADCSAMPLE_NUM * 2);
        ADC1Data.WriteNum = 1;
        ADC1Data.DataValid = 0;
        goto AfterDataAVR; // ��һ�����У����������ݴ���
    case 1:
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)(ADC1Data.ADC1_DataTemp2), ADCSAMPLE_NUM * 2);
        DataToAVRNow = ADC1Data.ADC1_DataTemp1;
        ADC1Data.WriteNum = 2;
        break;
    case 2:
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)(ADC1Data.ADC1_DataTemp1), ADCSAMPLE_NUM * 2);
        DataToAVRNow = ADC1Data.ADC1_DataTemp2;
        ADC1Data.WriteNum = 1;
        break;

    default:
        ADC1Data.WriteNum = 0;
        ADC1Data.DataValid = 0;
        break;
    }

    // ������һ������
    FastSum32 = 0;
    for (fori = 0; fori < ADCSAMPLE_NUM; fori += 2)
    {
        FastSum32 += DataToAVRNow[fori];
    }
    ADC1Data.ADC_IA = FastSum32 / (float)(ADCSAMPLE_NUM/2);
    FastSum32 = 0;
    for (fori = 1; fori < ADCSAMPLE_NUM; fori += 2)
    {
        FastSum32 += DataToAVRNow[fori];
    }
    ADC1Data.ADC_IB = FastSum32 / (float)(ADCSAMPLE_NUM/2);
    ADC1Data.DataValid = 1;
    I_A_Test = 0.5f * ((ADC1Data.ADC_IA - ADC_BIAS_IA) * ADC_K_IA) + 0.5f * I_A_Test;
    I_B_Test = 0.5f * ((ADC1Data.ADC_IB - ADC_BIAS_IB) * ADC_K_IB) + 0.5f * I_B_Test;

AfterDataAVR:
    return;
}

/**
  * @brief  Conversion DMA half-transfer callback in non blocking mode 
  * @param  hadc ADC handle
  * @retval None
  */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    // ���鴫����룬���Թر�ADCDMA��
    HAL_ADC_Stop_DMA(&hadc1);
}

// ADC����ʼ��
void ADCSense_Init(void)
{
    ADC1Data.WriteNum = 0;
    ADC1Data.DataValid = 0;
}
