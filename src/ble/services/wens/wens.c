/**
 * @file
 * @note WENS library
 * 
 * This is a library for the Wearable Exposure Notification Service (WENS).
 */

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "wens.h"
#include "../../uuid.h"
#include <stdint.h>

/* Zephyr includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>

#include <logging/log.h>
#include <zephyr.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME wens
LOG_MODULE_REGISTER(wens);

////////////////////////////////////////////////////////////////////////////////
// Type declarations
////////////////////////////////////////////////////////////////////////////////

/* This enum contains the different values the segmentation field in the
ens_log_t can take. This is defined in Table 4.7 in the WENS documentation. */
typedef enum
{
    SEGMENTATION_COMPLETE,
    SEGMENTATION_FIRST,
    SEGMENTATION_CONTINUATION,
    SEGMENTATION_LAST
} segmentation_t;

/* This struct is made up of the fields in the Temporary Key List 
characteristic defined in Table 4.14 in the WENS documentation. */
typedef struct
{
    uint32_t timestamp;
    uint8_t temporary_key[16];
} temp_key_list_t;

/* This enum is constructed of the opcodes for the RACP characteristic 
from Table 4.19 and 4.20 */
typedef enum
{
    REPORT_STORED_RECORDS,
    DELETE_STORED_RECORDS,
    ABORT_OPERATION,
    REPORT_NUMBER_OF_STORED_RECORDS,
    REPORT_NUMBER_OF_STORED_RECORDS_RESPONSE,
    RACP_RESPONSE_CODE,
    COMBINED_REPORT,
    COMBINED_REPORT_RESPONSE
} racp_opcode_t;

/* This enum is constructed of the opcodes for the WEN Status characteristic 
from Table 4.24 */
typedef enum
{
    // RFU = 0X00,
    PAUSE_ENS = 0X01,
    RESUME_ENS,
    CLEAR_ALL_ENS_DATA,
    CLEAR_ENS_ADV_LIST,
    // RFU = 0x05-0x1F
    WEN_STATUS_RESPONSE_CODE = 0X20
    // RFU = 0x21-0xFF
} wen_status_opcode_t;

/* This enum is constructed of the response opcodes for the WEN Status 
characteristic from 4.25 */
typedef enum
{
    SUCCESS,
    OPCODE_NOT_SUPORTED,
    INVALID_OPERAND,
    OPERATION_FAILED
    // RFU = 0X04-0XFF
} response_code_t;

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static ens_log_t records = {0};

static wen_features_t wen_features = {
    .wen_features = {.multiple_bonds_supported = 0x1,
                     .self_pause_resume_supported = 0x1,
                     .self_generation_of_temp_keys = 0x1,
                     .rfu = 0x00},
    .storage_capacity = 0x014D};

static ens_identifier_t ens_identifier = {.uuid = 0xFDF6, .version = "v1.2"};

static ens_settings_t ens_settings = {.data_retention = 0x0E,
                                      .temp_key_length = 0x10,
                                      .max_key_duration = 0x540,
                                      .ens_adv_length = 0x1D,
                                      .max_adv_duration = 0x0A,
                                      .scan_on_time = 0x04,
                                      .scan_off_time = 0x3C,
                                      .min_adv_interval = 0x0140,
                                      .max_adv_interval = 0x01B0,
                                      .self_pause_resume = 0x00};

static temp_key_list_t temp_key_list = {.timestamp = 0x43421277,
                                        .temporary_key = {}};

static wen_status_t wen_status = {.opcode = 0x00, .parameter = {}};

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static ssize_t _read_wen_features(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr, void *buf,
                                  uint16_t len, uint16_t offset);

static ssize_t _read_ens_identifier(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr, void *buf,
                                    uint16_t len, uint16_t offset);

static ssize_t _read_ens_settings(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr, void *buf,
                                  uint16_t len, uint16_t offset);

static ssize_t _write_ens_settings(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   const void *buf, uint16_t len,
                                   uint16_t offset, uint8_t flags);

static ssize_t _read_temporary_key_list(struct bt_conn *conn,
                                        const struct bt_gatt_attr *attr,
                                        void *buf, uint16_t len,
                                        uint16_t offset);

static ssize_t _write_temporary_key_list(struct bt_conn *conn,
                                         const struct bt_gatt_attr *attr,
                                         const void *buf, uint16_t len,
                                         uint16_t offset, uint8_t flags);

static ssize_t _write_racp(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr, const void *buf,
                           uint16_t len, uint16_t offset, uint8_t flags);

static ssize_t _write_wen_status(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 const void *buf, uint16_t len, uint16_t offset,
                                 uint8_t flags);

static void _indicate_cb(struct bt_conn *conn,
                         struct bt_gatt_indicate_params *params, uint8_t err);

static void _indicate_destroy_cb(struct bt_gatt_indicate_params *params);

