#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>  // For sockaddr_in (still needed indirectly)
#include <arpa/inet.h>   // Optional now, but harmless
#include <netdb.h>       // Added for getaddrinfo
#include <unistd.h>
#include <string>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage: ledctl <hostname> <port> <command...>" << std::endl;
        return 1;
    }

    std::string hostname = argv[1];
    std::string port_str = argv[2];  // Port as string for getaddrinfo

    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname.c_str(), port_str.c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return 1;
    }

    int sock = -1;
    for (p = res; p != nullptr; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock < 0) continue;

        if (connect(sock, p->ai_addr, p->ai_addrlen) < 0) {
            close(sock);
            sock = -1;
            continue;
        }
        break;  // Connected successfully
    }

    freeaddrinfo(res);  // Free the linked list

    if (sock < 0) {
        std::cerr << "Connect error" << std::endl;
        return 1;
    }

    std::string cmd;
    for (int i = 3; i < argc; ++i) {
        if (i > 3) cmd += " ";
        cmd += argv[i];
    }

    send(sock, cmd.c_str(), cmd.size(), 0);
    close(sock);
    return 0;
}
