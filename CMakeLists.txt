cmake_minimum_required(VERSION 3.8)
project(Trckinglib)

include_directories(/usr/local/include/opencv4/opencv2)
include_directories(/usr/include/python3.10)

set(SOURCE_LIB src/tracking.cpp src/pymodule.cpp /usr/local/include/opencv4/opencv2/ /usr/include/python3.10/)

add_library(libpython STATIC IMPORTED)
set_property(TARGET libpython PROPERTY IMPORTED_LOCATION /usr/lib/x86_64-linux-gnu/libpython3.10.a)

add_library(libopencv SHARED IMPORTED)
set_property(TARGET libopencv PROPERTY IMPORTED_LOCATION /usr/local/bin/)

add_library(pymodule SHARED ${SOURCE_LIB})

target_link_libraries(pymodule libpython libopencv)