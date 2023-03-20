add_executable(misterpico misterpico.c main.c tusb_lwip_glue.c usb_descriptors.c 
    ${PICO_TINYUSB_PATH}/lib/networking/dhserver.c
    ${PICO_TINYUSB_PATH}/lib/networking/rndis_reports.c
)

#pico_enable_stdio_usb(misterpico 1)

pico_add_extra_outputs(misterpico)

target_compile_options(misterpico PRIVATE -Wall)

pico_enable_stdio_uart(misterpico 1)
pico_enable_stdio_usb(misterpico 0)
pico_enable_stdio_semihosting(misterpico 0)

# add_compile_definitions(CFG_TUSB_DEBUG=3)
target_link_libraries(misterpico i2c_slave pico_stdio pico_stdlib pico_multicore pico_unique_id hardware_pwm hardware_clocks hardware_adc tinyusb_device tinyusb_board pico_lwip pico_lwip_arch)
target_include_directories(misterpico PRIVATE
        ${PICO_TINYUSB_PATH}/lib/networking
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )

