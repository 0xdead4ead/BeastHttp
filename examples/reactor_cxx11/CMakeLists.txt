cmake_minimum_required(VERSION 3.11)

find_package(Boost 1.66 COMPONENTS system thread regex)

set(BEASTHTTP_EXAMPLE_NAME reactor_cxx11)

include_directories(${BEASTHTTP_INCLUDE_PATH})
include_directories(${Boost_INCLUDE_DIRS})

set(SRCS main.cxx)

add_executable(${BEASTHTTP_EXAMPLE_NAME} ${SRCS})

target_link_libraries(${BEASTHTTP_EXAMPLE_NAME} Boost::system Boost::thread Boost::regex pthread)
