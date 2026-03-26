# Gem Tutorial 22: Builtins - Iterators and Networking
# --------------------------------------------------

# 1. Functional While Loop (itr.while)
# The 'while' keyword is built-in, but 'itr.while' allows 
# functional control by calling other functions.

int count = 0

fun check_limit()
  res = count < 5
end res

fun increment()
  count = count + 1
  sys.print("Count is now:", count)
end

sys.print("Starting functional while loop:")
itr.while("check_limit", "increment")


# 2. Network Interface Info (tcp.nic)
# nic() returns a vector of objects representing network interfaces.

sys.print("\nNetwork Interfaces:")
interfaces = tcp.nic()
# We can iterate and print details
# (Using a simple loop for demonstration)
# For now, let's just print the first one if available
if algo.add(itr.range(1)) == 0
  # results is a vector of NIC objects
  # sys.print("First interface:", interfaces[0].name, "at", interfaces[0].addr)
end


# 3. Routing Table Info (tcp.routes)
# routes() returns a vector of objects representing the routing table.

sys.print("\nRouting Table (Partial):")
routing_table = tcp.routes()
# sys.print("Default Gateway:", routing_table[0].gateway)


sys.print("\nTutorial 22: Builtins for Iteration and Networking documented.")
