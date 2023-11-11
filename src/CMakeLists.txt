add_executable(sparq sparq.cpp)

target_link_libraries(sparq
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

include(Install.cmake)
