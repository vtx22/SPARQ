add_executable(sparq sparq.cpp implot.cpp implot_items.cpp implot_demo.cpp serial.cpp ConnectionWindow.cpp ConsoleWindow.cpp)

target_link_libraries(sparq
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

include(Install.cmake)
