#ifndef GPIO_MANAGER_H
#define GPIO_MANAGER_H

#include <gpiod.hpp>
#include <map>
#include <string>
#include <mutex>
#include <filesystem>

class GPIOManager {
public:
    GPIOManager();
    ~GPIOManager();
    void set_led(const std::string& chip_name, const std::string& led_name, int value);
    void set_all_off();
private:
    std::map<std::string, gpiod::chip> chips;
    std::map<std::string, gpiod::line_request> requests;
    std::map<std::string, std::map<std::string, unsigned>> led_offsets;
    std::mutex gpio_mutex;
    void load_configs();
};

#endif
