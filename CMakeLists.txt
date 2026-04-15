cmake_minimum_required(VERSION 3.21)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

project(SPARQ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.5.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# ─────────────────────────────────────────────
#  Main sources
# ─────────────────────────────────────────────
add_subdirectory(src)
