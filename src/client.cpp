#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>  // Added
#include <arpa/inet.h>   // Added for inet_pton
#include <unistd.h>
#include <string>

int main(int argc, char** argv) {
    if (argc < 4) {  // Updated usage
        std::cerr << "Usage: ledctl <server_ip> <port> <command...>" << std::endl;
        return 1;
    }

    std::string server_ip = argv[1];
    int port = std::stoi(argv[2]);  // Parse port from args

    int sock = socket(AF_INET, SOCK_STREAM, 0);  // Changed to AF_INET
    if (sock < 0) return 1;

    sockaddr_in addr;  // Changed to sockaddr_in
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip.c_str(), &addr.sin_addr) <= 0) {  // Convert IP string to binary
        std::cerr << "Invalid address / Address not supported" << std::endl;
        close(sock);
        return 1;
    }

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Connect error" << std::endl;
        close(sock);
        return 1;
    }

    std::string cmd;
    for (int i = 3; i < argc; ++i) {  // Start from argv[3] for commands
        if (i > 3) cmd += " ";
        cmd += argv[i];
    }

    send(sock, cmd.c_str(), cmd.size(), 0);
    close(sock);
    return 0;
}
