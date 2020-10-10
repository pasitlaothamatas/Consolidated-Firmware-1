#include <stdlib.h>
#include "App_ReadCell.h"

struct CellVoltages
{
    void (*configure_daisy_chain)(void);
    ExitCode (*read_cell_voltages)(void);

    // A pointer to the 2D array containing the measured cell voltages for each
    // cell monitoring IC.
    uint16_t *cell_voltages;
};

struct CellVoltages *App_ReadCells_Create(
    void (*configure_daisy_chain)(void),
    ExitCode (*calculate_cell_voltages)(void))
{
    struct CellVoltages *cell_voltages   = malloc(sizeof(struct CellVoltages));
    cell_voltages->configure_daisy_chain = configure_daisy_chain;
    cell_voltages->read_cell_voltages    = calculate_cell_voltages;

    return cell_voltages;
}

void App_ReadCells_Configure(const struct CellVoltages *cell_voltages)
{
    cell_voltages->configure_daisy_chain();
}

ExitCode App_ReadCells_ReadCellVoltages(
    const struct CellVoltages *const cell_voltages)
{
    return cell_voltages->read_cell_voltages();
}
