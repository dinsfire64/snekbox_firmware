# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(snek_box C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# use local tinyusb
set(PICO_TINYUSB_PATH ${CMAKE_SOURCE_DIR}/include/tinyusb)

# use custom board
set(PICO_BOARD_HEADER_DIRS ${CMAKE_SOURCE_DIR}) 
set(PICO_BOARD snek_box_board)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Include External Libs
set(PICO_PIO_USB_PATH ${CMAKE_SOURCE_DIR}/include/Pico-PIO-USB)
add_subdirectory(${PICO_PIO_USB_PATH} pico_pio_usb)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/include/tusb_xinput xinput_host)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/include/i2c_slave)

# Tell CMake where to find the executable source file
add_executable(${PROJECT_NAME} 
    debug.c
    main.c
    settings.c
    ws2812.c
    input_report.c
    i2c.c
    
    targets/__targets.c
    targets/ps2_phy.c
    targets/ps2_controller.c
    targets/joybus.c
    targets/xboxog_descriptors.c
    targets/xboxog_tusb_driver.c
    targets/xboxog.c

    handlers/__handlers.c
    handlers/ds3.c
    handlers/ds4.c
    handlers/ds5.c
    handlers/xinput_handler.c
    handlers/smx.c
    handlers/keyboard.c
    handlers/mouse.c
    handlers/icedragon.c
    handlers/switch_pro.c
    handlers/ltek.c
    handlers/dforce.c
    handlers/gamo.c
    handlers/generic_softmat.c
    handlers/dual_ps2.c
    handlers/zuiki.c
    handlers/santroller.c
    handlers/b2l.c
    handlers/ddr_grandprix.c
    handlers/infinitas.c
    
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/dcd_pio_usb.c
    ${PICO_TINYUSB_PATH}/src/portable/raspberrypi/pio_usb/hcd_pio_usb.c
)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
    pico_pio_usb
    tinyusb_host
    tinyusb_device
    xinput_host
    hardware_i2c
    i2c_slave
)

# auto versioning
string(TIMESTAMP BUILD_TIME "%Y%m%d-%H%M" UTC)
SET(VER_CURR_SNEKBOX "v0.4 (${BUILD_TIME})")
configure_file(version.h.in version.h @ONLY)

# info for picotool
pico_set_program_description(${PROJECT_NAME} "snek box")
pico_set_program_version(${PROJECT_NAME} "${VER_CURR_SNEKBOX}")
pico_set_program_url(${PROJECT_NAME} "icedragon.io/snek-box/")

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# use tinyusb implementation
target_compile_definitions(${PROJECT_NAME} PRIVATE PIO_USB_USE_TINYUSB)

# needed so tinyusb can find tusb_config.h
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

# bring in the pio for the PSX slave.
pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/targets/psxSPI.pio)

# for gcn/joybus
pico_generate_pio_header(${PROJECT_NAME}  ${CMAKE_SOURCE_DIR}/targets/joybus.pio)

# ws2812
pico_generate_pio_header(${PROJECT_NAME}  ${CMAKE_SOURCE_DIR}/ws2812.pio)

# add flash target (make flash)
add_custom_target(flash DEPENDS ${PROJECT_NAME}.elf)
add_custom_command(TARGET flash
    USES_TERMINAL
    COMMAND openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program ${PROJECT_NAME}.elf verify reset exit")
add_custom_target(pflash DEPENDS ${PROJECT_NAME}.uf2)
    add_custom_command(TARGET pflash
        USES_TERMINAL
        COMMAND picotool load ${PROJECT_NAME}.uf2 -f && picotool reboot)