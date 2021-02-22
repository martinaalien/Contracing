#ifndef STORAGE_H
#define STORAGE_H

#include <stddef.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define SIZE_OF_ONE_ENTRY 33 // The size of one ENS log entry in bytes

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for writing an ENS log entry to the external memory.
 * 
 * @param timestamp The timestamp of the received advertisement packet.
 * @param rolling_proximity The rolling proximity identifier from the received advertisement packet.
 * @param rssi The RSSI from the received advertisement packet.
 * @param tx_power The TX power from the received advertisement packet.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int storage_write_entry(int timestamp, uint8_t rolling_proximity[],
                        uint8_t rssi, uint8_t tx_power);

/**
 * @brief Function for reading from the external memory.
 * 
 * @param offset Offset (byte aligned) to read.
 * @param buf Buffer that will be filled with the data that is read.
 * @param len Number of bytes to read.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int storage_read(uint32_t offset, uint8_t buf[], size_t len);

/**
 * @brief Function for erasing the whole external memory.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int storage_delete_all(void);

#endif // STORAGE_H