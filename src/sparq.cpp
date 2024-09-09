#include "SPARQ.hpp"

SPARQ::SPARQ()
{
}

int SPARQ::init()
{
    object_init();

    if (window_init() < 0)
    {
        return -1;
    }

    return 0;
}

void SPARQ::object_init()
{
    static Serial serial_port;
    static DataHandler data_handler(&serial_port);
    static ConsoleWindow console_window;
    static ConnectionWindow connection_window(&serial_port);
    static PlottingWindow plotting_window;
    static DataWindow data_window;

    _sp = &serial_port;
    _data_handler = &data_handler;
    _console_window = &console_window;
    _connection_window = &connection_window;
    _plotting_window = &plotting_window;
    _data_window = &data_window;
}

int SPARQ::window_init()
{
    static sf::RenderWindow window(sf::VideoMode(1280, 720), std::string("SPARQ - ") + SPARQ_VERSION);

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

    _window = &window;

    return 0;
}

int SPARQ::run()
{
    sf::Clock deltaClock;
    while (_window->isOpen())
    {
        sf::Event event;
        while (_window->pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
            {
                return close_app();
            }

            if (event.type == sf::Event::Resized)
            {
                // update the view to the new size of the window
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                _window->setView(sf::View(visibleArea));
            }
        }

        // == UPDATE == //
        ImGui::SFML::Update(*_window, deltaClock.restart());

        // == DRAWING == //
        ImGui::DockSpaceOverViewport();

        _data_handler->update();
        _plotting_window->update(_data_handler->get_datasets());
        _console_window->update();
        _connection_window->update();
        _data_window->update();

        _window->clear(sf::Color(20, 20, 20));
        ImGui::SFML::Render(*_window);
        _window->display();
    }

    return close_app();
}

int SPARQ::close_app()
{
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    // ImGui::SFML::Shutdown();

    _window->close();

    return 0;
}