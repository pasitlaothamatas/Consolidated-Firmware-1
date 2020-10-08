#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "App_CellMonitoring.h"
#include "App_SharedExitCode.h"

struct CellVoltages
{
    void (*configure_daisy_chain)(void);
    ExitCode (*read_cell_voltages)(void);

    uint16_t *accumulator_voltages;
    uint32_t  num_of_accumulator_segments;
    uint32_t  num_of_cell_voltages_per_cell_monitoring_chip;

    float  average_pack_voltage;
    float *average_segment_voltages;
    float  max_cell_voltage;
    float  min_cell_voltage;
};

static float App_CalculateAverage(
    float *  array,
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
    uint16_t *(*get_cell_voltages)(void),
    uint32_t num_of_daisy_chained_devices)
{
    struct CellVoltages *cell_voltages = malloc(sizeof(struct CellVoltages));
    assert(cell_voltages != NULL);

    cell_voltages->configure_daisy_chain = configure_daisy_chain;
    cell_voltages->read_cell_voltages    = calculate_cell_voltages;
    cell_voltages->accumulator_voltages  = get_cell_voltages();

    // Each accumulator segment cell voltages is measured by a cell monitoring
    // chip. Thus, the total number of accumulator segments is equal to the
    // number of cell monitoring devices.
    cell_voltages->num_of_accumulator_segments = num_of_daisy_chained_devices;
    cell_voltages->average_pack_voltage        = 0.0f;
    cell_voltages->max_cell_voltage            = 0.0f;
    cell_voltages->average_segment_voltages =
        calloc(num_of_daisy_chained_devices, sizeof(uint16_t));
    cell_voltages->min_cell_voltage = 0.0f;

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

void App_CellVoltages_CalculateAverageVoltages(
    struct CellVoltages *cell_voltages)
{
    // Calculate the average segment voltages.
    for (size_t i = 0U;
         i < cell_voltages->num_of_cell_voltages_per_cell_monitoring_chip; i++)
    {
        float *segment_cell_voltages =
            (float *)(&cell_voltages->accumulator_voltages[i]);

        cell_voltages->average_segment_voltages[i] =
            App_CalculateAverage(
                segment_cell_voltages,
                cell_voltages->num_of_cell_voltages_per_cell_monitoring_chip,
                cell_voltages->num_of_cell_voltages_per_cell_monitoring_chip) /
            10000;
    }

    // Calculate the average pack voltage.
    cell_voltages->average_pack_voltage = App_CalculateAverage(
        cell_voltages->average_segment_voltages,
        cell_voltages->num_of_accumulator_segments,
        cell_voltages->num_of_accumulator_segments);
}
