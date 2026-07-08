#pragma once

#include "imgui.h"
#include "sparq_config.h"

constexpr auto SPARQ_MESSAGE_HEADER_LENGTH = 5u;
constexpr auto SPARQ_BYTES_PER_VALUE_PAIR = 5u;
constexpr auto SPARQ_CHECKSUM_LENGTH = 1u;
constexpr uint16_t SPARQ_MAX_PAYLOAD_LENGTH = 0xFFFF;
constexpr auto SPARQ_MIN_MESSAGE_LENGTH = (SPARQ_MESSAGE_HEADER_LENGTH + 1 + SPARQ_CHECKSUM_LENGTH);
constexpr auto SPARQ_MAX_MESSAGE_LENGTH = (SPARQ_MESSAGE_HEADER_LENGTH + SPARQ_MAX_PAYLOAD_LENGTH + SPARQ_CHECKSUM_LENGTH);
constexpr uint8_t SPARQ_DEFAULT_SIGNATURE = 0xFF;

namespace spq::helper
{
    /**
     * @brief Checks if the system is little-endian.
     * @return true if the system is little-endian, false otherwise.
     */
    [[nodiscard]]
    constexpr bool is_little_endian() noexcept
    {
        return std::endian::native == std::endian::little;
    }

    /**
     * @brief Computes the XOR checksum of a given data span.
     * @param data The span of data to compute the checksum for.
     * @param length The number of bytes to consider from the data span.
     * @return The computed XOR checksum as a uint8_t.
     */
    [[nodiscard]]
    constexpr uint8_t xor8_cs(std::span<const uint8_t> const data, std::size_t const length) noexcept
    {
        uint8_t cs{};

        for (auto const& b : data.first(std::min(length, data.size())))
        {
            cs ^= b;
        }

        return cs;
    }

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

}

namespace spq::ui
{
    enum class plot_type_t : uint8_t
    {
        timeseries,
        xy,
        single_value,
        bar,
        pie,
        heatmap,
        fft,
        COUNT
    };

    constexpr std::array<char const*, static_cast<std::size_t>(plot_type_t::COUNT)> plot_type_names{
        "Timeseries",
        "XY",
        "Single Value",
        "Bar Chart",
        "Pie Chart",
        "Heatmap",
        "FFT"};

    enum class x_fit_t : uint8_t
    {
        manual,
        all,
        last_n,
        COUNT
    };

    constexpr std::array<char const*, static_cast<std::size_t>(x_fit_t::COUNT)> x_fit_names{
        "Manual",
        "All",
        "Last N"};

    enum class y_fit_t : uint8_t
    {
        manual,
        all,
        COUNT
    };

    constexpr std::array<char const*, static_cast<std::size_t>(y_fit_t::COUNT)> y_fit_names{
        "Manual",
        "All"};

    struct heatmap_settings_t
    {
        bool normalize_xy{};
        bool show_values{};
        float scale_min{};
        float scale_max{100.f};
        bool autoscale{};
        bool invert_scale{};
        int rows{1};
        int cols{1};
        bool smooth{};
        int smoothing_factor{5};
    };

    struct plot_settings_t
    {
        std::unordered_set<std::size_t> ids_to_plot{};

        plot_type_t type{};
        x_fit_t x_fit{};
        y_fit_t y_fit{};
        bool equal{};

        heatmap_settings_t heatmap_settings{};
    };

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
    enum class header_control_t : uint8_t
    {
        LSB_FIRST = (1 << 7),
        CS_EN = (1 << 6),
        MSG_TYPE = (1 << 2) + (1 << 3),
        SIGNED = (1 << 1),
        INTEGER = (1 << 0),
    };

    enum class message_type_t : uint8_t
    {
        ID_PAIR = 0b00,
        STRING = 0b01,
        BULK_SINGLE_ID = 0b10,
        SENDER_COMMAND = 0b11,
    };

    enum class sender_command_t : uint8_t
    {
        CLEAR_CONSOLE,         // Remote clear the console
        CLEAR_ALL_DATASETS,    // Remote clear all datasets data but keep the settings
        CLEAR_SINGLE_DATASET,  // Remote clear a single datasets data but keep the settings
        DELETE_ALL_DATASETS,   // Remote delete all datasets
        DELETE_SINGLE_DATASET, // Remote delete single dataset
        SET_DATASET_NAME,      // Remote set the name of a given dataset
        SWITCH_PLOT_TYPE,      // Remote switch the plot type (e.g. line, heatmap, etc.)
    };

