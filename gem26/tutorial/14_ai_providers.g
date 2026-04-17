# Gem Tutorial 14: Multiple AI Providers
# ----------------------------------------
# Covers: ai.useMistral, ai.useOllama, ai.useGemini, ai.key, ai.prompt

sys.print("--- Gem AI Providers Tutorial ---")

# 1. Default: Gemini
# ai.key("YOUR_GEMINI_API_KEY")
# sys.print(ai.prompt("Name three STEM languages."))

# 2. Switch to Mistral API
ai.useMistral("mistral-small")
# ai.key("YOUR_MISTRAL_API_KEY")
# sys.print(ai.prompt("Tell me a joke about C++."))

# 3. Native Mistral C++ bridge (fastest, no HTTP overhead)
# sys.print(ai.prompt_native("What is entropy?"))

# 4. Local model via Ollama (no API key needed)
# ai.useOllama("llama3")
# ai.useOllama("mistral", "http://192.168.1.10:11434")  # remote Ollama host
# sys.print(ai.prompt("Summarize the Gem language."))

# 5. Switch back to Gemini
ai.useGemini()
sys.print("Active provider reset to Gemini.")

# 6. Use active provider for code translation
# ai.useMistral()
# use "my_script.py"   # translates with whichever provider is active

sys.print("Tutorial 14 complete.")
