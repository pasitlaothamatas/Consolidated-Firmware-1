#include <assert.h>
#include <stdlib.h>
#include "Io_LTC6813.h"
#include "config/Io_LTC6813Configs.h"

static uint16_t Io_CalculatePacketErrorCode(uint8_t *data, size_t num_of_bytes);

static void Io_TransmitCommand(
    uint16_t        tx_command,
    uint16_t        size_tx_command,
    struct LTC6813 *ltc_6813);

static void Io_TransmitCommandAndReceiveData(
    uint16_t        tx_command,
    uint8_t *       rx_data,
    uint16_t        size_tx_command,
    uint16_t        size_data_received,
    struct LTC6813 *ltc_6813);

static uint16_t
    Io_CalculatePacketErrorCode(uint8_t *const data, size_t num_of_bytes)
{
    size_t   lut_index;
    uint16_t pec = 16U;

    for (size_t i = 0U; i < num_of_bytes; i++)
    {
        lut_index = ((pec >> 7) ^ data[i]) & 0xFF;
        pec       = (uint16_t)((pec << 8) ^ PEC_15_LUT[lut_index]);
    }

    // Set the LSB of the pec to 0
    return (uint16_t)(pec << 1);
}

static void Io_TransmitCommand(
    uint16_t              tx_command,
    uint16_t              size_tx_command,
    struct LTC6813 *const ltc_6813)
{
    uint8_t _tx_command[4];
    _tx_command[0]           = (uint8_t)tx_command;
    _tx_command[1]           = (uint8_t)(tx_command >> 8);
    uint16_t _tx_command_pec = Io_CalculatePacketErrorCode(_tx_command, 2U);
    _tx_command[2]           = (uint8_t)(_tx_command_pec >> 8);
    _tx_command[3]           = (uint8_t)_tx_command_pec;

    // TODO: Include Io_SharedSpi_TransmitOnly
    UNUSED(size_tx_command);
    UNUSED(ltc_6813);
}

static void Io_TransmitCommandAndReceiveData(
    uint16_t        tx_command,
    uint8_t *       rx_data,
    uint16_t        size_tx_command,
    uint16_t        size_data_received,
    struct LTC6813 *ltc_6813)
{
    uint8_t _tx_command[4];
    _tx_command[0]           = (uint8_t)tx_command;
    _tx_command[1]           = (uint8_t)(tx_command >> 8);
    uint16_t _tx_command_pec = Io_CalculatePacketErrorCode(_tx_command, 2U);
    _tx_command[2]           = (uint8_t)(_tx_command_pec >> 8);
    _tx_command[3]           = (uint8_t)(_tx_command_pec);

    // TODO: Include the Io_SharedSpi_MultipleTransmitReceive
    UNUSED(size_data_received);
    UNUSED(size_tx_command);
    UNUSED(rx_data);
    UNUSED(ltc_6813);
}

enum LTC6813_CellVoltageRegisterGroup
{
    LTC6813_REGISTER_GROUP_A,
    LTC6813_REGISTER_GROUP_B,
    LTC6813_REGISTER_GROUP_C,
    LTC6813_REGISTER_GROUP_D,
    LTC6813_REGISTER_GROUP_E,
    LTC6813_REGISTER_GROUP_F,
    LTC6813_NUM_OF_REGISTER_GROUPS
};

struct LTC6813
{
    SPI_HandleTypeDef *hspi;
    uint16_t           cell_voltages[NUM_OF_CELLS_PER_LTC6813];
};

struct LTC6813 *Io_LTC6813_Create(SPI_HandleTypeDef *hspi)
{
    assert(hspi != NULL);

    struct LTC6813 *ltc6813 = malloc(sizeof(struct LTC6813));
    ltc6813->hspi           = hspi;

    return ltc6813;
}

void Io_LTC6813_StartCellADCConversion(struct LTC6813 *ltc_6813)
{
    Io_TransmitCommand(ADCV, NUM_OF_TX_BYTES_PER_LTC6813_CMD, ltc_6813);
}

bool Io_LTC6813_IsAdcReady(struct LTC6813 *ltc_6813)
{
    uint8_t rx_data;
    Io_TransmitCommandAndReceiveData(
        PLADC, &rx_data, NUM_OF_TX_BYTES_PER_LTC6813_CMD, 1U, ltc_6813);

    return (rx_data > 0U);
}

