"""
public_trader.py — Public.com trading module + CLI

Module usage:
    from public_trader import PublicTrader

CLI usage:
    python public_trader.py --help
"""

import os
import uuid
import argparse
from decimal import Decimal

from public_api_sdk import (
    PublicApiClient,
    PublicApiClientConfiguration,
    ApiKeyAuthConfig,
    OrderInstrument,
    InstrumentType,
    OrderRequest,
    OrderSide,
    OrderType,
    OrderExpirationRequest,
    TimeInForce,
    PreflightRequest,
    OptionExpirationsRequest,
    OptionChainRequest,
    MultilegOrderRequest,
    OrderLegRequest,
    LegInstrument,
    LegInstrumentType,
    OpenCloseIndicator,
    HistoryRequest,
)
from public_api_sdk.exceptions import APIError


# ---------------------------------------------------------------------------
# Module API
# ---------------------------------------------------------------------------

class PublicTrader:
    """Thin wrapper around PublicApiClient for stocks, ETFs, and options."""

    def __init__(self, api_key: str, account_number: str):
        self._client = PublicApiClient(
            ApiKeyAuthConfig(api_secret_key=api_key),
            config=PublicApiClientConfiguration(default_account_number=account_number),
        )

    def close(self):
        self._client.close()

    # -- Account --

    def accounts(self):
        return self._client.get_accounts().accounts

    def portfolio(self):
        return self._client.get_portfolio()

    def history(self, page_size: int = 20):
        return self._client.get_history(HistoryRequest(page_size=page_size))

    # -- Market data --

    def quote(self, *symbols: str, instrument_type: InstrumentType = InstrumentType.EQUITY):
        instruments = [OrderInstrument(symbol=s, type=instrument_type) for s in symbols]
        return self._client.get_quotes(instruments)

    def option_expirations(self, symbol: str):
        req = OptionExpirationsRequest(
            instrument=OrderInstrument(symbol=symbol, type=InstrumentType.EQUITY)
        )
        return self._client.get_option_expirations(req).expirations

    def option_chain(self, symbol: str, expiration_date: str):
        req = OptionChainRequest(
            instrument=OrderInstrument(symbol=symbol, type=InstrumentType.EQUITY),
            expiration_date=expiration_date,
        )
        return self._client.get_option_chain(req)

    def option_greeks(self, osi_symbol: str):
        return self._client.get_option_greek(osi_symbol)

    # -- Orders --

    def preflight(self, symbol: str, side: OrderSide, qty: Decimal,
                  limit_price: Decimal, instrument_type: InstrumentType = InstrumentType.EQUITY):
        req = PreflightRequest(
            instrument=OrderInstrument(symbol=symbol, type=instrument_type),
            order_side=side,
            order_type=OrderType.LIMIT,
            expiration=OrderExpirationRequest(time_in_force=TimeInForce.DAY),
            quantity=qty,
            limit_price=limit_price,
        )
        return self._client.perform_preflight_calculation(req)

    def buy(self, symbol: str, qty: Decimal, limit_price: Decimal,
            instrument_type: InstrumentType = InstrumentType.EQUITY):
        return self._place(symbol, OrderSide.BUY, qty, limit_price, instrument_type)

    def sell(self, symbol: str, qty: Decimal, limit_price: Decimal,
             instrument_type: InstrumentType = InstrumentType.EQUITY):
        return self._place(symbol, OrderSide.SELL, qty, limit_price, instrument_type)

    def _place(self, symbol, side, qty, limit_price, instrument_type):
        req = OrderRequest(
            order_id=str(uuid.uuid4()),
            instrument=OrderInstrument(symbol=symbol, type=instrument_type),
            order_side=side,
            order_type=OrderType.LIMIT,
            expiration=OrderExpirationRequest(time_in_force=TimeInForce.DAY),
            quantity=qty,
            limit_price=limit_price,
        )
        return self._client.place_order(req)

    def buy_option(self, osi_symbol: str, qty: int, limit_price: Decimal):
        return self._place_option(osi_symbol, OrderSide.BUY, qty, limit_price)

    def sell_option(self, osi_symbol: str, qty: int, limit_price: Decimal):
        return self._place_option(osi_symbol, OrderSide.SELL, qty, limit_price)

    def _place_option(self, osi_symbol, side, qty, limit_price):
        req = OrderRequest(
            order_id=str(uuid.uuid4()),
            instrument=OrderInstrument(symbol=osi_symbol, type=InstrumentType.OPTION),
            order_side=side,
            order_type=OrderType.LIMIT,
            expiration=OrderExpirationRequest(time_in_force=TimeInForce.DAY),
            quantity=qty,
            limit_price=limit_price,
        )
        return self._client.place_order(req)

    def spread(self, sell_osi: str, buy_osi: str, qty: int, limit_price: Decimal):
        """Place a two-leg vertical spread (sell one leg, buy another)."""
        req = MultilegOrderRequest(
            order_id=str(uuid.uuid4()),
            quantity=qty,
            type=OrderType.LIMIT,
            limit_price=limit_price,
            expiration=OrderExpirationRequest(time_in_force=TimeInForce.DAY),
            legs=[
                OrderLegRequest(
                    instrument=LegInstrument(symbol=sell_osi, type=LegInstrumentType.OPTION),
                    side=OrderSide.SELL,
                    open_close_indicator=OpenCloseIndicator.OPEN,
                    ratio_quantity=1,
                ),
                OrderLegRequest(
                    instrument=LegInstrument(symbol=buy_osi, type=LegInstrumentType.OPTION),
                    side=OrderSide.BUY,
                    open_close_indicator=OpenCloseIndicator.OPEN,
                    ratio_quantity=1,
                ),
            ],
        )
        return self._client.place_multileg_order(req)

    def order_status(self, order_id: str):
        return self._client.get_order(order_id=order_id)

    def cancel(self, order_id: str):
        self._client.cancel_order(order_id=order_id)


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

