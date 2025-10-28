#include "i2c_driver.h"
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_i2c.h>

void I2C_Init(void)
{
    SYSCFG_DL_I2C_0_init();
}
