# Test fin extensions
sys.print("fin object: " + fin)
sys.print("fin.www: " + fin.www)
sys.print("fin.high_yield_bonds: " + fin.get("high_yield_bonds"))
sys.print("fin.high_yield_etfs: " + fin.get("high_yield_etfs"))
sys.print("fin.high_yield_equities: " + fin.get("high_yield_equities"))
sys.print("fin.dashboard: " + fin.get("dashboard"))

bonds = fin.high_yield_bonds()
sys.print("Bonds fetched: " + bonds.length)
if bonds.length > 0 {
    sys.print("First bond: " + bonds[0].description + " - Yield: " + bonds[0].yield_to_maturity)
}

equities = fin.high_yield_equities()
sys.print("Equities fetched: " + equities.length)
if equities.length > 0 {
    sys.print("First equity: " + equities[0].description + " - Dividend Yield: " + equities[0].dividend_yield_recent)
}
