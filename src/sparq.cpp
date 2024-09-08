#include "SPARQ.hpp"

SPARQ::SPARQ()
{
}

int SPARQ::init()
{
    static Serial serial_port;
    static DataHandler data_handler(&serial_port);
    static ConsoleWindow console_window;
    static ConnectionWindow connection_window(&serial_port);
    static PlottingWindow plotting_window;

    _sp = &serial_port;
    _data_handler = &data_handler;
    _console_window = &console_window;
    _connection_window = &connection_window;
    _plotting_window = &plotting_window;

    return 0;
}

int SPARQ::run()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), std::string("SPARQ - ") + SPARQ_VERSION);

    window.setFramerateLimit(SPARQ_MAX_FPS);
    window.setVerticalSyncEnabled(SPARQ_VSYNC);

    ::ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);

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

        if (ImGui::Begin("Graphing"))
        {
        }
        ImGui::End();

        if (ImGui::Begin("Measurement"))
        {
        }
        ImGui::End();

        _data_handler->update();
        _plotting_window->update();
        _console_window->update();
        _connection_window->update();

        window.clear(sf::Color(20, 20, 20));
        ImGui::SFML::Render(window);
        window.display();
    }

    return close_app(window);
}

int SPARQ::close_app(sf::RenderWindow &window)
{
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    // ImGui::SFML::Shutdown();

    window.close();

    return 0;
}