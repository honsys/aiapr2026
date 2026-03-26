# Test Gem Async Builtin

fun slow_task(name, seconds)
    sys.print("Task " + name + " starting, will sleep for " + seconds + "s")
    sys.exec("sleep " + seconds)
    sys.print("Task " + name + " finished")
    return "Result from " + name
end

# Test 1: Basic async execution
sys.print("--- Test 1: Basic Async ---")
t1 = sys.async("slow_task", ["Task1", "2"])
sys.print("Thread object: " + t1)
sys.print("Is finished? " + t1.is_finished())

# Test 2: Wait for result
sys.print("\n--- Test 2: Wait for Result ---")
res1 = t1.wait()
sys.print("Result: " + res1)
sys.print("Is finished? " + t1.is_finished())

# Test 3: Timeout
sys.print("\n--- Test 3: Timeout ---")
# 5s task, 2s timeout
t2 = sys.async("slow_task", ["Task2", "5"], 2.0) 
res2 = t2.wait()
sys.print("Result (should be timeout error): " + res2)

# Test 4: Mixed params
sys.print("\n--- Test 4: Mixed Params ---")
fun add_task(a, b)
    return a + b
end
t3 = sys.async("add_task", [10.0, 20.0])
sys.print("Result of 10 + 20: " + t3.wait())
