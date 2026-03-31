#!/usr/bin/env python3
import requests

_url = "https://api.public.com/userapiauthservice/personal/access-tokens"
#_valid = 60*24*31 # 31 day validity of secret
_valid = 1440 # max minutes valid allowed as of 31mar2026
print(_valid, _url)

headers = {
    "Content-Type": "application/json",
    "User-Agent": "public-dev-docs"
}

request_body = {
  "validityInMinutes": _valid,
  "secret": "string"
}

response = requests.post(_url, headers=headers, json=request_body)
data = response.json()
print(data)

