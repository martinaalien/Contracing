////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "ble.h"
#include "connection.h"

/* Zephyr includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/hci.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <zephyr/types.h>

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int ble_init(void)
{
    int err;

    connection_init();

    /* Initialize the Bluetooth Subsystem */
    err = bt_enable(NULL);
    if (err)
    {
        printk("Bluetooth init failed (err %d)\n", err);
        return 1;
    }

    printk("Bluetooth initialized\n");

    return 0;
}