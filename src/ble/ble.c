////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "ble.h"
#include "../gaens/gaens.h"
#include "advertise.h"
#include "connection.h"
#include "scan.h"
#include "services/bs/bas.h"
#include "services/dis/dis.h"
#include "services/wens/wens.h"
#include <stddef.h>

/* Zephyr includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/hci.h>

#include <logging/log.h>
#include <sys/util.h>
#include <zephyr/types.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME ble
LOG_MODULE_REGISTER(ble);

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int ble_init(void)
{
    int err;

    connection_init();

    err = gaens_init();
    if (err)
    {
        LOG_ERR("Failed to initialize gaens library");
    }

    /* Initialize the Bluetooth Subsystem */
    err = bt_enable(NULL);
    if (err)
    {
        LOG_ERR("Bluetooth init failed (err %d)\n", err);
        return 1;
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

    LOG_INF("Bluetooth initialized\n");

    return 0;
}