    struct dataset_t
    {
        std::size_t id{};
        char name_buffer[64] = {0};
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
            std::snprintf(name_buffer, sizeof(name_buffer), "%s", name.c_str());
        }
    };

    struct message_header_t
    {
        uint8_t signature{};
        uint8_t control{};
        uint16_t payload_length{};
        uint8_t checksum{};

        message_header_t()
        {
        }

        message_header_t(uint8_t const* buffer)
        {
            from_array(buffer);
        }

        constexpr void from_array(uint8_t const* buffer) noexcept
        {
            signature = buffer[0];
            control = buffer[1];

            auto const message_endianess = static_cast<bool>(control & static_cast<uint8_t>(header_control_t::LSB_FIRST));

            if (message_endianess == spq::helper::is_little_endian())
            {
                payload_length = (buffer[2] << 8) + buffer[3];
            }
            else
            {
                payload_length = (buffer[3] << 8) + buffer[2];
            }

            checksum = buffer[4];
        }

        constexpr void to_array(uint8_t* buffer) const noexcept
        {
            buffer[0] = signature;
            buffer[1] = control;
            buffer[2] = payload_length >> 8;
            buffer[3] = payload_length & 0xFF;
            buffer[4] = checksum;
        }
    };

    struct message_t
    {
        message_header_t header;
        std::vector<uint8_t> ids;
        std::vector<double> values;
        uint16_t checksum{};
        bool valid{};
        uint64_t timestamp{};
        std::string string_data{};
        message_type_t message_type{};
        sender_command_t command_type{};
        std::vector<uint8_t> command_data{};
        uint16_t nval{};

        [[nodiscard]]
        constexpr double buffer_to_double(uint8_t const* data) const noexcept
        {
            auto const msg_endian = static_cast<bool>(
                header.control & static_cast<uint8_t>(header_control_t::LSB_FIRST));

            uint32_t const value32 = [&] {
                uint32_t const le = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
                uint32_t const be = (data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0];
                return (msg_endian == spq::helper::is_little_endian()) ? le : be;
            }();

            auto const is_integer = static_cast<bool>(
                header.control & static_cast<uint8_t>(header_control_t::INTEGER));
            auto const is_signed = static_cast<bool>(
                header.control & static_cast<uint8_t>(header_control_t::SIGNED));

            if (!is_integer)
            {
                return std::bit_cast<float>(value32);
            }

            if (is_signed)
            {
                return std::bit_cast<int32_t>(value32);
            }

            return value32;
        }

        constexpr void parse_msg_id_pair(uint8_t const* data)
        {
            nval = header.payload_length / SPARQ_BYTES_PER_VALUE_PAIR;
            ids.resize(nval);
            values.resize(nval);

            for (uint8_t pair = 0; pair < nval; pair++)
            {
                auto const pair_index = SPARQ_MESSAGE_HEADER_LENGTH + pair * SPARQ_BYTES_PER_VALUE_PAIR;

                ids[pair] = data[pair_index];
                values[pair] = buffer_to_double(&data[pair_index + 1]);
            }
        }

        constexpr void parse_msg_bulk_single_id(uint8_t const* data)
        {
            nval = (header.payload_length - 1) / 4;

            ids.assign(nval, data[SPARQ_MESSAGE_HEADER_LENGTH]);
            values.resize(nval);

            uint8_t const* ptr = data + SPARQ_MESSAGE_HEADER_LENGTH + 1;
            for (double& value : values)
            {
                value = buffer_to_double(ptr);
                ptr += 4; // TODO: Fix magic number
            }
        }

        constexpr void parse_msg_sender_command(uint8_t const* data)
        {
            command_type = static_cast<sender_command_t>(data[SPARQ_MESSAGE_HEADER_LENGTH]);

            if (header.payload_length <= 1)
            {
                return;
            }

            auto const additional_command_payload_length = header.payload_length - 1;
            uint8_t const* first_payload_ptr = &data[SPARQ_MESSAGE_HEADER_LENGTH];

            command_data.resize(additional_command_payload_length);
            std::copy(first_payload_ptr + 1, first_payload_ptr + additional_command_payload_length + 1, command_data.begin());
        }

        constexpr void from_array(uint8_t const* data) noexcept
        {
            header.from_array(data);

            if (header.payload_length == 0)
            {
                return;
            }

            message_type = static_cast<message_type_t>(header.control >> 2 & 0b11);

            switch (message_type)
            {
            case message_type_t::STRING:
                string_data = std::string(reinterpret_cast<char const*>(&data[SPARQ_MESSAGE_HEADER_LENGTH]), header.payload_length);
                break;
            case message_type_t::ID_PAIR:
                parse_msg_id_pair(data);
                break;
            case message_type_t::BULK_SINGLE_ID:
                parse_msg_bulk_single_id(data);
                break;
            case message_type_t::SENDER_COMMAND:
                parse_msg_sender_command(data);
                break;
            default:
                break;
            }

            checksum = data[SPARQ_MESSAGE_HEADER_LENGTH + header.payload_length];
        }
    };
}
