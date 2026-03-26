# Test high yield bonds, etfs, and equities
sys.print("--- High Yield Bonds ---")
bonds = fin.high_yield_bonds()
sys.print("Bonds type check: " + bonds)
sys.print("Bonds length: " + bonds.length)

sys.print("\n--- High Yield ETFs ---")
etfs = fin.high_yield_etfs()
sys.print("ETFs length: " + etfs.length)

sys.print("\n--- High Yield Equities ---")
equities = fin.high_yield_equities()
sys.print("Equities length: " + equities.length)
