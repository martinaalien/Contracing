
#include "logging/log.h"

#include "gaens.h"
#include "crypto.h"

#include <sys/time.h>
#include <unistd.h>
#include <posix/time.h>
#include <sys/printk.h>

#define LOG_MODULE_NAME gaens_test
LOG_MODULE_REGISTER(gaens_test);

void print_array_hex(uint8_t *arr, uint8_t len, char *str)
{
    printk("%s", str);
    for (int i = 0; i < len; i++)
    {
        printk("%.2x ", arr[i]);
    }
    printk("\n");
}

void print_array_char(uint8_t *arr, uint8_t len, char *str)
{
    printk("%s", str);
    for (int i = 0; i < len; i++)
    {
        printk("%c ", arr[i]);
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

void test_rpi(void)
{
    printk("------------------------------------------------------------\n");
    printk("Testing RPI. Test will do the following:\n"
           "1. Create an RPI and check immediately if it should be updated\n"
           "2. Forward the current time by 10 minutes (600 seconds)\n"
           "3. Check if a new RPI should be created\n"
           "4. Create new RPI and compare it with the previous to ensure they"
           "are not the same\n"
           "5. Decrypt the current RPI to validate its contents\n"
    );
    printk("------------------------------------------------------------\n");
    
    printk("1. Creating RPI\n");
    uint32_t start_time = 1615051437;
    printk("\tSetting time to %d\n", start_time);
    set_current_time(start_time);
    gaens_update_rpi();
    uint8_t rpi1[RPI_LENGTH] = {0};
    gaens_get_rpi(rpi1);
    print_array_hex(rpi1, RPI_LENGTH, "\t1st RPI: ");
    printk("\tShould update rpi: %d (should be 0)\n", gaens_ble_addr_expired());
    
    uint32_t end_time = start_time + 600;
    printk("2. Advancing time 10 minutes to %d\n", end_time);
    set_current_time(end_time);
    
    printk("3. Should update rpi: %d (should be 1)\n", gaens_ble_addr_expired());
    
    printk("4. Generating new RPI\n");
    gaens_update_rpi();
    uint8_t rpi2[RPI_LENGTH] = {0};
    gaens_get_rpi(rpi2);
    print_array_hex(rpi2, RPI_LENGTH, "\t2nd RPI: ");

    printk("\t1st and 2nd RPI equal: %d (should be 0)\n", arrays_eq(rpi1, rpi2, RPI_LENGTH));    

    printk("5. Decrypting current RPI\n");
    uint8_t correct_dec_rpi[16] = "EN-RPI";
    uint32_t current_en_interval_number;
    crypto_en_interval_number(&current_en_interval_number);
    memcpy(&correct_dec_rpi[12], &current_en_interval_number, sizeof(current_en_interval_number));
    print_array_hex(correct_dec_rpi, RPI_LENGTH, "\tCorrect decrypted RPI: ");
    uint8_t actual_dec_rpi[RPI_LENGTH] = {0};
    gaens_get_rpi_decrypted(actual_dec_rpi);
    print_array_hex(actual_dec_rpi, RPI_LENGTH, "\tActual decrypted RPI: ");
    printk("\tCorrect and actual decrypted RPI equal: %d (should be 1)\n", 
            arrays_eq(correct_dec_rpi, actual_dec_rpi, RPI_LENGTH));
}

void test_aem(void)
{
    printk("------------------------------------------------------------\n");
    printk("Testing AEM. Test will do the following:\n"
           "1. Create a dummy metadata and encrypt it\n"
           "2. Encrypt same data again and check that the result is the same\n"
           "3. Forward the current time by 10 minutes (600 seconds) and update"
           "the keys\n"
           "4. Encrypt the same dummy AEM data to ensure the encrypted data is"
           "different from before\n"
           "5. Decrypt dummy AEM data to valdidate its contents\n"
    );
    printk("------------------------------------------------------------\n");

    printk("1. Creating dummy AEM\n");
    uint8_t dummy_metadata[AEM_LENGTH] = "HELP";
    print_array_hex(dummy_metadata, AEM_LENGTH, "\tDummy metadatadata: ");
    uint8_t output_aem_1[AEM_LENGTH];
    gaens_encrypt_metadata(dummy_metadata, AEM_LENGTH, output_aem_1);
    print_array_hex(output_aem_1, AEM_LENGTH, "\tOutput AEM: ");

    printk("2. Encrypting same dummy data again\n");
    uint8_t output_aem_2[AEM_LENGTH];
    gaens_encrypt_metadata(dummy_metadata, AEM_LENGTH, output_aem_2);
    print_array_hex(output_aem_2, AEM_LENGTH, "\tOutput AEM: ");
    printk("\tFirst and second AEM equal: %d (should be 1)\n", arrays_eq(output_aem_1, output_aem_2, AEM_LENGTH));

    printk("3. Forwarding time by 10 minutes and updating RPI\n");
    uint32_t current_time = get_current_time();
    set_current_time(current_time + 600);
    gaens_update_rpi();

    printk("4. Encypting same metadata again\n");
    uint8_t output_aem_3[AEM_LENGTH];
    gaens_encrypt_metadata(dummy_metadata, AEM_LENGTH, output_aem_3);
    print_array_hex(output_aem_3, AEM_LENGTH, "\tOutput AEM: ");
    printk("\tFirst and second AEM equal: %d (should be 0)\n", arrays_eq(output_aem_1, output_aem_3, AEM_LENGTH));

    printk("5. Decrypting dummy AEM data\n");
    uint8_t decrypted_aem[AEM_LENGTH];
    gaens_decrypt_metadata(output_aem_3, AEM_LENGTH, decrypted_aem);
    print_array_hex(output_aem_3, AEM_LENGTH, "\tDecrypted AEM: ");
    printk("\tDecrypted AEM equal to dummy metadata: %d (should be 1)\n", arrays_eq(dummy_metadata, decrypted_aem, AEM_LENGTH));
}

void gaens_test_run_all(void)
{
    printk("============================================================\n");
    printk("Starting GAENS tests\n");
    printk("============================================================\n");
    
    gaens_init();
    test_rpi();
    test_aem();

}