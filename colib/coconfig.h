#pragma once

#if defined(__linux) || defined(__unix__)
  #define IS_LINUX
  #if defined(__amd64__) || defined(__x86_64__)
    #define IS_X64
  #endif
  #if defined(__i386__)
    #define IS_X86
  #endif
  #if defined(__MIPSEL__)
    #define IS_MIPS
  #endif
#endif

#if defined(_MSC_VER)
  #define IS_WINDOWS
  #if defined(_WIN64) || defined(_M_X64)
    #define IS_X64
  #endif
  #if defined(_M_IX86)
    #define IS_X86
  #endif
#endif

#if defined(__APPLE__)
  #define IS_MAC
  #if defined(__amd64__) || defined(__x86_64__)
    #define IS_X64
  #endif
#endif

#if defined(IS_LINUX) && defined(IS_X64)
  #define CO_CREATE co_create_posix_x64
#endif
#if defined(IS_LINUX) && defined(IS_X86)
  #define CO_CREATE co_create_generic_x86
#endif
#if defined(IS_LINUX) && defined(IS_MIPS)
  #define CO_CREATE co_create_linux_mips
#endif

#if defined(IS_WINDOWS) && defined(IS_X64)
  #define CO_CREATE co_create_win_x64
#endif
#if defined(IS_WINDOWS) && defined(IS_X86)
  #define CO_CREATE co_create_generic_x86
#endif

#if defined(IS_MAC) && defined(IS_X64)
  #define CO_CREATE co_create_posix_x64
#endif

// throw error if CO_CREATE is not defined
#if !defined(CO_CREATE)
  #error "unsupported colib platform"
#endif

// make sure nullptr is defined
#if !defined(_MSC_VER) && __cplusplus <= 199711L
  #define nullptr 0
#endif
