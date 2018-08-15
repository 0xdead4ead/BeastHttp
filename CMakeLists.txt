cmake_minimum_required(VERSION 2.8)

project(beast_http_server)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex1_server")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex2_client")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex3_server")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex4_server")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex5_json_rpc_server")
add_subdirectory("${PROJECT_SOURCE_DIR}/examples/ex6_json_rpc_client")

