#include <assert.h>
#include <math.h>

#include "Io_WheelSpeedSensors.h"
#include "Io_SharedFreqOnlyPwmInput.h"
#include "main.h"

// TODO: Configure timer prescaler value to be able to measure min and max
// frequencies

static const float MPS_TO_KPH_CONVERSION_FACTOR = 3.6;

// TODO: Figure out what these constants will be for the new car
static const int   RELUCTOR_RING_TOOTH_COUNT = 48;

//Tire diameter measured in metres
static const float TIRE_DIAMETER             = 0.52;

//static const float MAX_WHEEL_SPEED           = 200.0;

static const float ARC_LENGTH_PER_TICK =
    (M_PI * TIRE_DIAMETER) /
    RELUCTOR_RING_TOOTH_COUNT;

static struct FreqOnlyPwmInput *left_wheel_speed_sensor,
    *right_wheel_speed_sensor;

void Io_WheelSpeeds_Init(
    TIM_HandleTypeDef *htim_left_wheel_speed_sensor,
    TIM_HandleTypeDef *htim_right_wheel_speed_sensor)
{
    assert(htim_left_wheel_speed_sensor != NULL);
    assert(htim_right_wheel_speed_sensor != NULL);

    left_wheel_speed_sensor = Io_SharedFreqOnlyPwmInput_Create(
        htim_left_wheel_speed_sensor, TIMx_FREQUENCY / TIM16_PRESCALER,
        TIM_CHANNEL_1, TIM16_AUTO_RELOAD_REG, HAL_TIM_ACTIVE_CHANNEL_1);

    right_wheel_speed_sensor = Io_SharedFreqOnlyPwmInput_Create(
        htim_right_wheel_speed_sensor, TIMx_FREQUENCY / TIM17_PRESCALER,
        TIM_CHANNEL_1, TIM17_AUTO_RELOAD_REG, HAL_TIM_ACTIVE_CHANNEL_1);
}

float Io_WheelSpeedSensors_GetLeftSpeed(void)
{
    // Wheel_Speed_Kph = (Arc_Length / Tick ) * (Measured_Frequency) * (Mps_To_Kph)
    // Note: Measured_Frequency = Number_of_Ticks / s
    return MPS_TO_KPH_CONVERSION_FACTOR * ARC_LENGTH_PER_TICK /
           (Io_SharedFreqOnlyPwmInput_GetFrequency(left_wheel_speed_sensor) /
            (float)TIM16_PRESCALER);
}

float Io_WheelSpeedSensors_GetRightSpeed(void)
{
    // Wheel_Speed_Kph = (Arc_Length / Tick ) * (Measured_Frequency) * (Mps_To_Kph)
    // Note: Measured_Frequency = Number_of_Ticks / s
    return MPS_TO_KPH_CONVERSION_FACTOR * ARC_LENGTH_PER_TICK /
           (Io_SharedFreqOnlyPwmInput_GetFrequency(right_wheel_speed_sensor) /
            (float)TIM17_PRESCALER);
}

void Io_WheelSpeedSensors_InputCaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim ==
            Io_SharedFreqOnlyPwmInput_GetTimerHandle(left_wheel_speed_sensor) &&
        htim->Channel == Io_SharedFreqOnlyPwmInput_GetTimerActiveChannel(
                             left_wheel_speed_sensor))
    {
        Io_SharedFreqOnlyPwmInput_Tick(left_wheel_speed_sensor);
    }

    else if (
        htim == Io_SharedFreqOnlyPwmInput_GetTimerHandle(
                    right_wheel_speed_sensor) &&
        htim->Channel == Io_SharedFreqOnlyPwmInput_GetTimerActiveChannel(
                             right_wheel_speed_sensor))
    {
        Io_SharedFreqOnlyPwmInput_Tick(right_wheel_speed_sensor);
    }
}

void Io_WheelSpeedSensors_CheckIfRightSensorIsActive(void)
{
    Io_SharedFreqOnlyPwmInput_CheckIfPwmIsActive(right_wheel_speed_sensor);
}

void Io_WheelSpeedSensors_CheckIfLeftSensorIsActive(void)
{
    Io_SharedFreqOnlyPwmInput_CheckIfPwmIsActive(left_wheel_speed_sensor);
}
