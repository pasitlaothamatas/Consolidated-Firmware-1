#include <assert.h>
#include <stdlib.h>
#include "App_SharedSignal.h"

struct Signal
{
    bool is_callback_active;

    // The last time the entry signal was observed to be low, in milliseconds
    uint32_t entry_last_time_low_ms;

    // The last time the entry signal was observed to be high, in milliseconds
    uint32_t entry_last_time_high_ms;

    // The last time the exit signal was observed to be low, in milliseconds
    uint32_t exit_last_time_low_ms;

    // The last time the exit signal was observed to be high, in milliseconds
    uint32_t exit_last_time_high_ms;

    // The function to call to check if this signal entry condition is high
    bool (*is_entry_high)(struct World *);

    // The function to call to check if the signal exit condition is high
    bool (*is_exit_high)(struct World *);

    // The world associated with this signal
    struct World *world;

    // When and how to trigger the callback function for this signal
    struct SignalCallback callback;
};

struct Signal *App_SharedSignal_Create(
    uint32_t initial_time_ms,
    bool (*is_entry_high)(struct World *),
    bool (*is_exit_high)(struct World *),
    struct World *        world,
    struct SignalCallback callback)
{
    struct Signal *signal = malloc(sizeof(struct Signal));
    assert(signal != NULL);

    signal->is_callback_active      = false;
    signal->entry_last_time_low_ms  = initial_time_ms;
    signal->entry_last_time_high_ms = initial_time_ms;
    signal->exit_last_time_low_ms   = initial_time_ms;
    signal->exit_last_time_high_ms  = initial_time_ms;
    signal->is_entry_high           = is_entry_high;
    signal->is_exit_high            = is_exit_high;
    signal->world                   = world;
    signal->callback                = callback;

    return signal;
}

void App_SharedSignal_Destroy(struct Signal *signal)
{
    free(signal);
}

uint32_t App_SharedSignal_GetEntryLastTimeLowMs(const struct Signal *signal)
{
    return signal->entry_last_time_low_ms;
}

uint32_t App_SharedSignal_GetEntryLastTimeHighMs(const struct Signal *signal)
{
    return signal->entry_last_time_high_ms;
}

uint32_t App_SharedSignal_GetExitLastTimeLowMs(const struct Signal *signal)
{
    return signal->exit_last_time_low_ms;
}

uint32_t App_SharedSignal_GetExitLastTimeHighMs(const struct Signal *signal)
{
    return signal->exit_last_time_high_ms;
}

bool App_SharedSignal_IsCallbackActive(const struct Signal *const signal)
{
    return signal->is_callback_active;
}

void App_SharedSignal_Update(struct Signal *signal, uint32_t current_time_ms)
{
    if (signal->is_entry_high(signal->world))
    {
        signal->entry_last_time_high_ms = current_time_ms;
    }
    else
    {
        signal->entry_last_time_low_ms = current_time_ms;
    }

    if (signal->is_exit_high(signal->world))
    {
        signal->exit_last_time_high_ms = current_time_ms;
    }
    else
    {
        signal->exit_last_time_low_ms = current_time_ms;
    }

    if (!signal->is_callback_active)
    {
        const uint32_t entry_time_since_last_low =
            current_time_ms - signal->entry_last_time_low_ms;

        if (entry_time_since_last_low >= signal->callback.entry_high_ms)
        {
            signal->callback.function(signal->world);
            signal->is_callback_active = true;
        }
    }
    else
    {
        const uint32_t exit_time_since_last_low =
            current_time_ms - signal->exit_last_time_low_ms;

        if (exit_time_since_last_low >= signal->callback.exit_high_ms)
        {
            signal->is_callback_active = false;
        }
        else
        {
            signal->callback.function(signal->world);
        }
    }
}
