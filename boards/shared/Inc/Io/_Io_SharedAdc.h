#pragma once

#include <stm32f3xx_hal.h>

struct AdcInput;

struct AdcInput *Io_SharedAdc_Init(ADC_HandleTypeDef *adc_handle);
