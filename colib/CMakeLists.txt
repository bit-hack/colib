cmake_minimum_required(VERSION 2.8)
project(colib)

set(COLIB_TARGET "target" CACHE STRING "Set CoLib target processor and ABI")
set_property(CACHE COLIB_TARGET PROPERTY STRINGS 
  WIN_x86
  WIN_x86_64
  MAC_AMD64
  LINUX_x86
  LINUX_AMD64)

set(SOURCE_FILES colib.cpp coconfig.h colib.h)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  enable_language(ASM_MASM)

  if (${COLIB_TARGET} STREQUAL "WIN_x86_64")
    message(STATUS "colib target is MSVC x64")
    set(ASM_FILES colib_masm_win_x64.asm)
  endif()

  if (${COLIB_TARGET} STREQUAL "WIN_x86")
    message(STATUS "colib target is MSVC x86")
    set(ASM_FILES colib_masm_win_x86.asm)
    SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    /SAFESEH:NO")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
  endif()
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  enable_language(C ASM)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

  if(${COLIB_TARGET} STREQUAL "LINUX_ARMV7A")
    set(ASM_FILES colib_gas_linux_arm32.s)
    message(STATUS "colib target is arm32")
  endif()

  if(${COLIB_TARGET} STREQUAL "LINUX_AMD64")
    set(ASM_FILES colib_gas_linux_x64.s)
    message(STATUS "colib target is linux x64")
  endif()

  if(${COLIB_TARGET} STREQUAL "LINUX_x86")
    message(STATUS "colib target is LINUX x86")
    set(ASM_FILES colib_gas_linux_x86.s)
  endif()

  if(${COLIB_TARGET} STREQUAL "MAC_AMD64")
    set(ASM_FILES colib_gas_mac_x64.s)
    message(STATUS "colib target is Mac OS X x64")
  endif()
endif()

if(NOT ASM_FILES)
  message(FATAL_ERROR "COLIB_TARGET must be specified")
endif()

SET(ASM_OPTIONS "-x assembler-with-cpp")
SET(CMAKE_ASM_FLAGS "${CFLAGS} ${ASM_OPTIONS}")

add_library(colib ${SOURCE_FILES} ${ASM_FILES})

if(${COLIB_TARGET} MATCHES LINUX_x86)
  set_target_properties(colib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
endif()

# this way is good for cmake > 2.8
#target_include_directories(colib PUBLIC "${CURRENT_SOURCE_DIR}")
