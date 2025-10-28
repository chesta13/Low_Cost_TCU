// mock_modem.c
#include <stdio.h>
#include <string.h>

void Modem_SendCommand(const char *cmd)
{
    printf("[MOCK MODEM CMD]: %s\n", cmd);
}

void Modem_ReadResponse(void)
{
    printf("[MOCK MODEM RESP]: OK\n");
}
