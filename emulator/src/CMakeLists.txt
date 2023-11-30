include_directories(${CMAKE_SOURCE_DIR}/processor/include)
include_directories(${CMAKE_SOURCE_DIR}/memory/include)
include_directories(${CMAKE_SOURCE_DIR}/peripheral/include)
include_directories(${CMAKE_SOURCE_DIR}/debugger/include)
include_directories(${CMAKE_SOURCE_DIR}/emulator/include)
link_libraries(debugger memory processor elf_reader)

add_executable(emulator emulator.cpp)