#pragma once

#include "imgui.h"
#include "sparq/decoder.hpp"
#include "sparq_config.h"

#include <sparq/protocol.hpp>

namespace spq::helper
{
    /**
     * @brief Adds or removes a value from an unordered set. If the value is already present, it will be removed; if it is not present, it will be added.
     * @tparam T The type of the elements in the unordered set.
     * @param set The unordered set to modify.
     * @param value The value to add or remove from the set.
     */
    template <typename T>
    constexpr void add_or_remove_from_set(std::unordered_set<T>& set, T const value)
    {
        if (set.contains(value))
        {
            set.erase(value);
        }
        else
        {
            set.insert(value);
        }
    }

    [[nodiscard]]
    constexpr uint8_t hex_chars_to_byte(char const high, char const low) noexcept
    {
        auto char_to_hex_value = [](char const c) -> uint8_t {
            if (c >= '0' && c <= '9')
            {
                return c - '0';
            }
            if (c >= 'A' && c <= 'F')
            {
                return c - 'A' + 10;
            }
            if (c >= 'a' && c <= 'f')
            {
                return c - 'a' + 10;
            }

            return 0;
        };

        return (char_to_hex_value(high) << 4) | char_to_hex_value(low);
    }

    [[nodiscard]]
    constexpr std::uint64_t now_ms() noexcept
    {
        using namespace std::chrono;
        return static_cast<std::uint64_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
    }

}

namespace spq::ui
{
    struct marker_t
    {
        std::string name{"M"};
        double x{};
        double y{};
        uint8_t ds_index{};
        int16_t ds_id{-1}; // TODO: Make this a size_t and handle the case where no dataset is selected
        bool hidden{};
        ImVec4 color{1.f, 1.f, 1.f, 1.f};
    };

}

namespace spq::data
{
    struct dataset_t
    {
        std::size_t id{};
        std::array<char, 64> name_buffer{};
        std::string name{};
        std::string name_with_id{};
        ImVec4 color{1.f, 0.f, 0.f, 1.f};
        bool hidden{};
        bool display_square{};

        std::vector<double> absolute_times;
        std::vector<double> relative_times;
        std::vector<double> samples;
        std::vector<double> y_values;

        constexpr void append_raw_values(
            double const sample,
            double const rel_time,
            double const abs_time,
            double const y_value)
        {
            samples.push_back(sample);
            relative_times.push_back(rel_time);
            absolute_times.push_back(abs_time);
            y_values.push_back(y_value);
        }

        constexpr void clear() noexcept
        {
            absolute_times.clear();
            relative_times.clear();
            samples.clear();
            y_values.clear();
        }

        void set_name(std::string const& new_name) noexcept
        {
            name = new_name;
            name_with_id = name + " [" + std::to_string(id) + "]";
            std::snprintf(name_buffer.data(), sizeof(name_buffer), "%s", name.c_str());
        }
    };

    struct timestamped_message_view
    {
        message_view view{};
        uint64_t timestamp{};
    };
}
