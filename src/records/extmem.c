////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "extmem.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Zephyr includes */
#include <device.h>
#include <devicetree.h>
#include <drivers/flash.h>
#include <logging/log.h>
#include <zephyr.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME extmem
LOG_MODULE_REGISTER(extmem);

#define FLASH_DEVICE DT_LABEL(DT_INST(0, jedec_spi_nor))

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

const struct device *flash_dev;

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int extmem_init(void)
{
    LOG_INF("Initializing external memory\n");

    flash_dev = device_get_binding(FLASH_DEVICE);

    if (!flash_dev)
    {
        LOG_ERR("External memory driver %s was not found!\n", FLASH_DEVICE);
        return -1;
    }

    LOG_INF("External memory initialized\n");
    return 0;
}

int extmem_read(uint32_t offset, uint8_t buf[], size_t len)
{
    int rc;

    LOG_INF("Attempting to read %u bytes\n", len);
    memset(buf, 0, len);
    rc = flash_read(flash_dev, offset, buf, len);
    if (rc != 0)
    {
        LOG_ERR("Flash read failed! %d\n", rc);
        return -1;
    }

    LOG_INF("Flash read succeeded\n");

    return 0;
}

int extmem_write(uint32_t offset, const void *data, size_t len)
{
    int rc;

    flash_write_protection_set(flash_dev, false);

    LOG_INF("Attempting to write %u bytes\n", len);
    rc = flash_write(flash_dev, offset, data, len);
    if (rc != 0)
    {
        LOG_ERR("Flash write failed! %d\n", rc);
        return -1;
    }

    LOG_INF("Flash write succeeded\n");

    return 0;
}

int extmem_erase(uint32_t offset, size_t size)
{
    int rc;

    flash_write_protection_set(flash_dev, false);

    rc = flash_erase(flash_dev, offset, size);
    if (rc != 0)
    {
        LOG_ERR("Flash erase failed! %d\n", rc);
        return -1;
    }

    LOG_INF("Flash erase succeeded\n");

    return 0;
}