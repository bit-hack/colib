
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

FILE(GLOB SOURCE_FILES "*.cpp")
FILE(GLOB HEADER_FILES "*.h")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
    SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
    SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
endif()

# thist must suffice for cmake <= 2.8
include_directories("${CMAKE_SOURCE_DIR}/colib")

add_executable(colib_tests ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(colib_tests colib)
