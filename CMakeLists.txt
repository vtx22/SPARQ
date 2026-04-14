cmake_minimum_required(VERSION 3.21)
project(SPARQ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─────────────────────────────────────────────
#  Options
# ─────────────────────────────────────────────
option(SPARQ_RELEASE "Strip console window on Windows (release build)" OFF)

# ─────────────────────────────────────────────
#  Build SFML from source
# ─────────────────────────────────────────────
set(SFML_BUILD_AUDIO     OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK   OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC       OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUIT OFF CACHE BOOL "" FORCE)

if(WIN32)
    set(SFML_STATIC_LIBRARIES TRUE  CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS     FALSE CACHE BOOL "" FORCE)
else()
    set(BUILD_SHARED_LIBS     TRUE  CACHE BOOL "" FORCE)
endif()

add_subdirectory(src/sfml-source)

# ─────────────────────────────────────────────
#  Main sources
# ─────────────────────────────────────────────
add_subdirectory(src)
