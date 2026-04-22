import requests

url = "https://api.public.com/userapiauthservice/personal/access-tokens"
headers = {
    "Content-Type": "application/json",
    "User-Agent": "public-dev-docs"
}

request_body = {
  "validityInMinutes": 123,
  "secret": "string"
}

response = requests.post(url, headers=headers, json=request_body)
data = response.json()
print(data)
