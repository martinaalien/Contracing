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
                                const uint8_t gaens_service_data[],
                                uint8_t rssi);

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int storage_write_entry(int timestamp, const uint8_t gaens_service_data[],
                        uint8_t rssi)
{
    uint8_t entry[SIZE_OF_ONE_ENTRY] = {};

    _pack_ens_log_entry(entry, timestamp, gaens_service_data, rssi);

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
 * @param gaens_service_data The rolling proximity identifier and associated
 * encrypted metadata from the received advertisement packet.
 * @param rssi The RSSI from the received advertisement packet.
 */
static void _pack_ens_log_entry(uint8_t buf[], int timestamp,
                                const uint8_t gaens_service_data[],
                                uint8_t rssi)
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
    buf[8] = 0x19;

    // This is the ENS-specific data in the LTV structure field
    buf[9] = 0x10;  // The length of the ENS specific data
    buf[10] = 0x00; // The type indicating that this is ENS-specific data
    buf[11] = gaens_service_data[0];
    buf[12] = gaens_service_data[1];
    buf[13] = gaens_service_data[2];
    buf[14] = gaens_service_data[3];
    buf[15] = gaens_service_data[4];
    buf[16] = gaens_service_data[5];
    buf[17] = gaens_service_data[6];
    buf[18] = gaens_service_data[7];
    buf[19] = gaens_service_data[8];
    buf[20] = gaens_service_data[9];
    buf[21] = gaens_service_data[10];
    buf[22] = gaens_service_data[11];
    buf[23] = gaens_service_data[12];
    buf[24] = gaens_service_data[13];
    buf[25] = gaens_service_data[14];
    buf[26] = gaens_service_data[15];
    buf[27] = gaens_service_data[16];
    buf[28] = gaens_service_data[17];
    buf[29] = gaens_service_data[18];
    buf[30] = gaens_service_data[19];

    // This is the RSSI in the LTV structure field
    buf[31] = 0x01; // Length of the RSSI value
    buf[32] = 0x02; // The type indicating that this is the RSSI
    buf[33] = rssi;
}