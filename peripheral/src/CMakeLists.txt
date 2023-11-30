include_directories(${CMAKE_SOURCE_DIR}/processor/include)
include_directories(${CMAKE_SOURCE_DIR}/peripheral/include)
add_library(clock clock.cpp)
add_library(p1 p1.cpp)
