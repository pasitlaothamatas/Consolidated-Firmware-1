#pragma once

#define NUM_OF_LTC6813 1U
#define NUM_CELLS_PER_LTC6813 18U
#define NUM_CELLS_PER_LTC6813_REGISTER_GROUP 3U

#define NUM_OF_TX_BYTES_PER_LTC6813_CMD 4U
#define NUM_OF_BYTES_PER_LTC6813_REGISTER 6U
#define NUM_OF_PEC_BYTES 2U
#define NUM_CELLS_PER_REGISTER_GROUP 3U

#define NUM_CMD_BYTES 4U
#define NUM_OF_RX_BYTES 8U

#define DCP_DISABLED 0U
#define CELL_CH_ALL 0U

// Setting the ADC conversion frequency
#define MD_27KHZ_14KHZ 1U
#define MD_7KHZ_3KHZ 2U

// This is choosing what mode we are operating the ADC with
#define ADCOPT MD_7KHZ_3KHZ
#define REFON 1U
#define DTEN 0U
#define DCP DCP_DISABLED

#define CELL_UNDERVOLTAGE_THRESHOLD 0x4E1
#define CELL_OVERVOLTAGE_THRESHOLD 0x8CA
