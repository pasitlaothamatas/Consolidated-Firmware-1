#include <stdlib.h>
#include <assert.h>
#include "App_CellMonitoring.h"
#include "App_SharedExitCode.h"

struct CellVoltages
{
    void (*configure)(void);
    ExitCode (*calculate_cell_voltages)(void);
    uint16_t *cell_voltages;
};

struct CellVoltages *App_CellVoltages_Create(
    void (*configure)(void),
    ExitCode (*calculate_cell_voltages)(void),
    uint16_t *(*get_cell_voltages)(void))
{
    struct CellVoltages *cell_voltages = malloc(sizeof(struct CellVoltages));
    assert(cell_voltages != NULL);

    cell_voltages->cell_voltages           = get_cell_voltages();
    cell_voltages->configure               = configure;
    cell_voltages->calculate_cell_voltages = calculate_cell_voltages;

    return cell_voltages;
}

void App_CellVoltages_Destroy(struct CellVoltages *cell_voltages)
{
    free(cell_voltages);
}

void App_CellVoltages_Configure(struct CellVoltages *cell_voltages)
{
    cell_voltages->configure();
}

ExitCode
    App_CellVoltages_CalculateCellVoltages(struct CellVoltages *cell_voltages)
{
    return cell_voltages->calculate_cell_voltages();
}

uint16_t *App_CellVoltages_GetCellVoltages(
    struct CellVoltages *cell_voltages,
    size_t               ic)
{
    return &(cell_voltages->cell_voltages[ic]);
}
