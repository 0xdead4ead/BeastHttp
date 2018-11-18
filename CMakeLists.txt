cmake_minimum_required(VERSION 3.11)

project(BeastHttp VERSION 106)

if(NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS -std=c++14)
endif()

message("CXX=" ${CMAKE_CXX_COMPILER})
message("CXXFLAGS=" ${CMAKE_CXX_FLAGS})

add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex1_server")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex2_client")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex3_server")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex4_server")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex5_json_rpc_server")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex6_json_rpc_client")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex7_ssl_server")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex8_ssl_client")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex9_sync_client")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex10_sync_ssl_client")
