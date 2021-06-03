/**
 * @file
 * @brief Time module
 * 
 * This is a module for setting and getting the current time.
 */

#ifndef TIME_H
#define TIME_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Set the current time.
 * 
 * @param current_time Time you want to set.
 *
 * @return int 0 on success, negative otherwise
 */
int set_current_time(uint32_t current_time);

/**
 * @brief Get the current time.
 * 
 * @param time Pointer to store the time in.
 * 
 * @return int 0 on success, negative otherwise
 */
int get_current_time(uint32_t *time);

#endif