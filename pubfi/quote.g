use "public.gm"

# Example: Get quote for AAPL
string api_key = sys.env("PUBLICOM_KEY_ENV")
if api_key == ""
  sys.print("Error: PUBLICOM_KEY_ENV not set")
  sys.exit()
end

client = PublicApiClient(ApiKeyAuthConfig(api_key))
instr = OrderInstrument("AAPL", InstrumentType.EQUITY)
quotes = client.get_quotes([instr])
sys.print("Quotes:", quotes)
client.close()
