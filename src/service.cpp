#include "gpio_manager.h"
#include "parser.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <atomic>
#include <thread>
#include <memory>
#include <sys/stat.h>

int main() {
    GPIOManager gpio;

    const char* socket_path = "/run/led-control.sock";
    unlink(socket_path);

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind error" << std::endl;
        close(server_fd);
        return 1;
    }

    if (chmod(socket_path, 0666) < 0) {
        std::cerr << "Chmod error" << std::endl;
        close(server_fd);
        unlink(socket_path);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen error" << std::endl;
        close(server_fd);
        unlink(socket_path);
        return 1;
    }

    std::atomic<bool> stop(false);
    std::shared_ptr<std::thread> current_seq_thread;
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) continue;

        char buffer[1024] = {0};
        read(client_fd, buffer, 1024);
        close(client_fd);

        std::string cmd(buffer);
        std::cout << "Received: " << cmd << std::endl;

        // Stop current
        if ((current_seq_thread) && (current_seq_thread->joinable())) {
            stop = true;
            current_seq_thread->join();
            gpio.set_all_off();
            stop = false;
        }

        if (cmd == "off") continue;
        try {
            Parser p(cmd);
            auto res = p.parse();
            if (!res.root) continue;

            current_seq_thread = std::make_shared<std::thread>([&gpio, &stop, res]() {
                int loops = res.loop_count;
                while (loops < 0 || loops-- > 0) {
                    if (stop) break;
                    res.root->execute(gpio, stop);
                }
            });
        } catch (const std::exception& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
        }
    }

    close(server_fd);
    unlink(socket_path);
    return 0;
}
