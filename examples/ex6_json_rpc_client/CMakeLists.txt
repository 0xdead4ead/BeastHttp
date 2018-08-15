cmake_minimum_required(VERSION 2.8)

find_package(Boost 1.66 COMPONENTS system thread regex filesystem)

set(OUTPUT_NAME ex6_json_rpc_client)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/examples/${OUTPUT_NAME}")
include_directories(${Boost_INCLUDE_DIRS})
set(SOURCES
    ex6_json_rpc_client.cpp)
set(HEADERS
    ${PROJECT_SOURCE_DIR}/include/server.hpp
    ${PROJECT_SOURCE_DIR}/include/client.hpp
    ${PROJECT_SOURCE_DIR}/include/base.hpp
    ${PROJECT_SOURCE_DIR}/include/list_cb.hpp
    ${PROJECT_SOURCE_DIR}/include/router.hpp
    ${PROJECT_SOURCE_DIR}/include/session.hpp)

add_executable(${OUTPUT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${OUTPUT_NAME} Boost::system Boost::thread Boost::regex Boost::filesystem pthread icui18n)

