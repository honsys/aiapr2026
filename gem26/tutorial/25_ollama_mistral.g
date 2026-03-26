# Gem Tutorial 25: Open Source AI with Ollama and Mistral
# -----------------------------------------------------

# Gem now explicitly supports open-source AI models via Ollama.
# This allows you to run Mistral, Llama, and other models locally.

# 1. Using Ollama
# Ensure you have Ollama running (typically on localhost:11434).
# You can download it from ollama.com

# Switch to Ollama provider
ai.useOllama("mistral")

sys.print("Asking local Mistral (via Ollama) for a science fact...")
# response = ai.prompt("Tell me an interesting fact about quantum mechanics.")
# sys.print("Ollama Mistral:", response)

# 2. Using different models with Ollama
# ai.useOllama("llama3", "localhost:11434") # Now supports optional host
# response = ai.prompt("What is the capital of France?")
# sys.print("Ollama Llama3:", response)

# 3. Mistral API vs Native Bridge
ai.useMistral("mistral-small") # Uses curl and Mistral API
ai.setKey("YOUR_MISTRAL_API_KEY")
# ai.prompt("Hello Mistral!")

# ai.prompt_native("Hello from Native Cython Bridge!") # Uses mistralai Python client

# 4. Why use Open Source?
# - Privacy: Data stays on your machine.
# - Cost: No API fees.
# - Flexibility: Use any model supported by Ollama.

sys.print("Open source AI support is now fully integrated into Gem.")
