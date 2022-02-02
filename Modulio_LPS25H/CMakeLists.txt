cmake_minimum_required(VERSION 3.13)
set(ENV{PICO_SDK_PATH} ~/pico/pico-sdk)	# SDKへのパスが環境変数に必要なのでここで設定しておいた

include(pico_sdk_import.cmake)
project(main_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()


include_directories ( 
	$("..")
	$("../Lib_Modulio")
	)
add_executable(test_lps25h
	main.c
	../Lib_Modulio/Modulio_RPico.c
	../Lib_Modulio/ACM1602.c
	../Lib_Modulio/LPS25H.c
)

pico_enable_stdio_usb(test_lps25h 1)
pico_enable_stdio_uart(test_lps25h 1)
pico_add_extra_outputs(test_lps25h)
target_link_libraries(test_lps25h pico_stdlib hardware_i2c)


