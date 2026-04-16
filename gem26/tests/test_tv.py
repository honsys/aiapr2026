import requests
import json

def test_tv(type_str):
    url = "https://scanner.tradingview.com/america/scan"
    filter_type = ["stock", "fund"]
    sort_by = "dividend_yield_recent"
    columns = ["name", "close", "change", "dividend_yield_recent", "description", "type"]
    
    if "bond" in type_str:
        url = "https://scanner.tradingview.com/bond/scan"
        columns = ["name", "description", "yield_to_maturity", "close", "coupon", "maturity_date"]
        payload = {
            "filter": [
                {"left": "yield_to_maturity", "operation": "greater", "right": 0}
            ],
            "options": {"lang": "en"},
            "markets": ["bond"],
            "columns": columns,
            "sort": {"sortBy": "yield_to_maturity", "sortOrder": "desc"},
            "range": [0, 50]
        }

    else:
        if "etf" in type_str:
            filter_type = ["fund"]
        elif "equit" in type_str or "stock" in type_str:
            filter_type = ["stock"]
        
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
        "User-Agent": "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    }
    response = requests.post(url, json=payload, headers=headers)
    print(f"Status Code for {type_str}: {response.status_code}")
    try:
        data = response.json()
        if response.status_code != 200:
            print(f"Error for {type_str}: {data}")
            return

        if "data" in data and data["data"] is not None:
            print(f"Data length for {type_str}: {len(data['data'])}")
            if len(data['data']) > 0:
                print(f"First item d: {data['data'][0]['d']}")
        else:
            print(f"No 'data' in response for {type_str}: {data}")
    except Exception as e:
        print(f"Error parsing JSON for {type_str}: {e}")

test_tv("bonds")
test_tv("etfs")
test_tv("equities")
