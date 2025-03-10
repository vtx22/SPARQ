#include "SPARQ.hpp"

SPARQ::SPARQ()
{
}

int SPARQ::init()
{
    std::cout << "\n=== SPARQ " << SPARQ_VERSION << " ===\n\n";
    std::cout << "Initializing ...\n\n";
    std::cout << "System Endianess: " << (sparq_is_little_endian() ? "Little Endian" : "Big Endian") << "\n";

    object_init();

    if (window_init() < 0)
    {
        return -1;
    }

    set_style(ULTRA_DARK);

    return 0;
}

void SPARQ::object_init()
{
    std::cout << "Initializing objects ...\n";

    static Serial serial_port;
    static ConsoleWindow console_window;
    static DataHandler data_handler(&serial_port, &console_window);

    static ConnectionWindow connection_window(&data_handler, &serial_port);
    static PlottingWindow plotting_window(&data_handler);
    static DataWindow data_window(&data_handler);
    static MeasureWindow measure_window(&data_handler);
    static ViewWindow view_window(&data_handler);
    static StatisticsWindow statistics_window(&data_handler);
    static SettingsWindow settings_window(&data_handler);

    _sp = &serial_port;
    _data_handler = &data_handler;

    _console_window = &console_window;
    _connection_window = &connection_window;
    _plotting_window = &plotting_window;
    _data_window = &data_window;
    _measure_window = &measure_window;
    _view_window = &view_window;
    _statistics_window = &statistics_window;
    _settings_window = &settings_window;
}

int SPARQ::window_init()
{
    auto &config = ConfigHandler::get_instance();

    std::cout << "Initializing window ...\n";

    sf::ContextSettings settings;
    settings.antialiasingLevel = std::stoi(config.ini["graphics"]["antialiasing"]);

    static sf::RenderWindow window(sf::VideoMode(1280, 720), std::string("SPARQ - ") + SPARQ_VERSION, sf::Style::Default, settings);

    std::cout << "Loading " << SPARQ_ICON_FILE << " ...\n";
    sf::Image icon;
    if (icon.loadFromFile(SPARQ_ICON_FILE))
    {
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }
    else
    {
        std::cerr << "Failed to load " << SPARQ_ICON_FILE << "!\n";
        ImGui::InsertNotification({ImGuiToastType::Error, SPARQ_NOTIFY_DURATION_ERR, "Failed to load icon.png!"});
    }

    window.setFramerateLimit(SPARQ_MAX_FPS);
    bool vsync_enabled = config.ini["graphics"]["vsync"] == "1";
    window.setVerticalSyncEnabled(vsync_enabled);

    BOOL USE_DARK_MODE = true;
    DwmSetWindowAttribute(window.getSystemHandle(), 20, &USE_DARK_MODE, sizeof(USE_DARK_MODE));
    ShowWindow(window.getSystemHandle(), SW_MAXIMIZE);

    if (!ImGui::SFML::Init(window))
    {
        std::cerr << "IMGUI SFML Window Init failed!\n";
        return -1;
    }

    std::cout << "Loading " << SPARQ_FONT << " ...\n";
    float baseFontSize = 18.f;
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF(SPARQ_FONT, baseFontSize);

    float iconFontSize = 16.f;
    static constexpr ImWchar iconsRanges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig iconsConfig;
    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    iconsConfig.GlyphMinAdvanceX = 16.f;
    iconsConfig.GlyphOffset = ImVec2(1.f, 0);
    io.Fonts->AddFontFromMemoryCompressedTTF(fa_solid_900_compressed_data, fa_solid_900_compressed_size, iconFontSize, &iconsConfig, iconsRanges);

    if (!ImGui::SFML::UpdateFontTexture())
    {
        std::cerr << "Could not load font!\n";
        return -1;
    }

    ImGui::CreateContext();
    ImPlot::CreateContext();

    _window = &window;

    config.apply_in_context_settings();

    std::cout << "\nInitialization complete!\n\n";

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
        ImGui::DockSpaceOverViewport(0, ImGuiDockNodeFlags_PassthruCentralNode);

        _data_handler->update();

        _console_window->update();
        _plotting_window->draw();
        _connection_window->draw();
        _data_window->draw();
        _measure_window->draw();
        _view_window->draw();
        _statistics_window->draw();
        _settings_window->draw();

        // Render Notifications
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.00f));
        ImGui::RenderNotifications();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);

        _window->clear(sf::Color(20, 20, 20, 255));
        ImGui::SFML::Render(*_window);

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
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