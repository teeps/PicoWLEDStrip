cmake_minimum_required(VERSION 3.12)

add_executable(NeoPixel NeoPixel.cpp 
        NeoPixelMQTT.cpp 
        MQTTState.cpp
        MQTTTaskInterface.cpp
        ToText.cpp
        RGBColour.cpp)
pico_generate_pio_header(NeoPixel ${CMAKE_CURRENT_LIST_DIR}/NeoPixel.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR})


target_sources(NeoPixel PRIVATE ${PICO_SDK_PATH}/lib/lwip/src/apps/mqtt/mqtt.c)

set(FREERTOS_KERNEL_PATH /home/mark/pico/FreeRTOS-Kernel)

target_include_directories(NeoPixel PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/../FSM
        ${FREERTOS_KERNEL_PATH}/include
        /home/mark/pico/FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/include
        $(PICO_SDK_PATH))

target_link_libraries(NeoPixel PRIVATE 
    pico_stdlib 
    FreeRTOS-Kernel-Heap4
    hardware_pio
    pico_cyw43_arch_lwip_sys_freertos
    pico_lwip
    )

pico_add_extra_outputs(NeoPixel)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

