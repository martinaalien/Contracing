////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

#include "ble/advertise.h"
#include "ble/ble.h"
#include "ble/scan.h"
#include "records/extmem.h"
#include "records/storage.h"
#include <stdio.h>
#include <string.h>

/* Zephyr includes */
#include <logging/log.h>
#include <zephyr.h>
#include <sys/printk.h>

#include "gaens/gaens.h"
#include "gaens/crypto.h"

#include <sys/time.h>
#include <unistd.h>
#include <posix/time.h>


////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

#define LOG_MODULE_NAME main
LOG_MODULE_REGISTER(main);

////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////

void print_array(uint8_t *arr, uint8_t len, char *str)
{
    printk("%s", str);
    for (int i = 0; i < len; i++)
    {
        printk("%.2x ", arr[i]);
    }
    printk("\n");
}

int arrays_eq(uint8_t *arr1, uint8_t *arr2, uint8_t len)
{
    for (int i = 0; i < len; i++)
    {
        if (arr1[i] != arr2[i])
        {
            return 0;
        }
    }
    return 1;
}

void set_current_time(uint32_t current_time)
{
    struct timespec ts = {
        .tv_sec = current_time,
        .tv_nsec = 0,
    }; 
    clock_settime(CLOCK_REALTIME, &ts);
}

uint32_t get_current_time(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint32_t)ts.tv_sec;
}

void main(void)
{
    int err;

    LOG_INF("It's alive!\n");

    err = ble_init();
    if (err)
    {
        LOG_ERR("Failed to initialize BLE");
    }

    err = extmem_init();
    if (err)
    {
        LOG_ERR("Failed to initialize external memory");
    }

    err = advertise_start();
    if (err)
    {
        LOG_ERR("Failed to start advertising");
    }

    err = scan_start();
    if (err)
    {
        LOG_ERR("Failed to start scanning");
    }
    printk("Starting GAENS tests\n");

    uint32_t start_time = 1615051437;

    set_current_time(start_time);

    gaens_update_rpi();
    uint8_t rpi[RPI_LENGTH] = {0};
    gaens_get_rpi(rpi);
    print_array(rpi, RPI_LENGTH, "RPI: ");
    printk("Should update rpi: %d\n", gaens_ble_addr_expired());
    
    printk("Advancing time 10 minutes\n");
    set_current_time(start_time + 600);
    printk("Should update rpi: %d\n", gaens_ble_addr_expired());
    gaens_update_rpi();
    gaens_get_rpi(rpi);
    print_array(rpi, RPI_LENGTH, "RPI: ");


    // uint32_t en_num = 10;
    // crypto_en_interval_number(&en_num);
    // printk("Current EN interval number: %d\n", en_num);
    // uint32_t current_time = (uint32_t) time(NULL);
    // LOG_INF("Should be: %d", current_time / 600);
    // LOG_INF("Current time: %d", current_time);
}