/**
 * @file
 * @note The main module
 */

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "ble/advertise.h"
#include "ble/ble.h"
#include "ble/scan.h"
#include "ble/services/wens/wens.h"
#include "gaens/crypto.h"
#include "gaens/gaens.h"
#include "records/extmem.h"
#include "records/storage.h"
#include "time/time.h"
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

    err = gaens_init();
    if (err)
    {
        LOG_ERR("Failed to initialize gaens library");
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