#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

void SPI_Init(void);
void SPI_ChipSelect(bool select);
uint8_t SPI_TransferByte(uint8_t data);
void SPI_Transfer(const uint8_t *tx_data, uint8_t *rx_data, uint16_t len);

#endif // SPI_DRIVER_H_