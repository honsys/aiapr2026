fun test_f(type, S, K, r, sigma, T)
    sys.print("Types:", type, S, K, r, sigma, T)
    res = S + K
end res

sys.print("Calling test_f...")
p = test_f("call", 150.0, 150.0, 0.05, 0.30, 0.25)
sys.print("Price:", p)
