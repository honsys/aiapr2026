# Gem Tutorial 30: BSM Builtin Object
# -----------------------------------
# This tutorial demonstrates the 'bsm' builtin module.
# The 'bsm' module inherits from 'fin' and provides specialized
# methods like 'price_american'.

sys.print("--- Gem BSM Builtin Tutorial ---")

# 1. Using the 'bsm' builtin directly
# It inherits 'ticker', 'bs_price', 'greeks', etc. from 'fin'
sys.print("Analyzing market for American Options...")
sys.print("Date:", algo.now())

# Analyze AAPL and GOOGL using builtin bsm.price_american
# bsm.price_american(ticker, type, duration)
# duration can be "weekly", "monthly", or default (3 months)

p1 = bsm.price_american("AAPL", "call", "weekly")
sys.print("AAPL Weekly American Call Price:", p1)

p2 = bsm.price_american("GOOGL", "put", "monthly")
sys.print("GOOGL Monthly American Put Price:", p2)

sys.print("------------------------------------------")

# 2. bsm also has access to all fin methods
nvda = bsm.ticker("NVDA")
sys.print("NVDA Spot (via bsm.ticker):", nvda.price)

sys.print("Tutorial 30 Complete: You have mastered the BSM module!")
