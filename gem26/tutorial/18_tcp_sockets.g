# Gem Tutorial 18: TCP Sockets
# ----------------------------

# 1. Starting a TCP Server
# listen(port) returns a server file descriptor
# accept(server_fd) returns a Socket object with .fd and .ip

sys.print("TCP Server example (conceptual):")
sys.print("server_fd = tcp.listen(8081)")
sys.print("client = tcp.accept(server_fd)")
sys.print("msg = tcp.recv(client.fd)")
sys.print("tcp.send(client.fd, \"Acknowledged\")")
sys.print("tcp.close(client.fd)")
sys.print("tcp.close(server_fd)")

# 2. TCP Client
# connect(host, port) returns a Socket object
sys.print("\nTCP Client example (conceptual):")
sys.print("socket = tcp.connect(\"127.0.0.1\", 8081)")
sys.print("tcp.send(socket.fd, \"Hello Server\")")
sys.print("response = tcp.recv(socket.fd)")
sys.print("tcp.close(socket.fd)")

# 3. Builtin inheritance
# The tcp object can use sys methods too
tcp.print("\nThe tcp module is ready for networking tasks.")
