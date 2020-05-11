#pragma once
#include "Io_SharedAdc.h"

struct SteeringAngleSensor;

void Io_SteeringAngleSensor_Init(
    struct AdcInput *adc_input,
    size_t           rank,
    float            offset_voltage);

float Io_SteeringAngleSensor_GetAngle(void);

void Io_SteeringAngleSensor_AdcConvCpltCallback(ADC_HandleTypeDef *hadc);
