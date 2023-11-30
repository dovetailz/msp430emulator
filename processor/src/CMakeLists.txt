include_directories(${CMAKE_SOURCE_DIR}/processor/include)
include_directories(${CMAKE_SOURCE_DIR}/memory/include)
add_library(processor processor.cpp processor_opcodes.cpp)