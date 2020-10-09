#include <stdlib.h>
#include <assert.h>
#include "App_CellMonitoring.h"
#include "App_SharedExitCode.h"

struct CellVoltages
{
    void (*configure_daisy_chain)(void);
    ExitCode (*read_cell_voltages)(void);

    // A pointer to the 2D array containing the measured cell voltages for each cell monitoring IC.
    float *accumulator_voltages;
    uint32_t  num_of_accumulator_segments;
    uint32_t  num_of_cell_voltages_per_chip;
    uint32_t  num_of_cells_per_daisy_chain;


    // An array containing the average cell voltage (V) for each accumulator segment.
    float *average_segment_voltages;
    float  average_cell_voltage;
    float  pack_voltage;
    float  max_cell_voltage;
    float  min_cell_voltage;
};

static float App_CalculateAverage(
    float    array[],
    uint32_t num_of_samples,
    uint32_t array_size)
{
    // The number of samples used to compute the average cannot exceed the size
    // of the array.
    assert(num_of_samples >= array_size);

    float calculated_average = 0.0f;
    for (size_t i = 0U; i < num_of_samples; i++)
    {
        calculated_average += array[i];
    }

    return calculated_average / (float)num_of_samples;
}

struct CellVoltages *App_CellVoltages_Create(
    void (*configure_daisy_chain)(void),
    ExitCode (*calculate_cell_voltages)(void),
    float *(*get_cell_voltages)(void),
    uint32_t num_of_daisy_chained_devices,
    uint32_t num_of_cell_voltages_per_chip)
{
    struct CellVoltages *cell_voltages = malloc(sizeof(struct CellVoltages));
    assert(cell_voltages != NULL);

    cell_voltages->configure_daisy_chain = configure_daisy_chain;
    cell_voltages->read_cell_voltages    = calculate_cell_voltages;
    cell_voltages->accumulator_voltages  = get_cell_voltages();

    // Each accumulator segment cell voltages is measured by a cell monitoring
    // chip. Thus, the total number of accumulator segments is equal to the
    // number of cell monitoring devices.
    cell_voltages->average_segment_voltages =
        calloc(num_of_daisy_chained_devices, sizeof(uint16_t));
    assert(cell_voltages->average_segment_voltages != NULL);

    cell_voltages->num_of_accumulator_segments = num_of_daisy_chained_devices;
    cell_voltages->num_of_cell_voltages_per_chip =
        num_of_cell_voltages_per_chip;
    cell_voltages->num_of_cells_per_daisy_chain =
        num_of_cell_voltages_per_chip * num_of_daisy_chained_devices;

    cell_voltages->pack_voltage         = 0.0f;
    cell_voltages->max_cell_voltage     = 0.0f;
    cell_voltages->min_cell_voltage     = 0.0f;
    cell_voltages->average_cell_voltage = 0.0f;

    return cell_voltages;
}

void App_CellVoltages_Destroy(struct CellVoltages *cell_voltages)
{
    free(cell_voltages->average_segment_voltages);
    free(cell_voltages);
}

void App_CellVoltages_Configure(struct CellVoltages *cell_voltages)
{
    cell_voltages->configure_daisy_chain();
}

ExitCode
    App_CellVoltages_CalculateCellVoltages(struct CellVoltages *cell_voltages)
{
    return cell_voltages->read_cell_voltages();
}

void App_CellVoltages_Tick(struct CellVoltages *cell_voltages)
{
    // Get the pointer to the 2D array containing the cell voltages for each cell monitoring IC.
    float *segment_cell_voltages = (float *)(cell_voltages->accumulator_voltages);
    float min_cell_voltage = *segment_cell_voltages;
    for (size_t i = 1U; i < cell_voltages->num_of_cells_per_daisy_chain; i++)
    {
        float current_cell_voltage = segment_cell_voltages[i];
        if (min_cell_voltage > current_cell_voltage)
        {
            min_cell_voltage = current_cell_voltage;
        }
    }
    cell_voltages->min_cell_voltage = min_cell_voltage;

    // Get the accumulator's maximum cell voltage.
    float max_cell_voltage = *segment_cell_voltages;
    for (size_t i = 1U; i < cell_voltages->num_of_cells_per_daisy_chain; i++)
    {
        float current_cell_voltage = segment_cell_voltages[i];
        if (max_cell_voltage < current_cell_voltage)
        {
            max_cell_voltage = current_cell_voltage;
        }
    }
    cell_voltages->max_cell_voltage = max_cell_voltage;

    // Calculate the average segment voltages.
    for (size_t current_segment = 0U;
         current_segment < cell_voltages->num_of_accumulator_segments;
         current_segment++)
    {
        segment_cell_voltages =
            (float *)(&cell_voltages->accumulator_voltages
                           [current_segment *
                            cell_voltages->num_of_cell_voltages_per_chip]);

        cell_voltages->average_segment_voltages[current_segment] =
            App_CalculateAverage(
                segment_cell_voltages,
                cell_voltages->num_of_cell_voltages_per_chip,
                cell_voltages->num_of_cell_voltages_per_chip);
    }

    // Calculate the average cell voltage. Use the previously calculated segment
    // voltages to compute the average measured cell voltage.
    cell_voltages->average_cell_voltage = App_CalculateAverage(
        cell_voltages->average_segment_voltages,
        cell_voltages->num_of_accumulator_segments,
        cell_voltages->num_of_accumulator_segments);

    // Calculate the pack voltage of the accumulator.
    cell_voltages->pack_voltage =
        cell_voltages->average_cell_voltage *
        (float)cell_voltages->num_of_cells_per_daisy_chain;
}

float *App_CellVoltages_GetAverageSegmentVoltages(
    struct CellVoltages *cell_voltages)
{
    return cell_voltages->average_segment_voltages;
}

float App_CellVoltages_GetAverageCellVoltage(struct CellVoltages *cell_voltages)
{
    return cell_voltages->average_cell_voltage;
}

float App_CellVoltages_GetPackVoltage(struct CellVoltages *cell_voltages)
{
    return cell_voltages->pack_voltage;
}

float App_CellVoltages_GetMaxCellVoltage(struct CellVoltages *cell_voltages)
{
    return cell_voltages->max_cell_voltage;
}

float App_CellVoltages_GetMinCellVoltage(struct CellVoltages *cell_voltages)
{
    return cell_voltages->min_cell_voltage;
}
