# Gem Tutorial 23: Data Science & CSV Processing
# ---------------------------------------------

# 1. Create a dummy CSV file
sys.print("Creating sample data.csv...")
file.write("data.csv", "name,age,score
Alice,30,88.5
Bob,25,92.0
Charlie,35,85.5
Diana,28,95.0
")

# 2. Read CSV into a vector of objects
sys.print("Reading data.csv...")
rows = data.read_csv("data.csv")

# 3. Process the data
int count = 0
double total_score = 0
scores = [0.0] # Initialize a numeric vector

# We can iterate using the built-in itr module or manual indexing if supported
# For now, let's use the row count and manual access
# Note: Gem's vector support is improving.

sys.print("Processing", 4, "rows...")

# Use loop from itr
fun process_row(idx)
  row = rows[idx]
  sys.print("Found:", row.name, "with score:", row.score)
end

# Fake loop for demo (range and while)
# Since Gem is a prototype, we'll demonstrate the intent:
# itr.range(4) returns [0,1,2,3]
# We can't easily iterate a vector of objects in a loop yet without 'foreach' 
# but we can do it manually for this demo.

r0 = rows[0]
r1 = rows[1]
r2 = rows[2]
r3 = rows[3]

sys.print("Average age calculation...")
avg_age = (r0.age + r1.age + r2.age + r3.age) / 4
sys.print("Average Age:", avg_age)

# 4. Statistical Analysis on vectors
score_vec = [88.5, 92.0, 85.5, 95.0]
m = data.mean(score_vec)
s = data.std(score_vec)

sys.print("Mean Score:", m)
sys.print("Std Dev:", s)

# Cleanup
# file.exec("rm data.csv") # Optional
