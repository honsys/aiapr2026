# Makefile for pubfi - public.com Python to Gem API converter

# Detect OS
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
else
    DETECTED_OS := $(shell uname -s)
endif

# Configuration
GEM ?= gem
PY_SDK_SRC ?= ../publicdotcom-py/src/public_api_sdk
OUT_DIR ?= .

.PHONY: all convert clean help examples test

all: convert examples

convert:
	@echo "Converting Python SDK to Gem API..."
	$(GEM) py2gem.g $(PY_SDK_SRC) $(OUT_DIR)

examples:
	@echo "Generating examples..."
	@printf 'use "public.gm"\n\n# Example: List accounts\nstring api_key = sys.env("PUBLICOM_KEY_ENV")\nif api_key == ""\n  sys.print("Error: PUBLICOM_KEY_ENV not set")\n  sys.exit()\nend\n\nclient = PublicApiClient(ApiKeyAuthConfig(api_key))\naccounts = client.get_accounts()\nsys.print("Accounts:", accounts)\nclient.close()\n' > example_accounts.g
	@printf 'use "public.gm"\n\n# Example: Get quote for AAPL\nstring api_key = sys.env("PUBLICOM_KEY_ENV")\nif api_key == ""\n  sys.print("Error: PUBLICOM_KEY_ENV not set")\n  sys.exit()\nend\n\nclient = PublicApiClient(ApiKeyAuthConfig(api_key))\ninstr = OrderInstrument("AAPL", InstrumentType.EQUITY)\nquotes = client.get_quotes([instr])\nsys.print("Quotes:", quotes)\nclient.close()\n' > example_quote.g

test:
	@echo "Running basic test (requires PUBLICOM_KEY_ENV)..."
	$(GEM) public.g accounts

clean:
	@echo "Cleaning up generated files..."
	rm -f public.gm public.g example_*.g
	rm -f exceptions.gm auth_config.gm auth_manager.gm auth_provider.gm async_auth_provider.gm api_client.gm async_api_client.gm
	rm -f models_instrument_type.gm models_auth.gm models_account.gm models_history.gm models_instrument.gm models_quote.gm models_portfolio.gm models_subscription.gm models_order.gm models_option.gm models_new_order.gm models_async_new_order.gm
	rm -f public_api_client.gm async_public_api_client.gm subscription_manager.gm async_subscription_manager.gm order_subscription_manager.gm async_order_subscription_manager.gm price_stream.gm async_price_stream.gm strategy_preflight.gm async_strategy_preflight.gm

help:
	@echo "pubfi Makefile"
	@echo "Detected OS: $(DETECTED_OS)"
	@echo ""
	@echo "Targets:"
	@echo "  make convert    - Run py2gem conversion"
	@echo "  make examples   - Generate example scripts"
	@echo "  make all        - convert + examples"
	@echo "  make test       - Run public.g accounts (requires API key)"
	@echo "  make clean      - Remove generated .gm and .g files"
	@echo ""
	@echo "Variables:"
	@echo "  GEM             - Path to gem executable (default: gem)"
	@echo "  PY_SDK_SRC      - Path to publicdotcom-py SDK (default: ../publicdotcom-py/src/public_api_sdk)"
