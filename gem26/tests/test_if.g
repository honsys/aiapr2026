duration = "weekly"
T = 0.25
if duration == "weekly"
    T = 7.0 / 365.0
end
sys.print("T:", T)
