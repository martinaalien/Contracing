////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "advertise.h"
#include "uuid.h"
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

#define UUID16_LENGTH             2
#define GAENS_SERVICE_DATA_LENGTH 22

#define GAENS_ADV_PARAMETERS BT_LE_ADV_PARAM(0, 0x140, 0x1B0, NULL) // 200-270ms
#define WENS_ADV_PARAMETERS  BT_LE_ADV_PARAM(1, 0x640, 0x6E0, NULL) // 1.0-1.1s

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static bool advertise_active = false;

static uint8_t gaens_service_data[UUID16_LENGTH + GAENS_SERVICE_DATA_LENGTH] =
    {};

static const struct bt_data ad_wens[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_WENS_VAL)),
};

static const struct bt_data ad_gaens[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, 0x1A),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_GAENS_VAL)),
    BT_DATA(BT_DATA_SVC_DATA16, gaens_service_data, GAENS_SERVICE_DATA_LENGTH)};

static struct bt_le_ext_adv *adv_set;

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static int bt_ext_advertising_start(struct bt_le_adv_param *param,
                                    const struct bt_data *ad, size_t ad_len);

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int advertise_change_gaens_service_data(uint8_t *rpi, uint8_t rpi_length,
                                        uint8_t *aem, uint8_t aem_length)
{
    uint8_t data[rpi_length + aem_length];

    if (advertise_active)
    {
        LOG_ERR("Cannot change GAENS service data while advertising is active");
        return -1;
    }

    // Copy RPI in to data
    memcpy(data, rpi, rpi_length);

    // Add AEM at the end of data
    memcpy(&data[rpi_length], aem, aem_length);

    // Fill in service data UUID
    gaens_service_data[0] = BT_UUID_GAENS_VAL & 0xFF;
    gaens_service_data[1] = BT_UUID_GAENS_VAL >> 8;

    // Replace old GAENS service data with new data
    memcpy(&gaens_service_data[UUID16_LENGTH], data, GAENS_SERVICE_DATA_LENGTH);

    return 0;
}

int advertise_start()
{
    int err;

    err = bt_ext_advertising_start(GAENS_ADV_PARAMETERS, ad_gaens,
                                   ARRAY_SIZE(ad_gaens));
    if (err)
    {
        LOG_INF("GAENS advertising failed to start (err %d)", err);
        return -1;
    }

    err = bt_le_adv_start(WENS_ADV_PARAMETERS, ad_wens, ARRAY_SIZE(ad_wens),
                          NULL, 0);
    if (err)
    {
        LOG_INF("WENS advertising failed to start (err %d)", err);
        return -1;
    }

    advertise_active = true;

    LOG_INF("Advertising started successfully");

    return 0;
}

int advertise_stop()
{
    int err;

    // Stop advertising
    err = bt_le_ext_adv_stop(adv_set);
    if (err)
    {
        LOG_ERR("Failed to stop GAENS advertising (err %d)", err);
        return -1;
    }

    // Delete extended advertising set to free connection object
    err = bt_le_ext_adv_delete(adv_set);
    if (err)
    {
        LOG_ERR("Failed to delete GAENS advertising set (err %d)", err);
        return -1;
    }

    err = bt_le_adv_stop();
    if (err)
    {
        LOG_ERR("Failed to stop WENS advertising (err %d)", err);
        return -1;
    }

    advertise_active = false;

    LOG_INF("Advertising stopped");
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for starting an extended advertising set
 * 
 * @param param Advertising parameters
 * @param ad Advertising data
 * @param ad_len The length of the advertising data
 * 
 * @return int Returns 0 on success, negative otherwise
 */
static int bt_ext_advertising_start(struct bt_le_adv_param *param,
                                    const struct bt_data *ad, size_t ad_len)
{
    int err;
    struct bt_le_ext_adv_start_param ext_adv_start_param = {0};

    err = bt_le_ext_adv_create(param, NULL, &adv_set);
    if (err)
    {
        LOG_ERR("Could not create %d advertising set (err %d)", param->sid,
                err);
        return err;
    }

    err = bt_le_ext_adv_set_data(adv_set, ad, ad_len, NULL, 0);
    if (err)
    {
        LOG_ERR("Could not set data for %d advertising set (err %d)",
                param->sid, err);
        return err;
    }

    err = bt_le_ext_adv_start(adv_set, &ext_adv_start_param);
    if (err)
    {
        LOG_ERR("Advertising for set %d failed to start (err %d)", param->sid,
                err);
        return err;
    }

    return err;
}