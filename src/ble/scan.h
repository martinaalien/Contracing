/**
 * @file
 * @brief Bluetooth Low Energy scan module
 * 
 * This is a module for controlling the BLE scanning.
 */

#ifndef SCAN_H
#define SCAN_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

/* Zephyr includes */
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for changing scan parameters
 * 
 * @param parameters Scan parameters
 */
void scan_set_parameters(struct bt_le_scan_param parameters);

/**
 * @brief Function for starting to scan
 * 
 * @return int Returns 0 on success, negative otherwise
 */
int scan_start();

/**
 * @brief Function for stopping to scan
 * 
 * @return int Returns 0 on success, negative otherwise
 */
int scan_stop();

#endif // SCAN_H