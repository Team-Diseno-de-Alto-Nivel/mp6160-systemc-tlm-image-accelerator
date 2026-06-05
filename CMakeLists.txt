cmake_minimum_required(VERSION 3.16)
project(systemc_image_processor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use a pre-installed SystemC when SYSTEMC_HOME is set.
# Otherwise, fetch and build SystemC 2.3.4 automatically.
if(DEFINED ENV{SYSTEMC_HOME})
    message(STATUS "Using SystemC from SYSTEMC_HOME=$ENV{SYSTEMC_HOME}")
    set(SYSTEMC_HOME $ENV{SYSTEMC_HOME})
    include_directories(${SYSTEMC_HOME}/include)
    find_library(SYSTEMC_LIB
        NAMES systemc
        PATHS ${SYSTEMC_HOME}/lib ${SYSTEMC_HOME}/lib-linux64 ${SYSTEMC_HOME}/lib-macosx64
        REQUIRED)
    add_executable(sim src/main.cpp)
    target_link_libraries(sim ${SYSTEMC_LIB})
else()
    message(STATUS "SYSTEMC_HOME not set — downloading SystemC 2.3.4 via FetchContent (requires internet)")
    include(FetchContent)
    set(DISABLE_ASYNC_UPDATES    ON  CACHE INTERNAL "")
    set(BUILD_SOURCE_DOCUMENTATION OFF CACHE INTERNAL "")
    FetchContent_Declare(
        SystemC
        URL https://github.com/accellera-official/systemc/archive/refs/tags/2.3.4.tar.gz
    )
    FetchContent_MakeAvailable(SystemC)
    add_executable(sim src/main.cpp)
    target_link_libraries(sim SystemC::systemc)
endif()
