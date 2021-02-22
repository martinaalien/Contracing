////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "connection.h"
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

#define LOG_MODULE_NAME connection
LOG_MODULE_REGISTER(connection);

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

/* BLE connection */
static struct bt_conn *conn;

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static void _connected(struct bt_conn *connected, uint8_t err);
static void _disconnected(struct bt_conn *disconn, uint8_t reason);

////////////////////////////////////////////////////////////////////////////////
// Type declarations
///////////////////////////////////////////////////////////////////////////////

static struct bt_conn_cb conn_callbacks = {
    .connected = _connected,
    .disconnected = _disconnected,
};

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

void connection_init() { bt_conn_cb_register(&conn_callbacks); }

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

static void _connected(struct bt_conn *connected, uint8_t err)
{
    if (err)
    {
        LOG_ERR("Connection failed (err %u)", err);
    }
    else
    {
        LOG_INF("Connected");

        if (!conn)
        {
            conn = bt_conn_ref(connected);
        }
    }
}

static void _disconnected(struct bt_conn *disconn, uint8_t reason)
{
    if (conn)
    {
        bt_conn_unref(conn);
        conn = NULL;
    }

    LOG_INF("Disconnected (reason %u)", reason);
}