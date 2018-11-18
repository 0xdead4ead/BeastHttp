cmake_minimum_required(VERSION 3.11)

set(OUTPUT_NAME ex8_ssl_client)

find_package(Boost 1.66.0 REQUIRED COMPONENTS system thread regex)
find_package(OpenSSL COMPONENTS SSL)

include_directories(${Boost_INCLUDE_DIRS})
include_directories("${PROJECT_SOURCE_DIR}/include")

set(SOURCES
    ex8_ssl_client.cpp)
set(HEADERS
    ${PROJECT_SOURCE_DIR}/include/ssl/base.hpp
    ${PROJECT_SOURCE_DIR}/include/ssl/client.hpp
    ${PROJECT_SOURCE_DIR}/include/ssl/session.hpp)

add_executable(${OUTPUT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${OUTPUT_NAME} Boost::system Boost::thread Boost::regex  pthread icui18n)


if ( NOT OPENSSL_SSL_LIBRARY)
    message(FATAL_ERROR "The OpenSSL ssl lib is not found! ")
endif()

target_link_libraries(${OUTPUT_NAME} OpenSSL::SSL)
