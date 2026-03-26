# ... bsm object definition ...
# (Let's just use a stub for bsm)
obj bsm_stub() : fin
    fun price_american(t, tp, d)
        sys.print("Stub Pricing:", t)
    end
end
_trader = bsm_stub()
_tickers = ["GOOGL", "AMZN"]
_i = 0
fun loop_body()
    symbol = _tickers[_i]
    _trader.price_american(symbol, "call", "weekly")
    _i = _i + 1
end

loop_body()
sys.print("Index after 1:", _i)
loop_body()
sys.print("Index after 2:", _i)
