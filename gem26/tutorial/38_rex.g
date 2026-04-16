# Gem Tutorial 38: rex — Regular Expressions
# The 'rex' builtin provides full regex support using C++ std::regex (ECMAScript syntax).
# All methods accept an optional flags string: "i" = case-insensitive.

sys.print("--- Gem rex Tutorial ---")

string log = "2026-04-16 ERROR: disk full. 2026-04-17 INFO: backup done. 2026-04-17 ERROR: timeout."
string email = "Contact us at support@gem-lang.org or sales@gem-lang.org for help."
string csv   = "alpha,beta,,gamma,,delta"

# 1. match — test whether a pattern exists anywhere in the text
sys.print("\n1. match")
sys.print("Has ERROR?",   rex.match(log, "ERROR"))
sys.print("Has WARNING?", rex.match(log, "WARNING"))
sys.print("Case-insensitive 'error'?", rex.match(log, "error", "i"))

# 2. find — return the first matching substring
sys.print("\n2. find")
string first_date = rex.find(log, "\\d{4}-\\d{2}-\\d{2}")
sys.print("First date:", first_date)

string first_email = rex.find(email, "[\\w.+-]+@[\\w-]+\\.[a-z]{2,}")
sys.print("First email:", first_email)

# 3. findall — return all non-overlapping matches as a list
sys.print("\n3. findall")
list dates = rex.findall(log, "\\d{4}-\\d{2}-\\d{2}")
sys.print("All dates:", dates)

list emails = rex.findall(email, "[\\w.+-]+@[\\w-]+\\.[a-z]{2,}")
sys.print("All emails:", emails)

list errors = rex.findall(log, "ERROR|INFO")
sys.print("Levels found:", errors)

# 4. groups — capture groups from the first match
sys.print("\n4. groups")
list parts = rex.groups(first_date, "(\\d{4})-(\\d{2})-(\\d{2})")
sys.print("Year:", parts[0], "Month:", parts[1], "Day:", parts[2])

list addr = rex.groups(first_email, "([\\w.+-]+)@([\\w-]+\\.[a-z]{2,})")
sys.print("User:", addr[0], "Domain:", addr[1])

# 5. sub — replace only the first match
sys.print("\n5. sub (first match only)")
string redacted = rex.sub(email, "[\\w.+-]+@[\\w-]+\\.[a-z]{2,}", "[REDACTED]")
sys.print(redacted)

# 6. gsub — replace all matches (global substitution)
sys.print("\n6. gsub (all matches)")
string all_redacted = rex.gsub(email, "[\\w.+-]+@[\\w-]+\\.[a-z]{2,}", "[REDACTED]")
sys.print(all_redacted)

string vowels_gone = rex.gsub("Hello World", "[aeiou]", "*", "i")
sys.print("Vowels removed:", vowels_gone)

string clean_log = rex.gsub(log, "\\d{4}-\\d{2}-\\d{2} ", "")
sys.print("Log without dates:", clean_log)

# 7. split — split text on a regex delimiter
sys.print("\n7. split")
list fields = rex.split(csv, ",+")
sys.print("CSV fields:", fields)

list words = rex.split("one  two\tthree", "\\s+")
sys.print("Words:", words)

# 8. count — count non-overlapping matches
sys.print("\n8. count")
sys.print("ERROR count:", rex.count(log, "ERROR"))
sys.print("Date count:",  rex.count(log, "\\d{4}-\\d{2}-\\d{2}"))
sys.print("Vowel count:", rex.count("Hello World", "[aeiou]", "i"))

sys.print("\nTutorial 38: rex complete.")
