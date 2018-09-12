cmake_minimum_required(VERSION 2.8)
project(test)
enable_language(CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "valid configurations" FORCE)

find_package(Boost REQUIRED)

include_directories(../include)
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES
	../include/ww898/utf_converters.hpp
	utf_converters_test.cpp)

add_executable(utf-cpp-test ${SOURCE_FILES})

target_compile_definitions(utf-cpp-test PRIVATE
	BOOST_ALL_NO_LIB
	BOOST_TEST_MODULE=unit-cpp
	WW898_BOOST_TEST_INCLUDED
	WW898_ENABLE_PERFORMANCE_TESTS)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_definitions(utf-cpp-test PRIVATE
		_SCL_SECURE_NO_WARNINGS)
	target_compile_options(utf-cpp-test PRIVATE
		"$<$<CONFIG:Release>:/GL>"
		"$<$<CONFIG:Release>:/Ox>"
		"$<$<CONFIG:Release>:/Ob2>"
		"$<$<CONFIG:Release>:/Ot>"
		"$<$<CONFIG:Release>:/Oi>"
		"$<$<CONFIG:Release>:/Oy->")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(utf-cpp-test PRIVATE -std=c++11 -Wall -Wextra)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_compile_options(utf-cpp-test PRIVATE -std=c++11 -stdlib=libc++ -Wall -Wextra)
	set_target_properties(utf-cpp-test PROPERTIES LINK_FLAGS -stdlib=libc++)
endif()
