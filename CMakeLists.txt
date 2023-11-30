cmake_minimum_required(VERSION 3.10)

# Project name
project(MSP430 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in
        googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

add_subdirectory(test)
add_subdirectory(processor)
add_subdirectory(memory)
add_subdirectory(tools)
add_subdirectory(debugger)
add_subdirectory(peripheral)
add_subdirectory(emulator)

configure_file(.clang-format .clang-format)
add_custom_target(format
    COMMAND ../format.sh)

enable_testing()