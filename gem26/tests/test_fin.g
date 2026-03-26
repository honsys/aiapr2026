# Test Gem Finance Module
# ----------------------

sys.print("Fetching data for AAPL...")
aapl = fin.ticker("AAPL")

sys.print("Symbol:", aapl.symbol)
sys.print("Price:", aapl.price)
sys.print("Currency:", aapl.currency)
sys.print("High:", aapl.high)
sys.print("Low:", aapl.low)

if aapl.price > 0
  sys.print("\nSuccess: Fetched real-time data for AAPL.")
else
  sys.print("\nError: Could not fetch data.")
end
