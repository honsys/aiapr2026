# Gem Tutorial 20: Complete TCP Client
# -----------------------------------

string host = "127.0.0.1"
int port = 8082

sys.print("Connecting to", host, "on port", port)
socket = tcp.connect(host, port)

if socket.fd > 0
  sys.print("Connected!")
  
  tcp.send(socket.fd, "Greetings from Gem Client!")
  
  response = tcp.recv(socket.fd)
  sys.print("Server response:", response)
  
  tcp.close(socket.fd)
  sys.print("Client closed.")
end
