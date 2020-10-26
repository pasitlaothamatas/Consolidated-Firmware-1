#pragma once

#include "assert.h"

#define NUM_OF_CELLS_READ_PER_IC 16U
#define NUM_OF_CELLS_PER_IC 18U
#define NUM_OF_CELLS_PER_ACCUMULATOR \
    (NUM_OF_CELLS_READ_PER_IC * NUM_OF_CELLS_PER_IC)

static_assert(
    NUM_OF_CELLS_READ_PER_IC <= NUM_OF_CELLS_PER_IC,
    "The number of cells read per IC must be less than or equal to the number "
    "of cells per IC.");

enum CellMonitoringICs
{
    CELL_MONITORING_IC_0,
    CELL_MONITORING_IC_1,
    NUM_OF_CELL_MONITORING_ICS
};
