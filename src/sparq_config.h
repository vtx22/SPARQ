#pragma once

#define SPARQ_VERSION "v0.5.1"

constexpr auto SPARQ_MAX_FPS = 120;
constexpr auto SPARQ_FONT = "./assets/roboto.ttf";
constexpr auto SPARQ_CONFIG_FILE = "config.ini";
constexpr auto SPARQ_ICON_FILE = "./assets/icon.png";

#define SPARQ_RECEIVE_LOOP_DELAY 1ms
constexpr auto SPARQ_RECEIVE_LOOP_DELAY_INTERVAL_MS = 100;

constexpr auto SPARQ_NOTIFY_DURATION_OK = 3000;
constexpr auto SPARQ_NOTIFY_DURATION_ERR = 5000;
