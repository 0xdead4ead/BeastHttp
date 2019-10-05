cmake_minimum_required(VERSION 3.11)

project(BeastHttp VERSION 3.0 LANGUAGES CXX)

find_package(Boost 1.70 COMPONENTS system thread)
find_package(OpenSSL COMPONENTS SSL Crypto)

set(BEASTHTTP_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/../include)
set(BEASTHTTP_EXAMPLES_DIR ${PROJECT_SOURCE_DIR}/examples)
set(BEASTHTTP_TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)
set(BEASTHTTP_STATIC_DIR ${PROJECT_SOURCE_DIR}/../static)

option(BEASTHTTP_USE_MAKE_SHARED "Allocate memory object with std::make_shared" ON)
option(BEASTHTTP_BUILD_STATIC_LIBS "Build static lib for Boost.Asio/Boost.Beast" ON)

configure_file (
  "${PROJECT_SOURCE_DIR}/cmake/version.hxx.in"
  "${BEASTHTTP_INCLUDE_PATH}/http/base/version.hxx"
)

configure_file (
  "${PROJECT_SOURCE_DIR}/cmake/config.hxx.in"
  "${BEASTHTTP_INCLUDE_PATH}/http/base/config.hxx"
)

if(NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
endif()

if( NOT CMAKE_BUILD_TYPE )
   SET(CMAKE_BUILD_TYPE Debug)
endif()

message("CXX=" ${CMAKE_CXX_COMPILER})
message("CXXFLAGS=" ${CMAKE_CXX_FLAGS})
message("CMAKE_BUILD_TYPE=" ${CMAKE_BUILD_TYPE})

include_directories(${BEASTHTTP_INCLUDE_PATH})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

if(BEASTHTTP_BUILD_STATIC_LIBS)
    add_definitions(-DBOOST_ASIO_SEPARATE_COMPILATION=1)
    add_definitions(-DBOOST_BEAST_SEPARATE_COMPILATION=1)

    add_library (
        asio STATIC
        ${BEASTHTTP_STATIC_DIR}/asio.cpp
    )

    add_library (
        asio-ssl STATIC
        ${BEASTHTTP_STATIC_DIR}/asio_ssl.cpp
    )

    add_library (
        beast STATIC
        ${BEASTHTTP_STATIC_DIR}/beast.cpp
    )
endif()

file(GLOB_RECURSE BEASTHTTP_HEADS ${BEASTHTTP_INCLUDE_PATH}/*.hxx)

add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor")
if (OPENSSL_FOUND)
    add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_ssl")
endif()
add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_cxx11")
add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_timers")
if (OPENSSL_FOUND)
    add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_flex")
endif()
add_subdirectory("${BEASTHTTP_EXAMPLES_DIR}/reactor_sse")
enable_testing ()
add_subdirectory("${BEASTHTTP_TESTS_DIR}/display")
add_subdirectory("${BEASTHTTP_TESTS_DIR}/basic_router")
add_subdirectory("${BEASTHTTP_TESTS_DIR}/chain_router")
add_subdirectory("${BEASTHTTP_TESTS_DIR}/param")
