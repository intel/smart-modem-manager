cmake_minimum_required(VERSION 2.8)
project(smm_netconf)

option(WITH_TESTS "Unit tests" OFF)

set(SMM_NETCONF_MAJOR_VERSION 0)
set(SMM_NETCONF_MINOR_VERSION 1)
set(SMM_NETCONF_PATCH_VERSION 0)
set(SMM_NETCONF_VERSION_STRING ${SMM_NETCONF_MAJOR_VERSION}.${SMM_NETCONF_MINOR_VERSION}.${SMM_NETCONF_PATCH_VERSION})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(WITH_TESTS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSMM_NETCONF_LOG_TO_STDOUT")
ADD_DEFINITIONS(-DSMM_NETCONF_LOG_TO_STDOUT)
endif()

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB_RECURSE smm_netconf_cpp_files src/*.cpp)
file(GLOB smm_netconf_public_h_files inc/*.h)

add_library(smm_netconf
    SHARED
    ${smm_netconf_cpp_files})
target_link_libraries(smm_netconf "pthread")
target_link_libraries(smm_netconf "smm_core")
target_link_libraries(smm_netconf "rilcore_client")

set_target_properties(smm_netconf
    PROPERTIES
    VERSION ${SMM_NETCONF_VERSION_STRING}
    SOVERSION ${SMM_NETCONF_VERSION_STRING})
install(TARGETS smm_netconf LIBRARY DESTINATION lib)

