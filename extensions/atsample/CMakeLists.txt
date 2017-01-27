cmake_minimum_required(VERSION 2.8)
project(smm_atsample)

option(WITH_TESTS "Unit tests" OFF)

set(SMM_ATSAMPLE_MAJOR_VERSION 0)
set(SMM_ATSAMPLE_MINOR_VERSION 1)
set(SMM_ATSAMPLE_PATCH_VERSION 0)
set(SMM_ATSAMPLE_VERSION_STRING ${SMM_ATSAMPLE_MAJOR_VERSION}.${SMM_ATSAMPLE_MINOR_VERSION}.${SMM_ATSAMPLE_PATCH_VERSION})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(WITH_TESTS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSMM_ATSAMPLE_LOG_TO_STDOUT")
ADD_DEFINITIONS(-DSMM_ATSAMPLE_LOG_TO_STDOUT)
endif()

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB_RECURSE smm_atsample_cpp_files src/*.cpp)
file(GLOB smm_atsample_public_h_files inc/*.h)

add_library(smm_atsample
    SHARED
    ${smm_atsample_cpp_files})
target_link_libraries(smm_atsample "-pthread")
target_link_libraries(smm_atsample "smm_core")
target_link_libraries(smm_atsample "rilcore_client")

set_target_properties(smm_atsample
    PROPERTIES
    VERSION ${SMM_ATSAMPLE_VERSION_STRING}
    SOVERSION ${SMM_ATSAMPLE_VERSION_STRING})
install(TARGETS smm_atsample LIBRARY DESTINATION lib)

