////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "storage.h"
#include "extmem.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Private variables
////////////////////////////////////////////////////////////////////////////////

static uint32_t sequence_number = 0;
static uint32_t memory_offset = 0;

////////////////////////////////////////////////////////////////////////////////
// Private function declarations
////////////////////////////////////////////////////////////////////////////////

static uint8_t *_pack_ens_log_entry(int timestamp, uint8_t rolling_proximity[],
                                    uint8_t rssi, uint8_t tx_power);

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int storage_write_entry(int timestamp, uint8_t rolling_proximity[],
                        uint8_t rssi, uint8_t tx_power)
{
    uint8_t *entry;

    entry = _pack_ens_log_entry(timestamp, rolling_proximity, rssi, tx_power);

    extmem_write(memory_offset, entry, SIZE_OF_ONE_ENTRY);

    sequence_number += 1;

    memory_offset += SIZE_OF_ONE_ENTRY;

    return 0;
}

int storage_read(uint32_t offset, uint8_t buf[], size_t len)
{
    uint8_t buf[SIZE_OF_ONE_ENTRY];

    memset(buf, 0, SIZE_OF_ONE_ENTRY);

    extmem_read(offset, buf, SIZE_OF_ONE_ENTRY);

    return 0;
}

int storage_delete_all(void)
{
    if (extmem_erase(0, EXTMEM_CHIP_SIZE) != 0)
    {
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
 * @param timestamp The timestamp of the received advertisement packet.
 * @param rolling_proximity The rolling proximity identifier from the received advertisement packet.
 * @param rssi The RSSI from the received advertisement packet.
 * @param tx_power The TX power from the received advertisement packet.
 * 
 * @return uint8_t* An array containing an ENS log entry.
 */
static uint8_t *_pack_ens_log_entry(int timestamp, uint8_t rolling_proximity[],
                                    uint8_t rssi, uint8_t tx_power)
{
    static uint8_t ens_log_entry[SIZE_OF_ONE_ENTRY] = {};

    ens_log_entry[0] = sequence_number >> 16;
    ens_log_entry[1] = sequence_number >> 8;
    ens_log_entry[2] = sequence_number;
    ens_log_entry[3] = timestamp >> 24;
    ens_log_entry[4] = timestamp >> 16;
    ens_log_entry[5] = timestamp >> 8;
    ens_log_entry[6] = timestamp;
    ens_log_entry[7] = 0x2;
    ens_log_entry[8] = 0x1;
    ens_log_entry[9] = 16;
    ens_log_entry[10] = 0x0;
    ens_log_entry[11] = rolling_proximity[0];
    ens_log_entry[12] = rolling_proximity[1];
    ens_log_entry[13] = rolling_proximity[2];
    ens_log_entry[14] = rolling_proximity[3];
    ens_log_entry[15] = rolling_proximity[4];
    ens_log_entry[16] = rolling_proximity[5];
    ens_log_entry[17] = rolling_proximity[6];
    ens_log_entry[18] = rolling_proximity[7];
    ens_log_entry[19] = rolling_proximity[8];
    ens_log_entry[20] = rolling_proximity[9];
    ens_log_entry[21] = rolling_proximity[10];
    ens_log_entry[22] = rolling_proximity[11];
    ens_log_entry[23] = rolling_proximity[12];
    ens_log_entry[24] = rolling_proximity[13];
    ens_log_entry[25] = rolling_proximity[14];
    ens_log_entry[26] = rolling_proximity[15];
    ens_log_entry[27] = 1;
    ens_log_entry[28] = 0x02;
    ens_log_entry[29] = rssi;
    ens_log_entry[30] = 1;
    ens_log_entry[31] = 0x03;
    ens_log_entry[32] = tx_power;

    return ens_log_entry;
}