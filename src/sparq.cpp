#include "sparq.hpp"

SPARQ::SPARQ()
    : _sp(),
      _console_window(),
      _data_handler(_sp, _console_window),
      _connection_window(_data_handler, _sp),
      _data_window(_data_handler),
      _measure_window(_data_handler),
      _view_window(
          _data_handler,
          [this]() { add_plotting_window(); },
          [this]() { return get_selected_plot_settings(); }),
      _statistics_window(_data_handler),
      _settings_window(_data_handler),
      _debug_window(_data_handler)
{
    register_windows();
}

int SPARQ::init()
{
    std::cout << "\n=== SPARQ " << SPARQ_VERSION << " ===\n\n";
    std::cout << "Initializing ...\n\n";
    std::cout << "System Endianess: " << (spq::helper::is_little_endian() ? "Little Endian" : "Big Endian") << "\n";

    if (window_init() < 0)
    {
        return -1;
    }

    set_style(ULTRA_DARK);

    return 0;
}

void SPARQ::register_windows()
{
    _windows.clear();

    _windows.push_back(_connection_window);
    _windows.push_back(_data_window);
    _windows.push_back(_measure_window);
    _windows.push_back(_view_window);
    _windows.push_back(_statistics_window);
    _windows.push_back(_settings_window);
#ifdef SPARQ_DEBUG_BUILD
    _windows.push_back(_debug_window);
#endif

    // add one default plotting window at startup
    add_plotting_window();
}

int SPARQ::window_init()
{
    auto& config = ConfigHandler::get_instance();

    std::cout << "Initializing window ...\n";

    sf::ContextSettings settings;
    settings.antialiasingLevel = std::stoi(config.ini["graphics"]["antialiasing"]);

    _render_window.create(sf::VideoMode(1280, 720), std::string("SPARQ - ") + SPARQ_VERSION, sf::Style::Default, settings);

    std::cout << "Loading " << SPARQ_ICON_FILE << " ...\n";
    sf::Image icon;
    if (icon.loadFromFile(SPARQ_ICON_FILE))
    {
        _render_window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }
    else
    {
        std::cerr << "Failed to load " << SPARQ_ICON_FILE << "!\n";
        ImGui::InsertNotification({ImGuiToastType::Error, SPARQ_NOTIFY_DURATION_ERR, "Failed to load icon.png!"});
    }

    _render_window.setFramerateLimit(SPARQ_MAX_FPS);
    _render_window.setVerticalSyncEnabled(config.ini["graphics"]["vsync"] == "1");

#ifdef SPARQ_WINDOWS_BUILD
    constexpr BOOL USE_DARK_MODE = true;
    DwmSetWindowAttribute(_render_window.getSystemHandle(), 20, &USE_DARK_MODE, sizeof(USE_DARK_MODE));
    ShowWindow(_render_window.getSystemHandle(), SW_MAXIMIZE);
#endif

    if (!ImGui::SFML::Init(_render_window))
    {
        std::cerr << "IMGUI SFML Window Init failed!\n";
        return -1;
    }

    std::cout << "Loading " << SPARQ_FONT << " ...\n";
    constexpr auto base_font_size = 18.f;
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF(SPARQ_FONT, base_font_size);

    constexpr auto icon_font_size = 16.f;
    static constexpr ImWchar iconsRanges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig iconsConfig;
    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    iconsConfig.GlyphMinAdvanceX = 16.f;
    iconsConfig.GlyphOffset = ImVec2(1.f, 0);
    io.Fonts->AddFontFromMemoryCompressedTTF(
        fa_solid_900_compressed_data,
        fa_solid_900_compressed_size,
        icon_font_size,
        &iconsConfig,
        iconsRanges);

    if (!ImGui::SFML::UpdateFontTexture())
    {
        std::cerr << "Could not load font!\n";
        return -1;
    }

    ImGui::CreateContext();
    ImPlot::CreateContext();

    config.apply_in_context_settings();

    std::cout << "\nInitialization complete!\n\n";

    return 0;
}

int SPARQ::run()
{
    sf::Clock deltaClock;
    while (_render_window.isOpen())
    {
        sf::Event event;
        while (_render_window.pollEvent(event))
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
                _render_window.setView(sf::View(visibleArea));
            }
        }

        // == UPDATE == //
        ImGui::SFML::Update(_render_window, deltaClock.restart());

        // == DRAWING == //
        ImGui::DockSpaceOverViewport(0, ImGuiDockNodeFlags_PassthruCentralNode);

        update_windows();

        // Render Notifications
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 3.f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 1.00f));
        ImGui::RenderNotifications();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(1);

        _render_window.clear(sf::Color(20, 20, 20, 255));
        ImGui::SFML::Render(_render_window);

        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        _render_window.display();
    }

    return close_app();
}

int SPARQ::close_app()
{
    ImPlot::DestroyContext();
    ImGui::SFML::Shutdown();

    _render_window.close();

    return 0;
}

void SPARQ::update_windows()
{
    _console_window.update();

    for (auto& w : _windows)
    {
        w.get().draw();
    }

    for (auto& pw : _plotting_windows)
    {
        pw.get()->draw();
    }

    cleanup_closed_plotting_windows();
}

constexpr void SPARQ::add_plotting_window()
{
    auto const id = _next_id++;

    _plotting_windows.push_back(
        std::make_unique<PlottingWindow>(
            _data_handler,
            id,
            [this](std::size_t selected_id) {
                select_plot(selected_id);
            }));

    select_plot(id);
}

constexpr std::optional<std::reference_wrapper<PlottingWindow>> SPARQ::find_plot_by_id(std::size_t id) noexcept
{
    for (auto& plot : _plotting_windows)
    {
        if (plot->id() == id)
        {
            return *plot;
        }
    }

    return std::nullopt;
}

constexpr std::optional<std::reference_wrapper<spq::plotting::plot_settings>> SPARQ::get_selected_plot_settings() noexcept
{
    if (!_selected_plot_id)
    {
        return std::nullopt;
    }

    auto plot = find_plot_by_id(*_selected_plot_id);

    if (!plot)
    {
        return std::nullopt;
    }

    return plot->get().settings();
}

constexpr void SPARQ::select_plot(std::size_t id) noexcept
{
    _selected_plot_id = id;
    highlight_selected_plot_only();
}

constexpr void SPARQ::highlight_selected_plot_only() noexcept
{
    if (!_selected_plot_id)
    {
        return;
    }

    for (auto& plot : _plotting_windows)
    {
        plot->set_selected(plot->id() == *_selected_plot_id);
    }
}

constexpr void SPARQ::cleanup_closed_plotting_windows() noexcept
{
    // Delete windows that should close and deselect if it was selected
    std::erase_if(_plotting_windows, [this](auto const& w) {
        if (!w->close_triggered())
        {
            return false;
        }

        if (_selected_plot_id && w->id() == *_selected_plot_id)
        {
            _selected_plot_id = std::nullopt;
        }

        return true;
    });
}
