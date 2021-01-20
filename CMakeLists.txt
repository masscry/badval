cmake_minimum_required(VERSION 3.10)

project(badval
  VERSION 1.0
  DESCRIPTION "Small multivalue library"
  LANGUAGES CXX
)

add_library(setup INTERFACE)
target_compile_features(setup INTERFACE cxx_std_14)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  target_compile_options(setup INTERFACE --coverage)
  target_link_libraries(setup INTERFACE --coverage)
endif()

add_library(badval INTERFACE)

target_sources(badval INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/badval.hpp>
)

target_include_directories(badval INTERFACE include)
target_link_libraries(badval INTERFACE setup)

add_executable(badtest
  test/badtest.cpp
)

target_link_libraries(badtest PRIVATE badval setup)
