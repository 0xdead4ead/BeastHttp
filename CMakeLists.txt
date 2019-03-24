cmake_minimum_required(VERSION 3.11)

set (BEASTHTTP_VERSION_MAJOR 2)
set (BEASTHTTP_VERSION_MINOR 1)

project(BeastHttp)

configure_file (
  "${PROJECT_SOURCE_DIR}/cmake/version.hxx.in"
  "${PROJECT_SOURCE_DIR}/include/base/version.hxx"
)

if(NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS -std=c++14)
endif()

if( NOT CMAKE_BUILD_TYPE )
   SET(CMAKE_BUILD_TYPE Debug)
endif()

message("CXX=" ${CMAKE_CXX_COMPILER})
message("CXXFLAGS=" ${CMAKE_CXX_FLAGS})
message("CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})

set(BEASTHTTP_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include)
set(BEASTHTTP_COMMON_PATH ${PROJECT_SOURCE_DIR}/common)

set(BEASTHTTP_EXAMPLES_DIR ${PROJECT_SOURCE_DIR}/examples)
set(BEASTHTTP_TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)

add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor")
add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_extented")
add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_ssl")
add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_cxx11")
add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_timers")
add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_flex")
enable_testing ()
add_subdirectory("${BEASTHTTP_TESTS_DIR}/display")
add_subdirectory("${BEASTHTTP_TESTS_DIR}/basic_router")
add_subdirectory("${BEASTHTTP_TESTS_DIR}/chain_router")
add_subdirectory("${BEASTHTTP_TESTS_DIR}/param")
