
#include "gaens.h"
#include <string.h>
#include <logging/log.h>

#define LOG_MODULE_NAME crypto
LOG_MODULE_REGISTER(crypto);

static uint32_t _tek_validity_timestamp = 0;
static uint8_t _current_tek[TEK_LENGTH] = {0};
static uint8_t _current_rpi[RPI_LENGTH] = {0};
static uint8_t _current_rpik[RPIK_LENGTH] = {0};
static uint8_t _current_aemk[AEMK_LENGTH] = {0};

int gaens_get_rpi(uint8_t *rpi)
{
    memcpy(rpi, _current_rpi, RPI_LENGTH);
    return 0;
}

int gaens_update_rpi(void)
{
    if (crypto_rpi(_current_rpik, _current_rpi) < 0)
    {
        LOG_ERR("Failed to update rolling proximity identifier");
        return -1;
    }
    return 0;
}

int gaens_update_tek(void)
{
    if (crypto_tek(_current_tek, TEK_LENGTH, &_tek_validity_timestamp) < 0)
    {
        LOG_ERR("Failed to update temporary exposure key");
        return -1;
    }
    return 0;
}