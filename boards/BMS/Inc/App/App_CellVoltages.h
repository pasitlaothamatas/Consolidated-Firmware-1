#pragma once

enum Segments
{
    SEGMENT_0,
    SEGMENT_1,
    SEGMENT_2,
    SEGMENT_3,
    SEGMENT_4,
    SEGMENT_5,
    NUM_OF_SEGMENTS
};

/**
 * Initialize a set of functions used to calculate the average segment voltages,
 * the average cell voltage, the pack voltage, the minimum cell voltage, and the
 * maximum cell voltage.
 * @param get_cell_voltages A pointer to a 2D array containing cell voltages
 * measured by a daisy chain of cell monitoring chips.
 * @param num_of_segments The number of segments on the accumulator.
 * @param num_of_cells_per_segment The number of cell voltage measurements per
 * segment.
 */
void App_CellVoltages_Init(
    uint16_t *(*get_cell_voltages)(void),
    uint32_t num_of_cells_per_segment);

/**
 * Get the average voltage for the 0th segment, measured in V.
 * @return Get the average voltage for the 0th segment, measured in V.
 */
float App_CellVoltages_GetSegment0Average(void);

/**
 * Get the average voltage for the 1st segment, measured in V.
 * @return Get the average voltage for the 1st segment, measured in V.
 */
float App_CellVoltages_GetSegment1Average(void);

/**
 * Get the average voltage for the 2nd segment, measured in V.
 * @return Get the average voltage for the 2nd segment, measured in V.
 */
float App_CellVoltages_GetSegment2Average(void);

/**
 * Get the average voltage for the 3rd segment, measured in V.
 * @return The average voltage of the 3rd segment, measured in V.
 */
float App_CellVoltages_GetSegment3Average(void);

/**
 * Get the average voltage for the 4th segment, measured in V.
 * @return The average voltage of the 4th segment, measured in V.
 */
float App_CellVoltages_GetSegment4Average(void);

/**
 * Get the average voltage for the 5th segment, measured in V.
 * @return The average voltage of the 5th segment, measured in V.
 */
float App_CellVoltages_GetSegment5Average(void);

/**
 * Get the pack voltage, measured in V.
 * @return The pack voltage, measured in V.
 */
float App_CellVoltages_GetPack(void);

/**
 * Get the average cell voltage, measured in V.
 * @return The average cell voltage, measured in V.
 */
float App_CellVoltages_GetAverageCell(void);

/**
 * Get the minimum cell voltage, measured in V.
 * @return The minimum cell voltage, measured in V.
 */
float App_CellVoltages_GetMinCell(void);

/**
 * Get the maximum cell voltage, measured in V.
 * @return The maximum cell voltage, measured in V.
 */
float App_CellVoltages_GetMaxCell(void);
