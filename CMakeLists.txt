# Find Zephyr. This also loads Zephyr's build system.
cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr)
project(my_zephyr_app)

# Add your source file to the "app" target. This must come after
# find_package(Zephyr) which defines the target.
target_sources(app PRIVATE src/main.c  
                           src/ble/ble.c 
                           src/ble/advertise.c 
                           src/ble/scan.c 
                           src/ble/connection.c
                           src/records/extmem.c
                           src/records/storage.c
                           src/gaens/crypto.c
                           src/gaens/gaens.c
                           src/ble/services/wens/wens.c
                           src/gaens/gaens_test.c)