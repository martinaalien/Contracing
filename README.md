# GAENS and WENS wearable implementation
This is an wearable implementation of Google/Apple Exposure Notification  
System (GAENS) and Wearable Exposure Notification Service (WENS). The  
implementation is created for Nordic Semiconductor's nRF52833 SoC.

## Install Zephyr
To be able to build this project you need to install the Zephyr devlopment  
environment. This can be done by following their [Getting Started Guide]("https://docs.zephyrproject.org/latest/getting_started/index.html").  

## Comments
This sections includes some comments regarding the implementation.

### Serial communication power consumption
Having enabled serial communications disables zephyr to put the device in sleep  
mode. In order to decrease the power consumption and enable sleep mode one has  
to change `CONFIG_SERIAL=y` to `CONFIG_SERIAL=n` in `prj.conf`. This overwrites  
`UART_CONSOLE`, `STDOUT_CONSOLE` and `LOG_BACKEND_UART`, which all will give a  
warning, but those can be disregarded.

### undefined reference to `mbedtls_hkdf'
Zephyr uses an own config file for defining MBEDTLS configurations and HKDF,  
which is required by GAENS, is not included. To fix this error you need to add  
the following to `modules/crypto/mbedtls/configs/config-tls-generic.h` in  
your zephyr project:

    #define MBEDTLS_HKDF_C