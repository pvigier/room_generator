cmake_minimum_required(VERSION 3.10)

# Project name

project(RoomGenerator VERSION 1.0 LANGUAGES CXX)

# Set warnings

function(setWarnings target)
    target_compile_options(${target} PRIVATE
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wnull-dereference
        -Wuseless-cast
        -Wdouble-promotion)
endfunction()

# Set standard

function(setStandard target)
    target_compile_features(${target} PRIVATE cxx_std_20)
endfunction()

# Create library

add_library(room_generator
    src/CornerConstraint.cpp
    src/DistanceToAnyWallConstraint.cpp
    src/DistanceToBorderConstraint.cpp
    src/DistanceToWallConstraint.cpp
    src/Grid.cpp
    src/NextToDoorConstraint.cpp
    src/Solver.cpp)
target_include_directories(room_generator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
setWarnings(room_generator)
setStandard(room_generator)

include(GNUInstallDirs)
install(TARGETS room_generator)
install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})


# Examples

option (BUILD_EXAMPLES "Build the examples." ON)
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()