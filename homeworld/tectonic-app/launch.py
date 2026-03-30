#!/usr/bin/env python3
"""
Native WebKit window launcher for macOS and Linux.
Uses pywebview (WKWebView on macOS, WebKitGTK on Linux).
"""
import os, sys, threading, http.server, socketserver, webbrowser

PORT = 3131
PUBLIC = os.path.join(os.path.dirname(__file__), "public")

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *a, **kw):
        super().__init__(*a, directory=PUBLIC, **kw)
    def log_message(self, *_): pass  # silence

def start_server():
    with socketserver.TCPServer(("127.0.0.1", PORT), Handler) as httpd:
        httpd.serve_forever()

threading.Thread(target=start_server, daemon=True).start()

URL = f"http://127.0.0.1:{PORT}/"

try:
    import webview
    window = webview.create_window(
        "Deep Time — Earth & Solar System Tectonic History",
        URL,
        width=1280, height=800,
        min_size=(800, 500),
        background_color="#000000",
    )
    # Expose Python API to JS (for future native integration)
    class API:
        def get_platform(self): return sys.platform
    webview.start(debug=False)

except ImportError:
    print("pywebview not found — opening in default browser")
    webbrowser.open(URL)
    input("Press Enter to quit…")
