
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

int gaens_init(void)
{
    if (crypto_init() < 0)
    {
        LOG_ERR("Failed to init crypto module");
        return -1;
    }

    if (gaens_update_keys() < 0)
    {
        LOG_ERR("Failed to initiate keys");
        return -1;
    }

    return 0;
}

int gaens_get_rpi(uint8_t *rpi)
{
    memcpy(rpi, current_rpi, RPI_LENGTH);
    return 0;
}

int gaens_get_rpi_decrypted(uint8_t *dec_rpi)
{
    if (crypto_rpi_decrypt(current_rpik, current_rpi, dec_rpi) < 0)
    {
        LOG_ERR("Failed to decrypt rolling proximity identifier");
        return -1;
    }

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
    uint8_t rpi_copy[RPI_LENGTH];
    memcpy(rpi_copy, current_rpi, RPI_LENGTH);

    if (crypto_aem(current_aemk, rpi_copy, metadata, metadata_len, aem) < 0)
    {
        LOG_ERR("Failed to encrypt metadata");
        return -1;
    }

    return 0;
}

int gaens_decrypt_metadata(const uint8_t *aem, const uint8_t aem_len, 
                            uint8_t *decrypted_aem)
{
    uint8_t rpi_copy[RPI_LENGTH];
    memcpy(rpi_copy, current_rpi, RPI_LENGTH);

    if (crypto_aem_decrypt(aem, aem_len, current_aemk, rpi_copy, 
                           decrypted_aem) < 0)
    {
        LOG_ERR("Failed to decrypt AEM");
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