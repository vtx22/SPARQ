#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML.h"
#include "imgui.h"
#include "implot.h"

#include "sparq_config.h"

#include <cmath>

#include <vector>
#include <string>
#include <iostream>

int main(int argc, char *argv[])
{
    float x[256];
    float y[256];

    for (uint16_t i = 0; i < 256; i++)
    {
        x[i] = i;
        y[i] = sin(i / 255.0 * 2 * 3.14);
    }

    // ConnectionWindow connection_window;
    // ConsoleWindow console_window;

    sf::RenderWindow window(sf::VideoMode(1280, 720), std::string("SPARQ - ") + SPARQ_VERSION);

    window.setFramerateLimit(SPARQ_MAX_FPS);
    window.setVerticalSyncEnabled(SPARQ_VSYNC);

    if (!ImGui::SFML::Init(window))
    {
        std::cout << "IMGUI SFML Window Init failed!" << std::endl;
        return -1;
    }
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::CreateContext();
    ImPlot::CreateContext();

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
            {
                goto shutdown;
            }

            if (event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }

            if (event.type == sf::Event::MouseWheelMoved)
            {
            }
            if (event.type == sf::Event::MouseMoved)
            {
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        // Create Dockspace so that the windows can stick to the main view
        ImGuiID main_dockspace = ImGui::DockSpaceOverViewport();

        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        ImGui::Begin("Plot");

        if (ImPlot::BeginPlot("Serial Data", ImVec2(-1, -1)))
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3);
            ImPlot::SetupAxes("Time", "");
            ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImPlot::PlotLine("1", x, y, 255);
            ImPlot::PopStyleColor();
            ImPlot::EndPlot();
        }

        ImGui::End();

        // connection_window.update();
        // console_window.update();

        ImGui::Begin("Graphing");
        ImGui::End();

        ImGui::Begin("Measurement");
        ImGui::End();

        window.clear(sf::Color(20, 20, 20));
        ImGui::SFML::Render(window);
        window.display();
    }

shutdown:
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    ImGui::SFML::Shutdown();

    return 0;
}