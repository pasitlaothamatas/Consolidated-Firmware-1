#include <assert.h>
#include "_Io_SharedAdc.h"
#include "Io_SteeringAngleSensor.h"

struct SteeringAngleSensor
{
    float            angle;
    struct AdcInput *adc_input;
    size_t           rank;
    float            offset_voltage;
};

static struct SteeringAngleSensor steering_angle_sensor;

void Io_SteeringAngleSensor_Init(
    struct AdcInput *adc_input,
    size_t           rank,
    float            offset_voltage)
{
    assert(adc_input != NULL);
    assert(rank > 0U);
    assert(rank <= _Io_SharedAdc_GetNumActiveChannel(adc_input));

    steering_angle_sensor.angle     = 0.0f;
    steering_angle_sensor.adc_input = adc_input;
    steering_angle_sensor.rank      = rank;

    // Voltage when the steering angle is 0 deg
    steering_angle_sensor.offset_voltage = offset_voltage;
}

float Io_SteeringAngleSensor_GetAngle(void)
{
    return steering_angle_sensor.angle;
}

void Io_SteeringAngleSensor_AdcConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    // Scale factor used to map the adc measured voltage to an angle
    // static const float DEGREES_PER_VOLT = (float)(360.0 / 3.3);

    if (hadc == _Io_SharedAdc_GetAdcHandle(steering_angle_sensor.adc_input))
    {
        // steering_angle_sensor.angle =
        // (float)((_Io_SharedAdc_GetChannelVoltage(steering_angle_sensor.adc_input,
        // steering_angle_sensor.rank) - steering_angle_sensor.offset_voltage) /
        // DEGREES_PER_VOLT);
        steering_angle_sensor.angle = _Io_SharedAdc_GetChannelVoltage(
            steering_angle_sensor.adc_input, steering_angle_sensor.rank);
    }
}
