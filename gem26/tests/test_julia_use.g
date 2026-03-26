use "bsm_pde.jl"
sys.print("Julia PDE module loaded successfully.")
# Test calling a function from it
res = enhanced_bsm_american("AAPL", "call", "weekly", 150.0, 150.0, 0.05, 0.2)
sys.print("Result:", res)
