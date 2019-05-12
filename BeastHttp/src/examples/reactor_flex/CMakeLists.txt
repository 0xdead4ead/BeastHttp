cmake_minimum_required(VERSION 3.11)

find_package(OpenSSL COMPONENTS SSL Crypto)

set(BEASTHTTP_EXAMPLE_NAME reactor_flex)

file(GLOB HEADS ${BEASTHTTP_EXAMPLES_DIR}/${BEASTHTTP_EXAMPLE_NAME}/*.hxx)
file(GLOB SRCS ${BEASTHTTP_EXAMPLES_DIR}/${BEASTHTTP_EXAMPLE_NAME}/*.cxx)

add_executable(${BEASTHTTP_EXAMPLE_NAME} ${HEADS} ${SRCS})

target_link_libraries(${BEASTHTTP_EXAMPLE_NAME} Boost::system Boost::thread Boost::regex pthread)

if ( NOT OPENSSL_SSL_LIBRARY)
    message(FATAL_ERROR "The OpenSSL ssl lib is not found! ")
endif()

target_link_libraries(${BEASTHTTP_EXAMPLE_NAME} OpenSSL::SSL OpenSSL::Crypto)
