#ifndef ADVERTISE_H
#define ADVERTISE_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <bluetooth/bluetooth.h>

typedef enum
{
    ADVERTISE_GAENS, // Advertise non-connectable Google/Apple ENS packets
    ADVERTISE_WENS   // Advertise connectable WENS packets
} advertise_packet_type;

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for starting to advertise
 * 
 * @param packet_type The packet type to advertise. ADVERTISE_GAENS
 * and ADVERTISE_WENS are available.
 * 
 * @return int Returns 0 on success, negative otherwise
 */
int advertise_start(advertise_packet_type packet_type);

/**
 * @brief Function for stopping to advertise
 * 
 * @return int Returns 0 on success, negative otherwise
 */
int advertise_stop();

#endif // ADVERTISE_H