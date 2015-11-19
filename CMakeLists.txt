cmake_minimum_required(VERSION 2.8)
project(colib)

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
endif()

add_subdirectory(colib)
add_subdirectory(tests)
