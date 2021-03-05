
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "gaens.h"
#include <logging/log.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME gaens
LOG_MODULE_REGISTER(gaens);

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static uint32_t ble_addr_change_timestamp = 0;
static uint32_t current_tek_valid_from = 0;
static uint8_t current_tek[TEK_LENGTH] = {0};
static uint8_t current_rpi[RPI_LENGTH] = {0};
static uint8_t current_rpik[RPIK_LENGTH] = {0};
static uint8_t current_aemk[AEMK_LENGTH] = {0};

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int gaens_get_rpi(uint8_t *rpi)
{
    memcpy(rpi, current_rpi, RPI_LENGTH);
    return 0;
}

int gaens_get_tek(uint8_t *tek, uint32_t *tek_timestamp)
{
    memcpy(tek, current_tek, TEK_LENGTH);
    *tek_timestamp = current_tek_valid_from;
    return 0;
}

int gaens_update_rpi(void)
{
    if (crypto_rpi(current_rpik, current_rpi) < 0)
    {
        LOG_ERR("Failed to update rolling proximity identifier");
        return -1;
    }

    if (crypto_en_interval_number(&ble_addr_change_timestamp) < 0)
    {
        LOG_ERR("Failed to update the BLE address change timestamp");
        return -1;
    }

    LOG_INF("RPI updated");

    return 0;
}

int gaens_update_keys(void)
{
    if (crypto_tek(current_tek, TEK_LENGTH, &current_tek_valid_from) < 0)
    {
        LOG_ERR("Failed to update temporary exposure key");
        return -1;
    }

    if (crypto_rpik(current_tek, TEK_LENGTH, current_rpik, RPIK_LENGTH) < 0)
    {
        LOG_ERR("Failed to update rolling proximity identifier key");
        return -1;
    }

    if (crypto_aemk(current_tek, TEK_LENGTH, current_aemk, AEMK_LENGTH) < 0)
    {
        LOG_ERR("Failed to update associated encrypted metadata key");
        return -1;
    }

    LOG_INF("TEK, RPIK, AEMK updated");

    return 0;
}

int gaens_encrypt_metadata(const uint8_t *metadata, const uint8_t metadata_len,
                           uint8_t *aem)
{
    if (crypto_aem(current_aemk, current_rpi, metadata, metadata_len, aem) <
        0)
    {
        LOG_ERR("Failed to encrypt metadata");
        return -1;
    }

    return 0;
}

int gaens_ble_addr_expired(void)
{
    uint32_t en_interval_num;
    if (crypto_en_interval_number(&en_interval_num) < 0)
    {
        LOG_ERR("Failed to get exposure notification interval number");
        return -1;
    }

    // When the interval number increases by 1, 10 minutes have passed, i.e. a
    // timeout has occured
    if (en_interval_num > ble_addr_change_timestamp)
    {
        LOG_INF("Current BLE address expired");
        return 1;
    }

    return 0;
}

int gaens_tek_expired(void)
{
    uint32_t en_interval_num;
    if (crypto_en_interval_number(&en_interval_num) < 0)
    {
        LOG_ERR("Failed to get exposure notification interval number");
        return -1;
    }

    if (en_interval_num >= current_tek_valid_from + TEK_ROLLING_PERIOD)
    {
        LOG_INF("Current TEK expired");
        return 1;
    }

    return 0;
}