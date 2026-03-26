# Test TCP Routing and Configuration
sys.print("--- Network Interfaces ---")
int ifaces = tcp.interfaces()
# Due to many entries, let's just print the first few if possible
# or print the whole list if it's manageable
sys.print(ifaces)

sys.print("\n--- Routing Table ---")
int routes = tcp.routes()
sys.print(routes)

# Test property access on results
if algo.add(0) == 0 # dummy
  first_iface = ifaces[0]
  sys.print("\nFirst Interface Name:", first_iface.name)
  sys.print("First Interface Addr:", first_iface.addr)
end
