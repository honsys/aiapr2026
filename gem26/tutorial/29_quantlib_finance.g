# Tutorial 29: QuantLib Financial Analysis
# ----------------------------------------
# This tutorial demonstrates the deep integration of QuantLib into the 'fin' module.
# QuantLib is the industry standard C++ library for quantitative finance.

sys.print("--- Gem QuantLib Advanced Tutorial ---")

# 1. Advanced Date Handling
# QuantLib provides precise date arithmetic used in financial contracts.
sys.print("\n1. Date Arithmetic:")
today = fin.date(9, 3, 2026)
next_year = fin.add_days(9, 3, 2026, 365)
diff = fin.diff_days(9, 3, 2026, 9, 3, 2027)

sys.print("Today:", today.str)
sys.print("Same day next year (via add_days):", next_year.str)
sys.print("Days between 2026 and 2027:", diff)

# 2. Business Calendars & Holiday Detection
# Calendars are critical for settlement and accrual logic.
sys.print("\n2. Business Calendars & Holidays:")
cal_nyse = "USA"
cal_london = "UK"

is_holiday_ny = fin.is_holiday(cal_nyse, 1, 1, 2026) # Jan 1st
is_holiday_ldn = fin.is_holiday(cal_london, 25, 12, 2026) # Christmas

sys.print("Is Jan 1st a holiday in NY?", is_holiday_ny)
sys.print("Is Dec 25th a holiday in London?", is_holiday_ldn)

# 3. Black-Scholes Pricing & Greeks
# Beyond just the price (NPV), risk management requires "Greeks" (Delta, Gamma, etc.)
sys.print("\n3. Black-Scholes Pricing & Greeks:")

# Parameters for an At-The-Money (ATM) Option
type = "call"
strike = 150.0
spot = 150.0
rate = 0.045  # 4.5% risk-free rate
vol = 0.22    # 22% annual volatility
time = 0.5    # 6 months

# Get full Greeks object
risk = fin.greeks(type, strike, spot, rate, vol, time)

sys.print("Option Price (NPV):", risk.npv)
sys.print("Delta (Sensitivity to Spot):", risk.delta)
sys.print("Gamma (Sensitivity to Delta):", risk.gamma)
sys.print("Theta (Time Decay):", risk.theta)
sys.print("Vega  (Sensitivity to Vol):", risk.vega)

# 4. Scenario Analysis
# We can use Gem's power to run quick scenarios.
sys.print("\n4. Volatility Surface Scan:")
sys.print("Vol % | Call Price")
sys.print("------------------")

# Functional range loop
itr.while(fun(i) i < 40 end, fun(i)
    v = i / 100.0
    p = fin.bs_price("call", 100, 100, 0.05, v, 1.0)
    sys.print(i, "%   | ", p)
    i = i + 5
end, 10)

# 5. Combining Real-time Data with Greeks
sys.print("\n5. Real-time Risk Analysis (NVDA):")
nvda = fin.ticker("NVDA")
sys.print("NVDA Spot:", nvda.price)

# Calculate Greeks for NVDA at current spot, 10% OTM call
otm_strike = nvda.price * 1.10
nvda_risk = fin.greeks("call", otm_strike, nvda.price, 0.05, 0.35, 0.25)

sys.print("NVDA 10% OTM Call Price:", nvda_risk.npv)
sys.print("NVDA Call Delta:", nvda_risk.delta)

sys.print("\n--- Tutorial Completed ---")
