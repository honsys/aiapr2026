# Test Network Interface Configuration (NICs)
sys.print("--- Network Interface Cards (NICs) ---")
int interfaces = tcp.nic()
sys.print(interfaces)

# Test first interface details
if algo.add(0) == 0
  nic = interfaces[0]
  sys.print("\nNIC Name:", nic.name)
  sys.print("NIC IP:", nic.addr)
  sys.print("NIC Mask:", nic.netmask)
  sys.print("NIC Bcast:", nic.broadcast)
end

sys.print("\n--- Routing Info ---")
sys.print(tcp.routes())
