#include <stdlib.h>
#include <assert.h>
#include "App_InRangeCheck.h"
#include "App_CellVoltages.h"

struct CellMonitoring
{
    struct InRangeCheck *pack;
    struct InRangeCheck *min;
    struct InRangeCheck *max;
    struct InRangeCheck *average_cell;
    struct InRangeCheck *segment_0;
    struct InRangeCheck *segment_1;
    struct InRangeCheck *segment_2;
    struct InRangeCheck *segment_3;
    struct InRangeCheck *segment_4;
    struct InRangeCheck *segment_5;
};

struct CellMonitoring *
    App_CellMonitoring_Create(float min_cell_voltage, float max_cell_voltage)
{
    struct CellMonitoring *cell_monitoring =
        malloc(sizeof(struct CellMonitoring));
    assert(cell_monitoring != NULL);

    cell_monitoring->segment_0 = App_InRangeCheck_Create(
        App_CellVoltages_GetSegment0Average, min_cell_voltage,
        max_cell_voltage);
    cell_monitoring->segment_1 = App_InRangeCheck_Create(
        App_CellVoltages_GetSegment1Average, min_cell_voltage,
        max_cell_voltage);
    cell_monitoring->segment_2 = App_InRangeCheck_Create(
        App_CellVoltages_GetSegment2Average, min_cell_voltage,
        max_cell_voltage);
    cell_monitoring->segment_3 = App_InRangeCheck_Create(
        App_CellVoltages_GetSegment3Average, min_cell_voltage,
        max_cell_voltage);
    cell_monitoring->segment_4 = App_InRangeCheck_Create(
        App_CellVoltages_GetSegment4Average, min_cell_voltage,
        max_cell_voltage);
    cell_monitoring->segment_5 = App_InRangeCheck_Create(
        App_CellVoltages_GetSegment5Average, min_cell_voltage,
        max_cell_voltage);
    cell_monitoring->pack = App_InRangeCheck_Create(
        App_CellVoltages_GetPack, min_cell_voltage, max_cell_voltage);
    cell_monitoring->min = App_InRangeCheck_Create(
        App_CellVoltages_GetMinCell, min_cell_voltage, max_cell_voltage);
    cell_monitoring->min = App_InRangeCheck_Create(
        App_CellVoltages_GetMaxCell, min_cell_voltage, max_cell_voltage);
    cell_monitoring->average_cell = App_InRangeCheck_Create(
        App_CellVoltages_GetAverageCell, min_cell_voltage, max_cell_voltage);

    return cell_monitoring;
}

struct InRangeCheck *
    App_CellMonitoring_GetSegment0(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->segment_0;
}

struct InRangeCheck *
    App_CellMonitoring_GetSegment1(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->segment_1;
}

struct InRangeCheck *
    App_CellMonitoring_GetSegment2(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->segment_2;
}

struct InRangeCheck *
    App_CellMonitoring_GetSegment3(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->segment_3;
}

struct InRangeCheck *
    App_CellMonitoring_GetSegment4(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->segment_4;
}

struct InRangeCheck *
    App_CellMonitoring_GetSegment5(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->segment_5;
}

struct InRangeCheck *
    App_CellMonitoring_GetPack(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->pack;
}

struct InRangeCheck *
    App_CellMonitoring_GetMin(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->min;
}

struct InRangeCheck *
    App_CellMonitoring_GetMax(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->max;
}

struct InRangeCheck *
    App_CellMonitoring_GetAverageCell(struct CellMonitoring *cell_monitor)
{
    return cell_monitor->average_cell;
}
