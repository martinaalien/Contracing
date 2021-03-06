////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "time.h"

/* Zephyr includes */
#include <logging/log.h>
#include <posix/time.h>

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME time
LOG_MODULE_REGISTER(time);

////////////////////////////////////////////////////////////////////////////////
// Public functions
////////////////////////////////////////////////////////////////////////////////

int set_current_time(uint32_t current_time)
{
    struct timespec ts = {
        .tv_sec = current_time,
        .tv_nsec = 0,
    };

    if (clock_settime(CLOCK_REALTIME, &ts) < 0)
    {
        LOG_ERR("Failed to set current time");
        return -1;
    }

    return 0;
}

int get_current_time(uint32_t *time)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts))
    {
        LOG_ERR("Failed to fetch time");
        return -1;
    }

    *time = (uint32_t)ts.tv_sec;

    return 0;
}