project(unicbor_test VERSION 1.0.0)
cmake_minimum_required(VERSION 3.1)

if (TARGET unicbor)
	# Part of the main project
	add_library(unicbor::unicbor ALIAS unicbor)
else()
	# Stand-alone build
	find_package(unicbor CONFIG REQUIRED)
endif ()

enable_testing()
find_package(GTest)
add_subdirectory(../contrib/googletest/googletest gtest)

function(create_test NAME)
	add_executable(test_${NAME} test_${NAME}.cpp)
	target_link_libraries(test_${NAME} gtest gtest_main unicbor)
	GTEST_ADD_TESTS(test_${NAME} 0 test_${NAME}.cpp)
endfunction(create_test)

create_test(packer)
create_test(objectpacker)
create_test(objectunpacker)
create_test(objectprinter)