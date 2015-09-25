project(cborpp_text VERSION 1.0.0)
cmake_minimum_required(VERSION 3.1)

if (TARGET cborpp)
    # Part of the main project
    add_library(cborpp::cborpp ALIAS cborpp)
else()
    # Stand-alone build
    find_package(cborpp CONFIG REQUIRED)
endif ()

enable_testing()
find_package(GTest)
add_subdirectory(../contrib/googletest/googletest gtest)

function(create_test NAME)
    add_executable(test_${NAME} test_${NAME}.cpp)
    target_link_libraries(test_${NAME} gtest gtest_main cborpp)
    GTEST_ADD_TESTS(test_${NAME} 0 test_${NAME}.cpp)
endfunction(create_test)

create_test(packer)
create_test(objectpacker)
create_test(objectstreampacker)