////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "scan.h"
#include "../gaens/crypto.h"
#include "../records/storage.h"
#include "uuid.h"
#include <stddef.h>
#include <unistd.h>

/* Zephyr includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#include <sys/byteorder.h>
#include <sys/time.h>
#include <sys/util.h>

#include <logging/log.h>
#include <zephyr/types.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME scan
LOG_MODULE_REGISTER(scan);

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
    .interval = 0x3E80, // 10 seconds. This should ideally be 1 minute, but for
                        // some reason zephyr does not allow it.
    .window = 0x01E0,   // 300 milliseconds
};

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static void _scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
                     struct net_buf_simple *buf);

static bool _data_cb(struct bt_data *data, void *rssi_buf);

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

    LOG_INF("Scan parameters changed\n");
}

int scan_start()
{
    int err;

    err = bt_le_scan_start(&scan_param, _scan_cb);
    if (err)
    {
        LOG_ERR("Starting scanning failed (err %d)\n", err);
        return -1;
    }

    scan_active = true;

    LOG_INF("Scanning started\n");
    return 0;
}

int scan_stop()
{
    int err;

    err = bt_le_scan_stop();
    if (err)
    {
        LOG_ERR("Stopping scanning failed (err %d)\n", err);
        return -1;
    }

    scan_active = false;

    LOG_INF("Scanning stopped\n");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The scan callback function which is triggered when a packet is received.
 * 
 * @param addr The Bluetooth address of the device which the packet was received from.
 * @param rssi The RSSI value of the packet.
 * @param adv_type The advertise type.
 * @param buf Buffer for the advertising data.
 */
static void _scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
                     struct net_buf_simple *buf)
{
    if (adv_type == BT_GAP_ADV_TYPE_ADV_SCAN_IND ||
        adv_type == BT_GAP_ADV_TYPE_ADV_NONCONN_IND)
    {
        int8_t rssi_buf[sizeof(rssi)] = {rssi};
        bt_data_parse(buf, _data_cb, rssi_buf);
    }
}

/**
 * @brief Function for parsing the received advertising packet
 * 
 * @param data The advertising data.
 * @param rssi_buf  The RSSI value.
 * @return bool Continue parsing data if true.
 */
static bool _data_cb(struct bt_data *data, void *rssi_buf)
{
    uint16_t u16;
    struct bt_uuid *uuid;
    int8_t *rssi = rssi_buf;
    uint32_t en_interval_number;

    switch (data->type)
    {
    case BT_DATA_UUID16_ALL:
        if (data->data_len % sizeof(uint16_t) != 0U)
        {
            LOG_ERR("Advertisement data malformed\n");
            return true;
        }

        for (int i = 0; i < data->data_len; i += sizeof(uint16_t))
        {
            memcpy(&u16, &data->data[i], sizeof(u16));
            uuid = BT_UUID_DECLARE_16(sys_le16_to_cpu(u16));

            if (bt_uuid_cmp(uuid, BT_UUID_GAENS))
            {
                continue; // Continue searching through the UUID list for
                          // GAENS UUID
            }

            if (!bt_uuid_cmp(uuid, BT_UUID_GAENS))
            {
                return true;
            }
        }

        return false;
    case BT_DATA_SVC_DATA16:
        memcpy(&u16, &data->data[0], sizeof(u16));
        uuid = BT_UUID_DECLARE_16(sys_le16_to_cpu(u16));

        if (bt_uuid_cmp(uuid, BT_UUID_GAENS))
        {
            return true;
        }

        crypto_en_interval_number(&en_interval_number);

        storage_write_entry(en_interval_number, &data->data[2], *rssi);

        return false;
    default:
        return true;
    }

    return true;
}