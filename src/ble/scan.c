////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "scan.h"

/* Zephyr includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <zephyr/types.h>

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static bool scan_active = false;

////////////////////////////////////////////////////////////////////////////////
// Type declarations
////////////////////////////////////////////////////////////////////////////////

static struct bt_le_scan_param scan_param = {
    .type = BT_HCI_LE_SCAN_PASSIVE,
    .options = BT_LE_SCAN_OPT_NONE,
    .interval = 0x0010,
    .window = 0x0010,
};

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static void _scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
                     struct net_buf_simple *buf);

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

void scan_set_parameters(struct bt_le_scan_param parameters)
{
    if (scan_active)
    {
        scan_stop();
        scan_param = parameters;
        scan_start();
    }
    else
    {
        scan_param = parameters;
    }

    printk("Scan parameters changed\n");
}

int scan_start()
{
    int err;

    err = bt_le_scan_start(&scan_param, _scan_cb);
    if (err)
    {
        printk("Starting scanning failed (err %d)\n", err);
        return -1;
    }

    scan_active = true;

    printk("Scanning started\n");
    return 0;
}

int scan_stop()
{
    int err;

    err = bt_le_scan_stop();
    if (err)
    {
        printk("Starting scanning failed (err %d)\n", err);
        return -1;
    }

    scan_active = false;

    printk("Scanning stopped\n");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

static void _scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
                     struct net_buf_simple *buf)
{
    printk("Scanner found packet!\n");
}