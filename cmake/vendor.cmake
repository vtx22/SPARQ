function(add_vendored_library name dir)
    file(GLOB srcs CONFIGURE_DEPENDS "${dir}/*.cpp")
    # STATIC explicitly: BUILD_SHARED_LIBS is forced TRUE on Linux in the root file
    add_library(${name} STATIC ${srcs})
    target_include_directories(${name} SYSTEM PUBLIC "${dir}")

    if (WIN32)
        target_compile_definitions(${name} PUBLIC
                SFML_STATIC NOMINMAX WIN32_LEAN_AND_MEAN)
    endif ()
endfunction()

set(V "${PROJECT_SOURCE_DIR}/src")

add_vendored_library(imgui "${V}/imgui")
add_vendored_library(implot "${V}/implot")
add_vendored_library(imstyles "${V}/ImStyles")
add_vendored_library(imgui_sfml "${V}/imgui-sfml")

# imgui's imconfig.h needs SFML headers and imgui-SFML_export.h
target_link_libraries(imgui PUBLIC sfml-graphics sfml-window sfml-system)
target_include_directories(imgui SYSTEM PUBLIC "${V}/imgui-sfml")

target_link_libraries(implot PUBLIC imgui)
target_link_libraries(imstyles PUBLIC implot)   # implot already pulls in imgui
target_link_libraries(imgui_sfml PUBLIC imgui)
# Serial backend (platform specific)
if (WIN32)
    add_vendored_library(serial "${V}/cpp-serial-win/src")
elseif (UNIX AND NOT APPLE)
    add_vendored_library(serial "${V}/cpp-serial-lin/src")
endif ()

# Platform GL / windowing libs needed by imgui-sfml
if (WIN32)
    target_compile_definitions(imgui_sfml PUBLIC IMGUI_SFML_VIEWPORTS_ENABLE)
    target_link_libraries(imgui_sfml PUBLIC opengl32 gdi32 dwmapi)
elseif (UNIX AND NOT APPLE)
    target_link_libraries(imgui_sfml PUBLIC GL)
endif ()