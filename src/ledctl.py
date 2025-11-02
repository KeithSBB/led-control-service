""" Install the HACS pyscript integration and place this file in the config/pyscript directory
    typical yaml usage as an action:
    
    action: pyscript.ledctl
    data:
      hostname: yoursatellite.local
      port: "8080"
      cmd: red 0.25, green 0.25, blue 0.25, loop forever
"""
import socket

@service
def ledctl(hostname, port, cmd):

    # Resolve hostname using getaddrinfo
    try:
        addr_info = socket.getaddrinfo(hostname, port, family=socket.AF_UNSPEC, type=socket.SOCK_STREAM)
    except socket.gaierror as e:
        print(f"getaddrinfo error: {e}")
        return

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
        print("Connect error")
        return

    # Send the command
    sock.sendall(cmd.encode('utf-8'))

    sock.close()



