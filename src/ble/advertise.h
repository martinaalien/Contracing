#ifndef ADVERTISE_H
#define ADVERTISE_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <bluetooth/bluetooth.h>

typedef enum
{
    ADVERTISE_GAENS,
    ADVERTISE_WENS
} advertise_packet_config;

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for starting to advertise
 * 
 * @param packet_config The packet configuration to advertise. ADVERTISE_GAENS
 * and ADVERTISE_WENS are available.
 * 
 * @return int Returns 0 on success, negative otherwise
 */
int advertise_start(advertise_packet_config packet_config);

/**
 * @brief Function for stopping to advertise
 * 
 * @return int Returns 0 on success, negative otherwise
 */
int advertise_stop();

#endif // ADVERTISE_H