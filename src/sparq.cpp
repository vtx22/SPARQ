#include <SFML/Graphics.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-SFML.h"
#include "imgui.h"

#define VERSION "v0.1"

int main(int argc, char *argv[])
{
    std::string win_title = std::string("SPARQ - ") + VERSION;
    sf::RenderWindow window(sf::VideoMode(1280, 720), win_title);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    ImGui::SFML::Init(window);
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

        ImGui::Begin("Settings");

        ImGui::End();

        window.clear(sf::Color(20, 20, 20));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}