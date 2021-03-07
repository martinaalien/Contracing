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

#endif // WENS_H