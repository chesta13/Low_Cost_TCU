#include "modem_manager.h"
#include "uart_driver.h"
#include "power_manager.h"
#include "config.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static ModemStatus_t modem_status;
static char rx_buffer[256];
static uint16_t rx_index = 0;

// AT command timeout
#define AT_TIMEOUT_MS  5000

void Modem_Init(void)
{
    memset(&modem_status, 0, sizeof(ModemStatus_t));
    modem_status.state = MODEM_STATE_OFF;
    modem_status.rssi = -999;
}

void Modem_Update(void)
{
    // State machine for modem management
    static uint32_t state_timer = 0;
    char response[128];
    
    switch (modem_status.state) {
        case MODEM_STATE_OFF:
            // Wait for power on command
            break;
            
        case MODEM_STATE_POWERING_ON:
            state_timer++;
            if (state_timer > 5000 / TASK_PERIOD_POWER) {  // 5 seconds
                modem_status.state = MODEM_STATE_INITIALIZING;
                state_timer = 0;
            }
            break;
            
        case MODEM_STATE_INITIALIZING:
            // Send AT command to check if modem is ready
            if (Modem_SendAT("AT", response, sizeof(response), 1000)) {
                if (strstr(response, "OK")) {
                    modem_status.state = MODEM_STATE_READY;
                    UART_SendString("[MODEM] Ready\r\n");
                }
            }
            break;
            
        case MODEM_STATE_READY:
            // Modem ready, waiting for network connection command
            break;
            
        case MODEM_STATE_CONNECTING:
            // Check network registration
            if (Modem_SendAT("AT+CEREG?", response, sizeof(response), 2000)) {
                if (strstr(response, "+CEREG: 0,1") || strstr(response, "+CEREG: 0,5")) {
                    modem_status.state = MODEM_STATE_CONNECTED;
                    modem_status.network_reg = 1;
                    UART_SendString("[MODEM] Network connected\r\n");
                }
            }
            break;
            
        case MODEM_STATE_CONNECTED:
            // Check signal strength periodically
            if (Modem_SendAT("AT+CSQ", response, sizeof(response), 1000)) {
                // Parse response: +CSQ: <rssi>,<ber>
                char *ptr = strstr(response, "+CSQ:");
                if (ptr) {
                    int rssi_raw = atoi(ptr + 6);
                    modem_status.rssi = -113 + (rssi_raw * 2);  // Convert to dBm
                }
            }
            break;
            
        case MODEM_STATE_ERROR:
            // Try to recover
            UART_SendString("[MODEM] Error - attempting reset\r\n");
            Modem_PowerOff();
            modem_status.state = MODEM_STATE_OFF;
            break;
            
        default:
            break;
    }
}

bool Modem_PowerOn(void)
{
    UART_SendString("[MODEM] Powering on...\r\n");
    Power_EnableModem(true);
    modem_status.state = MODEM_STATE_POWERING_ON;
    return true;
}

bool Modem_PowerOff(void)
{
    UART_SendString("[MODEM] Powering off...\r\n");
    Power_EnableModem(false);
    modem_status.state = MODEM_STATE_OFF;
    return true;
}

bool Modem_SendAT(const char *cmd, char *response, uint16_t max_len, uint32_t timeout_ms)
{
    // Clear RX buffer
    rx_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
    
    // Send command
    UART_SendString(cmd);
    UART_SendString("\r\n");
    
    // Wait for response (simplified - should use proper timeout)
    uint32_t start_time = 0;  // Get system tick
    while (start_time < timeout_ms) {
        // Check for received data
        // In real implementation, use UART RX interrupt
        
        if (strstr(rx_buffer, "OK") || strstr(rx_buffer, "ERROR")) {
            if (response) {
                strncpy(response, rx_buffer, max_len);
            }
            return true;
        }
        
        // Increment time
        start_time++;
    }
    
    return false;  // Timeout
}

bool Modem_ConnectNetwork(void)
{
    if (modem_status.state != MODEM_STATE_READY) {
        return false;
    }
    
    UART_SendString("[MODEM] Connecting to network...\r\n");
    modem_status.state = MODEM_STATE_CONNECTING;
    
    // Enable network registration
    char response[64];
    if (!Modem_SendAT("AT+CEREG=1", response, sizeof(response), 2000)) {
        return false;
    }
    
    return true;
}

bool Modem_SendData(const uint8_t *data, uint16_t len)
{
    if (modem_status.state != MODEM_STATE_CONNECTED) {
        return false;
    }
    
    // Send data via TCP/HTTP (implementation depends on protocol)
    modem_status.bytes_sent += len;
    
    return true;
}

bool Modem_GetGPS(GPS_Data_t *gps)
{
    if (gps == NULL) {
        return false;
    }
    
    char response[256];
    
    // Get GPS location (EG800G specific command)
    if (!Modem_SendAT("AT+QGPSLOC=2", response, sizeof(response), 3000)) {
        return false;
    }
    
    // Parse response: +QGPSLOC: <UTC>,<latitude>,<longitude>,<hdop>,<altitude>,<fix>,<cog>,<spkm>,<spkn>,<date>,<nsat>
    char *ptr = strstr(response, "+QGPSLOC:");
    if (ptr) {
        // Simple parsing (should be more robust)
        float lat, lon, alt, speed;
        int fix, nsat;
        
        if (sscanf(ptr + 10, "%*[^,],%f,%f,%*f,%f,%d,%*f,%f,%*[^,],%*[^,],%d",
                   &lat, &lon, &alt, &fix, &speed, &nsat) == 6) {
            gps->latitude = lat;
            gps->longitude = lon;
            gps->altitude = alt;
            gps->speed_kmh = speed;
            gps->satellites = nsat;
            gps->fix_valid = (fix >= 2);
            
            return true;
        }
    }
    
    return false;
}

ModemStatus_t Modem_GetStatus(void)
{
    return modem_status;
}