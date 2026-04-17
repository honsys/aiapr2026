# Gem Tutorial 15: Native Mistral Integration (Cython)
# --------------------------------------------------

# Gem now supports NATIVE integration with the Mistral Python client.
# This is achieved via a Cython bridge, providing better performance
# and direct access to the official client libraries.

# 1. Configuration
# You need to have the 'mistralai' python package installed.
# pip install mistralai

# 2. Using prompt_native
# Unlike 'prompt' which uses curl, 'prompt_native' calls the
# Python library directly from the Gem C++ core.

ai.useMistral("mistral-tiny")
# ai.key("YOUR_MISTRAL_API_KEY")

sys.print("Requesting response via Native Cython Bridge...")
# response = ai.prompt_native("What is the benefit of a Cython bridge?")
# sys.print("Native Mistral:", response)

# 3. Benefits
# - Zero overhead from shell command execution.
# - Leverages official Mistral client authentication and error handling.
# - Seamless integration between C++ performance and Python ease of use.
