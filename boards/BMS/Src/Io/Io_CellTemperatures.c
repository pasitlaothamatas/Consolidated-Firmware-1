#include <stdint.h>
#include "Io_SharedSpi.h"
#include "Io_LTC6813.h"
#include "Io_CellTemperatures.h"

#include "configs/App_CellConfigs.h"

static uint16_t internal_die_temperatures[NUM_OF_CELL_MONITORING_ICS];

ExitCode Io_CellTemperatures_ReadInternalDieTemperatures(void)
{
    RETURN_IF_EXIT_NOT_OK(Io_LTC6813_EnterReadyState())
    RETURN_IF_EXIT_NOT_OK(Io_LTC6813_StartInternalDieTemperatureConversion())
    RETURN_IF_EXIT_NOT_OK(Io_LTC6813_PollAdcConversion())

    uint8_t
        rx_internal_die_temp[NUM_OF_RX_BYTES * NUM_OF_CELL_MONITORING_ICS] = {
            0
        };
    uint8_t tx_cmd[NUM_OF_CMD_BYTES];

    // The command used to read data from status register A.
    uint32_t RDSTATA = 0x0010;

    tx_cmd[0] = (uint8_t)(RDSTATA >> 8);
    tx_cmd[1] = (uint8_t)(RDSTATA);
    uint16_t tx_cmd_pec15 =
        Io_CalculatePec15(tx_cmd, NUM_OF_PEC15_BYTES_PER_CMD);
    tx_cmd[2] = (uint8_t)(tx_cmd_pec15 >> 8);
    tx_cmd[3] = (uint8_t)(tx_cmd_pec15);

    for (size_t current_ic = 0; current_ic < NUM_OF_CELL_MONITORING_ICS;
         current_ic++)
    {
        if (Io_SharedSpi_TransmitAndReceive(
                Io_LTC6813_GetSpiInterface(), tx_cmd, NUM_OF_CMD_BYTES,
                rx_internal_die_temp,
                NUM_OF_RX_BYTES * NUM_OF_CELL_MONITORING_ICS) != HAL_OK)
        {
            return EXIT_CODE_ERROR;
        }

        // The upper 8 bytes of the internal die temperature is stored in the
        // 3rd index, while the lower bytes are stored in the 2nd index.
        uint32_t internal_die_temp = (uint32_t)(rx_internal_die_temp[2]) |
                                     (uint32_t)((rx_internal_die_temp[3] << 8));
        internal_die_temperatures[current_ic] = (uint16_t)internal_die_temp;

        // The upper 8 bytes of the PEC15 code read back is stored in the 6th
        // index, while the lower bytes are stored in the 7th index.
        uint32_t received_pec15 = (uint32_t)(rx_internal_die_temp[6] << 8) |
                                  (uint32_t)(rx_internal_die_temp[7]);

        // Calculate the PEC15 using the first 6 bytes of data received from the
        // chip.
        uint32_t calculated_pec15 = Io_CalculatePec15(
            &rx_internal_die_temp[current_ic * NUM_OF_RX_BYTES], 6U);
        if (received_pec15 != calculated_pec15)
        {
            return EXIT_CODE_ERROR;
        }
    }

    return EXIT_CODE_OK;
}

uint16_t *Io_CellTemperatures_GetInternalDieTemperatures(void)
{
    return &internal_die_temperatures[0];
}
