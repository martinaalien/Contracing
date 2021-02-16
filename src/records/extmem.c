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
#include <zephyr.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

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
    printk("Initializing external memory\n");

    flash_dev = device_get_binding(FLASH_DEVICE);

    if (!flash_dev)
    {
        printk("External memory driver %s was not found!\n", FLASH_DEVICE);
        return -1;
    }

    printk("External memory initialized\n");
    return 0;
}

int extmem_read(uint32_t offset, uint8_t buf[], size_t len)
{
    int rc;

    printk("Attempting to read %u bytes\n", len);
    memset(buf, 0, len);
    rc = flash_read(flash_dev, offset, buf, len);
    if (rc != 0)
    {
        printk("Flash read failed! %d\n", rc);
        return -1;
    }

    printk("Flash read succeeded\n");

    return 0;
}

int extmem_write(uint32_t offset, const void *data, size_t len)
{
    int rc;

    flash_write_protection_set(flash_dev, false);

    printk("Attempting to write %u bytes\n", len);
    rc = flash_write(flash_dev, offset, data, len);
    if (rc != 0)
    {
        printk("Flash write failed! %d\n", rc);
        return -1;
    }

    printk("Flash write succeeded\n");

    return 0;
}

int extmem_erase(uint32_t offset, size_t size)
{
    int rc;

    flash_write_protection_set(flash_dev, false);

    rc = flash_erase(flash_dev, offset, size);
    if (rc != 0)
    {
        printk("Flash erase failed! %d\n", rc);
        return -1;
    }

    printk("Flash erase succeeded\n");

    return 0;
}