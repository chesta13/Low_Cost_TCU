#include "flash_driver.h"
#include "config.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_flashctl.h>


bool Flash_Init(void)
{
    // Flash controller is initialized via SysConfig
    return true;
}

bool Flash_EraseSector(uint32_t address)
{
    // Unprotect flash
    DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
    
    // Erase sector
    DL_FlashCTL_eraseMemory(FLASHCTL, address, DL_FLASHCTL_COMMAND_SIZE_SECTOR);
    
    // Wait for operation to complete
    while (DL_FlashCTL_getControllerStatus(FLASHCTL) != DL_FLASHCTL_STATUS_IDLE);
    
    return true;
}

bool Flash_WritePage(uint32_t address, const uint8_t *data, uint16_t len)
{
    if (len > FLASH_PAGE_SIZE) {
        return false;
    }
    
    // Unprotect flash
    DL_FlashCTL_unprotectSector(FLASHCTL, address, DL_FLASHCTL_REGION_SELECT_MAIN);
    
    // Write data (must be 64-bit aligned)
    uint32_t *src = (uint32_t*)data;
    uint32_t words = (len + 3) / 4;
    
    for (uint32_t i = 0; i < words; i++) {
        DL_FlashCTL_programMemory32WithECCGenerated(FLASHCTL, address + (i * 4), src[i]);
    }
    
    // Wait for completion
    while (DL_FlashCTL_getControllerStatus(FLASHCTL) != DL_FLASHCTL_STATUS_IDLE);
    
    return true;
}

bool Flash_Read(uint32_t address, uint8_t *data, uint16_t len)
{
    uint8_t *flash_ptr = (uint8_t*)address;
    
    for (uint16_t i = 0; i < len; i++) {
        data[i] = flash_ptr[i];
    }
    
    return true;
}

bool Flash_Verify(uint32_t address, const uint8_t *data, uint16_t len)
{
    uint8_t *flash_ptr = (uint8_t*)address;
    
    for (uint16_t i = 0; i < len; i++) {
        if (flash_ptr[i] != data[i]) {
            return false;
        }
    }
    
    return true;
}