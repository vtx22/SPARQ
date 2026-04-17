#include "DebugWindow.hpp"

void DebugWindow::update_content()
{
    ImGui::SeparatorText("Debug");

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

            double angle = 2.0 * std::numbers::pi_v<float> * static_cast<double>(i) / static_cast<double>(N);
            ds.y_values.push_back(std::sin(angle));
        }

        _data_handler->add_dataset(ds);
    }
}
