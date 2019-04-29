cmake_minimum_required(VERSION 3.11)

find_package(Boost 1.66 COMPONENTS system thread regex)

set(BEASTHTTP_EXAMPLE_NAME reactor_timers)

include_directories(${BEASTHTTP_INCLUDE_PATH})
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB HEADS ${BEASTHTTP_EXAMPLES_DIR}/${BEASTHTTP_EXAMPLE_NAME}/*.hxx)
file(GLOB SRCS ${BEASTHTTP_EXAMPLES_DIR}/${BEASTHTTP_EXAMPLE_NAME}/*.cxx)

add_executable(${BEASTHTTP_EXAMPLE_NAME} ${HEADS} ${SRCS})

target_link_libraries(${BEASTHTTP_EXAMPLE_NAME} Boost::system Boost::thread Boost::regex pthread)
