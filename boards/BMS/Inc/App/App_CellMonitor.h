#pragma once

#include "App_InRangeCheck.h"
#include "App_SharedExitCode.h"

struct CellMonitor;

/**
 * Allocate and initialize a cell monitor.
 * @param configure_daisy_chain A function that can be called to configure the
 * daisy chain of cell measuring chips.
 * @param read_cell_voltages A function that can be called to read cell voltages
 * from the daisy chain of cell measuring chips.
 *
 * @param get_min_cell_voltage A function that can be called to get the minimum
 * cell voltage.
 * @param get_max_cell_voltage A function that can be called to get the max cell
 * voltage.
 * @param get_average_cell_voltage A function that can be called to get the
 * average cell voltage.
 * @param get_pack_voltage A function that can be called to get the pack
 * voltage.
 *
 * @param get_segment_0_voltage A function that can be called to get the 0th
 * segment voltage.
 * @param get_segment_1_voltage A function that can be called to get the 1st
 * segment voltage.
 * @param get_segment_2_voltage A function that can be called to get the 2nd
 * segment voltage.
 * @param get_segment_3_voltage A function that can be called to get the 3rd
 * segment voltage.
 * @param get_segment_4_voltage A function that can be called to get the 4th
 * segment voltage.
 * @param get_segment_5_voltage A function that can be called to get the 5th
 * segment voltage.
 *
 * @param min_cell_voltage The minimum allowed cell voltage, exclusive.
 * @param max_cell_voltage The maximum allowed cell voltage, exclusive.
 * @param min_segment_voltage The minimum allowed segment voltage, exclusive.
 * @param max_segment_voltage The maximum allowed segment voltage, exclusive.
 * @param min_pack_voltage The minimum allowed pack voltage, exclusive.
 * @param max_pack_voltage The maximum allowed pack voltage, exclusive.
 * @return A pointer to the created cell monitor, whose ownership is given to
 * the caller.
 */
struct CellMonitor *App_CellMonitor_Create(
    ExitCode (*configure_daisy_chain)(void),
    ExitCode (*read_cell_voltages)(void),
    float (*get_min_cell_voltage)(void),
    float (*get_max_cell_voltage)(void),
    float (*get_average_cell_voltage)(void),
    float (*get_pack_voltage)(void),
    float (*get_segment_0_voltage)(void),
    float (*get_segment_1_voltage)(void),
    float (*get_segment_2_voltage)(void),
    float (*get_segment_3_voltage)(void),
    float (*get_segment_4_voltage)(void),
    float (*get_segment_5_voltage)(void),
    float min_cell_voltage,
    float max_cell_voltage,
    float min_segment_voltage,
    float max_segment_voltage,
    float min_pack_voltage,
    float max_pack_voltage);

/**
 * Deallocate the memory used by the given cell monitor.
 * @param cell_monitor The cell monitor to deallocate.
 */
void App_CellMonitor_Destroy(struct CellMonitor *cell_monitor);

/**
 * Configure the daisy chain of cell measuring chips.
 * @param cell_monitor The given cell monitor.
 */
ExitCode App_CellMonitor_Configure(const struct CellMonitor *cell_monitor);

/**
 * Read cell voltages from the daisy chain of cell measuring chips.
 * @param cell_monitor The given cell monitor.
 * @return EXIT_CODE_OK if cell voltages were read successfully from the daisy
 * chain of cell measuring chips. Else, return EXIT_CODE_ERROR.
 */
ExitCode
    App_CellMonitor_ReadCellVoltages(const struct CellMonitor *cell_monitor);

/**
 * Get the 0th segment voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The 0th segment voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetSegment0VoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);

/**
 * Get the 1st segment voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The 1st segment voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetSegment1VoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);

/**
 * Get the 2nd segment voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The 2nd segment voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetSegment2VoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);

/**
 * Get the 3rd segment voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The 3rd segment voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetSegment3VoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);

/**
 * Get the 4th segment voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The 4th segment voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetSegment4VoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);

/**
 * Get the 5th segment voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The 5th segment voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetSegment5VoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);

/**
 * Get the pack voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The pack voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetPackVoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);

/**
 * Get the minimum cell voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The minimum cell voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetMinCellVoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);

/**
 * Get the maximum cell voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The maximum cell voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetMaxCellVoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);

/**
 * Get the average cell voltage in-range check.
 * @param cell_monitor The given cell monitor.
 * @return The average cell voltage in-range check for the given cell monitor.
 */
struct InRangeCheck *App_CellMonitor_GetAverageCellVoltageInRangeCheck(
    const struct CellMonitor *cell_monitor);