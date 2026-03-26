# Gem Tutorial 19: Complete TCP Server
# -----------------------------------

int port = 8082
int server_fd = tcp.listen(port)

if server_fd > 0
  sys.print("Server listening on port", port)
  
  # Wait for one connection
  client = tcp.accept(server_fd)
  
  if client.fd > 0
    sys.print("Accepted connection from", client.ip)
    
    msg = tcp.recv(client.fd)
    sys.print("Received:", msg)
    
    tcp.send(client.fd, "Hello from Gem Server!")
    tcp.close(client.fd)
  end
  
  tcp.close(server_fd)
  sys.print("Server closed.")
end
