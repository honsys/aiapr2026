use "public.gm"

# Example: List accounts
string api_key = sys.env("PUBLICOM_KEY_ENV")
if api_key == ""
  sys.print("Error: PUBLICOM_KEY_ENV not set")
  sys.exit()
end

client = PublicApiClient(ApiKeyAuthConfig(api_key))
accounts = client.get_accounts()
sys.print("Accounts:", accounts)
client.close()
