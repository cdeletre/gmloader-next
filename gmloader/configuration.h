#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace fs = std::filesystem;
struct gml_config {
    std::string save_dir;
    std::string apk_path;
    bool show_cursor;
    bool disable_controller;
    bool disable_depth;
    std::string force_platform;
    std::string fmod_bank_path;

    friend void from_json(const json& j, gml_config& c);
    void init_defaults();
    int parse_file(const char *path);
    void show_config();
};

extern gml_config gmloader_config;