def _trader_from_env() -> PublicTrader:
    api_key = os.environ.get("PUBLIC_API_KEY")
    account = os.environ.get("PUBLIC_ACCOUNT_NUMBER")
    if not api_key or not account:
        raise SystemExit(
            "Set PUBLIC_API_KEY and PUBLIC_ACCOUNT_NUMBER environment variables."
        )
    return PublicTrader(api_key, account)


def cmd_accounts(args):
    t = _trader_from_env()
    try:
        for a in t.accounts():
            print(f"{a.account_id}  type={a.account_type}")
    finally:
        t.close()


def cmd_portfolio(args):
    t = _trader_from_env()
    try:
        p = t.portfolio()
        print(f"Equity:        ${p.equity}")
        print(f"Buying power:  ${p.buying_power}")
    finally:
        t.close()


def cmd_quote(args):
    t = _trader_from_env()
    try:
        itype = InstrumentType.OPTION if args.option else InstrumentType.EQUITY
        for q in t.quote(*args.symbols, instrument_type=itype):
            print(f"{q.instrument.symbol:10s}  last=${q.last}  bid=${q.bid}  ask=${q.ask}")
    finally:
        t.close()


def cmd_expirations(args):
    t = _trader_from_env()
    try:
        for exp in t.option_expirations(args.symbol):
            print(exp)
    finally:
        t.close()


def cmd_chain(args):
    t = _trader_from_env()
    try:
        chain = t.option_chain(args.symbol, args.expiration)
        for contract in chain.options:
            print(
                f"{contract.instrument.symbol:25s}  "
                f"strike={contract.strike_price}  "
                f"bid={contract.bid}  ask={contract.ask}"
            )
    finally:
        t.close()


def cmd_greeks(args):
    t = _trader_from_env()
    try:
        g = t.option_greeks(args.osi).greeks
        print(f"delta={g.delta}  gamma={g.gamma}  theta={g.theta}  vega={g.vega}  iv={g.implied_volatility}")
    finally:
        t.close()


def cmd_preflight(args):
    t = _trader_from_env()
    try:
        side = OrderSide.BUY if args.side.upper() == "BUY" else OrderSide.SELL
        itype = InstrumentType.OPTION if args.option else InstrumentType.EQUITY
        r = t.preflight(args.symbol, side, Decimal(args.qty), Decimal(args.price), itype)
        print(f"Order value:  ${r.order_value}")
        print(f"Commission:   ${r.estimated_commission or 0:.2f}")
    finally:
        t.close()


def cmd_buy(args):
    t = _trader_from_env()
    try:
        if args.option:
            r = t.buy_option(args.symbol, int(args.qty), Decimal(args.price))
        else:
            r = t.buy(args.symbol, Decimal(args.qty), Decimal(args.price))
        print(f"Order placed: {r.order_id}")
    finally:
        t.close()


