#!/usr/bin/env python3
import os

_url = "https://api.public.com/userapiauthservice/personal/access-tokens"
_valid = 1440 # max minutes valid allowed as of 31mar2026
_key_secret = os.getenv('PUBLICOM_KEY_ENV')
if not(_key_secret): _key_secret = 'FxYrtlFIERwC12QZUeQeyuejwBlXwJxf'
print(_valid, _url, _key_secret)

def gen_token():
  import requests
  headers = {"Content-Type": "application/json", "User-Agent": "public-dev-docs"}
  request_body = {"validityInMinutes": _valid,"secret": 'FxYrtlFIERwC12QZUeQeyuejwBlXwJxf'}
  response = requests.post(_url, headers=headers, json=request_body)
  data = response.json() ; print(data)
  return data

def main(args):
  print(args)
  token = gen_token() ; print(token)

if __name__ == "__main__":
  import sys
  main(sys.argv[1:])



