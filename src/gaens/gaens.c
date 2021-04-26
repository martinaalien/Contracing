
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "gaens.h"
#include "../ble/advertise.h"

/* Zephyr includes */
#include <logging/log.h>
#include <random/rand32.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME gaens
LOG_MODULE_REGISTER(gaens);

#define AEM_VERSONING      0b01000000 // Defined in GAENS Bluetooth specification
#define AEM_TRANSMIT_POWER 0b0 // Defined in GAENS Bluetooth specification. 0dBm
#define RFU                0b0 // Ready for Future Use

#define LOWER_RANDOM_ROTATION_INTERVAL                                         \
    601 // Lower limit is greater than 10 minutes
#define UPPER_RANDOM_ROTATION_INTERVAL                                         \
    1199 // Upper limit is less than 20 minutes

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static uint32_t ble_addr_change_timestamp = 0;
static uint32_t current_tek_valid_from = 0;
static uint8_t current_tek[TEK_LENGTH] = {0};
static uint8_t current_rpi[RPI_LENGTH] = {0};
static uint8_t current_rpik[RPIK_LENGTH] = {0};
static uint8_t current_aemk[AEMK_LENGTH] = {0};

/* Metadata to be encrypted and advertised */
static uint8_t metadata[AEM_LENGTH] = {RFU, RFU, AEM_TRANSMIT_POWER,
                                       AEM_VERSONING};

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static int _gaens_random_rotation_interval(uint32_t *interval);

static void _rotate_rpi_handler(struct k_work *unused);
K_WORK_DEFINE(_rotate_rpi_work, _rotate_rpi_handler);

static void _rpi_rotation_timer_handler(struct k_timer *unused);
K_TIMER_DEFINE(_rpi_rotation_timer, _rpi_rotation_timer_handler, NULL);

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int gaens_init(void)
{
    // Initialize crypto library
    if (crypto_init() < 0)
    {
        LOG_ERR("Failed to init crypto module");
        return -1;
    }

    // Initiate keys
    if (gaens_update_keys() < 0)
    {
        LOG_ERR("Failed to initiate keys");
        return -1;
    }

    // Initiate RPI
    if (gaens_update_rpi() < 0)
    {
        LOG_ERR("Failed to update initial RPI");
        return -1;
    }

    // Fetch new RPI
    uint8_t rpi[RPI_LENGTH] = {0};
    if (gaens_get_rpi(rpi) < 0)
    {
        LOG_ERR("Failed to fetch initial RPI");
        return -1;
    }

    // Encrypt AEM
    uint8_t aem[AEM_LENGTH];
    if (gaens_encrypt_metadata(metadata, AEM_LENGTH, aem) < 0)
    {
        LOG_ERR("Failed to encrypt initial AEM");
        return -1;
    }

    // Change advertise data
    if (advertise_change_gaens_service_data(rpi, RPI_LENGTH, aem, AEM_LENGTH) <
        0)
    {
        LOG_ERR("Failed to update initial advertise data");
        return -1;
    }

    // Get random rotation interval between 10 and 20 minutes
    uint32_t random_time;
    if (_gaens_random_rotation_interval(&random_time) < 0)
    {
        LOG_ERR("Failed to fetch random rotation interval");
        return -1;
    }

    // Start a timer that will trigger in random_time
    k_timer_start(&_rpi_rotation_timer, K_SECONDS(random_time),
                  K_SECONDS(random_time));

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

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Returns a random time greater than 10 minutes and less
 * than 20 minutes.
 * 
 * @param interval A pointer to the random time interval in seconds.
 * 
 * @return int 0 on success, negative otherwise.
 */
static int _gaens_random_rotation_interval(uint32_t *interval)
{
    uint32_t random_number = 0;

    if (sys_csrand_get(&random_number, sizeof(random_number)) < 0)
    {
        LOG_ERR("Failed to get random number");
        return -1;
    }

    *interval = (random_number % (UPPER_RANDOM_ROTATION_INTERVAL -
                                  LOWER_RANDOM_ROTATION_INTERVAL + 1)) +
                LOWER_RANDOM_ROTATION_INTERVAL;

    return 0;
}

/**
 * @brief Work handler for changing the RPI, AEM and update advertise data. 
 * 
 * @param unused Not in use, but required.
 */
static void _rotate_rpi_handler(struct k_work *unused)
{
    // Stop advertising
    if (advertise_stop() < 0)
    {
        LOG_ERR("Failed to pause the advertising");
    }

    // Stop the timer
    k_timer_stop(&_rpi_rotation_timer);

    // Check if the Temporary Exposure Key has expired
    if (gaens_tek_expired() == 1)
    {
        if (gaens_update_keys() < 0)
        {
            LOG_ERR("Failed to update TEK");
        }
    }

    // Update RPI
    if (gaens_update_rpi() < 0)
    {
        LOG_ERR("Failed to update the RPI");
    }

    // Fetch new RPI
    uint8_t rpi[RPI_LENGTH] = {0};
    if (gaens_get_rpi(rpi) < 0)
    {
        LOG_ERR("Failed to fetch the RPI");
    }

    // Encrypt AEM
    uint8_t aem[AEM_LENGTH];
    if (gaens_encrypt_metadata(metadata, AEM_LENGTH, aem) < 0)
    {
        LOG_ERR("Failed to encrypt the metadata");
    }

    // Change advertise data
    if (advertise_change_gaens_service_data(rpi, RPI_LENGTH, aem, AEM_LENGTH) <
        0)
    {
        LOG_ERR("Failed to change the gaens service data to advertise");
    }

    // Get random rotation interval between 10 and 20 minutes
    uint32_t random_time;
    if (_gaens_random_rotation_interval(&random_time) < 0)
    {
        LOG_ERR("Failed to fetch random rotation interval");
    }

    // Start a timer that will trigger in random_time
    k_timer_start(&_rpi_rotation_timer, K_SECONDS(random_time),
                  K_SECONDS(random_time));

    // Start advertising
    if (advertise_start() < 0)
    {
        LOG_ERR("Failed to resume advertising");
    }

    LOG_INF("Successfully updated RPI and AEM");
}

/**
 * @brief Handler for submitting work for rotating the RPI.
 * 
 * @param unused Not in use, but required.
 */
static void _rpi_rotation_timer_handler(struct k_timer *unused)
{
    k_work_submit(&_rotate_rpi_work);
}