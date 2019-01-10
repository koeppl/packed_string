cmake_minimum_required(VERSION 3.0.2)
project (packed_strings)

#my own profile
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /home/niki/opt/lib)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} /home/niki/opt/include)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -I/home/niki/opt/include" )
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -L/home/niki/opt/lib" )


set(CXX_STANDARD c++17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -pedantic -march=native -std=${CXX_STANDARD} ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=native")

include(ExternalProject)

set(NO_CELERO 0)
find_path(CELERO_INCLUDE_DIR celero/Celero.h)
if(CELERO_INCLUDE_DIR STREQUAL "CELERO_INCLUDE_DIR-NOTFOUND")
	set(NO_CELERO 1)
	add_definitions(-DNO_CELERO=1)
endif()

# find_package(glog)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})


if(NO_CELERO EQUAL 0)
add_executable(bench_lce bench_lce.cpp)
# add_executable(bench_packed_string bench_packed_string.cpp packed_string.cpp)

target_link_libraries(bench_lce glog celero)
# target_link_libraries(bench_packed_string glog celero)
endif()


add_executable  (test_lce test_lce.cpp)
add_executable  (test_packed_string test_packed_string.cpp)

target_link_libraries(test_lce glog gtest pthread)
target_link_libraries(test_packed_string glog gtest pthread)

enable_testing()
add_test(NAME packed_string COMMAND test_packed_string)
add_test(NAME lce COMMAND test_lce)


MESSAGE( STATUS "With Celero?: " ${CELERO_INCLUDE_DIR} )
MESSAGE( STATUS "Built Type: " ${CMAKE_BUILD_TYPE} )
MESSAGE( STATUS "CXX Flags: " ${CMAKE_CXX_FLAGS} )
