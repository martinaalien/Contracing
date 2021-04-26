# Google Apple Exposure Notification System (GAENS) and Wearable Exposure Notification Service (WENS) wearable implementation

## Comments
This sections includes some comments regarding the implementation.

### undefined reference to `mbedtls_hkdf'
Zephyr uses an own config file for defining MBEDTLS configurations and HKDF, which is required by GAENS, is not included. To fix this error you need to add the following to `modules/crypto/mbedtls/configs/config-tls-generic.h` in your zephyr project:

    #define MBEDTLS_HKDF_C