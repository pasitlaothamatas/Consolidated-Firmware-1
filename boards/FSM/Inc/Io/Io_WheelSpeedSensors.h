#pragma once

#include <stm32f3xx_hal.h>

/**
 * @brief Initializes the left and right wheel speed sensors
 * @param htim_left_wheel_speed Timer handle for the left wheel speed
 * @param htim_right_wheel_speed Timer handle for the right wheel speed
 */
void Io_WheelSpeedSensors_Init(
    TIM_HandleTypeDef *htim_left_wheel_speed,
    TIM_HandleTypeDef *htim_right_wheel_speed);

/**
 * @brief Gets the wheel speed in km/h from the left wheel speed sensor
 * @return Returns the wheel speed in km/h
 */
float Io_WheelSpeedSensors_GetLeftSpeed(void);

/**
 * @brief Gets the wheel speed in km/h from the right wheel speed sensor
 * @return Returns the wheel speed in km/h
 */
float Io_WheelSpeedSensors_GetRightSpeed(void);

/**
 * @brief Check if the PWM signal of the left wheel speed sensor is active. If
 * it is inactive (i.e. It has been unplugged or unpowered), set the frequency
 * to NaN.
 *
 * @note This function should be called in the timer overflow interrupt
 *       for the PWM signal of the left wheel speed sensor.
 */
void Io_WheelSpeedSensors_CheckIfLeftSensorIsActive(void);

/**
 * @brief Check if the PWM signal of the right wheel speed sensor is active. If
 * it is inactive (i.e. It has been unplugged or unpowered), set the frequency
 * to NaN.
 *
 * @note This function should be called in the timer overflow interrupt
 *       for the PWM signal of the right wheel speed sensor.
 */
void Io_WheelSpeedSensors_CheckIfRightSensorIsActive(void);