def cmd_sell(args):
    t = _trader_from_env()
    try:
        if args.option:
            r = t.sell_option(args.symbol, int(args.qty), Decimal(args.price))
        else:
            r = t.sell(args.symbol, Decimal(args.qty), Decimal(args.price))
        print(f"Order placed: {r.order_id}")
    finally:
        t.close()


def cmd_spread(args):
    t = _trader_from_env()
    try:
        r = t.spread(args.sell_leg, args.buy_leg, int(args.qty), Decimal(args.price))
        print(f"Spread order placed: {r.order_id}")
    finally:
        t.close()


def cmd_status(args):
    t = _trader_from_env()
    try:
        o = t.order_status(args.order_id)
        print(f"Order {o.order_id}: status={o.status}")
    finally:
        t.close()


def cmd_cancel(args):
    t = _trader_from_env()
    try:
        t.cancel(args.order_id)
        print(f"Cancel request submitted for {args.order_id}")
    finally:
        t.close()


def cmd_history(args):
    t = _trader_from_env()
    try:
        h = t.history(page_size=args.limit)
        for tx in h.transactions:
            print(tx)
    finally:
        t.close()


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="public_trader",
        description="Trade stocks, ETFs, and options on Public.com",
    )
    sub = p.add_subparsers(dest="command", required=True)

    sub.add_parser("accounts", help="List accounts")
    sub.add_parser("portfolio", help="Show portfolio snapshot")

    q = sub.add_parser("quote", help="Get quotes")
    q.add_argument("symbols", nargs="+")
    q.add_argument("--option", action="store_true", help="Treat symbols as option OSI")

    exp = sub.add_parser("expirations", help="List option expiration dates")
    exp.add_argument("symbol")

    ch = sub.add_parser("chain", help="Show option chain")
    ch.add_argument("symbol")
    ch.add_argument("expiration", help="YYYY-MM-DD")

    gr = sub.add_parser("greeks", help="Get option Greeks")
    gr.add_argument("osi", help="OSI symbol e.g. AAPL260116C00270000")

    pf = sub.add_parser("preflight", help="Estimate order cost before placing")
    pf.add_argument("symbol")
    pf.add_argument("side", choices=["buy", "sell", "BUY", "SELL"])
    pf.add_argument("qty")
    pf.add_argument("price")
    pf.add_argument("--option", action="store_true")

    b = sub.add_parser("buy", help="Place a buy order (equity, ETF, or option)")
    b.add_argument("symbol")
    b.add_argument("qty")
    b.add_argument("price", help="Limit price")
    b.add_argument("--option", action="store_true", help="Symbol is an OSI option")

    s = sub.add_parser("sell", help="Place a sell order")
    s.add_argument("symbol")
    s.add_argument("qty")
    s.add_argument("price", help="Limit price")
    s.add_argument("--option", action="store_true")

    sp = sub.add_parser("spread", help="Place a two-leg vertical spread")
    sp.add_argument("sell_leg", help="OSI symbol to sell")
    sp.add_argument("buy_leg", help="OSI symbol to buy")
    sp.add_argument("qty", help="Number of contracts")
    sp.add_argument("price", help="Net limit price (credit positive, debit positive)")

    st = sub.add_parser("status", help="Check order status")
    st.add_argument("order_id")

    ca = sub.add_parser("cancel", help="Cancel an order")
    ca.add_argument("order_id")

    hi = sub.add_parser("history", help="Account transaction history")
    hi.add_argument("--limit", type=int, default=20)

    return p


COMMANDS = {
    "accounts": cmd_accounts,
    "portfolio": cmd_portfolio,
    "quote": cmd_quote,
    "expirations": cmd_expirations,
    "chain": cmd_chain,
    "greeks": cmd_greeks,
    "preflight": cmd_preflight,
    "buy": cmd_buy,
    "sell": cmd_sell,
    "spread": cmd_spread,
    "status": cmd_status,
    "cancel": cmd_cancel,
    "history": cmd_history,
}


def main():
    parser = build_parser()
    args = parser.parse_args()
    try:
        COMMANDS[args.command](args)
    except APIError as e:
        raise SystemExit(f"API error ({e.status_code}): {e.message}")


if __name__ == "__main__":
    main()
