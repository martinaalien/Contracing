////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "storage.h"
#include "extmem.h"
#include <string.h>

/* Zephyr includes */
#include <logging/log.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME storage
LOG_MODULE_REGISTER(storage);

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static uint32_t sequence_number = 0;
static uint32_t memory_offset = 0;

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static void _pack_ens_log_entry(uint8_t buf[], int timestamp,
                                const uint8_t rolling_proximity[], uint8_t rssi,
                                uint8_t tx_power);

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int storage_write_entry(int timestamp, const uint8_t rolling_proximity[],
                        uint8_t rssi, uint8_t tx_power)
{
    uint8_t entry[SIZE_OF_ONE_ENTRY] = {};

    _pack_ens_log_entry(entry, timestamp, rolling_proximity, rssi, tx_power);

    if (extmem_write(memory_offset, entry, SIZE_OF_ONE_ENTRY) != 0)
    {
        LOG_ERR("Failed to write ENS log entry to external memory\n");
        return -1;
    }

    sequence_number += 1;

    // According to the WENS specifications, the sequence number shall
    // roll over when reaching 0xFFFFFF
    if (sequence_number > 0xFFFFFF)
    {
        sequence_number = 0x0;
    }

    memory_offset += SIZE_OF_ONE_ENTRY;

    return 0;
}

int storage_read(uint32_t offset, uint8_t buf[], size_t len)
{
    memset(buf, 0, SIZE_OF_ONE_ENTRY);

    if (extmem_read(offset, buf, SIZE_OF_ONE_ENTRY) != 0)
    {
        LOG_ERR("Failed to read from external memory\n");
        return -1;
    }

    return 0;
}

int storage_delete_all(void)
{
    if (extmem_erase(0, EXTMEM_CHIP_SIZE) != 0)
    {
        LOG_ERR("Failed to erase the whole external memory\n");
        return -1;
    }

    memory_offset = 0;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Private functions
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for packing ENS log data in to an array.
 * 
 * @details The structure of an ENS Record is shown in Table 4.2 in the WENS 
 * specification. The packing of data in this function is according to these 
 * specifications. 
 * 
 * @note: It could be that the endianness needs to change.
 * 
 * @param buf A buffer that will be filled with an ENS record entry.
 * @param timestamp The timestamp of the received advertisement packet.
 * @param rolling_proximity The rolling proximity identifier from the received 
 * advertisement packet.
 * @param rssi The RSSI from the received advertisement packet.
 * @param tx_power The TX power from the received advertisement packet.
 */
static void _pack_ens_log_entry(uint8_t buf[], int timestamp,
                                const uint8_t rolling_proximity[], uint8_t rssi,
                                uint8_t tx_power)
{
    // Only the three least significant bytes of the sequence number is used
    // This is according to the WENS specifications
    buf[0] = sequence_number >> 16;
    buf[1] = sequence_number >> 8;
    buf[2] = sequence_number;

    buf[3] = timestamp >> 24;
    buf[4] = timestamp >> 16;
    buf[5] = timestamp >> 8;
    buf[6] = timestamp;

    // This is the length of the rest of the record in bytes
    buf[7] = 0x00;
    buf[8] = 0x18;

    // This is the ENS-specific data in the LTV structure field
    buf[9] = 0x10;  // The length of the ENS specific data
    buf[10] = 0x00; // The type indicating that this is ENS-specific data
    buf[11] = rolling_proximity[0];
    buf[12] = rolling_proximity[1];
    buf[13] = rolling_proximity[2];
    buf[14] = rolling_proximity[3];
    buf[15] = rolling_proximity[4];
    buf[16] = rolling_proximity[5];
    buf[17] = rolling_proximity[6];
    buf[18] = rolling_proximity[7];
    buf[19] = rolling_proximity[8];
    buf[20] = rolling_proximity[9];
    buf[21] = rolling_proximity[10];
    buf[22] = rolling_proximity[11];
    buf[23] = rolling_proximity[12];
    buf[24] = rolling_proximity[13];
    buf[25] = rolling_proximity[14];
    buf[26] = rolling_proximity[15];

    // This is the RSSI in the LTV structure field
    buf[27] = 0x01; // Length of the RSSI value
    buf[28] = 0x02; // The type indicating that this is the RSSI
    buf[29] = rssi;

    // This is the Tx Power Level in the LTV structure field
    buf[30] = 0x01; // Length of the Tx Power Level value
    buf[31] = 0x03; // The type indicating that this is the Tx Power Level
    buf[32] = tx_power;
}