static void _notify_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                    uint16_t value);

static void _indicate_ccc_cfg_changed(const struct bt_gatt_attr *attr,
                                      uint16_t value);

////////////////////////////////////////////////////////////////////////////////
// Service define
////////////////////////////////////////////////////////////////////////////////

BT_GATT_SERVICE_DEFINE(
    wens_svc, BT_GATT_PRIMARY_SERVICE(BT_UUID_WENS),
    BT_GATT_CHARACTERISTIC(BT_UUID_ENS_LOG, BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE, NULL, NULL, &records),
    BT_GATT_CCC(_notify_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_WEN_FEATURES,
                           (BT_GATT_CHRC_INDICATE | BT_GATT_CHRC_READ),
                           BT_GATT_PERM_READ, _read_wen_features, NULL,
                           &wen_features),
    BT_GATT_CCC(_indicate_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_ENS_IDENTIFIER,
                           (BT_GATT_CHRC_INDICATE | BT_GATT_CHRC_READ),
                           BT_GATT_PERM_READ, _read_ens_identifier, NULL,
                           &ens_identifier),
    BT_GATT_CCC(_indicate_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_ENS_SETTINGS,
                           (BT_GATT_CHRC_INDICATE | BT_GATT_CHRC_READ |
                            BT_GATT_CHRC_WRITE),
                           (BT_GATT_PERM_WRITE | BT_GATT_PERM_READ),
                           _read_ens_settings, _write_ens_settings,
                           &ens_settings),
    BT_GATT_CCC(_indicate_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_TEMPORARY_KEY_LIST,
                           (BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE),
                           (BT_GATT_PERM_WRITE | BT_GATT_PERM_READ),
                           _read_temporary_key_list, _write_temporary_key_list,
                           &temp_key_list),
    BT_GATT_CHARACTERISTIC(BT_UUID_RACP,
                           (BT_GATT_CHRC_INDICATE | BT_GATT_CHRC_WRITE),
                           BT_GATT_PERM_WRITE, NULL, _write_racp, NULL),
    BT_GATT_CCC(_indicate_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_WEN_STATUS,
                           (BT_GATT_CHRC_INDICATE | BT_GATT_CHRC_WRITE),
                           BT_GATT_PERM_WRITE, NULL, _write_wen_status,
                           &wen_status),
    BT_GATT_CCC(_indicate_ccc_cfg_changed,
                BT_GATT_PERM_READ | BT_GATT_PERM_WRITE));

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int wens_get_ens_settings(ens_settings_t *settings)
{
    *settings = ens_settings;
    return 0;
}

int wens_ens_log_notify(ens_log_t record)
{
    LOG_INF("Notifying ENS Record");

    records = record;

    return bt_gatt_notify(NULL, &wens_svc.attrs[2], &record, sizeof(record));
}

int wens_features_indicate(wen_features_t features)
{
    LOG_INF("Indicating WEN Features Characteristic");

    struct bt_gatt_indicate_params ind_params;

    ind_params.attr = &wens_svc.attrs[4];
    ind_params.func = _indicate_cb;
    ind_params.destroy = _indicate_destroy_cb;
    ind_params.data = &features;
    ind_params.len = sizeof(features);

    return bt_gatt_indicate(NULL, &ind_params);
}

int wens_ens_identifier_indicate(ens_identifier_t identifier)
{
    LOG_INF("Indicating ENS Identifier Characteristic");

    struct bt_gatt_indicate_params ind_params;

    ind_params.attr = &wens_svc.attrs[6];
    ind_params.func = _indicate_cb;
    ind_params.destroy = _indicate_destroy_cb;
    ind_params.data = &identifier;
    ind_params.len = sizeof(identifier);

    return bt_gatt_indicate(NULL, &ind_params);
}

int wens_ens_settings_indicate(ens_settings_t settings)
{
    LOG_INF("Indicating ENS Settings Characteristic");

    struct bt_gatt_indicate_params ind_params;

    ind_params.attr = &wens_svc.attrs[8];
    ind_params.func = _indicate_cb;
    ind_params.destroy = _indicate_destroy_cb;
    ind_params.data = &settings;
    ind_params.len = sizeof(settings);

    ens_settings = settings;

    return bt_gatt_indicate(NULL, &ind_params);
}

int wens_racp_indicate(uint8_t data)
{
    LOG_INF("Indicating RACP Characteristic");

    struct bt_gatt_indicate_params ind_params;

    ind_params.attr = &wens_svc.attrs[11];
    ind_params.func = _indicate_cb;
    ind_params.destroy = _indicate_destroy_cb;
    ind_params.data = &data;
    ind_params.len = sizeof(data);

    return bt_gatt_indicate(NULL, &ind_params);
}

