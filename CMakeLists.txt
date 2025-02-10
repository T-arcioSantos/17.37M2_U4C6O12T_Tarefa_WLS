# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================


cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_BOARD pico_w CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(17.37M2_U4C6O12T_Tarefa_WLS C CXX ASM)

pico_sdk_init()

add_executable(17.37M2_U4C6O12T_Tarefa_WLS 17.37M2_U4C6O12T_Tarefa_WLS.c )

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)
pico_generate_pio_header(17.37M2_U4C6O12T_Tarefa_WLS ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_sources(17.37M2_U4C6O12T_Tarefa_WLS PRIVATE 17.37M2_U4C6O12T_Tarefa_WLS.c inc/ssd1306.c)
pico_set_program_name(17.37M2_U4C6O12T_Tarefa_WLS "17.37M2_U4C6O12T_Tarefa_WLS")

target_link_libraries(17.37M2_U4C6O12T_Tarefa_WLS pico_stdlib hardware_pio)
pico_add_extra_outputs(17.37M2_U4C6O12T_Tarefa_WLS)

target_include_directories(17.37M2_U4C6O12T_Tarefa_WLS PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(17.37M2_U4C6O12T_Tarefa_WLS hardware_i2c)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(17.37M2_U4C6O12T_Tarefa_WLS 1)
pico_enable_stdio_usb(17.37M2_U4C6O12T_Tarefa_WLS 1)


