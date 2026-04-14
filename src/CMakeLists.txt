# ─────────────────────────────────────────────
#  Collect sources
# ─────────────────────────────────────────────
file(GLOB OWN_SOURCES     CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB IMGUI_SOURCES   CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp")
file(GLOB IMSFML_SOURCES  CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/imgui-sfml/*.cpp")
file(GLOB IMPLOT_SOURCES  CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/implot/*.cpp")
file(GLOB IMSTYLE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ImStyles/*.cpp")

set(ALL_SOURCES
    ${OWN_SOURCES}
    ${IMGUI_SOURCES}
    ${IMSFML_SOURCES}
    ${IMPLOT_SOURCES}
    ${IMSTYLE_SOURCES}
)

# ─────────────────────────────────────────────
#  Platform-specific sources
# ─────────────────────────────────────────────
if(WIN32)
    file(GLOB SERIAL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/cpp-serial-win/src/*.cpp")
    list(APPEND ALL_SOURCES ${SERIAL_SOURCES})

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SPARQ.res")
        list(APPEND ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/SPARQ.res")
    endif()
endif()

if(UNIX AND NOT APPLE)
    file(GLOB SERIAL_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/cpp-serial-lin/src/*.cpp")
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
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui-sfml
    ${CMAKE_CURRENT_SOURCE_DIR}/implot
    ${CMAKE_CURRENT_SOURCE_DIR}/ImStyles
)

if(WIN32)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp-serial-win/src
    )
endif()

if(UNIX AND NOT APPLE)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp-serial-lin/src
    )
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

    target_link_options(${PROJECT_NAME} PRIVATE
        -static-libstdc++
        -static-libgcc
        --static
    )

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
#  Compiler warnings
# ─────────────────────────────────────────────
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/src/config.ini
        ${CMAKE_SOURCE_DIR}/src/imgui.ini
        $<TARGET_FILE_DIR:${PROJECT_NAME}>

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets

    COMMENT "Copying runtime assets..."
)