cmake_minimum_required(VERSION 2.8)

find_package(Boost 1.66 COMPONENTS system thread regex)

set(OUTPUT_NAME ex2_client)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories(${Boost_INCLUDE_DIRS})
set(SOURCES
    ex2_client.cpp)
set(HEADERS
    ${PROJECT_SOURCE_DIR}/include/server.hpp
    ${PROJECT_SOURCE_DIR}/include/client.hpp
    ${PROJECT_SOURCE_DIR}/include/base.hpp
    ${PROJECT_SOURCE_DIR}/include/list_cb.hpp
    ${PROJECT_SOURCE_DIR}/include/router.hpp
    ${PROJECT_SOURCE_DIR}/include/session.hpp
    ${PROJECT_SOURCE_DIR}/include/param.hpp)

add_executable(${OUTPUT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${OUTPUT_NAME} Boost::system Boost::thread Boost::regex pthread icui18n)

