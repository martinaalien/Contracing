////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "bas.h"

/* Zephyr includes */
#include <errno.h>
#include <init.h>
#include <stdbool.h>
#include <sys/__assert.h>
#include <zephyr/types.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/bas.h>
#include <bluetooth/uuid.h>

#include <logging/log.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

LOG_MODULE_REGISTER(bas);

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static uint8_t battery_level = 100U;

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static void blvl_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                 uint16_t value);

static ssize_t read_blvl(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, uint16_t len, uint16_t offset);

////////////////////////////////////////////////////////////////////////////////
// Service define
////////////////////////////////////////////////////////////////////////////////

BT_GATT_SERVICE_DEFINE(bas, BT_GATT_PRIMARY_SERVICE(BT_UUID_BAS),
                       BT_GATT_CHARACTERISTIC(BT_UUID_BAS_BATTERY_LEVEL,
                                              BT_GATT_CHRC_READ |
                                                  BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_READ, read_blvl,
                                              NULL, &battery_level),
                       BT_GATT_CCC(blvl_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

uint8_t bt_bas_get_battery_level(void) { return battery_level; }

int bt_bas_set_battery_level(uint8_t level)
{
    int rc;

    if (level > 100U)
    {
        return -EINVAL;
    }

    battery_level = level;

    rc = bt_gatt_notify(NULL, &bas.attrs[1], &level, sizeof(level));

    return rc == -ENOTCONN ? 0 : rc;
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Battery level CCC config change callback function.
 * 
 * @note The function is called when the Client enables or disables 
 * notifications on the battery level attribute. 
 * 
 * @param attr   The attribute that's changed.
 * @param value  New value.
 */
static void blvl_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                 uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_INF("BAS Notifications %s", notif_enabled ? "enabled" : "disabled");
}

/**
 * @brief Read battery level function.
 * 
 * @param conn Connection object.
 * @param attr Attribute to read.
 * @param buf Buffer to store the value.
 * @param len Buffer length.
 * @param offset Start offset.
 * 
 * @return ssize_t number of bytes read in case of success or negative 
 * values in case of error.
 */
static ssize_t read_blvl(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, uint16_t len, uint16_t offset)
{
    uint8_t lvl8 = battery_level;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &lvl8, sizeof(lvl8));
}