/**
 * @file
 * @brief Bluetooth Low Energy library
 * 
 * This is a library for initializing the BLE communication of the device.
 */

#ifndef BLE_H
#define BLE_H

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for initializing the Bluetooth Subsystem.
 * 
 * @return int Returns 0 on success, negative otherwise
 */
int ble_init(void);

#endif // BLE_H