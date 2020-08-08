#include <stdbool.h>
#include <assert.h>
#include <stm32f3xx.h>
#include "Io_LTC6813.h"
#include "main.h"
#include "config/Io_LTC6813Configs.h"

// TODO: May need to add a transmit receive command
static uint16_t Io_CalculatePacketErrorCode(size_t num_of_bytes, uint8_t *data);
void            Io_LTC6813_TransmitCommand(uint8_t command[2], uint8_t *rx_data);

void Io_LTC6813_TransmitCommand(uint8_t command[2], uint8_t *rx_data)
{
    // The packet of bytes to be transmitted to the LTC6813
    uint8_t command_bytes[4];

    // Every command written to the LTC6813 are sent in the following order:
    // COMMAND0, COMMAND1, PEC0, PEC1
    command_bytes[0]   = command[0];
    command_bytes[1]   = command[1];
    uint16_t pec_bytes = Io_CalculatePacketErrorCode(2U, command);
    command_bytes[2]   = (uint8_t)(pec_bytes >> 8);
    command_bytes[3]   = (uint8_t)(pec_bytes);

    // TODO: Perform a spi transmit operation to send the command bytes to the
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port,SPI2_NSS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SPI2_NSS_GPIO_Port,SPI2_NSS_Pin, GPIO_PIN_SET);
    // LTC6813 chip
    UNUSED(command_bytes);
}

static uint16_t
    Io_CalculatePacketErrorCode(size_t num_of_bytes, uint8_t *const data)
{
    size_t   lut_index;
    uint32_t pec = 16U;

    for (size_t i = 0U; i < num_of_bytes; i++)
    {
        lut_index = ((pec >> 7) ^ data[i]) & 0xFF;
        pec       = (pec << 8) ^ PEC_15_LUT[lut_index];
    }

    // Set the LSB of the pec to 0
    return (uint16_t)(pec << 1);
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
    uint16_t           cell_voltages[NUM_OF_CELLS_PER_IC];
    uint16_t           pec[LTC6813_NUM_OF_REGISTER_GROUPS];
};

static struct LTC6813 ltc_6813;

void Io_LT6813_Init(SPI_HandleTypeDef *hspi)
{
    assert(hspi != NULL);

    ltc_6813.hspi = hspi;

    // Wake up the SPI device
}

void Io_LTC6813_StartCellADCConversion(void)
{
    // Pack ADCV command into the command format of the LTC6813
    uint8_t command[2];
    command[0] = (uint8_t)ADCV;
    command[1] = (uint8_t)(ADCV >> 8);
}

bool Io_LTC6813_IsAdcReady(void)
{
    uint8_t rx_data = 0xFF;
    uint8_t command[2];

    command[0] = 0x07;
    command[1] = 0x14;

    return (rx_data == 0xFF);
}

void Io_LTC6813_ReadAllCellVoltages(uint8_t *rx_cell_voltages)
{
    uint16_t _command;
    uint8_t  command[2];

    const size_t BYTE_IN_REG = 6U;

    for (size_t cell_register_group = LTC6813_REGISTER_GROUP_A;
         cell_register_group < LTC6813_NUM_OF_REGISTER_GROUPS;
         cell_register_group++)
    {
        switch (cell_register_group)
        {
            case LTC6813_REGISTER_GROUP_A:
                _command = RDCVA;
                break;
            case LTC6813_REGISTER_GROUP_B:
                _command = RDCVB;
                break;
            case LTC6813_REGISTER_GROUP_C:
                _command = RDCVC;
                break;
            case LTC6813_REGISTER_GROUP_D:
                _command = RDCVD;
                break;
            case LTC6813_REGISTER_GROUP_E:
                _command = RDCVE;
                break;
            case LTC6813_REGISTER_GROUP_F:
                _command = RDCVF;
                break;
            default:
                break;
        }

        command[0] = (uint8_t)_command;
        command[1] = (uint8_t)(_command >> 8);

        // TODO: The transmit command in the future should have the pointer to
        // rx_cell_voltages passed as a parameter (to read data back)
        Io_TransmitCommand(command);

        size_t pec_error = 0U;

        for (size_t current_ic = 0U; current_ic < NUM_OF_ICS; current_ic++)
        {
            size_t rx_cell_voltages_index = current_ic * NUM_OF_RX_BYTES;

            for (size_t current_cell = 0U;
                 current_cell < NUM_OF_CELLS_PER_REGISTER_GROUP; current_cell++)
            {
                ltc_6813.cell_voltages
                    [current_cell + ((cell_register_group - 1U) *
                                     NUM_OF_CELLS_PER_REGISTER_GROUP)] =
                    (uint16_t)(
                        (rx_cell_voltages[rx_cell_voltages_index] +
                         (rx_cell_voltages[rx_cell_voltages_index + 1] << 8)));

                // An individual cell voltage received is read back as two
                // bytes. Thus, we increment the index by two to get the next
                // cell voltage.
                rx_cell_voltages_index += 2;
            }

            uint16_t rx_pec = (uint16_t)(
                (rx_cell_voltages[rx_cell_voltages_index] << 8) |
                rx_cell_voltages[rx_cell_voltages_index + 1]);
            uint16_t calculated_pec = Io_CalculatePacketErrorCode(
                BYTE_IN_REG, &rx_cell_voltages[current_ic * NUM_OF_RX_BYTES]);

            if (rx_pec != calculated_pec)
            {
                pec_error++;
                ltc_6813.pec[cell_register_group] = true;
            }
            else
            {
                ltc_6813.pec[cell_register_group] = false;
            }

            // TODO: transfer the value of the pec error to a generic function
            // which calls this function
            UNUSED(pec_error);
        }
    }
}
