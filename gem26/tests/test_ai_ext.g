# Test AI extensions
ai.useOllama("mistral")
sys.print("AI Provider set to Ollama with model: " + ai.model)

ai.useMistral("mistral-tiny")
sys.print("AI Provider set to Mistral with model: " + ai.model)

sys.print("AI methods registered.")
