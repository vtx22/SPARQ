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
    # Match the static linking you had before on Windows
    set(SFML_STATIC_LIBRARIES TRUE  CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS     FALSE CACHE BOOL "" FORCE)
else()
    set(BUILD_SHARED_LIBS     TRUE  CACHE BOOL "" FORCE)
endif()

add_subdirectory(sfml-source)

# ─────────────────────────────────────────────
#  Collect sources
# ─────────────────────────────────────────────
file(GLOB OWN_SOURCES    CONFIGURE_DEPENDS "./*.cpp")
file(GLOB IMGUI_SOURCES  CONFIGURE_DEPENDS "imgui/*.cpp")
file(GLOB IMSFML_SOURCES CONFIGURE_DEPENDS "imgui-sfml/*.cpp")
file(GLOB IMPLOT_SOURCES CONFIGURE_DEPENDS "implot/*.cpp")
file(GLOB IMSTYLE_SOURCES CONFIGURE_DEPENDS "ImStyles/*.cpp")

set(ALL_SOURCES
    ${OWN_SOURCES}
    ${IMGUI_SOURCES}
    ${IMSFML_SOURCES}
    ${IMPLOT_SOURCES}
    ${IMSTYLE_SOURCES}
)

# ─────────────────────────────────────────────
#  Platform-specific sources & definitions
# ─────────────────────────────────────────────
if(WIN32)
    file(GLOB SERIAL_SOURCES CONFIGURE_DEPENDS "cpp-serial-win/src/*.cpp")
    list(APPEND ALL_SOURCES ${SERIAL_SOURCES})

    # Windows resource file (icon, version info, etc.)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SPARQ.res")
        list(APPEND ALL_SOURCES SPARQ.res)
    endif()
endif()

if(UNIX AND NOT APPLE)
    file(GLOB SERIAL_SOURCES CONFIGURE_DEPENDS "cpp-serial-lin/src/*.cpp")
    list(APPEND ALL_SOURCES ${SERIAL_SOURCES})
endif()

# ─────────────────────────────────────────────
#  Executable
# ─────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

# ─────────────────────────────────────────────
#  Include directories
# ─────────────────────────────────────────────
target_include_directories(${PROJECT_NAME} PRIVATE
    .
    imgui
    imgui-sfml
    implot
    ImStyles
)

if(WIN32)
    target_include_directories(${PROJECT_NAME} PRIVATE cpp-serial-win/src)
endif()

if(UNIX AND NOT APPLE)
    target_include_directories(${PROJECT_NAME} PRIVATE cpp-serial-lin/src)
endif()

# ─────────────────────────────────────────────
#  Compile definitions
# ─────────────────────────────────────────────
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        BUILD_WINDOWS
        IMGUI_SFML_VIEWPORTS_ENABLE
        SFML_STATIC
        SFML_USE_STATIC_STD_LIBS
    )
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE BUILD_LINUX)
endif()

# ─────────────────────────────────────────────
#  Link libraries
# ─────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        opengl32
        gdi32
        winmm
        dwmapi
    )

    # Static C/C++ runtimes – mirror -static-libstdc++ -static-libgcc
    target_link_options(${PROJECT_NAME} PRIVATE
        -static-libstdc++
        -static-libgcc
        --static
    )

    # Release: hide the console window (-mwindows)
    if(SPARQ_RELEASE)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            WIN32_EXECUTABLE TRUE
        )
    endif()
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()

# ─────────────────────────────────────────────
#  Compiler warnings  (mirrors -Wall)
# ─────────────────────────────────────────────
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)