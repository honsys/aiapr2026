#!/usr/bin/env python
import os

_url = "https://api.public.com/userapiauthservice/personal/access-tokens"
_valid = 1440 # max minutes valid allowed as of 31mar2026
_key_secret = None

print(_valid, _url, _key_secret)

def gen_token(key=_key_secret):
  import requests
  headers = {"Content-Type": "application/json", "User-Agent": "public-dev-docs"}
  request_body = {"validityInMinutes": _valid,"secret": key}
  response = requests.post(_url, headers=headers, json=request_body)
  data = response.json() # ; print(data)
  return data

def main(args):
  _key_secret = os.getenv('PUBLICOM_KEY_ENV') # ; print(_key_secret, args)
  token = gen_token(_key_secret) ; print(token)

if __name__ == "__main__":
  import sys
  main(sys.argv[1:])



