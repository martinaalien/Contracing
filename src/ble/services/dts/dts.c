////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "dts.h"
#include "../../uuid.h"

/* Zephyr includes */
#include <bluetooth/gatt.h>
#include <logging/log.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define DEVICE_TIME_FEATURE    "DEVICE TIME FEATURE"    // Temporary placeholder
#define DEVICE_TIME_PARAMETERS "DEVICE TIME PARAMETERS" // Temporary placeholder
#define DEVICE_TIME            "DEVICE TIME"            // Temporary placeholder

LOG_MODULE_REGISTER(dts);

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static ssize_t _read_str(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, uint16_t len, uint16_t offset);

static void _indicate_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                      uint16_t value);

static void _notify_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                    uint16_t value);

static ssize_t _write_control_point(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr,
                                    const void *buf, uint16_t len,
                                    uint16_t offset, uint8_t flags);

static ssize_t _write_racp(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr, const void *buf,
                           uint16_t len, uint16_t offset, uint8_t flags);

////////////////////////////////////////////////////////////////////////////////
// Service define
////////////////////////////////////////////////////////////////////////////////

BT_GATT_SERVICE_DEFINE(
    dts, BT_GATT_PRIMARY_SERVICE(BT_UUID_DTS),
    BT_GATT_CHARACTERISTIC(BT_UUID_DTS_FEATURE, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, _read_str, NULL,
                           DEVICE_TIME_FEATURE),
    BT_GATT_CHARACTERISTIC(BT_UUID_DTS_PARAMETERS, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, _read_str, NULL,
                           DEVICE_TIME_PARAMETERS),
    BT_GATT_CHARACTERISTIC(BT_UUID_DTS_DEVICE_TIME,
                           (BT_GATT_CHRC_READ | BT_GATT_CHRC_INDICATE),
                           BT_GATT_PERM_READ, _read_str, NULL, DEVICE_TIME),
    BT_GATT_CCC(_indicate_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_DTS_CONTROL_POINT,
                           (BT_GATT_CHRC_INDICATE | BT_GATT_CHRC_WRITE),
                           BT_GATT_PERM_WRITE, NULL, _write_control_point,
                           NULL),
    BT_GATT_CCC(_indicate_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_DTS_CHANGE_LOG_DATA, BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE, NULL, NULL, NULL),
    BT_GATT_CCC(_notify_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_DTS_RACP,
                           (BT_GATT_CHRC_INDICATE | BT_GATT_CHRC_WRITE),
                           BT_GATT_PERM_WRITE, NULL, _write_racp, NULL),
    BT_GATT_CCC(_indicate_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Control point write callback function.
 * 
 * @param conn   The connection that is requesting to write.
 * @param attr   The attribute that's being written.
 * @param buf    Buffer with the data to write.
 * @param len    Number of bytes in the buffer.
 * @param offset Offset to start writing from.
 * @param flags  Flags (BT_GATT_WRITE_*).
 * 
 * @return ssize_t Number of bytes written, or in case of an error
 *                 BT_GATT_ERR() with a specific ATT error code.
 */
static ssize_t _write_control_point(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr,
                                    const void *buf, uint16_t len,
                                    uint16_t offset, uint8_t flags)
{
    LOG_INF("Writing to CTS Control Point characteristic");

    // NOTE: Update when Control Point implementation is done.

    return len;
}

/**
 * @brief RACP write callback function.
 * 
 * @param conn   The connection that is requesting to write.
 * @param attr   The attribute that's being written.
 * @param buf    Buffer with the data to write.
 * @param len    Number of bytes in the buffer.
 * @param offset Offset to start writing from.
 * @param flags  Flags (BT_GATT_WRITE_*).
 * 
 * @return ssize_t Number of bytes written, or in case of an error
 *                 BT_GATT_ERR() with a specific ATT error code.
 */
static ssize_t _write_racp(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr, const void *buf,
                           uint16_t len, uint16_t offset, uint8_t flags)
{
    LOG_INF("Writing to DTS RACP characteristic");

    // NOTE: Update when RACP implementation is done.

    return len;
}

/**
 * @brief CCC config change callback function for indications.
 * 
 * @param attr   The attribute that's changed value.
 * @param value  New value.
 */
static void _indicate_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                      uint16_t value)
{
    ARG_UNUSED(attr);

    bool indicate_enabled = (value == BT_GATT_CCC_INDICATE);

    LOG_INF("DTS Indications %s", indicate_enabled ? "enabled" : "disabled");
}

/**
 * @brief CCC config change callback function for notifications.
 * 
 * @param attr   The attribute that's changed value.
 * @param value  New value.
 */
static void _notify_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                    uint16_t value)
{
    ARG_UNUSED(attr);

    bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

    LOG_INF("DTS Notifications %s", notif_enabled ? "enabled" : "disabled");
}

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