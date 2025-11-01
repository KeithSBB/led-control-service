import sys
import socket
import getaddrinfo  # Wait, no: it's socket.getaddrinfo

def main():
    if len(sys.argv) < 4:
        print("Usage: ledctl <hostname> <port> <command...>", file=sys.stderr)
        sys.exit(1)

    hostname = sys.argv[1]
    port_str = sys.argv[2]

    # Resolve hostname using getaddrinfo
    try:
        addr_info = socket.getaddrinfo(hostname, port_str, family=socket.AF_UNSPEC, type=socket.SOCK_STREAM)
    except socket.gaierror as e:
        print(f"getaddrinfo error: {e}", file=sys.stderr)
        sys.exit(1)

    sock = None
    for res in addr_info:
        af, socktype, proto, canonname, sa = res
        try:
            sock = socket.socket(af, socktype, proto)
        except OSError:
            continue

        try:
            sock.connect(sa)
            break  # Connected successfully
        except OSError:
            sock.close()
            sock = None
            continue

    if sock is None:
        print("Connect error", file=sys.stderr)
        sys.exit(1)

    # Build command from remaining args
    cmd = ' '.join(sys.argv[3:])

    # Send the command
    sock.sendall(cmd.encode('utf-8'))

    sock.close()
    sys.exit(0)

if __name__ == "__main__":
    main()
