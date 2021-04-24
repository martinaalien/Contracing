////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "ble/advertise.h"
#include "ble/ble.h"
#include "ble/scan.h"
#include "gaens/crypto.h"
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

    err = crypto_init();
    if (err)
    {
        LOG_ERR("Failed to initialize crypto library");
    }

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
}