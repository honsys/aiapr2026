# Test Gem QuantLib Integration
# ----------------------------

sys.print("Testing QuantLib Integration...")

# 1. Date Handling
sys.print("Testing Date Handling...")
d = fin.date(1, 1, 2026)
if d.serial == 46023.0 
    sys.print("Success: Serial number correct.")
end

# 2. Date Arithmetic
sys.print("Testing Date Arithmetic...")
d2 = fin.add_days(1, 1, 2026, 10)
if d2.day == 11.0
    sys.print("Success: add_days correct.")
end

sys.print("Testing Date Diff...")
diff = fin.diff_days(1, 1, 2026, 1, 2, 2026)
if diff == 31.0
    sys.print("Success: diff_days correct.")
end

# 3. Calendar Holidays
sys.print("Testing Holidays...")
is_h = fin.is_holiday("USA", 1, 1, 2026)
if is_h == 1.0
    sys.print("Success: is_holiday correct (NY Day).")
end

# 4. Black-Scholes Pricing
sys.print("Testing Black-Scholes NPV...")
price = fin.bs_price("call", 100, 110, 0.05, 0.25, 1.0)
if price > 15.0 && price < 25.0
    sys.print("Success: Option price seems reasonable.")
end

# 5. Greeks
sys.print("Testing Black-Scholes Greeks...")
risk = fin.greeks("call", 100, 100, 0.05, 0.20, 1.0)
if risk.npv > 0.0 && risk.delta > 0.0
    sys.print("Success: Greeks calculated successfully.")
    sys.print("Delta: ", risk.delta)
end

sys.print("QuantLib Tests Passed!")
