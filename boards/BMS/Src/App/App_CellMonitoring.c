#include <stdlib.h>
#include <assert.h>
#include "App_CellMonitoring.h"
#include "App_SharedExitCode.h"

// TODO: have a separate file responsible for the PEC15 exit codes
#include "Io_LTC6813.h"

struct CellMonitoring
{
    uint16_t *cell_voltages;
    bool      is_adc_awake;

    void (*configure_ic)(void);
    ExitCode (*start_adc_conversion)(void);
    PEC15Codes (*read_register_groups)(void);
    uint16_t *(*get_cell_voltages)(void);
};

struct CellMonitoring *App_CellMonitoring_Create(
    void (*configure_ic)(void),
    ExitCode (*start_adc_conversion)(void),
    PEC15Codes (*read_register_groups)(void),
    uint16_t *(*get_cell_voltages)(void))
{
    struct CellMonitoring *cell_monitoring =
        malloc(sizeof(struct CellMonitoring));
    assert(cell_monitoring != NULL);

    cell_monitoring->cell_voltages        = NULL;
    cell_monitoring->configure_ic         = configure_ic;
    cell_monitoring->start_adc_conversion = start_adc_conversion;
    cell_monitoring->get_cell_voltages    = get_cell_voltages;
    cell_monitoring->read_register_groups = read_register_groups;

    return cell_monitoring;
}

void App_CellMonitoring_Destroy(struct CellMonitoring *cell_monitoring)
{
    free(cell_monitoring);
}

void App_CellVoltages_Configure(struct CellMonitoring *cell_monitoring)
{
    cell_monitoring->configure_ic();
}

ExitCode
    App_CellMonitoring_ReadCellVoltages(struct CellMonitoring *cell_monitoring)
{
    cell_monitoring->start_adc_conversion();

    size_t num_attempts = 0U;
    while (cell_monitoring->read_register_groups() == PEC15_MISMATCH_ERROR)
    {
        ++num_attempts;
        if (num_attempts == 3U)
        {
            return EXIT_CODE_UNIMPLEMENTED;
        }
    }

    cell_monitoring->cell_voltages = cell_monitoring->get_cell_voltages();

    return EXIT_CODE_OK;
}

uint16_t *App_CellMonitoring_GetCellVoltages(
    struct CellMonitoring *cell_monitoring,
    size_t                 ic)
{
    return &(cell_monitoring->cell_voltages[ic]);
}
