cmake_minimum_required(VERSION 3.15)
project(RekindleSketch VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(RekindleSketch
    main.cpp
    RekindleSketch.h
    parm.h
    MurmurHash3.h
)

set_target_properties(RekindleSketch PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
