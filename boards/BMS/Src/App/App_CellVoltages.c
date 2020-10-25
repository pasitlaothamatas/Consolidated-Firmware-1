#include <stddef.h>
#include <stdint.h>
#include "configs/App_CellConfigs.h"
#include "App_CellVoltages.h"

static uint16_t *measured_cell_voltages;

/**
 * A function used to compute the sum of array elements.
 * @param array A pointer to the given array.
 * @param size The size of the given array.
 * @return The sum of elements for the given array.
 */
static float App_SumOfArrayElements(uint16_t *array, uint32_t size);

static float App_SumOfArrayElements(uint16_t *array, uint32_t size)
{
    uint32_t array_sum = 0U;
    for (size_t i = 0U; i < size; i++)
    {
        array_sum += array[i];
    }
    return (float)array_sum;
}

void App_CellVoltages_Init(uint16_t *(*get_cell_voltages)(void))
{
    // Get the pointer to the 2D array of cell voltages read back from the cell
    // monitoring daisy chain.
    measured_cell_voltages = get_cell_voltages();
}

float App_CellVoltages_GetMinCellVoltage(void)
{
    uint16_t min_cell_voltage = measured_cell_voltages[0];
    for (size_t current_cell = 1U; current_cell < NUM_OF_CELLS_PER_ACCUMULATOR;
         current_cell++)
    {
        uint16_t current_cell_voltage = measured_cell_voltages[current_cell];
        if (min_cell_voltage > current_cell_voltage)
        {
            min_cell_voltage = current_cell_voltage;
        }
    }

    return min_cell_voltage / 10000.0f;
}

float App_CellVoltages_GetMaxCellVoltage(void)
{
    float max_cell_voltage = measured_cell_voltages[0];
    for (size_t current_cell = 1U; current_cell < NUM_OF_CELLS_PER_ACCUMULATOR;
         current_cell++)
    {
        float current_cell_voltage = measured_cell_voltages[current_cell];
        if (max_cell_voltage < current_cell_voltage)
        {
            max_cell_voltage = current_cell_voltage;
        }
    }

    return (float)max_cell_voltage / 10000.0f;
}

float App_CellVoltages_GetPackVoltage(void)
{
    return (float)App_SumOfArrayElements(
               measured_cell_voltages, NUM_OF_CELLS_PER_ACCUMULATOR) /
           10000.0f;
}

float App_CellVoltages_GetAverageCellVoltage(void)
{
    return App_CellVoltages_GetPackVoltage() /
           (float)NUM_OF_CELLS_PER_ACCUMULATOR;
}

float App_CellVoltages_GetSegment0Voltage(void)
{
    return (float)App_SumOfArrayElements(
               &measured_cell_voltages
                   [CELL_MONITORING_IC_0 * NUM_OF_CELLS_READ_PER_IC],
               NUM_OF_CELLS_READ_PER_IC) /
           10000.0f;
}

float App_CellVoltages_GetSegment1Voltage(void)
{
    return (float)App_SumOfArrayElements(
               &measured_cell_voltages
                   [CELL_MONITORING_IC_1 * NUM_OF_CELLS_READ_PER_IC],
               NUM_OF_CELLS_READ_PER_IC) /
           10000.0f;
}

float App_CellVoltages_GetSegment2Voltage(void)
{
    return 0.0f;
}

float App_CellVoltages_GetSegment3Voltage(void)
{
    return 0.0f;
}

float App_CellVoltages_GetSegment4Voltage(void)
{
    return 0.0f;
}

float App_CellVoltages_GetSegment5Voltage(void)
{
    return 0.0f;
}
