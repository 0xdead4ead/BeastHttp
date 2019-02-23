cmake_minimum_required(VERSION 3.11)

find_package(Boost 1.66 COMPONENTS system thread regex)

set(BEASTHTTP_EXAMPLE_NAME reactor)

include_directories(${BEASTHTTP_INCLUDE_PATH})
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB BEASTHTTP_HEADS ${BEASTHTTP_INCLUDE_PATH}/*.hxx)
file(GLOB BEASTHTTP_REACTOR_HEADS ${BEASTHTTP_INCLUDE_PATH}/reactor/*.hxx)
file(GLOB BEASTHTTP_REACTOR_SSL_HEADS ${BEASTHTTP_INCLUDE_PATH}/reactor/ssl/*.hxx)

set(SRCS main.cxx)

add_executable(${BEASTHTTP_EXAMPLE_NAME} ${SRCS} ${BEASTHTTP_HEADS} ${BEASTHTTP_BASE_HEADS} ${BEASTHTTP_REACTOR_HEADS} ${BEASTHTTP_REACTOR_SSL_HEADS})

target_link_libraries(${BEASTHTTP_EXAMPLE_NAME} Boost::system Boost::thread Boost::regex pthread icui18n)
