cmake_minimum_required(VERSION 3.11)

find_package(Boost 1.67 COMPONENTS unit_test_framework REQUIRED)

set(BEASTHTTP_DISPLAY_TEST_NAME display)

set(SRCS main.cxx)

add_executable(${BEASTHTTP_DISPLAY_TEST_NAME} ${SRCS})

target_link_libraries(${BEASTHTTP_DISPLAY_TEST_NAME} Boost::system Boost::thread
    Boost::unit_test_framework pthread)

add_test (NAME ${BEASTHTTP_DISPLAY_TEST_NAME} COMMAND "${BEASTHTTP_DISPLAY_TEST_NAME}" "--log_level=test_suite")
