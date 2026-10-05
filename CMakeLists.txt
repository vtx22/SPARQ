cmake_minimum_required(VERSION 3.21)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5) # lets SFML 2.5.1 configure on new CMake

project(SPARQ LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)

if (WIN32)
    set(SFML_STATIC_LIBRARIES TRUE CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS FALSE CACHE BOOL "" FORCE)
else ()
    set(BUILD_SHARED_LIBS TRUE CACHE BOOL "" FORCE)
endif ()

FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.5.1
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)

FetchContent_Declare(SPARQ-Protocol
        GIT_REPOSITORY https://github.com/vtx22/SPARQ-Protocol.git
        GIT_TAG v2.0.1
        GIT_SHALLOW ON)

FetchContent_MakeAvailable(SFML SPARQ-Protocol)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(src)