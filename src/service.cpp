#include "gpio_manager.h"
#include "parser.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>  // Added for sockaddr_in and INADDR_ANY
#include <arpa/inet.h>   // Added for htons, inet_pton (if needed)
#include <unistd.h>
#include <atomic>
#include <thread>
#include <memory>
#include <cstring>

int main() {
    GPIOManager gpio;
    const int port = 8080;  // Specify the port here (e.g., 8080). Change to your desired port.
                            // Ports below 1024 require root privileges.

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);  // Changed to AF_INET
    if (server_fd < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return 1;
    }

    sockaddr_in addr;  // Changed to sockaddr_in
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;  // Listen on all interfaces (0.0.0.0)
    addr.sin_port = htons(port);  // Convert port to network byte order

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind error" << std::endl;
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen error" << std::endl;
        close(server_fd);
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
        if (current_seq_thread && current_seq_thread->joinable()) {
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
    return 0;
}
