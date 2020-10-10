#pragma once

enum Segments
{
    SEGMENT_0,
    SEGMENT_1,
    SEGMENT_2,
    SEGMENT_3,
    SEGMENT_4,
    SEGMENT_5,
    NUM_OF_SEGMENTS
};

float App_CellVoltages_GetSegment0Average(void);
float App_CellVoltages_GetSegment1Average(void);
float App_CellVoltages_GetSegment2Average(void);
float App_CellVoltages_GetSegment3Average(void);
float App_CellVoltages_GetSegment4Average(void);
float App_CellVoltages_GetSegment5Average(void);
float App_CellVoltages_GetPack(void);
float App_CellVoltages_GetAverageCell(void);
float App_CellVoltages_GetMinCell(void);
float App_CellVoltages_GetMaxCell(void);
