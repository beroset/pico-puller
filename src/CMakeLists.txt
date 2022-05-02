if (TARGET tinyusb_device)
    add_executable(pico-puller main.cpp M16C.cpp)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(pico-puller 1)
    pico_enable_stdio_uart(pico-puller 0)

    # compile pio program
    pico_generate_pio_header(pico-puller ${CMAKE_CURRENT_LIST_DIR}/m16c.pio)
    target_sources(pico-puller PRIVATE main.cpp)
    target_link_libraries(pico-puller PRIVATE pico_stdlib hardware_pio)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(pico-puller)

elseif(PICO_ON_DEVICE)
    message(WARNING "not building pico-puller because TinyUSB submodule is not initialized in the SDK")
endif()

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pico-puller.uf2 DESTINATION "${PICO_MOUNT_LOCATION}/RPI-RP2/")
