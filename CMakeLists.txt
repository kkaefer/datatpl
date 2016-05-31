cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(datatpl LANGUAGES CXX C)
include(cmake/mason.cmake)

mason_use(variant 1.1.0)
mason_use(geometry 0.7.0)

include_directories(${variant_INCLUDE_DIRS})
include_directories(${geometry_INCLUDE_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
