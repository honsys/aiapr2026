use "bsm_pde.gm"
sys.print("Testing enhanced_bsm_american...")
p = enhanced_bsm_american("AAPL", "call", "weekly", 150.0, 150.0, 0.05, 0.30)
sys.print("Price:", p)
