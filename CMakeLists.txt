cmake_minimum_required(VERSION 3.11)

project(beast_http_server VERSION 102)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

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
