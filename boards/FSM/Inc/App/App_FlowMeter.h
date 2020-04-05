#pragma once

#include <stdint.h>

struct FlowMeter;

/**
 * Create the flow meter to read flow rate from
 * @param get_flow_rate A function pointer to a function that
 * acquires the flow rate from the flow meter
 * @return A pointer to the created flow meter, whose ownership is given to the
 * caller
 */
struct FlowMeter *App_FlowMeter_Create(
    float (*get_flow_rate)(void),
    void (*set_periodic_signal)(
        struct CanTxInterface *can_tx_interface,
        float                  value));

/**
 * Read the flow rate from a flow meter
 * @param flow_meter The flow meter to read flow rate from
 * @return The flow rate of the flow meter (L/min)
 */
void App_FlowMeter_Tick(
    struct FlowMeter *primary_flow_meter,
    struct FlowMeter *secondary_flow_meter);