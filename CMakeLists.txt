cmake_minimum_required(VERSION 3.12)
set(PICO_SDK_PATH /home/mark/pico/pico-sdk/)
set(PICO_BOARD pico_w)
set(FREERTOS_PORT GCC_RP2040)
include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)
project(Pico-FreeRTOS-NeoPixel)

pico_sdk_init()

add_subdirectory(NeoPixelSource)
#add_subdirectory(tests)

#option(COMPILE_TESTS "Compile the tests" OFF)
#if(COMPILE_TESTS)
#  add_subdirectory(tests)
#endif(COMPILE_TESTS)