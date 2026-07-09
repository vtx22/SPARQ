target_sources(${PROJECT_NAME} PRIVATE
    "Window.hpp"

    "ConnectionWindow.hpp"

    "ConsoleWindow.hpp"
    "ConsoleWindow.cpp"

    "DataWindow.hpp"
    "DataWindow.cpp"

    "DebugWindow.hpp"
    "DebugWindow.cpp"

    "MeasureWindow.hpp"
    "MeasureWindow.cpp"
    
    "SettingsWindow.hpp"
    "SettingsWindow.cpp"

    "StatisticsWindow.hpp"

    "ViewWindow.hpp"
    "ViewWindow.cpp"
)

add_subdirectory(plotting)