/**
 * @file
 * @note External Memory module
 * 
 * This is a module for communicating with the external NOR flash memory.
 */
#ifndef EXTMEM_H
#define EXTMEM_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define EXTMEM_SUBSECTOR_SIZE 4096    // Size of one subsector in bytes
#define EXTMEM_SECTOR_SIZE    65536   // Size of one sector in bytes
#define EXTMEM_CHIP_SIZE      4194304 // Size of the chip in bytes

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for initializing the external memory.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int extmem_init(void);

/**
 * @brief Function for reading from the external memory.
 * 
 * @param offset Offset (byte aligned) to read.
 * @param buf Offset (byte aligned) to read.
 * @param len Number of bytes to read.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int extmem_read(uint32_t offset, uint8_t buf[], size_t len);

/**
 * @brief Function for writing from the external memory.
 * 
 * @param offset Starting offset for the write.
 * @param data Data to write.
 * @param len Number of bytes to write.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int extmem_write(uint32_t offset, const void *data, size_t len);

/**
 * @brief Function for erasing data on the external memory chip.
 * 
 * @details The size of the area to e erased has to be a multiple of 
 * the EXTMEM_SUBSECTOR_SIZE or EXTMEM_SECTOR_SIZE. To erase the whole
 * memory use EXTMEM_CHIP_SIZE.
 * 
 * @param offset Erase area starting offset.
 * @param size Size of area to be erased.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int extmem_erase(uint32_t offset, size_t size);

#endif // EXTMEM_H