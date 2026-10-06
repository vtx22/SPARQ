#pragma once

#define SPARQ_VERSION "v0.6.0"

constexpr auto SPARQ_MAX_FPS = 120;
constexpr auto SPARQ_FONT = "./assets/roboto.ttf";
constexpr auto SPARQ_CONFIG_FILE = "config.ini";
constexpr auto SPARQ_ICON_FILE = "./assets/icon.png";

constexpr auto SPARQ_RECEIVE_LOOP_DELAY = std::chrono::milliseconds(1);
constexpr auto SPARQ_RECEIVE_LOOP_DELAY_INTERVAL = std::chrono::milliseconds(100);
constexpr auto SPARQ_NOTIFY_DURATION_OK = 3000;
constexpr auto SPARQ_NOTIFY_DURATION_ERR = 5000;
