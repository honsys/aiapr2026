# hontrade.g — HonTrade webapp
# Public.com trading UI served via www.app
# Usage: gem hontrade.g [port]
# Requires: PUBLIC_API_KEY and PUBLIC_ACCOUNT_NUMBER env vars

use "hontrade.gm"

# ── port ──────────────────────────────────────────────────────────────────────

int _port = 8099
_args = sys.args()
if _args.length > 0
  _port = _args[0]
end

# ── CSS / shared layout ───────────────────────────────────────────────────────

_css = "<style>
body{font-family:sans-serif;max-width:900px;margin:40px auto;padding:0 20px;background:#f5f5f5}
h1{color:#1a1a2e}h2{color:#16213e;border-bottom:2px solid #0f3460;padding-bottom:6px}
nav a{margin-right:16px;color:#0f3460;text-decoration:none;font-weight:bold}
nav a:hover{text-decoration:underline}
table{width:100%;border-collapse:collapse;background:#fff;border-radius:8px;overflow:hidden;box-shadow:0 2px 6px rgba(0,0,0,.1)}
th{background:#0f3460;color:#fff;padding:10px 14px;text-align:left}
td{padding:9px 14px;border-bottom:1px solid #eee}
tr:last-child td{border-bottom:none}
form{background:#fff;padding:20px;border-radius:8px;box-shadow:0 2px 6px rgba(0,0,0,.1);max-width:480px}
label{display:block;margin-top:12px;font-weight:bold;color:#333}
input,select{width:100%;padding:8px;margin-top:4px;border:1px solid #ccc;border-radius:4px;box-sizing:border-box}
button{margin-top:16px;padding:10px 24px;background:#0f3460;color:#fff;border:none;border-radius:4px;cursor:pointer;font-size:15px}
button:hover{background:#16213e}
.card{background:#fff;padding:20px;border-radius:8px;box-shadow:0 2px 6px rgba(0,0,0,.1);margin-bottom:20px}
.tag{display:inline-block;padding:2px 8px;border-radius:12px;font-size:12px;font-weight:bold}
.buy{background:#d4edda;color:#155724}.sell{background:#f8d7da;color:#721c24}
.ok{color:#155724}.err{color:#721c24}
</style>"

_nav = "<nav><a href='/'>Portfolio</a><a href='/quote'>Quote</a><a href='/buy'>Buy</a><a href='/sell'>Sell</a><a href='/options'>Options</a><a href='/spread'>Spread</a><a href='/history'>History</a></nav><hr>"

fun page(title, body)
  res = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>" + title + " — HonTrade</title>" + _css + "</head><body><h1>🏦 HonTrade</h1>" + _nav + body + "</body></html>"
end res

# ── portfolio page ────────────────────────────────────────────────────────────

_pf = portfolio()
_pf_html = "<div class='card'><h2>Portfolio</h2><table><tr><th>Field</th><th>Value</th></tr><tr><td>Equity</td><td>$" + _pf.equity + "</td></tr><tr><td>Buying Power</td><td>$" + _pf.buying_power + "</td></tr></table></div>"

_accts = accounts()
_acct_rows = ""
int _ai = 0
while _ai < _accts.length
  _a = _accts[_ai]
  _acct_rows = _acct_rows + "<tr><td>" + _a.account_id + "</td><td>" + _a.account_type + "</td></tr>"
  _ai += 1
end
_accts_html = "<div class='card'><h2>Accounts</h2><table><tr><th>Account ID</th><th>Type</th></tr>" + _acct_rows + "</table></div>"

_home_html = page("Portfolio", _pf_html + _accts_html)

# ── quote page ────────────────────────────────────────────────────────────────

_quote_html = page("Quote", "<h2>Get Quote</h2><form method='get' action='/quote_result'><label>Symbol<input name='symbol' placeholder='AAPL'></label><label><input type='checkbox' name='option' value='1'> Option (OSI symbol)</label><button>Get Quote</button></form>")

# ── buy page ──────────────────────────────────────────────────────────────────

_buy_html = page("Buy", "<h2>Place Buy Order</h2><form method='get' action='/buy_result'><label>Symbol<input name='symbol' placeholder='AAPL'></label><label>Quantity<input name='qty' placeholder='10'></label><label>Limit Price<input name='price' placeholder='227.50'></label><label><input type='checkbox' name='option' value='1'> Option (OSI symbol)</label><button>Preview &amp; Buy</button></form>")

# ── sell page ─────────────────────────────────────────────────────────────────

_sell_html = page("Sell", "<h2>Place Sell Order</h2><form method='get' action='/sell_result'><label>Symbol<input name='symbol' placeholder='AAPL'></label><label>Quantity<input name='qty' placeholder='10'></label><label>Limit Price<input name='price' placeholder='227.50'></label><label><input type='checkbox' name='option' value='1'> Option (OSI symbol)</label><button>Preview &amp; Sell</button></form>")

# ── options page ──────────────────────────────────────────────────────────────

_options_html = page("Options", "<h2>Option Chain</h2><form method='get' action='/chain_result'><label>Underlying Symbol<input name='symbol' placeholder='AAPL'></label><label>Expiration (YYYY-MM-DD)<input name='expiry' placeholder='2026-01-16'></label><button>View Chain</button></form><h2>Option Greeks</h2><form method='get' action='/greeks_result'><label>OSI Symbol<input name='osi' placeholder='AAPL260116C00270000'></label><button>Get Greeks</button></form>")

# ── spread page ───────────────────────────────────────────────────────────────

_spread_html = page("Spread", "<h2>Vertical Spread</h2><form method='get' action='/spread_result'><label>Sell Leg (OSI)<input name='sell_leg' placeholder='AAPL260116C00270000'></label><label>Buy Leg (OSI)<input name='buy_leg' placeholder='AAPL260116C00280000'></label><label>Contracts<input name='qty' placeholder='1'></label><label>Net Limit Price<input name='price' placeholder='1.50'></label><button>Place Spread</button></form>")

# ── history page ──────────────────────────────────────────────────────────────

_hist = history(20)
_hist_rows = ""
int _hi = 0
while _hi < _hist.transactions.length
  _tx = _hist.transactions[_hi]
  _hist_rows = _hist_rows + "<tr><td>" + _tx + "</td></tr>"
  _hi += 1
end
_history_html = page("History", "<h2>Recent Transactions</h2><table><tr><th>Transaction</th></tr>" + _hist_rows + "</table>")

# ── dynamic result routes (server-side via sys.exec + python) ─────────────────

fun quote_result_html(symbol, is_option)
  if is_option == "1"
    q = python.eval("_trader_from_env().quote('" + symbol + "', instrument_type=InstrumentType.OPTION)[0]")
  else
    q = python.eval("_trader_from_env().quote('" + symbol + "')[0]")
  end
  body = "<div class='card'><h2>Quote: " + symbol + "</h2><table><tr><th>Field</th><th>Value</th></tr><tr><td>Last</td><td>$" + q.last + "</td></tr><tr><td>Bid</td><td>$" + q.bid + "</td></tr><tr><td>Ask</td><td>$" + q.ask + "</td></tr></table></div><a href='/quote'>← Back</a>"
end body

fun buy_result_html(symbol, qty, price, is_option)
  if is_option == "1"
    r = buy_option(symbol, qty, price)
  else
    r = buy(symbol, qty, price)
  end
  body = "<div class='card ok'><h2>✅ Buy Order Placed</h2><p>Order ID: <strong>" + r.order_id + "</strong></p><p>" + qty + " × " + symbol + " @ $" + price + "</p></div><a href='/'>← Portfolio</a>"
end body

fun sell_result_html(symbol, qty, price, is_option)
  if is_option == "1"
    r = sell_option(symbol, qty, price)
  else
    r = sell(symbol, qty, price)
  end
  body = "<div class='card ok'><h2>✅ Sell Order Placed</h2><p>Order ID: <strong>" + r.order_id + "</strong></p><p>" + qty + " × " + symbol + " @ $" + price + "</p></div><a href='/'>← Portfolio</a>"
end body

fun chain_result_html(symbol, expiry)
  c = chain(symbol, expiry)
  rows = ""
  int ci = 0
  while ci < c.options.length
    opt = c.options[ci]
    rows = rows + "<tr><td>" + opt.instrument.symbol + "</td><td>" + opt.strike_price + "</td><td>" + opt.bid + "</td><td>" + opt.ask + "</td></tr>"
    ci += 1
  end
  body = "<h2>Chain: " + symbol + " " + expiry + "</h2><table><tr><th>OSI</th><th>Strike</th><th>Bid</th><th>Ask</th></tr>" + rows + "</table><a href='/options'>← Back</a>"
end body

fun greeks_result_html(osi)
  g = greeks(osi).greeks
  body = "<div class='card'><h2>Greeks: " + osi + "</h2><table><tr><th>Greek</th><th>Value</th></tr><tr><td>Delta</td><td>" + g.delta + "</td></tr><tr><td>Gamma</td><td>" + g.gamma + "</td></tr><tr><td>Theta</td><td>" + g.theta + "</td></tr><tr><td>Vega</td><td>" + g.vega + "</td></tr><tr><td>IV</td><td>" + g.implied_volatility + "</td></tr></table></div><a href='/options'>← Back</a>"
end body

fun spread_result_html(sell_leg, buy_leg, qty, price)
  r = spread(sell_leg, buy_leg, qty, price)
  body = "<div class='card ok'><h2>✅ Spread Placed</h2><p>Order ID: <strong>" + r.order_id + "</strong></p><p>Sell: " + sell_leg + " / Buy: " + buy_leg + " × " + qty + " @ $" + price + "</p></div><a href='/'>← Portfolio</a>"
end body

# ── build routes ──────────────────────────────────────────────────────────────

_routes = {
  "/":              _home_html,
  "/quote":         _quote_html,
  "/buy":           _buy_html,
  "/sell":          _sell_html,
  "/options":       _options_html,
  "/spread":        _spread_html,
  "/history":       _history_html,
  "/quote_result":  page("Quote Result",  quote_result_html(www.param("symbol"), www.param("option"))),
  "/buy_result":    page("Buy Result",    buy_result_html(www.param("symbol"), www.param("qty"), www.param("price"), www.param("option"))),
  "/sell_result":   page("Sell Result",   sell_result_html(www.param("symbol"), www.param("qty"), www.param("price"), www.param("option"))),
  "/chain_result":  page("Chain",         chain_result_html(www.param("symbol"), www.param("expiry"))),
  "/greeks_result": page("Greeks",        greeks_result_html(www.param("osi"))),
  "/spread_result": page("Spread Result", spread_result_html(www.param("sell_leg"), www.param("buy_leg"), www.param("qty"), www.param("price")))
}

# ── start ─────────────────────────────────────────────────────────────────────

sys.print("HonTrade starting on http://localhost:" + _port)
www.app(_port, _routes)

while true
  sys.exec("sleep 10")
end
