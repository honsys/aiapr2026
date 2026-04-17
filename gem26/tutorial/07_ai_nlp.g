# Gem Tutorial 07: AI & NLP
# --------------------------
# Covers: ai.prompt, ai.useMistral, ai.useOllama, ai.useGemini, ai.prompt_native

sys.print("--- Gem AI & NLP Tutorial ---")

# 1. Default provider: Gemini
# response = ai.prompt("Write a haiku about the Gem language.")
# sys.print("Gemini:", response)

# 2. Switch to Mistral
ai.useMistral("mistral-small")
# ai.key("YOUR_MISTRAL_API_KEY")
# response = ai.prompt("Explain quantum entanglement in one sentence.")
# sys.print("Mistral:", response)

# 3. Native Mistral C++ bridge (high-performance)
# response = ai.prompt_native("What is the speed of light?")
# sys.print("Mistral native:", response)

# 4. Local model via Ollama
# ai.useOllama("llama3")
# response = ai.prompt("Summarize the Gem language in 3 bullet points.")
# sys.print("Ollama:", response)

# 5. Switch back to Gemini
ai.useGemini()

# 6. NLP: parse structured data from free text
string spoken = "Arrived at the Golden Gate Bridge, amazing fog, landmark bridge"
string parsed = ai.prompt("Extract a travel note. Return ONLY valid JSON with keys: title, note, tags (array). Text: " + spoken)
sys.print("Parsed travel note:", parsed)

# 7. AI-assisted code translation (use keyword)
# use "legacy_script.py"   # translates Python → Gem and executes
# use "model.jl"           # translates Julia → Gem and executes

sys.print("Tutorial 07 complete.")
