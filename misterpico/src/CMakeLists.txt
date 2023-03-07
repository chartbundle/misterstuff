add_executable(misterpico misterpico.c)

pico_enable_stdio_usb(misterpico 1)

pico_add_extra_outputs(misterpico)

target_compile_options(misterpico PRIVATE -Wall)

target_link_libraries(misterpico i2c_slave pico_stdlib hardware_pwm hardware_clocks hardware_adc tinyusb lwip)
target_include_directories(misterpico PRIVATE
        ${PICO_LWIP_PATH}/src/include
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        )

