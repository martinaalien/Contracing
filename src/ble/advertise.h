#ifndef ADVERTISE_H
#define ADVERTISE_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <bluetooth/bluetooth.h>

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for changing the GAENS service data to advertise.
 * 
 * @param rpi A pointer to the Rolling Proximity Identifier.
 * @param rpi_length The length of the RPI.
 * @param aem A pointer to the Associated Encrypted Metadata.
 * @param aem_length The length of the AEM.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int advertise_change_gaens_service_data(uint8_t *rpi, uint8_t rpi_length,
                                        uint8_t *aem, uint8_t aem_length);

/**
 * @brief Function for starting to advertise.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int advertise_start();

/**
 * @brief Function for stopping to advertise.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int advertise_stop();

#endif // ADVERTISE_H