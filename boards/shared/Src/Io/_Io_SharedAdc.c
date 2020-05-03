#include <stdint.h>
#include <assert.h>
#include "_Io_SharedAdc.h"
#include "App_SharedConstants.h"

// TODO: Declare these in a adc input config file
#define MAX_NUM_OF_ADC 2
#define MAX_NUM_OF_ADC_CHANNELS 8

#ifndef MAX_NUM_OF_ADC
#define MAX_NUM_OF_ADC 1
#endif

#ifndef NUM_ADC_CHANNELS
#define NUM_ADC_CHANNELS 16
#endif

struct AdcInput
{
    ADC_HandleTypeDef *adc_handle;
    uint16_t           adc_resolution;
    uint32_t           adc_max_rank;
    uint16_t *         adc_readings;
};

static uint32_t Io_GetAdcResolution (ADC_HandleTypeDef *hadc)
{
    switch (hadc->Init.Resolution)
    {
        case ADC_RESOLUTION_6B:
            return MAX_6_BITS_VALUE;
        case ADC_RESOLUTION_8B:
            return MAX_8_BITS_VALUE;
        case ADC_RESOLUTION_10B:
            return MAX_10_BITS_VALUE;
        case ADC_RESOLUTION_12B:
            return MAX_12_BITS_VALUE;
        default:
            return ERROR;
    }
    return SUCCESS;
}

uint16_t Io_SharedAdc_GetAdcReading(
    const struct AdcInput *const adc_input,
    uint32_t                     adc_rank);

struct AdcInput *Io_SharedAdc_Init(ADC_HandleTypeDef *hadc)
{
    assert(hadc != NULL);

    static size_t          alloc_index = 0;
    static struct AdcInput adc_inputs[MAX_NUM_OF_ADC];
    static uint16_t        adc_readings[MAX_NUM_OF_ADC][NUM_ADC_CHANNELS];

    assert(alloc_index < MAX_NUM_OF_ADC);
    struct AdcInput *const adc_input = &adc_inputs[alloc_index];

    adc_input->adc_readings  = &adc_readings[alloc_index][0];
    adc_input->adc_handle    = hadc;
    adc_input->adc_max_value = ;

    HAL_ADCEx_Calibration_Start(hadc, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(
        hadc, (uint32_t *)&adc_readings[alloc_index++][0], NUM_ADC_CHANNELS);

    return adc_input;
}

uint16_t Io_SharedAdc_GetAdcReading(
    const struct AdcInput *const adc_input,
    uint32_t                     adc_rank)
{
    assert(adc_rank < MAX_NUM_OF_ADC_CHANNELS - 1);
    return adc_input->adc_readings[adc_rank];
}
