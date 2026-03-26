# Gem Tutorial 14: Multiple AI Providers
# ------------------------------------

# Gem now supports both Google Gemini and Mistral AI.

# 1. Switching to Mistral AI
# Use the 'useMistral' method to switch providers.
# You can optionally specify a model name (default is 'mistral-tiny').
ai.useMistral("mistral-small")

# 2. Setting API Keys
# Each provider requires its own API key.
# ai.setKey("YOUR_MISTRAL_API_KEY")

# 3. Prompting
# sys.print("Asking Mistral for a joke...")
# response = ai.prompt("Tell me a joke about C++.")
# sys.print("Mistral:", response)

# 4. Switching back to Gemini
ai.useGemini()
# ai.setKey("YOUR_RAPIDAPI_KEY")

# 5. Translation with Mistral
# The 'use' keyword will use whichever provider is currently active.
# If you prefer Mistral for code translation:
# ai.useMistral()
# use "my_script.py"
