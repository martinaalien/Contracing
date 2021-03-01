
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "crypto.h"

#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <mbedtls/aes.h>
#include <mbedtls/hkdf.h>
#include <mbedtls/md_internal.h>

#include <logging/log.h>
#include <random/rand32.h>
#include <zephyr.h>

#define LOG_MODULE_NAME crypto
LOG_MODULE_REGISTER(crypto);

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

int _hkdf_generate_key(const uint8_t *int_key, const uint8_t int_key_len,
                      uint8_t *info, uint8_t info_len, uint8_t *out_key,
                      const uint8_t out_key_len);

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int crypto_en_interval_number(uint32_t *output)
{
    time_t epoch_timestamp = time(NULL);

    *output = (uint32_t)epoch_timestamp / 600;

    return 0;
}

int crypto_tek(uint8_t *tek, uint8_t tek_len, uint32_t *tek_timestamp)
{
    uint32_t *en_interval_number = 0;
    crypto_en_interval_number(en_interval_number);
    *tek_timestamp =
        (*en_interval_number / TEK_ROLLING_PERIOD) * TEK_ROLLING_PERIOD;

    if (sys_csrand_get(tek, tek_len) < 0)
    {
        LOG_ERR("Failed to generate cryptographically secure random number.");
        return -1;
    }

    return 0;
}

int crypto_rpik(const uint8_t *tek, const uint8_t tek_len, uint8_t *rpik,
               const uint8_t rpik_len)
{
    uint8_t *info = "EN-RPIK";

    if (_hkdf_generate_key(tek, tek_len, info, strlen(info), rpik, rpik_len) !=
        0)
    {
        LOG_ERR("Failed to generate rolling proximity identified key.");
        return -1;
    }

    return 0;
}

int crypto_rpi(const uint8_t *rpik, uint8_t *rpi)
{
    // Create data to be encrypted
    // Format: [<"EN-RPI"><000000000000><EN-INTERVAL-NUM>] (without <,>,")
    uint8_t padded_data[16] = "EN-RPI";
    uint32_t en_in_j;
    crypto_en_interval_number(&en_in_j);
    memcpy(&padded_data[12], &en_in_j, sizeof(en_in_j));

    // Create the encryption key
    mbedtls_aes_context ctx;
    if (mbedtls_aes_setkey_enc(&ctx, rpik, 128) != 0)
    {
        LOG_ERR("Failed to set AES encryption key.");
        return -1;
    }

    // Encrypt data to get rolling proximity identifier
    if (mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, padded_data, rpi) != 0)
    {
        LOG_ERR("Failed to create rolling proximity identifier from AES in "
                "mbedtls.");
        return -1;
    }

    return 0;
}

int crypto_aemk(const uint8_t *tek, const uint8_t tek_len, uint8_t *aemk,
               const uint8_t aemk_len)
{
    uint8_t *info = "EN-AEMK";

    if (_hkdf_generate_key(tek, tek_len, info, strlen(info), aemk, aemk_len) !=
        0)
    {
        LOG_ERR("Failed to generate associated encrypted metadata key.");
        return -1;
    }

    return 0;
}

int crypto_aem(const uint8_t *aemk, uint8_t *rpi, const uint8_t *bt_metadata,
              const uint8_t bt_metadata_len, uint8_t *aem)
{
    // Create the encryption key
    mbedtls_aes_context ctx;
    if (mbedtls_aes_setkey_enc(&ctx, aemk, 128) != 0)
    {
        LOG_ERR("Failed to set AES encryption key.");
        return -1;
    }

    uint8_t stream_block[16]; // Don't understand what this argument is for

    if (mbedtls_aes_crypt_ctr(&ctx, bt_metadata_len, 0, rpi, stream_block,
                              bt_metadata, aem) != 0)
    {
        LOG_ERR("Failed to create associated encrypted metadata from AES-CTR "
                "in mbedtls.");
        return -1;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Generate 16 bytes HKDF key from an input key
 * 
 * @param in_key Pointer to input key
 * @param in_key_len Input key length
 * @param info Pointer to info used in HKDF algorithm
 * @param info_len Length of info
 * @param out_key Pointer to store generated key in
 * @param out_key_len Length of output key (should be same as @c in_key_len)
 * @return int 0 on success, negative otherwise
 */
int _hkdf_generate_key(const uint8_t *in_key, const uint8_t in_key_len,
                      uint8_t *info, uint8_t info_len, uint8_t *out_key,
                      const uint8_t out_key_len)
{
    if (in_key_len != out_key_len)
    {
        LOG_ERR("Invalid arguments. Input key length and output key length "
                "must match.");
        return -1;
    }

    if (mbedtls_hkdf(&mbedtls_sha256_info, NULL, 0, in_key, in_key_len, info,
                     info_len, out_key, out_key_len) < 0)
    {
        LOG_ERR("Failed to generate key from HKDF in mbedtls.");
        return -1;
    }

    return 0;
}