#include <stddef.h>
#include <assert.h>
#include "App_CanTx.h"
#include "App_FlowMeter.h"

extern struct World *world;

#define MAX_NUM_OF_FLOWMETERS 2

struct FlowMeter
{
    float (*get_flow_rate)(void);
    void (*set_periodic_signal)(
        struct CanTxInterface *can_tx_interface,
        float                  value);
};

struct FlowMeter *App_FlowMeter_Create(
    float (*get_flow_rate)(void),
    void (*set_periodic_signal)(
        struct CanTxInterface *can_tx_interface,
        float                  value))
{
    assert(get_flow_rate != NULL);
    assert(set_periodic_signal != NULL);

    static struct FlowMeter flow_meters[MAX_NUM_OF_FLOWMETERS];
    static size_t           alloc_index = 0;

    assert(alloc_index < MAX_NUM_OF_FLOWMETERS);

    struct FlowMeter *const flow_meter = &flow_meters[alloc_index++];
    flow_meter->get_flow_rate          = get_flow_rate;
    flow_meter->set_periodic_signal    = set_periodic_signal;

    return flow_meter;
}

void App_FlowMeter_Tick(
    struct FlowMeter *primary_flow_meter,
    struct FlowMeter *secondary_flow_meter)
{
    assert(primary_flow_meter != NULL);
    assert(secondary_flow_meter != NULL);

    const float primary_flow_rate   = primary_flow_meter->get_flow_rate();
    const float secondary_flow_rate = secondary_flow_meter->get_flow_rate();

    primary_flow_meter->set_periodic_signal(
        App_SharedWorld_GetCanTx(world), primary_flow_rate);
    secondary_flow_meter->set_periodic_signal(
        App_SharedWorld_GetCanTx(world), secondary_flow_rate);
}
