#include <stdlib.h>
#include <assert.h>

#include "App_BmsWorld.h"

struct BmsWorld
{
    struct BmsCanTxInterface *can_tx_interface;
    struct BmsCanRxInterface *can_rx_interface;
    struct Imd *              imd;
    struct HeartbeatMonitor * heartbeat_monitor;
    struct RgbLedSequence *   rgb_led_sequence;
    struct Charger *          charger;
    struct OkStatus *         bms_ok;
    struct OkStatus *         imd_ok;
    struct OkStatus *         bspd_ok;
    struct CellMonitoring *   cell_monitor;
    struct WaitSignal *       cell_monitor_wait_signal;
    struct Clock *            clock;
};

struct BmsWorld *App_BmsWorld_Create(
    struct BmsCanTxInterface *const can_tx_interface,
    struct BmsCanRxInterface *const can_rx_interface,
    struct Imd *const               imd,
    struct HeartbeatMonitor *const  heartbeat_monitor,
    struct RgbLedSequence *const    rgb_led_sequence,
    struct Charger *const           charger,
    struct OkStatus *const          bms_ok,
    struct OkStatus *const          imd_ok,
    struct OkStatus *const          bspd_ok,
    struct CellMonitoring *const    cell_monitor,
    struct Clock *const             clock,

    bool (*is_cell_monitor_asleep)(struct BmsWorld *),
    void (*cell_monitor_callback_complete)(struct BmsWorld *))
{
    struct BmsWorld *world = (struct BmsWorld *)malloc(sizeof(struct BmsWorld));
    assert(world != NULL);

    world->can_tx_interface  = can_tx_interface;
    world->can_rx_interface  = can_rx_interface;
    world->imd               = imd;
    world->heartbeat_monitor = heartbeat_monitor;
    world->rgb_led_sequence  = rgb_led_sequence;
    world->charger           = charger;
    world->bms_ok            = bms_ok;
    world->imd_ok            = imd_ok;
    world->bspd_ok           = bspd_ok;
    world->cell_monitor      = cell_monitor;
    world->clock             = clock;

    struct WaitSignalCallback cell_monitor_callback = {
        .function = cell_monitor_callback_complete, .wait_duration_ms = 100U
    };
    world->cell_monitor_wait_signal = App_SharedWaitSignal_Create(
        0U, is_cell_monitor_asleep, world, cell_monitor_callback);

    return world;
}

void App_BmsWorld_Destroy(struct BmsWorld *world)
{
    free(world);
}

struct BmsCanTxInterface *
    App_BmsWorld_GetCanTx(const struct BmsWorld *const world)
{
    return world->can_tx_interface;
}

struct BmsCanRxInterface *
    App_BmsWorld_GetCanRx(const struct BmsWorld *const world)
{
    return world->can_rx_interface;
}

struct Imd *App_BmsWorld_GetImd(const struct BmsWorld *const world)
{
    return world->imd;
}

struct HeartbeatMonitor *
    App_BmsWorld_GetHeartbeatMonitor(const struct BmsWorld *const world)
{
    return world->heartbeat_monitor;
}

struct RgbLedSequence *
    App_BmsWorld_GetRgbLedSequence(const struct BmsWorld *const world)
{
    return world->rgb_led_sequence;
}

struct Charger *App_BmsWorld_GetCharger(const struct BmsWorld *const world)
{
    return world->charger;
}

struct OkStatus *App_BmsWorld_GetBmsOkStatus(const struct BmsWorld *const world)
{
    return world->bms_ok;
}

struct OkStatus *App_BmsWorld_GetImdOkStatus(const struct BmsWorld *const world)
{
    return world->imd_ok;
}

struct OkStatus *
    App_BmsWorld_GetBspdOkStatus(const struct BmsWorld *const world)
{
    return world->bspd_ok;
}

struct CellMonitoring *
    App_BmsWorld_GetCellMonitor(const struct BmsWorld *const world)
{
    return world->cell_monitor;
}

struct Clock *App_BmsWorld_GetClock(const struct BmsWorld *const world)
{
    return world->clock;
}
