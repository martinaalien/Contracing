////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "advertise.h"

/* Zephyr includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <zephyr/types.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define DEVICE_NAME     CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static uint8_t mfg_data[] = {0xff, 0xff};

static bool advertise_active = false;

static const struct bt_data ad[] = {
    BT_DATA(BT_DATA_MANUFACTURER_DATA, mfg_data, 3),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int advertise_start()
{
    int err;

    /* Start advertising */
    err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err)
    {
        printk("Advertising failed to start (err %d)\n", err);
        return -1;
    }

    advertise_active = true;

    printk("Advertising started\n");
    return 0;
}

int advertise_stop()
{
    int err;

    /* Stop advertising */
    err = bt_le_adv_stop();
    if (err)
    {
        printk("Advertising failed to stop (err %d)\n", err);
        return -1;
    }

    advertise_active = false;

    printk("Advertising stopped\n");
    return 0;
}