#include <stm32f3xx_hal.h>

#include "Io_SteeringAngleSensor.h"

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    Io_SteeringAngleSensor_AdcConvCpltCallback(hadc);
}
