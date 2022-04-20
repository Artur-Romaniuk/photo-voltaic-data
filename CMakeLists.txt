# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(photo-voltaic-data)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

############################################################
set(FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -Wno-unused-parameter CACHE INTERNAL "")
set(CPP_FLAGS
    -fno-rtti -fno-exceptions -fno-threadsafe-statics
    -Wno-volatile
    -Wsuggest-override CACHE INTERNAL "")
set(DEBUG_FLAGS
    -Og
    -g3
    -ggdb CACHE INTERNAL "")
set(RELEASE_FLAGS
    -Og
    -g0 CACHE INTERNAL "")

idf_build_set_property(COMPILE_OPTIONS ${FLAGS} $<$<COMPILE_LANGUAGE:CXX>:${CPP_FLAGS} $<$<CONFIG:Debug>:${DEBUG_FLAGS}> $<$<CONFIG:Release>:${RELEASE_FLAGS}> APPEND)    
############################################################
