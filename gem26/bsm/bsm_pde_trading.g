# Gem Finance: American Option Analysis (Julia PDE Enhanced)
# --------------------------------------------------------

# Define global double quote placeholder
_DQ = "\""

# 1. Define the bsm object extending fin
obj bsm() : fin
    fun price_american(ticker_sym, type, duration)
        data = .ticker(ticker_sym)
        double spot = data.price
        double strike = spot 
        double rate = 0.05
        double vol = 0.30
        
        sys.print("Analyzing:", ticker_sym, "| Type:", type, "| Duration:", duration)
        sys.print("  Current Price:", spot)
        
        temp_out = "/tmp/bsm_res.txt"
        
        # Build command dynamically to avoid Gem interpreter recording bug
        raw_cmd = "julia -e 'include(Qbsm_pde_nopkg.jlQ); print(enhanced_bsm_american(Qticker_symQ, QtypeQ, QdurationQ, spot_val, strike_val, rate_val, vol_val))' > temp_path"
        
        cmd = text.replace(raw_cmd, "temp_path", temp_out)
        cmd = text.replace(cmd, "ticker_sym", ticker_sym)
        cmd = text.replace(cmd, "type", type)
        cmd = text.replace(cmd, "duration", duration)
        cmd = text.replace(cmd, "spot_val", spot)
        cmd = text.replace(cmd, "strike_val", strike)
        cmd = text.replace(cmd, "rate_val", rate)
        cmd = text.replace(cmd, "vol_val", vol)
        cmd = text.replace(cmd, "Q", _DQ)
        
        sys.exec(cmd)
        
        _bsm_price = 0.0
        val_str = text.read(temp_out)
        
        # Loader script to get the numeric value back
        sys.exec("echo '_bsm_price = " + val_str + "' > /tmp/bsm_load.g")
        use "/tmp/bsm_load.g"
        
        sys.print("  American Option Price (PDE):", _bsm_price)
        sys.print("--------------------------------------------------------")
        
        res = _bsm_price
    end res
end

# 2. Demonstration Program
sys.print("--- Gem BSM American Option Trading Simulation ---")
sys.print("Date:", algo.now())
sys.print("--------------------------------------------------------")

trader = bsm()

# Tickers: "GOOGL", "AMZN", "AAPL", "AMD", "NVDA"
# Explicitly analyze each ticker due to loop limitations
trader.price_american("GOOGL", "call", "weekly")
trader.price_american("GOOGL", "put", "monthly")

trader.price_american("AMZN", "call", "weekly")
trader.price_american("AMZN", "put", "monthly")

trader.price_american("AAPL", "call", "weekly")
trader.price_american("AAPL", "put", "monthly")

trader.price_american("AMD", "call", "weekly")
trader.price_american("AMD", "put", "monthly")

trader.price_american("NVDA", "call", "weekly")
trader.price_american("NVDA", "put", "monthly")

sys.print("Analysis Complete.")
