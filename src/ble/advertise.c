////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "advertise.h"
#include <stddef.h>

/* Zephyr includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/uuid.h>

#include <logging/log.h>
#include <stddef.h>
#include <sys/util.h>
#include <zephyr/types.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME advertise
LOG_MODULE_REGISTER(advertise);

#define GAENS_SERVICE_DATA_LENGTH 22

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static const uint8_t WENS_FLAGS[] = {(BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)};
static const uint8_t WENS_UUID[] = {
    0x00,
    0xFF}; // WENS UUID. NOTE: This is not the correct UUID, but is just a placeholder until the correct UUID is found.
static const uint8_t GAENS_UUID[] = {0x6F, 0xFD}; // Google/Apple ENS UUID
static const uint8_t GAENS_FLAGS[] = {
    0x1A}; // The flags are defined by the GAENS specification

static bool advertise_active = false;

static uint8_t gaens_service_data[GAENS_SERVICE_DATA_LENGTH] = {};

static const struct bt_data ad_wens[] = {
    BT_DATA(BT_DATA_FLAGS, WENS_FLAGS, sizeof(WENS_FLAGS)),
    BT_DATA(BT_DATA_UUID16_ALL, WENS_UUID, sizeof(WENS_UUID))};

static const struct bt_data ad_gaens[] = {
    BT_DATA(BT_DATA_FLAGS, GAENS_FLAGS, sizeof(GAENS_FLAGS)),
    BT_DATA(BT_DATA_UUID16_ALL, GAENS_UUID, sizeof(GAENS_UUID)),
    BT_DATA(BT_DATA_SVC_DATA16, gaens_service_data, GAENS_SERVICE_DATA_LENGTH)};

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int advertise_start(advertise_packet_type packet_type)
{
    int err;

    /* Start advertising */
    if ((packet_type == ADVERTISE_GAENS) & !advertise_active)
    {
        // NOTE: Fill gaens_service_data with the service data here

        // 1. Fill in the UUID
        gaens_service_data[0] = GAENS_UUID[0];
        gaens_service_data[1] = GAENS_UUID[1];

        // 2. Fill in the Proximity Identifier

        // 3. Fill in the Associated Encrypted Metadata

        err = bt_le_adv_start(BT_LE_ADV_NCONN, ad_gaens, ARRAY_SIZE(ad_gaens),
                              NULL, 0);
    }
    else if ((packet_type == ADVERTISE_WENS) & !advertise_active)
    {
        err = bt_le_adv_start(BT_LE_ADV_CONN, ad_wens, ARRAY_SIZE(ad_wens),
                              NULL, 0);
    }
    else
    {
        LOG_ERR("Invalid advertise packet config (config=%d) or advertising \
                 is already active (active=%d)\n",
                packet_type, advertise_active);
        return -1;
    }

    if (err)
    {
        LOG_ERR("Advertising failed to start (err %d)\n", err);
        return -1;
    }

    advertise_active = true;

    LOG_INF("Advertising started\n");
    return 0;
}

int advertise_stop()
{
    int err;

    // Stop advertising
    err = bt_le_adv_stop();
    if (err)
    {
        LOG_ERR("Advertising failed to stop (err %d)\n", err);
        return -1;
    }

    advertise_active = false;

    LOG_INF("Advertising stopped\n");
    return 0;
}