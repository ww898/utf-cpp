cmake_minimum_required(VERSION 2.8)
project(test)
enable_language(CXX)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "valid configurations" FORCE)

find_package(Boost REQUIRED COMPONENTS
	unit_test_framework)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(../include)

set(SOURCE_FILES
	main.cpp
	utf_converters_test.cpp)

add_executable(utf-cpp-test ${SOURCE_FILES})

target_compile_definitions(utf-cpp-test PRIVATE
	BOOST_ALL_NO_LIB)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(utf-cpp-test PRIVATE
		"$<$<CONFIG:Release>:/GL>"
		"$<$<CONFIG:Release>:/Ox>"
		"$<$<CONFIG:Release>:/Ob2>"
		"$<$<CONFIG:Release>:/Ot>"
		"$<$<CONFIG:Release>:/Oi>"
		"$<$<CONFIG:Release>:/Oy->")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(utf-cpp-test PRIVATE -std=c++11)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	target_compile_options(utf-cpp-test PRIVATE -std=c++11 -stdlib=libc++)
	set_target_properties(utf-cpp-test PROPERTIES LINK_FLAGS -stdlib=libc++)
endif()

target_link_libraries(utf-cpp-test
	libboost_unit_test_framework${CMAKE_STATIC_LIBRARY_SUFFIX})
