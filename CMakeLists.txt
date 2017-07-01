cmake_minimum_required(VERSION 3.7)
project(AutomaticTableDetection)

# Find boost packages
set (CMAKE_CXX_FLAGS "--std=gnu++11 -lpthread ${CMAKE_CXX_FLAGS}")
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
FIND_PACKAGE(Boost 1.40 COMPONENTS regex program_options REQUIRED )
link_directories(/usr/local/lib/ /usr/lib/ ${Boost_LIBRARY_DIR})
include_directories(/usr/local/include/ DocProc/src/ ${Boost_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES src/main.cc)
add_executable(AutomaticTableDetection ${SOURCE_FILES})


target_link_libraries(AutomaticTableDetection lept tesseract opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs pthread gomp ${Boost_LIBRARIES} docproc)
