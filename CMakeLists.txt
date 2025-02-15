cmake_minimum_required(VERSION 3.20)
project(eerratic_test)

find_package(GTest REQUIRED)

add_executable(eerratic_test
    test/test_eerratic_timer.cpp
)

target_include_directories(eerratic_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${GTEST_INCLUDE_DIRS}
)

target_link_libraries(eerratic_test
    GTest::GTest
    pthread
)

target_compile_features(
    eerratic_test
    PRIVATE
    cxx_std_17
)

enable_testing()
add_test(NAME eerratic_test COMMAND eerratic_test)
