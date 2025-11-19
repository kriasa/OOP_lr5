cmake_minimum_required(VERSION 3.10)
project(5_laba VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) # Используем C++17, как договаривались
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_NO_PROGRESS TRUE
    DOWNLOAD_TIMEOUT 600

    DOWNLOAD_EXTRACT_TIMESTAMP TRUE 
)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME} main.cpp pmr_stack.h)

enable_testing()
add_executable(test_pmr_stack tests/test_pmr_stack.cpp pmr_stack.h)

target_link_libraries(test_pmr_stack PRIVATE GTest::gtest_main)
add_test(NAME RunPMRStackTests COMMAND test_pmr_stack)