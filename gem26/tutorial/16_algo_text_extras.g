# Gem Tutorial 16: Advanced Algo and Text
# ----------------------------------------

# 1. Advanced Sorting and Slices
data = [50.0, 10.0, 30.0, 20.0, 40.0]
# Sort entire array
sorted_all = algo.sort(data)
sys.print("Sorted all:", sorted_all)

# Sort a slice (index 1 to 4)
sorted_slice = algo.sort(data, 1, 4)
sys.print("Sorted slice [1:4]:", sorted_slice)

# 2. String Manipulation
s = "The quick brown fox"
upper_fox = text.replace(s, "fox", "FOX")
sys.print("Replace:", upper_fox)

part = text.sub(s, 4, 5)
sys.print("Substring (4,5):", part)

full = text.concat("Hello ", "Gem ", "World!")
sys.print("Concat:", full)

# 3. Date Manipulation
now = algo.timestamp()
tomorrow = algo.date_add(now, 1)
diff = algo.date_diff(now, tomorrow)
sys.print("Date diff (days):", diff)

# 4. Dictionary Sorting (Internal logic)
# (Dictionaries are created via builtin object properties or as a map type)
# Example dictionary sorting would be demonstrated here if Gem had literal dict syntax.
# Currently dictionaries can be manipulated via GemValue maps.
