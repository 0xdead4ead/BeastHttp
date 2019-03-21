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

add_subdirectory("${PROJECT_SOURCE_DIR}/examples/reactor")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/reactor_extented")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/reactor_ssl")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/reactor_cxx11")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/reactor_timers")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/reactor_flex")
enable_testing ()
add_subdirectory("${PROJECT_SOURCE_DIR}/tests/display")
add_subdirectory("${PROJECT_SOURCE_DIR}/tests/basic_router")
add_subdirectory("${PROJECT_SOURCE_DIR}/tests/chain_router")
add_subdirectory("${PROJECT_SOURCE_DIR}/tests/param")
