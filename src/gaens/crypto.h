/**
 * @file
 * @note Cryptography module
 * 
 * This is a module for handling GAENS cryptography.
 */

#ifndef CRYPTO_H
#define CRYPTO_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief The rolling period for changing Temporary Exposure Keys in units of
 * 10 minutes. 144 therefore represents represents 24 hours.
 */
#define TEK_ROLLING_PERIOD 144

/**
 * @brief Length of a Temporary Exposure Key as specified in GAENS cryptography
 * specifications (in bytes).
 */
#define TEK_LENGTH 16

/**
 * @brief Length of a Rolling Proximity Identifier Key as specified in GAENS
 * cryptograhy specifications (in bytes). 
 */
#define RPIK_LENGTH 16

/**
 * @brief Length of a Rolling Proximity Identifier as specified in GAENS
 * cryptography specifications (in bytes).
 */
#define RPI_LENGTH 16

/**
 * @brief Length of an Associated Encrypted Metadata Key as specified in GAENS
 * cryptography specifications (in bytes).
 */
#define AEMK_LENGTH 16

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Initialize the crypto library. This initializes the keys used in the
 * two AES encryptions for the RPI and AEM.
 * 
 * @return int 0 on success, negative otherwise
 */
int crypto_init(void);

/**
 * @brief Generate an Exposure Notification Interval Number. This number 
 * specifies a 10 minute window, meaning each time this number is incremented
 * by 1, 10 minutes have passed.
 * 
 * @param output Pointer to where to store the exposure notification interval
 * number
 * @return int 0 on success
 */
int crypto_en_interval_number(uint32_t *output);

/**
 * @brief Derive a Temporary Exposure Key and timestamp from which the key is
 * valid. The key will be valid from the value specified in @c tek_timestamp
 * and until a full @c TEK_ROLLING_PERIOD has occured.
 * 
 * @param tek Output to store the temporary exposure key
 * @param tek_len Length of the output (should be @c TEK_LENGTH)
 * @param tek_timestamp Timestamp from which @c tek is valid (in units of 10 
 * minutes)
 * @return int 0 on success, negative otherwise 
 */
int crypto_tek(uint8_t *tek, uint8_t tek_len, uint32_t *tek_timestamp);

/**
 * @brief Derive a Rolling Proximity Identifier Key from a Temporary Exposure 
 * Key. 
 * 
 * @param tek Pointer to temporary exposure key
 * @param tek_len Length of @c tek (should be @c TEK_LENGTH)
 * @param rpik Pointer to store rolling proximity identifier key in
 * @param rpik_len Length of output (should be @c RPIK_LENGTH)
 * @return int 0 on success, negative otherwise
 */
int crypto_rpik(const uint8_t *tek, const uint8_t tek_len, uint8_t *rpik,
                const uint8_t rpik_len);

/**
 * @brief Derive a Rolling Proximity Identifier from a Rolling Proximity 
 * Identifier Key.
 * 
 * @param rpik Pointer to rolling proximity identifier key
 * @param rpi Pointer to store rolling proximity identifier key in (must be of
 * the same length as @c rpik)
 * @return int 0 on success, negative otherwise
 */
int crypto_rpi(const uint8_t *rpik, uint8_t *rpi);

/**
 * @brief Decrypt a Rolling Proximity Identifier
 * 
 * @param rpik Pointer to rolling proximity identifier key used to encrypt the
 * RPI
 * @param rpi Pointer to rolling proximity identifier to decrypt
 * @param dec_rpi Pointer to store decrypted RPI in (should be @c RPI_LENGTH)
 * @return int 0 on success, negative otherwise
 */
int crypto_rpi_decrypt(const uint8_t *rpik, const uint8_t *rpi,
                       uint8_t *dec_rpi);

/**
 * @brief Derive Associated Encrypted Metadata Key from a Temporary Exposure
 * Key.
 * 
 * @param tek Pointer to temporary exposure key
 * @param tek_len Length of @c tek (should be @c TEK_LENGTH)
 * @param aemk Pointer to store associated encrypted metadata in
 * @param aemk_len Length of output (should be @c AEMK_LENGTH)
 * @return int 0 on success, negative otherwise
 */
int crypto_aemk(const uint8_t *tek, const uint8_t tek_len, uint8_t *aemk,
                const uint8_t aemk_len);

/**
 * @brief Encrypt bluetooth metadata using an Associated Encrypted Metadata Key.
 * Here the current Rolling Proximity Identifier is used as part of the
 * encryption.
 * 
 * @param aemk Pointer to Associated encrypted metadata key
 * @param rpi Pointer to a copy of current Rolling proximity identifier. It is
 * important that this is a copy of of current RPI as the encryption algorithm
 * may change it. (Should be of length @c RPI_LENGTH)
 * @param bt_metadata Pointer to bluetooth metadata to encrypt
 * @param bt_metadata_len Length of Bluetooth metadata to encrypt
 * @param aem Pointer to store the associated encrypted metadata in (should be
 * of size @c bt_metadata_len)
 * @return int 0 on success, negative otherwise
 */
int crypto_aem(const uint8_t *aemk, uint8_t *rpi, const uint8_t *bt_metadata,
               const uint8_t bt_metadata_len, uint8_t *aem);

/**
 * @brief Decrypt associated encrypted metadata based on a given RPI and AEMK
 * used for encryption.
 * 
 * @param aem Pointer to associated encrypted metadata to encrypt
 * @param aem_len Length of @c aem (should be AEM_LENGTH)
 * @param aemk Pointer to associated encrypted metadata key which was used when
 * encrypting
 * @param rpi Pointer to rolling proximity identifier used when encrypting
 * @param aem_dec Pointer to store decrypted metadata in
 * @return int 0 on success, negative otherwise
 */
int crypto_aem_decrypt(const uint8_t *aem, const uint8_t aem_len,
                       const uint8_t *aemk, uint8_t *rpi, uint8_t *aem_dec);

#endif // CRYPTO_H