void Io_LTC6813_Configure(struct LTC6813 *ltc_6813)
{
    static const uint8_t DEFAULT_CFG_REGISTER[4] = {
        (uint8_t)(REFON << 2) + (uint8_t)(DTEN << 1) + ADCOPT,
        (uint8_t)(CELL_UNDERVOLTAGE_THRESHOLD),
        (uint8_t)((CELL_OVERVOLTAGE_THRESHOLD & 0xF) << 4) +
            (uint8_t)(CELL_UNDERVOLTAGE_THRESHOLD >> 8),
        (uint8_t)(CELL_OVERVOLTAGE_THRESHOLD >> 4)
    };

    // Set the command to be transmitted
    uint8_t tx_command[4];
    tx_command[0]           = (uint8_t)(WRCFG >> 8);
    tx_command[1]           = (uint8_t)WRCFG;
    uint16_t tx_command_pec = Io_CalculatePacketErrorCode(tx_command, 2U);
    tx_command[2]           = (uint8_t)(tx_command_pec >> 8);
    tx_command[3]           = (uint8_t)tx_command_pec;

    uint8_t cfgra_data[TOTAL_NUM_OF_PAYLOAD_BYTES_LTC6813];

    for (size_t current_ltc6813 = TOTAL_NUM_OF_LTC6813; current_ltc6813 > 0;
         current_ltc6813--)
    {
        // The first payload data written is received by the last LTC6813 in the
        // daisy chain
        // The value 8 being the number of payload bytes
        size_t cfgra_data_index =
            current_ltc6813 *
            (NUM_OF_BYTES_PER_LTC6813_REGISTER * NUM_OF_PEC_BYTES);

        cfgra_data[cfgra_data_index]   = DEFAULT_CFG_REGISTER[0];
        cfgra_data[++cfgra_data_index] = DEFAULT_CFG_REGISTER[1];
        cfgra_data[++cfgra_data_index] = DEFAULT_CFG_REGISTER[2];
        cfgra_data[++cfgra_data_index] = DEFAULT_CFG_REGISTER[3];
        cfgra_data[++cfgra_data_index] = 0U;
        cfgra_data[++cfgra_data_index] = 0U;

        uint16_t payload_data_pec = Io_CalculatePacketErrorCode(
            cfgra_data, TOTAL_NUM_OF_PAYLOAD_BYTES_LTC6813);
        cfgra_data[cfgra_data_index] = (uint8_t)(payload_data_pec >> 8);
        cfgra_data[cfgra_data_index] = (uint8_t)payload_data_pec;
    }
    // TODO: Consider creating a more general function for this
    // HAL_SPI_Transmit for the commands used above
    UNUSED(ltc_6813);
}

void Io_LTC6813_ReadAllCellVoltages(
    uint8_t *             rx_cell_voltages,
    struct LTC6813 *const ltc_6813)
{
    uint16_t tx_command;

    for (size_t cell_register_group = LTC6813_REGISTER_GROUP_A;
         cell_register_group < LTC6813_NUM_OF_REGISTER_GROUPS;
         cell_register_group++)
    {
        switch (cell_register_group)
        {
            case LTC6813_REGISTER_GROUP_A:
                tx_command = RDCVA;
                break;
            case LTC6813_REGISTER_GROUP_B:
                tx_command = RDCVB;
                break;
            case LTC6813_REGISTER_GROUP_C:
                tx_command = RDCVC;
                break;
            case LTC6813_REGISTER_GROUP_D:
                tx_command = RDCVD;
                break;
            case LTC6813_REGISTER_GROUP_E:
                tx_command = RDCVE;
                break;
            case LTC6813_REGISTER_GROUP_F:
                tx_command = RDCVF;
                break;
            default:
                break;
        }

        Io_TransmitCommandAndReceiveData(
            tx_command, rx_cell_voltages, NUM_OF_TX_BYTES_PER_LTC6813_CMD,
            NUM_OF_BYTES_PER_LTC6813_REGISTER + NUM_OF_PEC_BYTES, ltc_6813);

        for (size_t current_ltc_6813 = 0U;
             current_ltc_6813 < TOTAL_NUM_OF_LTC6813; current_ltc_6813++)
        {
            size_t rx_cell_voltages_index = current_ltc_6813 * NUM_OF_RX_BYTES;
            for (size_t current_cell = 0U;
                 current_cell < NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP;
                 current_cell++)
            {
                ltc_6813->cell_voltages
                    [current_cell + ((cell_register_group - 1U) *
                                     NUM_OF_CELLS_PER_LTC6813_REGISTER_GROUP)] =
                    (uint16_t)(
                        (rx_cell_voltages[rx_cell_voltages_index] +
                         (rx_cell_voltages[rx_cell_voltages_index + 1] << 8)));

                // An individual cell voltage received is read back as two
                // bytes. Thus, we increment the index by two to get the next
                // cell voltage.
                rx_cell_voltages_index += 2;
            }

            uint16_t pec_received = (uint16_t)(
                (rx_cell_voltages[rx_cell_voltages_index] << 8) |
                rx_cell_voltages[rx_cell_voltages_index + 1]);
            uint16_t pec_calculated = Io_CalculatePacketErrorCode(
                &rx_cell_voltages[current_ltc_6813 * NUM_OF_RX_BYTES],
                NUM_OF_BYTES_PER_LTC6813_REGISTER);
            if (pec_received != pec_calculated)
            {
                // Do something
            }
        }
    }
}
