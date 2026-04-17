# Tutorial 43: Heterogeneous Arrays and Dictionaries (algo module)
# Demonstrates algo.array / algo.dict — arbitrary-content multidimensional
# arrays and key/value dictionaries built into the algo builtin.
#
# Run: ./gem tutorial/43_arrays_dicts.g

sys.print("=== 43: Arrays & Dictionaries ===")

# ─────────────────────────────────────────────
# Part 1: Heterogeneous 1-D arrays
# ─────────────────────────────────────────────
sys.print("\n--- 1-D heterogeneous array ---")

int row = algo.array([])          # empty array
algo.array_push(row, "Alice")
algo.array_push(row, 30.0)
algo.array_push(row, true)
algo.array_push(row, [1.0, 2.0, 3.0])   # nested numeric vector

sys.print("row length:", algo.array_len(row))   # 4
sys.print("row[0]:", algo.array_get(row, 0))    # Alice
sys.print("row[1]:", algo.array_get(row, 1))    # 30
sys.print("row[2]:", algo.array_get(row, 2))    # true
sys.print("row[3]:", algo.array_get(row, 3))    # [1, 2, 3]

# ─────────────────────────────────────────────
# Part 2: Multidimensional arrays
# ─────────────────────────────────────────────
sys.print("\n--- 2-D matrix (3x4, filled 0.0) ---")

int mat = algo.array([3, 4], 0.0)
sys.print("shape:", algo.array_shape(mat))      # [3, 4]

# Set some elements
algo.array_set(mat, 1.0, 0, 0)
algo.array_set(mat, 5.0, 1, 2)
algo.array_set(mat, 9.0, 2, 3)

sys.print("mat[0][0]:", algo.array_get(mat, 0, 0))   # 1
sys.print("mat[1][2]:", algo.array_get(mat, 1, 2))   # 5
sys.print("mat[2][3]:", algo.array_get(mat, 2, 3))   # 9
sys.print("mat[0][1]:", algo.array_get(mat, 0, 1))   # 0 (fill)

# 3-D array
sys.print("\n--- 3-D array (2x2x2, filled nil) ---")
int cube = algo.array([2, 2, 2])
algo.array_set(cube, "center", 0, 1, 1)
sys.print("cube[0][1][1]:", algo.array_get(cube, 0, 1, 1))   # center
sys.print("cube shape:", algo.array_shape(cube))              # [2, 2, 2]

# ─────────────────────────────────────────────
# Part 3: Dictionaries
# ─────────────────────────────────────────────
sys.print("\n--- Dictionary basics ---")

int d = algo.dict()
algo.dict_set(d, "name", "Bob")
algo.dict_set(d, "age", 42.0)
algo.dict_set(d, "active", true)
algo.dict_set(d, "scores", [88.0, 92.0, 95.0])

sys.print("name:", algo.dict_get(d, "name"))       # Bob
sys.print("age:", algo.dict_get(d, "age"))         # 42
sys.print("active:", algo.dict_get(d, "active"))   # true
sys.print("scores:", algo.dict_get(d, "scores"))   # [88, 92, 95]
sys.print("has age:", algo.dict_has(d, "age"))     # true
sys.print("has zip:", algo.dict_has(d, "zip"))     # false
sys.print("keys:", algo.dict_keys(d))              # [active, age, name, scores]
sys.print("len:", algo.dict_len(d))                # 4

# Update and delete
algo.dict_set(d, "age", 43.0)
sys.print("updated age:", algo.dict_get(d, "age")) # 43
algo.dict_del(d, "active")
sys.print("len after del:", algo.dict_len(d))      # 3

# ─────────────────────────────────────────────
# Part 4: Dict with initial pairs
# ─────────────────────────────────────────────
sys.print("\n--- Dict initialized with pairs ---")

int cfg = algo.dict("host", "localhost", "port", 8080.0, "debug", false)
sys.print("host:", algo.dict_get(cfg, "host"))   # localhost
sys.print("port:", algo.dict_get(cfg, "port"))   # 8080
sys.print("debug:", algo.dict_get(cfg, "debug")) # false

# ─────────────────────────────────────────────
# Part 5: Array of dicts (table pattern)
# ─────────────────────────────────────────────
sys.print("\n--- Array of dicts (table) ---")

int table = algo.array([])
int r1 = algo.dict("id", 1.0, "city", "Paris",  "pop", 2161000.0)
int r2 = algo.dict("id", 2.0, "city", "London", "pop", 8982000.0)
int r3 = algo.dict("id", 3.0, "city", "Tokyo",  "pop", 13960000.0)
algo.array_push(table, r1)
algo.array_push(table, r2)
algo.array_push(table, r3)

sys.print("rows:", algo.array_len(table))   # 3

int i = 0
while i < algo.array_len(table)
  int row2 = algo.array_get(table, i)
  sys.print(algo.dict_get(row2, "city"), "pop:", algo.dict_get(row2, "pop"))
  i += 1
end

# ─────────────────────────────────────────────
# Part 6: Dict of arrays (column store pattern)
# ─────────────────────────────────────────────
sys.print("\n--- Dict of arrays (column store) ---")

int cols = algo.dict()
algo.dict_set(cols, "x", [1.0, 2.0, 3.0, 4.0, 5.0])
algo.dict_set(cols, "y", [2.0, 4.0, 6.0, 8.0, 10.0])

int xs = algo.dict_get(cols, "x")
int ys = algo.dict_get(cols, "y")
sys.print("x:", xs)
sys.print("y:", ys)
sys.print("x[2]:", xs[2])   # 3
sys.print("y[4]:", ys[4])   # 10

sys.print("\n=== Tutorial 43 complete ===")
