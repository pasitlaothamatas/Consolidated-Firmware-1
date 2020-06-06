#pragma once

#include <stdbool.h>

/**
 * Check if the pressure from MSP-300-2K5-P-3-N-1 exceeds the brake pressure
 * threshold. If the pressure exceeds threshold, the brake is said to be
 * actuated
 * @return True if the brake is actuated, false if the brake is not actuated
 */
bool Io_MSP3002K5P3N1_IsBrakeActuated(void);

/**
 * Check if the MSP-300-2K5-P-3-N-1 is in an open circuit
 * or short circuit state. If the output voltage exceeds the short circuit
 * threshold or goes below the open circuit threshold, the brake pressure sensor
 * is open/short circuited
 * @return True if the brake is open/short circuited, false if the brake is ok
 */
bool Io_MSP3002K5P3N1_IsBrakeOpenOrShortCircuited(void);

/**
 * Get the pressure in psi from the MSP-300-2K5-P-3-N-1 brake pressure
 * sensor
 * @return The pressure of the brake pressure sensor in psi
 */
float Io_MSP3002K5P3N1_GetPressurePsi(void);
