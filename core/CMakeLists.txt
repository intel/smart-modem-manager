cmake_minimum_required(VERSION 2.8)
project(smm_core)

option(WITH_TESTS "Unit tests" OFF)

set(SMMCORE_MAJOR_VERSION 0)
set(SMMCORE_MINOR_VERSION 1)
set(SMMCORE_PATCH_VERSION 0)
set(SMMCORE_VERSION_STRING ${SMMCORE_MAJOR_VERSION}.${SMMCORE_MINOR_VERSION}.${SMMCORE_PATCH_VERSION})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror")

if(WITH_TESTS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSMMCORE_LOG_TO_STDOUT")
ADD_DEFINITIONS(-DSMMCORE_LOG_TO_STDOUT)
endif()

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(inc)
include_directories(inc/internal)

file(GLOB_RECURSE smm_core_cpp_files src/*.cpp)
file(GLOB_RECURSE smm_core_public_h_files inc/*.h)

add_library(smm_core
    SHARED
    ${smm_core_cpp_files})
target_link_libraries(smm_core "-pthread")
target_link_libraries(smm_core ${CMAKE_DL_LIBS})

set_target_properties(smm_core
    PROPERTIES
    VERSION ${SMMCORE_VERSION_STRING}
    SOVERSION ${SMMCORE_VERSION_STRING})
install(TARGETS smm_core LIBRARY DESTINATION lib)
install(FILES ${smm_core_public_h_files}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/smm_core)

