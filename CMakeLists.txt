cmake_minimum_required(VERSION 3.0)

project(pthreadpool)

option(PTHREADPOOL_TESTS "Build test suite" OFF)

add_library(pthreadpool STATIC pthreadpool.c)

if(WIN32)
add_subdirectory(pthread-win32)
set(PTHREAD_LIB pthreadVC3)
else()
set(PTHREAD_LIB pthread)
endif()

target_include_directories(pthreadpool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(pthreadpool PUBLIC ${PTHREAD_LIB})

if(PTHREADPOOL_TESTS)
include(CTest)
enable_testing()
add_subdirectory(test)
endif()