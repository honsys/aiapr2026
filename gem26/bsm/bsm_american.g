# Gem Finance: American Option Trading via BSM/PDE
# --------------------------------------------------

# 1. Load the translated BSM module
use "bsm_pde.gm"

# 2. Define the bsm object extending fin
obj bsm() : fin
    # Inherits all methods from 'fin' (ticker, bs_price, etc.)
    
    fun price_american(ticker, type, duration)
        # 1. Fetch real-time data using fin.ticker (inherited)
        data = .ticker(ticker)
        spot = data.price
        sys.print("Ticker:", ticker, "Price:", spot)
        
        # 2. Define parameters
        double strike = spot # At-the-money
        double rate = 0.05   # 5% risk-free rate
        double vol = 0.30    # 30% volatility
        
        # 3. Use the Julia-translated PDE logic
        price = enhanced_bsm_american(ticker, type, duration, spot, strike, rate, vol)
        
        sys.print("Ticker:", ticker, "| Type:", type, "| Duration:", duration)
        sys.print("  Spot:", spot, "| Strike:", strike)
        sys.print("  American Option Price (PDE Enhanced):", price)
        sys.print("--------------------------------------------------")
        
        res = price
    end res
end

# 3. Demonstration of Trading American Put/Call Options
sys.print("--- Gem BSM American Option Analysis (Julia PDE Enhanced) ---")
sys.print("Date:", algo.now())
sys.print("--------------------------------------------------")

# Create instance of our enhanced BSM trader
my_bsm = bsm()

# Tickers: "googl" and "amzn" and "aapl" and "amd" and "nvda"
string tickers = ["GOOGL", "AMZN", "AAPL", "AMD", "NVDA"]

# Iterate over all tickers
i = 0
itr.while(fun(i) i < 5 end, fun(i)
    ticker = tickers[i]
    
    # Analyze Weekly American Call
    my_bsm.price_american(ticker, "call", "weekly")
    
    # Analyze Monthly American Put
    my_bsm.price_american(ticker, "put", "monthly")
    
    i = i + 1
end, 0)

sys.print("Session Analysis Complete.")
