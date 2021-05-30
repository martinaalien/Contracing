/**
 * @file
 * @note UUID library
 * 
 * This is a library for UUIDs that is not defined in Zephyr.
 */

#ifndef UUID_H
#define UUID_H

#include <bluetooth/uuid.h>

////////////////////////////////////////////////////////////////////////////////
// Google/Apple Exposure Notification System
////////////////////////////////////////////////////////////////////////////////

#define BT_UUID_GAENS_VAL 0xFD6F
#define BT_UUID_GAENS     BT_UUID_DECLARE_16(BT_UUID_GAENS_VAL)

////////////////////////////////////////////////////////////////////////////////
// Wearable Exposure Notification Service
////////////////////////////////////////////////////////////////////////////////'

////////// Service UUID //////////
#define BT_UUID_WENS_VAL 0xFF00 // NOTE: Temporary
#define BT_UUID_WENS     BT_UUID_DECLARE_16(BT_UUID_WENS_VAL)

////////// Characteristics UUIDs //////////

/* ENS Log */
#define ENS_LOG_UUID    0xFF01 // NOTE: Temporary
#define BT_UUID_ENS_LOG BT_UUID_DECLARE_16(ENS_LOG_UUID)

/* WEN Features */
#define WEN_FEATURES_UUID    0xFF02 // NOTE: Temporary
#define BT_UUID_WEN_FEATURES BT_UUID_DECLARE_16(WEN_FEATURES_UUID)

/* ENS Identifier */
#define ENS_IDENTIFIER_UUID    0xFF03 // NOTE: Temporary
#define BT_UUID_ENS_IDENTIFIER BT_UUID_DECLARE_16(ENS_IDENTIFIER_UUID)

/* ENS Settings */
#define ENS_SETTINGS_UUID    0xFF04 // NOTE: Temporary
#define BT_UUID_ENS_SETTINGS BT_UUID_DECLARE_16(ENS_SETTINGS_UUID)

/* Temporary Key List */
#define TEMPORARY_KEY_LIST_UUID    0xFF05 // NOTE: Temporary
#define BT_UUID_TEMPORARY_KEY_LIST BT_UUID_DECLARE_16(TEMPORARY_KEY_LIST_UUID)

/* Record Access Control Point (RACP) */
#define RACP_UUID    0x2A52 // NOTE: Temporary
#define BT_UUID_RACP BT_UUID_DECLARE_16(RACP_UUID)

/* WEN status */
#define WEN_STATUS_UUID    0xFF06 // NOTE: Temporary
#define BT_UUID_WEN_STATUS BT_UUID_DECLARE_16(WEN_STATUS_UUID)

////////////////////////////////////////////////////////////////////////////////
// Device Time Service
////////////////////////////////////////////////////////////////////////////////

#define BT_UUID_DTS_VALUE 0x1847
#define BT_UUID_DTS       BT_UUID_DECLARE_16(BT_UUID_DTS_VALUE)

#define BT_UUID_DTS_FEATURE_VALUE 0x2B8E
#define BT_UUID_DTS_FEATURE       BT_UUID_DECLARE_16(BT_UUID_DTS_FEATURE_VALUE)

#define BT_UUID_DTS_PARAMETERS_VALUE 0x2B8F
#define BT_UUID_DTS_PARAMETERS       BT_UUID_DECLARE_16(BT_UUID_DTS_PARAMETERS_VALUE)

#define BT_UUID_DTS_DEVICE_TIME_VALUE 0x2B90
#define BT_UUID_DTS_DEVICE_TIME                                                \
    BT_UUID_DECLARE_16(BT_UUID_DTS_DEVICE_TIME_VALUE)

#define BT_UUID_DTS_CONTROL_POINT_VALUE 0x2B91
#define BT_UUID_DTS_CONTROL_POINT                                              \
    BT_UUID_DECLARE_16(BT_UUID_DTS_CONTROL_POINT_VALUE)

#define BT_UUID_DTS_CHANGE_LOG_DATA_VALUE 0x2B92
#define BT_UUID_DTS_CHANGE_LOG_DATA                                            \
    BT_UUID_DECLARE_16(BT_UUID_DTS_CHANGE_LOG_DATA_VALUE)

#define BT_UUID_DTS_RACP_VALUE 0x2A52
#define BT_UUID_DTS_RACP       BT_UUID_DECLARE_16(BT_UUID_DTS_RACP_VALUE)

#endif // UUID_H