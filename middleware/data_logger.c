#include "data_logger.h"
#include "flash_driver.h"
#include "modem_manager.h"
#include "uart_driver.h"
#include "config.h"
#include <string.h>
#include <stdio.h>

static TelemetryEntry_t log_buffer[MAX_LOG_ENTRIES];
static uint32_t log_index = 0;
static uint32_t total_entries = 0;

// Flash storage parameters
static uint32_t flash_write_address = FLASH_LOG_START_ADDR;

void DataLogger_Init(void)
{
    memset(log_buffer, 0, sizeof(log_buffer));
    log_index = 0;
    total_entries = 0;
    
    Flash_Init();
    
    UART_SendString("[LOGGER] Initialized\r\n");
}

void DataLogger_LogCAN(const CAN_Message_t *msg)
{
    if (msg == NULL) {
        return;
    }
    
    // Log CAN message (could store in separate buffer)
    // For now, just count
    total_entries++;
}

void DataLogger_LogTelemetry(const TelemetryEntry_t *entry)
{
    if (entry == NULL) {
        return;
    }
    
    // Add to circular buffer
    memcpy(&log_buffer[log_index], entry, sizeof(TelemetryEntry_t));
    log_index = (log_index + 1) % MAX_LOG_ENTRIES;
    total_entries++;
    
    // Auto-flush when buffer is full
    if (log_index == 0) {
        DataLogger_FlushToFlash();
    }
}

bool DataLogger_FlushToFlash(void)
{
    if (log_index == 0) {
        return true;  // Nothing to flush
    }
    
    UART_SendString("[LOGGER] Flushing to flash...\r\n");
    
    // Write buffer to flash
    uint32_t bytes_to_write = log_index * sizeof(TelemetryEntry_t);
    
    // Check if we need to erase sector
    if ((flash_write_address % FLASH_SECTOR_SIZE) == 0) {
        Flash_EraseSector(flash_write_address);
    }
    
    // Write in pages
    uint8_t *data_ptr = (uint8_t*)log_buffer;
    uint32_t remaining = bytes_to_write;
    
    while (remaining > 0) {
        uint16_t chunk_size = (remaining > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : remaining;
        
        if (!Flash_WritePage(flash_write_address, data_ptr, chunk_size)) {
            UART_SendString("[LOGGER] Flash write error!\r\n");
            return false;
        }
        
        flash_write_address += chunk_size;
        data_ptr += chunk_size;
        remaining -= chunk_size;
        
        // Wrap around if we reach end of log area
        if (flash_write_address >= (FLASH_LOG_START_ADDR + FLASH_LOG_SIZE)) {
            flash_write_address = FLASH_LOG_START_ADDR;
        }
    }
    
    // Clear buffer
    log_index = 0;
    
    UART_SendString("[LOGGER] Flush complete\r\n");
    return true;
}

bool DataLogger_SendToCloud(void)
{
    ModemStatus_t modem_status = Modem_GetStatus();
    
    if (modem_status.state != MODEM_STATE_CONNECTED) {
        UART_SendString("[LOGGER] Modem not connected\r\n");
        return false;
    }
    
    UART_SendString("[LOGGER] Sending data to cloud...\r\n");
    
    // Format data as JSON or binary protocol
    char json_buffer[512];
    int len = 0;
    
    // Example: Send last telemetry entry
    if (log_index > 0) {
        TelemetryEntry_t *entry = &log_buffer[log_index - 1];
        
        len = snprintf(json_buffer, sizeof(json_buffer),
            "{\"ts\":%u,\"lat\":%.6f,\"lon\":%.6f,\"spd\":%u,\"ax\":%d,\"ay\":%d,\"az\":%d,\"vsup\":%u,\"vbat\":%u,\"temp\":%d}",
            entry->timestamp_ms,
            entry->latitude,
            entry->longitude,
            entry->speed_kmh,
            entry->accel_x,
            entry->accel_y,
            entry->accel_z,
            entry->vsup_mV,
            entry->vbat_mV,
            entry->temperature_C
        );
        
        // Send via modem
        if (Modem_SendData((uint8_t*)json_buffer, len)) {
            UART_SendString("[LOGGER] Data sent successfully\r\n");
            return true;
        }
    }
    
    return false;
}

uint32_t DataLogger_GetEntryCount(void)
{
    return total_entries;
}