/**
 * @file
 * @note Battery Service library
 * 
 * This is a library for the BLE Battery Service.
 */

#ifndef BAS_H
#define BAS_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <zephyr/types.h>

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/** 
 * @brief Read battery level value.
 *
 * Read the characteristic value of the battery level
 *
 *  @return The battery level in percent.
 */
uint8_t bt_bas_get_battery_level(void);

/** 
 * @brief Update battery level value.
 *
 * Update the characteristic value of the battery level
 * This will send a GATT notification to all current subscribers.
 *
 *  @param level The battery level in percent.
 *
 *  @return Zero in case of success and error code in case of error.
 */
int bt_bas_set_battery_level(uint8_t level);

#endif // BAS_H
