#include <stdint.h>
#include "configs/App_CellConfigs.h"
#include "App_CellTemperatures.h"

struct CellTemperatures
{
    uint16_t *measured_internal_die_temperatures;
};

static struct CellTemperatures cell_temperatures;

void App_CellTemperatures_Init(uint16_t *(*get_internal_die_temperatures)(void))
{
    cell_temperatures.measured_internal_die_temperatures =
        get_internal_die_temperatures();
}

float App_CellTemperatures_GetInternalDieTemperature0(void)
{
    return cell_temperatures
                   .measured_internal_die_temperatures[CELL_MONITORING_IC_0] *
               100e-6f / 7.6e-3f -
           276.0f;
}

float App_CellTemperatures_GetInternalDieTemperature1(void)
{
    return cell_temperatures
                   .measured_internal_die_temperatures[CELL_MONITORING_IC_1] *
               100e-6f / 7.6e-3f -
           276.0f;
}