int wens_status_indicate(wen_status_t status)
{
    LOG_INF("Indicating ENS Status Characteristic");

    struct bt_gatt_indicate_params ind_params;

    ind_params.attr = &wens_svc.attrs[13];
    ind_params.func = _indicate_cb;
    ind_params.destroy = _indicate_destroy_cb;
    ind_params.data = &status;
    ind_params.len = sizeof(status);

    return bt_gatt_indicate(NULL, &ind_params);
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief WEN features read callback function.
 * 
 * @param conn Connection object.
 * @param attr Attribute to read.
 * @param buf Buffer to store the value read.
 * @param len Buffer length.
 * @param offset Start offset.
 *
 * @return ssize_t number of bytes read in case of success or negative values 
 * in case of error.
 */
static ssize_t _read_wen_features(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr, void *buf,
                                  uint16_t len, uint16_t offset)
{
    LOG_INF("Reading WEN Features characteristic");

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &wen_features,
                             sizeof(wen_features));
}

/**
 * @brief ENS identifier read callback function.
 * 
 * @param conn Connection object.
 * @param attr Attribute to read.
 * @param buf Buffer to store the value read.
 * @param len Buffer length.
 * @param offset Start offset.
 *
 * @return ssize_t number of bytes read in case of success or negative values 
 * in case of error.
 */
static ssize_t _read_ens_identifier(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr, void *buf,
                                    uint16_t len, uint16_t offset)
{
    LOG_INF("Reading ENS identifier characteristic");

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &ens_identifier,
                             sizeof(ens_identifier));
}

/**
 * @brief ENS settings read callback function.
 * 
 * @param conn Connection object.
 * @param attr Attribute to read.
 * @param buf Buffer to store the value read.
 * @param len Buffer length.
 * @param offset Start offset.
 *
 * @return ssize_t number of bytes read in case of success or negative values 
 * in case of error.
 */
static ssize_t _read_ens_settings(struct bt_conn *conn,
                                  const struct bt_gatt_attr *attr, void *buf,
                                  uint16_t len, uint16_t offset)
{
    LOG_INF("Reading ENS settings characteristic");

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &ens_settings,
                             sizeof(ens_settings));
}

/**
 * @brief ENS settings write callback function.
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
static ssize_t _write_ens_settings(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   const void *buf, uint16_t len,
                                   uint16_t offset, uint8_t flags)
{
    LOG_INF("Writing ENS settings characteristic");

    if (len > sizeof(ens_settings))
    {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    memcpy(&ens_settings, buf, len);

    // Update settings function call here

    return len;
}

/**
 * @brief Temporary key list read callback function.
 * 
 * @param conn Connection object.
 * @param attr Attribute to read.
 * @param buf Buffer to store the value read.
 * @param len Buffer length.
 * @param offset Start offset.
 *
 * @return ssize_t number of bytes read in case of success or negative values 
 * in case of error.
 */
static ssize_t _read_temporary_key_list(struct bt_conn *conn,
                                        const struct bt_gatt_attr *attr,
                                        void *buf, uint16_t len,
                                        uint16_t offset)
{
    LOG_INF("Reading temporary key list characteristic");

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &temp_key_list,
                             sizeof(temp_key_list));
}

/**
 * @brief Temporary key list write callback function.
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
static ssize_t _write_temporary_key_list(struct bt_conn *conn,
                                         const struct bt_gatt_attr *attr,
                                         const void *buf, uint16_t len,
                                         uint16_t offset, uint8_t flags)
{
    LOG_INF("Writing temporary key list characteristic");

    if (len > sizeof(temp_key_list))
    {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    memcpy(&temp_key_list, buf, len);
    return len;
}

/**
 * @brief Record Access Control Point (RACP) write callback function.
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
    LOG_INF("Writing RACP characteristic");

    return len;
}

/**
 * @brief WEN status write callback function.
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
static ssize_t _write_wen_status(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 const void *buf, uint16_t len, uint16_t offset,
                                 uint8_t flags)
{
    LOG_INF("Writing WEN status characteristic");

    if (len > sizeof(wen_status))
    {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    memcpy(&wen_status, buf, len);

    return len;
}

/**
 * @brief Indication callback function.
 * 
 * @param conn Connection object.
 * @param params Indication params object.
 * @param err ATT error code.
 */
static void _indicate_cb(struct bt_conn *conn,
                         struct bt_gatt_indicate_params *params, uint8_t err)
{
    LOG_INF("Indication %s\n", err != 0U ? "fail" : "success");
}

/**
 * @brief Indication destroy callback function.
 * 
 * @param params Indication params object.
 */
static void _indicate_destroy_cb(struct bt_gatt_indicate_params *params)
{
    LOG_INF("Indication complete\n");
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

    LOG_INF("WENS Notifications %s", notif_enabled ? "enabled" : "disabled");
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

    LOG_INF("WENS Indications %s", indicate_enabled ? "enabled" : "disabled");
}