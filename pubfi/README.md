# Pubfi - public.com Gem API

Pubfi is a conversion tool and Gem language implementation of the public.com Python SDK. It allows you to interact with the public.com trading API using the Gem language.

## Overview

The project consists of:
- `py2gem.gm`: A Gem module that automates the translation of the public.com Python SDK to Gem.
- `py2gem.g`: A CLI application that runs the translation process.
- `public.gm`: The resulting Gem module containing the full API (auto-generated).
- `public.g`: A Gem CLI application for interacting with public.com (auto-generated).

## Requirements

- [Gem Language](https://github.com/davidbhon/gem26) (0.1.0+)
- public.com Python SDK source code (usually located in `../publicdotcom-py`)

## Installation & Setup

1. Clone this repository into your workspace.
2. Ensure the `gem` executable is in your PATH or specify its location.
3. Run the conversion:

```bash
make all
```

This will generate `public.gm`, `public.g`, and several example files.

## Usage

### Using the CLI

Set your API key in the environment:

```bash
export PUBLICOM_KEY_ENV="your_api_key"
```

Then run commands using the generated `public.g`:

```bash
gem public.g accounts
gem public.g portfolio
gem public.g quote AAPL
```

### Using the Module in Gem

```gem
use "public.gm"

# Initialize client
string api_key = sys.env("PUBLICOM_KEY_ENV")
client = PublicApiClient(ApiKeyAuthConfig(api_key))

# Fetch accounts
accounts = client.get_accounts()
sys.print(accounts)

client.close()
```

## Makefile Commands

- `make convert`: Run the Python to Gem conversion.
- `make examples`: Generate example scripts.
- `make all`: Run both conversion and example generation.
- `make test`: Run a basic account check (requires API key).
- `make clean`: Remove all generated files.

## Supported Platforms

- **macOS**: Native execution via `gem`.
- **Linux**: Native execution via `gem`.
- **Windows 11**: Execution via `gem.exe` or through WSL using the Linux `gem` binary.

## Project Structure

- `py2gem.gm`: Conversion logic.
- `py2gem.g`: Conversion CLI runner.
- `Makefile`: Build and automation tool.
- `public.gm`: Auto-generated API module.
- `public.g`: Auto-generated CLI application.
- `example_*.g`: Auto-generated example scripts.
