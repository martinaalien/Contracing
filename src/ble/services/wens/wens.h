/**
 * @file
 * @note WENS library
 * 
 * This is a library for the Wearable Exposure Notification Service (WENS).
 */

#ifndef WENS_H
#define WENS_H

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "stdint.h"

////////////////////////////////////////////////////////////////////////////////
// Type declarations
////////////////////////////////////////////////////////////////////////////////

/* This struct makes up the 16 bits in the WEN features field from Table 4.10 
in the WENS documentation. */
typedef struct
{
    unsigned int multiple_bonds_supported : 1;
    unsigned int self_pause_resume_supported : 1;
    unsigned int self_generation_of_temp_keys : 1;
    unsigned int rfu : 13;
} features_t;

/* This struct is made up of the fields in the WEN Features 
characteristic defined in Table 4.9 in the WENS documentation. */
typedef struct
{
    features_t wen_features;
    uint16_t storage_capacity;
} wen_features_t;

/* This struct is made up of the fields in the WEN Status characteristic
from Table 4.23 in the WENS documentation. */
typedef struct
{
    uint8_t opcode;
    uint8_t parameter[18];
} wen_status_t;

/* This struct is made up of the fields in the ENS Settings 
characteristic defined in Table 4.13 in the WENS documentation. */
typedef struct
{
    uint8_t data_retention;
    uint8_t temp_key_length;
    uint16_t max_key_duration;
    uint8_t ens_adv_length;
    uint8_t max_adv_duration;
    uint8_t scan_on_time;
    uint16_t scan_off_time;
    uint16_t min_adv_interval;
    uint16_t max_adv_interval;
    uint8_t self_pause_resume;
} ens_settings_t;

/* This struct is made up of the fields in the ENS Log characteristic defined 
in Table 4.6 in the WENS documentation. */
typedef struct
{
    unsigned int segmentation : 2;
    unsigned int flags : 6; // Reserved for future use
    uint8_t *ens_payload;
} ens_log_t;

/* This struct is made up of the fields in the ENS Identifier characteristic
defined in Table 4.11 in the WENS documentation. */
typedef struct
{
    uint16_t uuid;
    char version[4];
} ens_identifier_t;

////////////////////////////////////////////////////////////////////////////////
// Function declarations
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function for indicating the WEN Feature characteristic.
 * 
 * @param data The data to indicate.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int wens_features_indicate(wen_features_t data);

/**
 * @brief Function for retreiving the ENS Settings.
 * 
 * @param settings The ENS Settings.
 * 
 * @return int Returns 0 on success, negative otherwise.
 */
int wens_get_ens_settings(ens_settings_t *settings);

/**
 * @brief Function for notifying ENS records on the ENS Log characteristic.
 * 
 * @param record ENS record.
 * 
 * @return int 0 in case of success or negative value in case of error.
 */
int wens_ens_log_notify(ens_log_t record);

/**
 * @brief Function for indicating ENS identifier characteristic.
 * 
 * @param identifier ENS identifier.
 * 
 * @return int 0 in case of success or negative value in case of error.
 */
int wens_ens_identifier_indicate(ens_identifier_t identifier);

/**
 * @brief Function for indicating ENS settings characteristic.
 * 
 * @param settings ENS settings.
 * 
 * @return int 0 in case of success or negative value in case of error.
 */
int wens_ens_settings_indicate(ens_settings_t settings);

/**
 * @brief Function for indicating WEN features characteristic.
 * 
 * @note The data parameter is not supposed to be an uint8_t, but it 
 * is set to that temporarily until more of the RACP is implemented.
 * 
 * @param data RACP data.
 * 
 * @return int 0 in case of success or negative value in case of error.
 */
int wens_racp_indicate(uint8_t data);

/**
 * @brief Function for indicating WEN status characteristic.
 * 
 * @param status WEN status.
 * 
 * @return int 0 in case of success or negative value in case of error.
 */
int wens_status_indicate(wen_status_t status);

#endif // WENS_H