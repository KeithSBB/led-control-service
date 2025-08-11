#include "gpio_manager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <vector>

GPIOManager::GPIOManager() {
    load_configs();
}

GPIOManager::~GPIOManager() {
    // RAII: chips and requests auto-close/release on destruct
}

void GPIOManager::load_configs() {
    std::string config_path = "/etc/led-control/leds.ini";
    std::ifstream file(config_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << config_path << std::endl;
        return;
    }

    std::string line, current_chip;
    std::map<std::string, std::vector<unsigned>> chip_offsets;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';') continue;
        if (line[0] == '[') {
            if (!current_chip.empty() && !chip_offsets[current_chip].empty()) {
                // Process previous chip
                gpiod::chip chip("/dev/" + current_chip);
                if (!chip) {
                    std::cerr << "Failed to open chip /dev/" << current_chip << std::endl;
                    continue;
                }
                chips[current_chip] = std::move(chip);

                auto builder = chips[current_chip].prepare_request().set_consumer("led-control");
                for (unsigned offset : chip_offsets[current_chip]) {
                    gpiod::line_settings settings;
                    settings.set_direction(gpiod::line::direction::OUTPUT);
                    settings.set_output_value(gpiod::line::value::INACTIVE);
                    builder.add_line_settings(offset, settings);
                }
                auto req = builder.do_request();
                if (!req) {
                    std::cerr << "Failed to request lines for " << current_chip << std::endl;
                    continue;
                }
                requests[current_chip] = std::move(req);
            }

            current_chip = line.substr(1, line.find(']') - 1);
            continue;
        }

        if (current_chip.empty()) continue;

        std::istringstream iss(line);
        std::string name, offset_str;
        std::getline(iss, name, '=');
        std::getline(iss, offset_str);
        if (offset_str.empty()) continue;

        unsigned offset;
        try {
            offset = std::stoul(offset_str);
        } catch (...) {
            continue;
        }

        led_offsets[current_chip][name] = offset;
        chip_offsets[current_chip].push_back(offset);
    }

    // Process last chip
    if (!current_chip.empty() && !chip_offsets[current_chip].empty()) {
        gpiod::chip chip("/dev/" + current_chip);
        if (!chip) {
            std::cerr << "Failed to open chip /dev/" << current_chip << std::endl;
            return;
        }
        chips[current_chip] = std::move(chip);

        auto builder = chips[current_chip].prepare_request().set_consumer("led-control");
        for (unsigned offset : chip_offsets[current_chip]) {
            gpiod::line_settings settings;
            settings.set_direction(gpiod::line::direction::OUTPUT);
            settings.set_output_value(gpiod::line::value::INACTIVE);
            builder.add_line_settings(offset, settings);
        }
        auto req = builder.do_request();
        if (!req) {
            std::cerr << "Failed to request lines for " << current_chip << std::endl;
            return;
        }
        requests[current_chip] = std::move(req);
    }
}

void GPIOManager::set_led(const std::string& chip_name, const std::string& led_name, int value) {
    std::lock_guard<std::mutex> lock(gpio_mutex);
    auto it_offsets = led_offsets.find(chip_name);
    if (it_offsets == led_offsets.end()) return;
    auto it_name = it_offsets->second.find(led_name);
    if (it_name == it_offsets->second.end()) return;

    unsigned offset = it_name->second;
    auto it_req = requests.find(chip_name);
    if (it_req == requests.end()) return;

    gpiod::line::value val = value ? gpiod::line::value::ACTIVE : gpiod::line::value::INACTIVE;
    it_req->second.set_value(offset, val);
}

void GPIOManager::set_all_off() {
    std::lock_guard<std::mutex> lock(gpio_mutex);
    for (const auto& chip_pair : requests) {
        std::string chip_name = chip_pair.first;
        auto& req = chip_pair.second;
        std::map<unsigned, gpiod::line::value> vals;
        for (const auto& offset_pair : led_offsets[chip_name]) {
            vals[offset_pair.second] = gpiod::line::value::INACTIVE;
        }
        req.set_values(vals);
    }
}
