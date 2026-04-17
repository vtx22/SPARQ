#include "DebugWindow.hpp"

void DebugWindow::update_content()
{
    ImGui::SeparatorText("Debug");
    ImGui::PushItemWidth(-FLT_MIN);

    if (ImGui::CollapsingHeader("Presets"))
    {
        if (ImGui::Button("Sine 1000"))
        {
            sparq_dataset_t ds{};

            constexpr auto N = 1000;
            ds.id = 100;
            ds.set_name("Sine 1000");
            ds.samples.reserve(N);
            ds.y_values.reserve(N);

            for (std::size_t i = 0; i < N; ++i)
            {
                ds.samples.push_back(static_cast<double>(i));

                auto const angle = 2.0 * std::numbers::pi_v<double> * static_cast<double>(i) / static_cast<double>(N);
                ds.y_values.push_back(std::sin(angle));
            }

            _data_handler.add_dataset(ds);
        }
    }

    if (ImGui::CollapsingHeader("Custom"))
    {
        static int id{};

        ImGui::InputInt("ID", &id);
        id = std::clamp(id, 0, 255);

        char name_buf[64] = {0};
        ImGui::InputText("Name", name_buf, sizeof(name_buf));

        constexpr std::array functions = {"lin", "sin", "cos"};
        static int selected = 0;
        ImGui::Combo("Function", &selected, functions.data(), functions.size());

        if (ImGui::Button("Add"))
        {
            sparq_dataset_t ds{.id = static_cast<int16_t>(id)};
            ds.set_name(std::string{std::strlen(name_buf) == 0 ? functions.at(selected) : name_buf});

            constexpr auto N = 1000;
            ds.samples.reserve(N);
            ds.y_values.reserve(N);

            for (std::size_t i = 0; i < N; ++i)
            {
                ds.samples.push_back(static_cast<double>(i));

                switch (selected)
                {
                default:
                case 0:
                {
                    ds.y_values.push_back(static_cast<double>(i));
                    break;
                }
                case 1:
                {
                    auto const angle = 2.0 * std::numbers::pi_v<double> * static_cast<double>(i) / static_cast<double>(N);
                    ds.y_values.push_back(std::sin(angle));
                    break;
                }
                case 2:
                {
                    auto const angle = 2.0 * std::numbers::pi_v<double> * static_cast<double>(i) / static_cast<double>(N);
                    ds.y_values.push_back(std::cos(angle));
                    break;
                }
                }
            }

            _data_handler.add_dataset(ds);
        }
    }
}
