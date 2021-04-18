#ifndef GAENS_H
#define GAENS_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "crypto.h"
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Length (in bytes) of the associated encrypted metadata as specified 
 * in the GAENS Bluetooth specification.
 */
#define AEM_LENGTH 4

/**
 * @brief Length (in bytes) of the service data to be sent in contact tracing
 * advertising packets as specified in the GAENS Bluetooth specification
 */
#define GAENS_SERVICE_DATA_LENGTH RPI_LENGTH + AEM_LENGTH

////////////////////////////////////////////////////////////////////////////////
// Public function declarations
////////////////////////////////////////////////////////////////////////////////

int gaens_init(void);

/**
 * @brief Get the current rolling proximity identifier (RPI).
 * 
 * @param rpi Pointer to store the current RPI in. Must be of length 
 * RPI_LENGTH
 * @return int 0 on success, negative otherwise
 */
int gaens_get_rpi(uint8_t *rpi);

/**
 * @brief Get the current rolling proximity identifier decrypted
 * 
 * @param dec_rpi Pointer to store the current decrypted RPI in. Must be of 
 * length RPI_LENGTH
 * @return int 0 on success, negative otherwise
 */
int gaens_get_rpi_decrypted(uint8_t *dec_rpi);

/**
 * @brief Get the current temporary exposure key (TEK) and the timestamp from
 * which it is valid.
 * 
 * @param tek Pointer to store the current TEK in. Must be of length RPI_LENGTH
 * @param tek_timestamp Timestamp from which current TEK is valid (in units of
 * 10 minutes)
 * @return int 0 on success, negative otherwise
 */
int gaens_get_tek(uint8_t *tek, uint32_t *tek_timestamp);

/**
 * @brief Derive a new rolling proximity identifier (RPI) and store this for
 * future use. The new RPI can be obtained by calling the function 
 * @c gaens_get_rpi.
 * 
 * @note This function should be called every time the function
 * @c gaens_ble_addr_expired returns 1, which happens once every 10 
 * minutes.
 * 
 * @return int 0 on success, negative otherwise
 */
int gaens_update_rpi(void);

/**
 * @brief Updates the temporary exposure key (TEK), rolling proximity identifier
 * key (RPIK), and associated encrypted metadata key (AEMK). The current TEK and
 * timestamp can be obtained by calling the function @c gaens_get_tek, while
 * the current RPIK and AEMK are internal to this module and cannot be 
 * extracted.
 * 
 * @note This function should be called every time the function 
 * @c gaens_tek_expired returns 1, which happens once every 24 hours.
 * 
 * @return int 0 on success, negative otherwise
 */
int gaens_update_keys(void);

/**
 * @brief Encrypt metadata to be stored in exposure notification 
 * advertisement packets.
 * 
 * @param metadata Pointer to metadata to encrypt
 * @param metadata_len Length of metadata to encrypt (should be @c AEM_LENGTH)
 * @param aem Pointer to store the associated encrypted metadata in (should be
 * of length @c AEM_LENGTH)
 * @return int 0 on success, negative otherwise
 */
int gaens_encrypt_metadata(const uint8_t *metadata, const uint8_t metadata_len,
                           uint8_t *aem);

int gaens_decrypt_metadata(const uint8_t *aem, const uint8_t aem_len, 
                            uint8_t *decrypted_aem);

/**
 * @brief Check if 10 minutes have passed since the last time the Bluetooth
 * address was changed, meaning the address must be changed. 
 * 
 * @note When this function returns 1, the current RPI will not be valid and 
 * thus the function @c gaens_update_rpi must be called in order to create a new
 * valid RPI.
 * 
 * @return int 0 if less than 10 minutes have passed, 1 if 10 minutes or more
 * have passed, negative on error
 */
int gaens_ble_addr_expired(void);

/**
 * @brief Check if temporary exposure key (TEK) has expired, i.e. 24 hours have 
 * passed since the last TEK key was generated.
 * 
 * @note When this function returns 1, the current TEK, RPIK and AEMK are no 
 * longer valid and thus the function @c gaens_update_keys must be called in 
 * order to update these keys.
 * 
 * @return int 0 if TEK has not expired, 1 if TEK has expired, negative on error
 */
int gaens_tek_expired(void);

#endif // GAENS_H