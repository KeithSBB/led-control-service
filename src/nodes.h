#ifndef NODES_H
#define NODES_H

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include "gpio_manager.h"

class Node {
public:
    virtual ~Node() = default;
    virtual void execute(GPIOManager& gpio, std::atomic<bool>& stop) = 0;
};

class SequenceNode : public Node {
public:
    std::vector<std::shared_ptr<Node>> steps;
    void execute(GPIOManager& gpio, std::atomic<bool>& stop) override {
        for (auto& step : steps) {
            if (stop) return;
            step->execute(gpio, stop);
        }
    }
};

class ParallelNode : public Node {
public:
    std::vector<std::shared_ptr<Node>> subs;
    void execute(GPIOManager& gpio, std::atomic<bool>& stop) override {
        std::vector<std::thread> threads;
        for (auto& sub : subs) {
            threads.emplace_back([&gpio, &stop, sub]() { sub->execute(gpio, stop); });
        }
        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }
    }
};

class LedActionNode : public Node {
public:
    std::string chip, name;
    double dwell; // -1 for inf, 0 for immediate off
    void execute(GPIOManager& gpio, std::atomic<bool>& stop) override {
        if (dwell == 0) {
            gpio.set_led(chip, name, 0);
            return;
        }
        gpio.set_led(chip, name, 1);
        if (dwell < 0) {
            while (!stop) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            gpio.set_led(chip, name, 0);
            return;
        }
        double slept = 0.0;
        while (slept < dwell && !stop) {
            double remaining = dwell - slept;
            int sleep_ms = std::min(1000, static_cast<int>(remaining * 1000 + 0.5));
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
            slept += static_cast<double>(sleep_ms) / 1000.0;
        }
        gpio.set_led(chip, name, 0);
    }
};

class OffNode : public Node {
public:
    double dwell; // -1 for inf, 0 for immediate off
    void execute(GPIOManager& gpio, std::atomic<bool>& stop) override {
        gpio.set_all_off();
        if (dwell == 0) return;
        if (dwell < 0) {
            while (!stop) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            return;
        }
        double slept = 0.0;
        while (slept < dwell && !stop) {
            double remaining = dwell - slept;
            int sleep_ms = std::min(1000, static_cast<int>(remaining * 1000 + 0.5));
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
            slept += static_cast<double>(sleep_ms) / 1000.0;
        }
    }
};

#endif
