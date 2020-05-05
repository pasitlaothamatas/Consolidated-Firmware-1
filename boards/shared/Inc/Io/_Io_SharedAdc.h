#pragma once
#include <stm32f3xx_hal.h>
#include "Io_SharedAdcConfig.h"

#ifndef MAX_NUM_OF_ADC
#define MAX_NUM_OF_ADC 1
#endif
#ifndef MAX_NUM_OF_ADC_CHANNELS
#define MAX_NUM_OF_ADC_CHANNELS 16
#endif

struct AdcInput;

struct AdcInput *
    _Io_SharedAdc_Create(ADC_HandleTypeDef *adc_handle, size_t vrefint_rank);

float _Io_SharedAdc_GetChannelVoltage(
    const struct AdcInput *adc_input,
    size_t                 adc_rank);

ADC_HandleTypeDef *_Io_SharedAdc_GetAdcHandle(const struct AdcInput *adc_input);

uint32_t _Io_SharedAdc_GetNumActiveChannel(const struct AdcInput *adc_input);
