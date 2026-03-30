#!/usr/bin/env bash
# Launch Deep Time webapp in native WebKit window
# Works on macOS (WKWebView) and Linux (WebKitGTK)

set -e
cd "$(dirname "$0")"

# Linux: ensure WebKitGTK is available
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  if ! python3 -c "import gi; gi.require_version('WebKit2','4.0')" 2>/dev/null; then
    echo "Installing WebKitGTK Python bindings…"
    sudo apt-get install -y python3-gi gir1.2-webkit2-4.0 2>/dev/null || \
    sudo dnf install -y python3-gobject webkit2gtk3 2>/dev/null || true
  fi
fi

# Ensure pywebview
python3 -c "import webview" 2>/dev/null || pip3 install pywebview --quiet

echo "🌌 Launching Deep Time — Earth & Solar System Tectonic History"
python3 launch.py
