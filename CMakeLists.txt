cmake_minimum_required(VERSION 3.8)
project(Trckinglib)

include_directories(/usr/local/include/opencv4)
include_directories(/usr/include/python3.10)
include_directories(./include)

set(SOURCE_LIB src/pymodule.cpp)

add_library(libpython SHARED IMPORTED)
set_property(TARGET libpython PROPERTY IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libpython3.10.so)

add_library(libcv_core SHARED IMPORTED)
set_property(TARGET libcv_core PROPERTY IMPORTED_LOCATION /usr/local/lib/libopencv_core.so)

add_library(libcv_imgproc SHARED IMPORTED)
set_property(TARGET libcv_imgproc PROPERTY IMPORTED_LOCATION /usr/local/lib/libopencv_imgproc.so.4.7.0)

add_library(libcv_highgui SHARED IMPORTED)
set_property(TARGET libcv_highgui PROPERTY IMPORTED_LOCATION /usr/local/lib/libopencv_highgui.so)

add_library(libcv_video SHARED IMPORTED)
set_property(TARGET libcv_video PROPERTY IMPORTED_LOCATION /usr/local/lib/libopencv_video.so)

add_library(libcv_imgcodecs SHARED IMPORTED)
set_property(TARGET libcv_imgcodecs PROPERTY IMPORTED_LOCATION /usr/local/lib/libopencv_imgcodecs.so)

add_library(libcv_videoio SHARED IMPORTED)
set_property(TARGET libcv_videoio PROPERTY IMPORTED_LOCATION /usr/local/lib/libopencv_videoio.so)

add_library(pymodule SHARED ${SOURCE_LIB})

target_link_libraries(pymodule libpython libcv_core libcv_imgproc libcv_highgui libcv_video libcv_imgcodecs libcv_videoio)