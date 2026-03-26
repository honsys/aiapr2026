# Gem Tutorial 07: AI & NLP
# --------------------------

# 1. AI Prompting (Gemini Pro)
sys.print("Asking Gemini for a haiku...")
# response = ai.prompt("Write a haiku about a gem language.")
# sys.print("Gemini:", response)

# 2. Natural Language Processing
text_to_analyze = "Gem is a wonderful language for automation and science."
cleaned = nlp.clean(text_to_analyze)
sys.print("Cleaned:", cleaned)

sentiment = nlp.sentiment(text_to_analyze)
sys.print("Sentiment Score:", sentiment)

# 3. Tokenization
words = nlp.tokenize(text_to_analyze)
sys.print("First word object:", words[0].text)
# (Note: list indexing implemented via internal logic)

# 4. Polyglot Translation (The 'use' keyword)
# The 'use' keyword uses Gemini AI to translate code to Gem.
# Example:
# use "test_python.py"
# This will translate the Python code to Gem and execute it.
# It supports: Python, R, Julia, Fortran, and C++.

