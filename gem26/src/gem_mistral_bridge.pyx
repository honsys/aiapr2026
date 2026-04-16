# Cython bridge for Mistral AI Python Client (mistralai >= 1.0, Python 3.14)
# Regenerate with: cython --cplus gem_mistral_bridge.pyx

from libc.stdlib cimport malloc, free
from libc.string cimport strcpy

cdef public char* call_mistral_native(char* api_key, char* model, char* prompt):
    cdef char* c_string
    cdef char* c_err
    try:
        from mistralai import Mistral

        client = Mistral(api_key=api_key.decode('utf-8'))
        response = client.chat.complete(
            model=model.decode('utf-8'),
            messages=[{"role": "user", "content": prompt.decode('utf-8')}]
        )
        response_str = response.choices[0].message.content
        byte_str = response_str.encode('utf-8')

        c_string = <char*>malloc(len(byte_str) + 1)
        if c_string == NULL:
            return NULL
        strcpy(c_string, byte_str)
        return c_string
    except Exception as e:
        err_msg = ("Error: " + str(e)).encode('utf-8')
        c_err = <char*>malloc(len(err_msg) + 1)
        if c_err == NULL:
            return NULL
        strcpy(c_err, err_msg)
        return c_err

cdef public char* call_yfinance_ticker(char* symbol):
    cdef char* c_string
    cdef char* c_err
    try:
        import yfinance as yf
        import json

        ticker = yf.Ticker(symbol.decode('utf-8'))
        info = ticker.info

        data = {
            "symbol": info.get("symbol"),
            "price": info.get("currentPrice") or info.get("regularMarketPrice"),
            "open": info.get("open"),
            "high": info.get("dayHigh"),
            "low": info.get("dayLow"),
            "volume": info.get("volume"),
            "currency": info.get("currency")
        }

        response_str = json.dumps(data)
        byte_str = response_str.encode('utf-8')

        c_string = <char*>malloc(len(byte_str) + 1)
        if c_string == NULL:
            return NULL
        strcpy(c_string, byte_str)
        return c_string
    except Exception as e:
        err_msg = ("Error: " + str(e)).encode('utf-8')
        c_err = <char*>malloc(len(err_msg) + 1)
        if c_err == NULL:
            return NULL
        strcpy(c_err, err_msg)
        return c_err

cdef public char* call_tradingview_high_yield(char* type):
    cdef char* c_string
    cdef char* c_err
    try:
        import requests
        import json

        type_str = type.decode('utf-8').lower()

        url = "https://scanner.tradingview.com/america/scan"
        columns = ["name", "close", "change", "dividend_yield_recent", "description", "type"]

        if "bond" in type_str:
            url = "https://scanner.tradingview.com/bond/scan"
            columns = ["name", "description", "yield_to_maturity", "close", "coupon", "maturity_date"]
            payload = {
                "filter": [{"left": "yield_to_maturity", "operation": "greater", "right": 0}],
                "markets": ["bond"],
                "columns": columns,
                "sort": {"sortBy": "yield_to_maturity", "sortOrder": "desc"},
                "range": [0, 50]
            }
        else:
            filter_type = ["fund"] if "etf" in type_str else ["stock"]
            payload = {
                "filter": [
                    {"left": "dividend_yield_recent", "operation": "greater", "right": 5},
                    {"left": "type", "operation": "in_range", "right": filter_type}
                ],
                "markets": ["america"],
                "columns": columns,
                "sort": {"sortBy": "dividend_yield_recent", "sortOrder": "desc"},
                "range": [0, 50]
            }

        headers = {
            "Content-Type": "application/json",
            "User-Agent": "Mozilla/5.0"
        }
        response = requests.post(url, json=payload, headers=headers, timeout=10)
        data = response.json()

        results = []
        if data and "data" in data and data["data"] is not None:
            for item in data["data"]:
                d = {col: item["d"][i] for i, col in enumerate(columns) if i < len(item["d"])}
                results.append(d)

        response_str = json.dumps(results)
        byte_str = response_str.encode('utf-8')

        c_string = <char*>malloc(len(byte_str) + 1)
        if c_string == NULL:
            return NULL
        strcpy(c_string, byte_str)
        return c_string
    except Exception as e:
        err_msg = ("Error: " + str(e)).encode('utf-8')
        c_err = <char*>malloc(len(err_msg) + 1)
        if c_err == NULL:
            return NULL
        strcpy(c_err, err_msg)
        return c_err
