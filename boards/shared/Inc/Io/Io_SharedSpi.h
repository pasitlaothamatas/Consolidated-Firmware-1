#pragma once

#include <stm32f3xx_hal.h>

struct SharedSpi;

/**
 * Allocate and initialize a SPI interface
 * @param hspi The handle of the given SPI interface
 * @param nss_port Pointer to the GPIO port of the given SPI interface
 * @param nss_pin The GPIO pin of the given SPI interface
 * @return Pointer to the allocated and initialized SPI interface
 */
struct SharedSpi *Io_SharedSpi_Create(
    SPI_HandleTypeDef *hspi,
    GPIO_TypeDef *     nss_port,
    uint16_t           nss_pin);

/**
 * Set the NSS pin low for the given SPI interface
 * @param spi The given SPI interface
 */
void Io_SharedSpi_SetNssLow(const struct SharedSpi *spi);

/**
 * Set the NSS pin high for the given SPI interface
 * @param spi The given SPI interface
 */
void Io_SharedSpi_SetNssHigh(const struct SharedSpi *spi);

/**
 * Transmit and receive data to the device connected to the given SPI interface
 * @param spi The given SPI interface
 * @param tx_data The data transmitted to the device connected to the SPI
 * interface
 * @param tx_size The size of the data transmitted to the device connected to
 * the SPI interface
 * @param rx_data The data received from the device connected ot the SPI
 * interface
 * @param rx_size The size of the data received from the device connected to the
 * SPI interface
 * @return The HAL status of the data transmission and reception
 */
HAL_StatusTypeDef Io_SharedSpi_TransmitAndReceive(
    const struct SharedSpi *spi,
    uint8_t *               tx_data,
    uint16_t                tx_size,
    uint8_t *               rx_data,
    uint16_t                rx_size);

/**
 * Transmit data to the device connected to the given SPI interface
 * @param spi The given SPI interface
 * @param tx_data The data transmitted to the device connected to the SPI
 * interface
 * @param tx_size The size of the data transmitted to the device connected to
 * the SPI interface
 * @return The HAL status of the data transmission
 */
HAL_StatusTypeDef Io_SharedSpi_Transmit(
    const struct SharedSpi *spi,
    uint8_t *               tx_data,
    uint16_t                tx_size);

/**
 * Receive data from the device connected to the given SPI interface
 * @param spi The given SPI interface
 * @param rx_data The data received from the device connected to the SPI
 * interface
 * @param rx_size The size of the data received from the device connected to the
 * SPI interface
 * @return The HAL status of the data reception
 */
HAL_StatusTypeDef Io_SharedSpi_Receive(
    const struct SharedSpi *spi,
    uint8_t *               rx_data,
    uint16_t                rx_size);

/**
 * Transmit multiple copies of the data packet to the device connected to the
 * given SPI interface without toggling the NSS pin
 * @param spi The given SPI interface
 * @param tx_data The data transmitted to the device connected to the SPI
 * interface
 * @param tx_size The size of the data transmitted to the device connected to
 * the SPI interface
 * @param num_tx_data_copies The number of copies of tx_data transmitted to the
 * device connected to the SPI interface
 * @return The HAL status of the data transmission
 */
HAL_StatusTypeDef Io_SharedSpi_MultipleTransmitWithoutNssToggle(
    const struct SharedSpi *const spi,
    uint8_t *                     tx_data,
    uint16_t                      tx_size,
    size_t                        num_tx_data_copies);

/**
 * Transmit data to the device connected to the given SPI interface without
 * toggling the NSS pin
 * @param spi The given SPI interface
 * @param tx_data The data transmitted to the device connected to the SPI
 * interface
 * @param tx_size The size of the data transmitted to the device connected to
 * the SPI interface
 * @return The HAL status of the data transmission
 */
HAL_StatusTypeDef Io_SharedSpi_TransmitWithoutNssToggle(
    const struct SharedSpi *const spi,
    uint8_t *                     tx_data,
    uint16_t                      tx_size);