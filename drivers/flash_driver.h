#ifndef FLASH_DRIVER_H_
#define FLASH_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#define FLASH_SECTOR_SIZE   4096
#define FLASH_PAGE_SIZE     256

bool Flash_Init(void);
bool Flash_EraseSector(uint32_t address);
bool Flash_WritePage(uint32_t address, const uint8_t *data, uint16_t len);
bool Flash_Read(uint32_t address, uint8_t *data, uint16_t len);
bool Flash_Verify(uint32_t address, const uint8_t *data, uint16_t len);

#endif // FLASH_DRIVER_H_