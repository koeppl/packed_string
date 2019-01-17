cmake_minimum_required(VERSION 3.0.2)
project (packed_strings)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

set(CXX_STANDARD c++17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -march=native -std=${CXX_STANDARD} ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=native")


##########
# precalculation
##########

add_executable  (precalc_lcp precalc_lcp.cpp)
# target_link_libraries(precalc_lcp)

add_custom_command (
	OUTPUT "${CMAKE_BINARY_DIR}/lcp_gen.cpp"
	COMMENT "calculate fastest longest common prefix function for each string length..."
	COMMAND precalc_lcp "${CMAKE_BINARY_DIR}/lcp_gen.cpp"
	DEPENDS precalc_lcp
)
 
##########
# library
##########

add_library(packed_strings SHARED
	lcp.cpp 
	"${CMAKE_BINARY_DIR}/lcp_gen.cpp"
	)
target_include_directories(packed_strings PUBLIC ${CMAKE_SOURCE_DIR})

install(TARGETS packed_strings LIBRARY DESTINATION lib)
install(FILES dcheck.hpp lcp.hpp character.hpp DESTINATION include/packed)


##########
# plots
##########

add_executable  (plot_lcp plot_lcp.cpp)
target_link_libraries(plot_lcp packed_strings)

##########
# examples
##########

add_executable  (example_character example_character.cpp)


##########
# BENCHMARKS
##########

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


##########
# TESTS
##########

find_package(GTest)
if(GTest_FOUND)
    include_directories(${GTEST_INCLUDE_DIR})

    add_executable  (test_lcp test_lcp.cpp lcp.cpp)
    add_executable  (test_character test_character.cpp)

    target_link_libraries(test_lcp gtest pthread)
    target_link_libraries(test_character gtest pthread)

    enable_testing()
    add_test(NAME character COMMAND test_character)
    add_test(NAME lcp COMMAND test_lcp)
endif()


MESSAGE( STATUS "With Celero?: " ${CELERO_INCLUDE_DIR} ${CELERO_LIB_DIR} )
MESSAGE( STATUS "With Gtest?: " ${GTEST_INCLUDE_DIR} )
MESSAGE( STATUS "Built Type: " ${CMAKE_BUILD_TYPE} )
MESSAGE( STATUS "CXX Flags: " ${CMAKE_CXX_FLAGS} )
