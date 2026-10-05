include(vendor)

file(GLOB OWN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if (WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/SPARQ.res")
    list(APPEND OWN_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/SPARQ.res")
endif ()

add_executable(SPARQ ${OWN_SOURCES})
add_subdirectory(windows)

target_include_directories(SPARQ PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_precompile_headers(SPARQ PRIVATE pch.hpp)

target_link_libraries(SPARQ PRIVATE
        imgui_sfml
        implot
        imstyles
        serial
        sfml-graphics sfml-window sfml-system)

if (TARGET SPARQ-Protocol)
    target_link_libraries(SPARQ PRIVATE SPARQ-Protocol)
else ()
    target_include_directories(SPARQ SYSTEM PRIVATE "${sparq-protocol_SOURCE_DIR}")
endif ()

target_compile_definitions(SPARQ PRIVATE
        $<$<CONFIG:Debug>:SPARQ_DEBUG_BUILD>
        $<$<NOT:$<CONFIG:Debug>>:SPARQ_RELEASE_BUILD>
        $<$<BOOL:${WIN32}>:SPARQ_WINDOWS_BUILD>
        $<$<AND:$<BOOL:${UNIX}>,$<NOT:$<BOOL:${APPLE}>>>:BUILD_LINUX>)

target_compile_options(SPARQ PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>)

if (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(SPARQ PRIVATE -static -Wl,--allow-multiple-definition)
endif ()

add_custom_command(TARGET SPARQ POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/default-configs/config.ini
        ${CMAKE_CURRENT_SOURCE_DIR}/default-configs/imgui.ini
        $<TARGET_FILE_DIR:SPARQ>
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:SPARQ>/assets
        COMMENT "Copying runtime assets...")