cmake_minimum_required(VERSION 3.0)
project(virtual_sensor VERSION 0.1.0)
# Add the path to the librdkafka header files to the include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the path to the librdkafka library to the linker path
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

# Define your target and add the librdkafka library to it
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(vsensor ${SOURCES})

target_link_libraries(vsensor rdkafka++ pthread ixwebsocket spdlog)
