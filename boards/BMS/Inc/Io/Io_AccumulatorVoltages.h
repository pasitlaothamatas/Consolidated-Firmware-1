#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "App_SharedExitCode.h"

/**
 * Read raw cell voltages from the cell monitoring chips
 * @return EXIT_CODE_OK if the raw cell voltages (100µV) were acquired
 * successfully from all cell monitoring chips. Else, EXIT_CODE_ERROR
 */
ExitCode Io_AccumulatorVoltages_ReadRawCellVoltages(void);

/**
 * Get the current minimum cell voltage out of all accumulator cell voltages
 * @return The current minimum voltage (V) out of all accumulator cell voltages
 */
float Io_AccumulatorVoltages_GetMinCellVoltage(void);

/**
 * Get the current maximum cell voltage out of all accumulator cell voltages
 * @return The current maximum voltage (V) out of all accumulator cell voltages
 */
float Io_AccumulatorVoltages_GetMaxCellVoltage(void);

/**
 * Get the pack voltage for the accumulator
 * @return The accumulator pack voltage (V)
 */
float Io_AccumulatorVoltages_GetPackVoltage(void);

/**
 * Get the accumulator cell voltage
 * @return The accumulator cell voltage (V)
 */
float Io_AccumulatorVoltages_GetAverageCellVoltage(void);

/**
 * Get the voltage for the 0th accumulator segment
 * @return The voltage for the 0th accumulator segment (V)
 */
float Io_AccumulatorVoltages_GetSegment0Voltage(void);

/**
 * Get the voltage for the 1st accumulator segment
 * @return The voltage for the 1st accumulator segment (V)
 */
float Io_AccumulatorVoltages_GetSegment1Voltage(void);

/**
 * Get the voltage for the 2nd accumulator segment
 * @return The voltage for the 2nd accumulator segment (V)
 */
float Io_AccumulatorVoltages_GetSegment2Voltage(void);

/**
 * Get the voltage for the 3rd accumulator segment
 * @return The voltage for the 3rd accumulator segment (V)
 */
float Io_AccumulatorVoltages_GetSegment3Voltage(void);

/**
 * Get the voltage for the 4th accumulator segment
 * @return The voltage for the 4th accumulator segment (V)
 */
float Io_AccumulatorVoltages_GetSegment4Voltage(void);

/**
 * Get the voltage for the 5th accumulator segment
 * @return The voltage for the 5th accumulator segment (V)
 */
float Io_AccumulatorVoltages_GetSegment5Voltage(void);
