////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "dis.h"

/* Zephyr includes */
#include <errno.h>
#include <init.h>
#include <stddef.h>
#include <string.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <logging/log.h>
#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <bluetooth/hci.h>
#include <bluetooth/uuid.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define BT_DIS_MODEL_REF      "nRF52833_QIAA"
#define BT_DIS_MANUF_REF      "Manufacturer"
#define BT_DIS_FW_REV_STR_REF "Zephyr Firmware"
#define BT_DIS_HW_REV_STR_REF "Zephyr Hardware"

LOG_MODULE_REGISTER(dis);

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static ssize_t _read_str(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, uint16_t len, uint16_t offset);

////////////////////////////////////////////////////////////////////////////////
// Service define
////////////////////////////////////////////////////////////////////////////////

BT_GATT_SERVICE_DEFINE(
    dis_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_DIS),

    BT_GATT_CHARACTERISTIC(BT_UUID_DIS_MODEL_NUMBER, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, _read_str, NULL,
                           BT_DIS_MODEL_REF),
    BT_GATT_CHARACTERISTIC(BT_UUID_DIS_MANUFACTURER_NAME, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, _read_str, NULL,
                           BT_DIS_MANUF_REF),
    BT_GATT_CHARACTERISTIC(BT_UUID_DIS_FIRMWARE_REVISION, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, _read_str, NULL,
                           BT_DIS_FW_REV_STR_REF),
    BT_GATT_CHARACTERISTIC(BT_UUID_DIS_HARDWARE_REVISION, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, _read_str, NULL,
                           BT_DIS_HW_REV_STR_REF), );

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////'

/**
 * @brief Function for reading the characteristic.
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
static ssize_t _read_str(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, attr->user_data,
                             strlen(attr->user_data));
}