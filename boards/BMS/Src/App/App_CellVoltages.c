#include <stdlib.h>
#include <assert.h>
#include "App_CellVoltages.h"
#include "Io_LTC6813.h"

struct CellVoltages
{
    void (*configure_ic)(void);
    uint16_t *(*get_cell_voltages)(void);
    PEC15Codes (*read_register_groups)(void);
};

struct CellVoltages *App_CellVoltages_Create(
    void (*configure_ic)(void),
    uint16_t *(*get_cell_voltages)(void),
    PEC15Codes (*read_register_groups)(void))
{
    struct CellVoltages *cell_voltages = malloc(sizeof(struct CellVoltages));
    assert(cell_voltages != NULL);

    cell_voltages->configure_ic         = configure_ic;
    cell_voltages->get_cell_voltages    = get_cell_voltages;
    cell_voltages->read_register_groups = read_register_groups;

    return cell_voltages;
}

void App_CellVoltages_Destroy(struct CellVoltages *cell_voltages)
{
    free(cell_voltages);
}

void App_CellVoltages_Configure(struct CellVoltages *cell_voltages)
{
    cell_voltages->configure_ic();
}
