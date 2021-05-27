/**
 * @file
 * @note Storage module
 * 
 * This is a module for adding, reading and manipulating Exposure Notification
 *  records.
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stddef.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define SIZE_OF_ONE_ENTRY 30 // The size of one ENS log entry in bytes

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for writing an ENS log entry to the external memory.
 * 
 * @param timestamp The timestamp of the received advertisement packet.
 * @param gaens_service_data The rolling proximity identifier and associated
 * encrypted metadata from the received advertisement packet.
 * @param rssi The RSSI from the received advertisement packet.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int storage_write_entry(int timestamp, const uint8_t gaens_service_data[],
                        uint8_t rssi);

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