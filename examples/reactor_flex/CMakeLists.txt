cmake_minimum_required(VERSION 3.11)

find_package(Boost 1.66 COMPONENTS system thread regex)
find_package(OpenSSL COMPONENTS SSL Crypto)

set(BEASTHTTP_EXAMPLE_NAME reactor_flex)

include_directories(${BEASTHTTP_INCLUDE_PATH})
include_directories(${BEASTHTTP_COMMON_PATH})
include_directories(${Boost_INCLUDE_DIRS})

set(SRCS main.cxx)

add_executable(${BEASTHTTP_EXAMPLE_NAME} ${SRCS})

target_link_libraries(${BEASTHTTP_EXAMPLE_NAME} Boost::system Boost::thread Boost::regex pthread)

if ( NOT OPENSSL_SSL_LIBRARY)
    message(FATAL_ERROR "The OpenSSL ssl lib is not found! ")
endif()

target_link_libraries(${BEASTHTTP_EXAMPLE_NAME} OpenSSL::SSL OpenSSL::Crypto)
