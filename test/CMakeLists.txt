cmake_minimum_required(VERSION 3.5)
project(test)
enable_language(CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "valid configurations" FORCE)

find_package(Boost REQUIRED)
if(NOT Boost_FOUND)
	message(FATAL_ERROR "Failed to find boost library")
endif()

include_directories(../include)
include_directories(${Boost_INCLUDE_DIRS})

set(SOURCE_FILES
	../include/ww898/cp_utf8.hpp
	../include/ww898/cp_utf16.hpp
	../include/ww898/cp_utf32.hpp
	../include/ww898/cp_utfw.hpp
	../include/ww898/utf_config.hpp
	../include/ww898/utf_selector.hpp
	../include/ww898/utf_sizes.hpp
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
                /W3
		"$<$<CONFIG:Release>:/GL>"
		"$<$<CONFIG:Release>:/Ox>"
		"$<$<CONFIG:Release>:/Ob2>"
		"$<$<CONFIG:Release>:/Ot>"
		"$<$<CONFIG:Release>:/Oi>"
		"$<$<CONFIG:Release>:/Oy->")

	if(MSVC_VERSION MATCHES "^19[1-9][0-9]$")
		target_compile_options(utf-cpp-test PRIVATE /std:c++17)
	elseif(MSVC_VERSION STREQUAL 1900)
		target_compile_options(utf-cpp-test PRIVATE /std:c++14)
	elseif(MSVC_VERSION STREQUAL 1800)
	else()
		message(FATAL_ERROR "Unknown Microsoft Visual C++ compiler version ${MSVC_VERSION}")
	endif()

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(utf-cpp-test PRIVATE
		-std=c++11
		-Wall
		-Wextra
		-Wno-unused-parameter)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_compile_options(utf-cpp-test PRIVATE
		-std=c++11
		-stdlib=libc++
		-Wall
		-Wextra
		-Wno-unused-parameter)
	set_target_properties(utf-cpp-test PROPERTIES LINK_FLAGS -stdlib=libc++)
endif()
