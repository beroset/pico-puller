cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico-puller 
    VERSION 1.2.0
    LANGUAGES C CXX ASM 
    DESCRIPTION "A Raspberry Pi Pico-based memory dumper for the M16C processor"
)
option(WITH_DOC "Create project documentation" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_CXX_ENABLE_EXCEPTIONS 0)
if (PICO_MOUNT_LOCATION)
else() 
    message(FATAL_ERROR "no Pico mount point selected: you must define PICO_MOUNT_LOCATION")
endif() 

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

# Add main source dir
add_subdirectory(src)
if (WITH_DOC)
    add_subdirectory(doc)
endif()

install(PROGRAMS scripts/pico-puller TYPE BIN)
