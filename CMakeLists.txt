cmake_minimum_required(VERSION 3.0.2)
project (packed_strings)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set(CXX_STANDARD c++17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -pedantic -march=native -std=${CXX_STANDARD} ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=native")


## lcp library

add_executable  (precalc_lcp precalc_lcp.cpp)
# target_link_libraries(precalc_lcp)

add_executable  (plot_lcp precalc_lcp_plot.cpp)
# target_link_libraries(precalc_lcp_plot)

add_custom_command (
	OUTPUT "${CMAKE_BINARY_DIR}/lcp_gen.cpp"
	COMMENT "calculate fastest longest common prefix function for each string length..."
	COMMAND precalc_lcp "${CMAKE_BINARY_DIR}/lcp_gen.cpp"
	DEPENDS precalc_lcp
)

add_library(packed_lcp SHARED
	lcp.cpp 
	"${CMAKE_BINARY_DIR}/lcp_gen.cpp"
	)
target_include_directories(packed_lcp PUBLIC ${CMAKE_SOURCE_DIR})

install(TARGETS packed_lcp LIBRARY DESTINATION lib)
install(FILES lcp.hpp DESTINATION include)



## BENCHMARKS
set(NO_CELERO 0)
find_path(CELERO_INCLUDE_DIR celero/Celero.h)
find_library(CELERO_LIB libcelero.so)
if(CELERO_INCLUDE_DIR STREQUAL "CELERO_INCLUDE_DIR-NOTFOUND" OR CELERO_LIB_DIR STREQUAL "CELERO_LIB-NOTFOUND")
	set(NO_CELERO 1)
	add_definitions(-DNO_CELERO=1)
else()
	include_directories(${CELERO_INCLUDE_DIR})
endif()

if(NO_CELERO EQUAL 0)
	add_executable(bench_lcp bench_lcp.cpp lcp.cpp)
	target_link_libraries(bench_lcp ${CELERO_LIB})
endif()


## TESTS

find_package(GTest)
include_directories(${GTEST_INCLUDE_DIR})

add_executable  (test_lcp test_lcp.cpp lcp.cpp)
add_executable  (test_packed_string test_packed_string.cpp)

target_link_libraries(test_lcp gtest pthread)
target_link_libraries(test_packed_string gtest pthread)

enable_testing()
add_test(NAME packed_string COMMAND test_packed_string)
add_test(NAME lcp COMMAND test_lcp)


MESSAGE( STATUS "With Celero?: " ${CELERO_INCLUDE_DIR} ${CELERO_LIB_DIR} )
MESSAGE( STATUS "With Gtest?: " ${GTEST_INCLUDE_DIR} )
MESSAGE( STATUS "Built Type: " ${CMAKE_BUILD_TYPE} )
MESSAGE( STATUS "CXX Flags: " ${CMAKE_CXX_FLAGS} )
