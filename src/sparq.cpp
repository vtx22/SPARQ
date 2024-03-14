#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML.h"
#include "imgui.h"
#include "implot.h"

#include "sparq_config.h"

#include "ConsoleWindow.hpp"

struct sparq_data_point
{
    float x;
    float y;

} typedef sparq_data_point;

struct sparq_dataset
{
    uint8_t uuid;
    std::vector<float> x_values;
    std::vector<float> y_values;

} typedef sparq_dataset;

std::vector<sparq_dataset> data;

int close_app(sf::RenderWindow &window);

int main(int argc, char *argv[])
{

    sparq_dataset sine_ds;
    sine_ds.uuid = 1;

    for (uint16_t i = 0; i < 256; i++)
    {
        sine_ds.x_values.push_back(i);
        sine_ds.y_values.push_back(sin(i / 255.0 * 2 * 3.14));
    }

    data.push_back(sine_ds);

    ConsoleWindow console_window;

    // ConnectionWindow connection_window;

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
                return close_app(window);
            }

            if (event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }

        // == UPDATE == //
        ImGui::SFML::Update(window, deltaClock.restart());

        console_window.update();

        // == DRAWING == //
        ImGui::DockSpaceOverViewport();

        if (ImGui::BeginMainMenuBar())
        {
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
        }

        if (ImGui::Begin("Plot"))
        {

            if (ImPlot::BeginPlot("Data", ImVec2(-1, -1)))
            {
                ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3);
                ImPlot::SetupAxes("Time", "");
                ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

                for (auto &ds : data)
                {
                    ImPlot::PlotLine("ds.uuid", ds.x_values.data(), ds.y_values.data(), 255);
                }

                ImPlot::PopStyleColor();
                ImPlot::EndPlot();
            }

            ImGui::End();
        }

        if (ImGui::Begin("Graphing"))
        {
            ImGui::End();
        }

        if (ImGui::Begin("Measurement"))
        {
            ImGui::End();
        }

        window.clear(sf::Color(20, 20, 20));
        ImGui::SFML::Render(window);
        window.display();
    }

    return close_app(window);
}

int close_app(sf::RenderWindow &window)
{
    // ImPlot::DestroyContext();
    // ImGui::DestroyContext();
    // ImGui::SFML::Shutdown();

    window.close();

    return 0;
}