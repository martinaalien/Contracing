////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "ble/advertise.h"
#include "ble/ble.h"
#include "ble/scan.h"
#include "ble/services/wens/wens.h"
#include "records/extmem.h"
#include "records/storage.h"
#include <stdio.h>
#include <string.h>

/* Zephyr includes */
#include <logging/log.h>
#include <zephyr.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME main
LOG_MODULE_REGISTER(main);

////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////

void main(void)
{
    int err;

    LOG_INF("It's alive!\n");

    err = ble_init();
    if (err)
    {
        LOG_ERR("Failed to initialize BLE");
    }

    err = extmem_init();
    if (err)
    {
        LOG_ERR("Failed to initialize external memory");
    }

    err = advertise_start();
    if (err)
    {
        LOG_ERR("Failed to start advertising");
    }

    err = scan_start();
    if (err)
    {
        LOG_ERR("Failed to start scanning");
    }

    // extmem_erase(0, EXTMEM_SUBSECTOR_SIZE);

    // uint8_t rp[16] = {1,2,3,4,5,6,7,8,9,1,2,3,4,5,5, 6};
    // storage_write_entry(0x22223333, rp, 0xaa, 0xdd);
    // storage_write_entry(0x22223333, rp, 0xaa, 0xdd);
    // storage_write_entry(0x22223333, rp, 0xaa, 0xdd);
    // storage_read_block();
}