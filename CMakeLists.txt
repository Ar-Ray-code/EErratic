cmake_minimum_required(VERSION 3.20)
project(eerratic_test)

find_package(GTest REQUIRED)

# sample eerratic timer ================================================
add_executable(eerratic_timer_c
    example/eerratic_timer/c/main.c
)
target_include_directories(eerratic_timer_c
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(eerratic_timer_cpp
    example/eerratic_timer/cpp/main.cpp
)
target_include_directories(eerratic_timer_cpp
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_library(eerratic_timer_class
    src/eerratic_timer_class.cpp
)
target_include_directories(eerratic_timer_class
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(eerratic_timer_class_cpp
    example/eerratic_timer_class/cpp/main.cpp
)
target_include_directories(eerratic_timer_class_cpp
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(eerratic_timer_class_cpp
    eerratic_timer_class
)

# Test ================================================================
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
