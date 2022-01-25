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
add_executable(test_wlcd
	main.c
	../Lib_Modulio/Modulio_RPico.c
	../Lib_Modulio/ACM1602.c
	../Lib_Modulio/PCA9632.c
	../Lib_Modulio/LPS25H.c
	../Lib_Modulio/ADXL345.c
)

pico_enable_stdio_usb(test_wlcd 1)
pico_enable_stdio_uart(test_wlcd 1)
pico_add_extra_outputs(test_wlcd)
target_link_libraries(test_wlcd pico_stdlib hardware_i2c)


