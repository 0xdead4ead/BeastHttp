cmake_minimum_required(VERSION 3.11)

find_package(Boost 1.70 COMPONENTS unit_test_framework REQUIRED)

set(BEASTHTTP_CHAIN_ROUTER_TEST_NAME chain_router)

set(SRCS main.cxx)

add_executable(${BEASTHTTP_CHAIN_ROUTER_TEST_NAME} ${SRCS})

target_link_libraries(${BEASTHTTP_CHAIN_ROUTER_TEST_NAME} Boost::system Boost::thread
    Boost::unit_test_framework pthread)

if(BEASTHTTP_BUILD_STATIC_LIBS)
    target_link_libraries(${BEASTHTTP_CHAIN_ROUTER_TEST_NAME} asio beast)
endif()

add_test (NAME ${BEASTHTTP_CHAIN_ROUTER_TEST_NAME} COMMAND "${BEASTHTTP_CHAIN_ROUTER_TEST_NAME}" "--log_level=test_suite")

add_definitions(-DBEASTHTTP_TEST_ROUTER)
