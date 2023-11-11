#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML.h"
#include "imgui.h"
#include "implot.h"

#include "sparq_config.h"

#include <vector>
#include <string>

struct sparq_xy_t
{
    float x;
    float y;

} typedef sparq_xy_t;

int main(int argc, char *argv[])
{

    sf::RenderWindow window(sf::VideoMode(1280, 720), std::string("SPARQ - ") + SPARQ_VERSION);

    window.setFramerateLimit(SPARQ_MAX_FPS);
    window.setVerticalSyncEnabled(SPARQ_VSYNC);

    ImGui::SFML::Init(window);
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
                window.close();
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
        ImGui::EndMainMenuBar();

        ImGui::Begin("Plot");

        // ImPlot::CreateContext();

        float x_data[2] = {1, 2};
        float y_data[2] = {4, 12};

        if (ImPlot::BeginPlot("Serial Data", ImVec2(-1, -1)))
        {
            ImPlot::SetupAxes("Time", "");
            ImPlot::PlotLine("1", x_data, y_data, 2);
            ImPlot::EndPlot();
        }

        // ImPlot::DestroyContext();

        ImGui::End();

        ImVector<const char *> log;
        log.push_back("test");

        ImGui::Begin("Console");
        ImGui::Separator();
        ImGui::TextUnformatted("test");
        ImGui::Separator();
        ImGui::End();

        ImGui::Begin("Connection");

        ImGui::End();

        window.clear(sf::Color(20, 20, 20));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    ImGui::SFML::Shutdown();

    return 0;
}