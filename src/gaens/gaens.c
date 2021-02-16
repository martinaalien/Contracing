
#include "gaens.h"
#include <string.h>

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <mbedtls/hkdf.h>
#include <mbedtls/md_internal.h>
#include <random/rand32.h>
#include <tinycrypt/aes.h>
#include <zephyr.h>

#define TEK_ROLLING_PERIOD 144

int gaens_en_interval_number(uint32_t *buf)
{
    time_t epoch_timestamp = time(NULL);

    *buf = (uint32_t)epoch_timestamp / 600;

    return 0;
}

int gaens_tek(uint16_t *tek, uint32_t *tek_timestamp)
{
    uint32_t *en_interval_number = 0;
    gaens_en_interval_number(en_interval_number);
    *tek_timestamp =
        (*en_interval_number / TEK_ROLLING_PERIOD) * TEK_ROLLING_PERIOD;

    if (sys_csrand_get(tek, sizeof(*tek)) < 0)
    {
        return -1;
    }

    return 0;
}

int gaens_rpik(const uint16_t tek, uint16_t *rpik)
{
    char *info = "EN-RPIK";
    if (mbedtls_hkdf(&mbedtls_sha256_info, NULL, 0, (uint8_t *)&tek,
                     sizeof(tek), (unsigned char *)info, strlen(info),
                     (unsigned char *)rpik, sizeof(*rpik)) < 0)
    {
        printf("Failed to generate rolling proximity identified key\n");
        return -1;
    }

    return 0;
}

int gaens_rpi(const uint16_t rpik)
{
    // Create array of padded data as defined in crypto spec p. 7
    unsigned char padded_data[16] = "EN-RPI";
    uint32_t en_in_j;
    gaens_en_interval_number(&en_in_j);
    memcpy(&padded_data[12], &en_in_j, sizeof(en_in_j));

    // struct tc_aes_key_sched_struct s;
    // strcpy(s.words, )
    return 0;
}