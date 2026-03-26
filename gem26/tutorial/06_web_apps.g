# Gem Tutorial 06: Web & Networking
# ---------------------------------

# 1. Wget
sys.print("Downloading Gem logo...")
# www.wget("https://raw.githubusercontent.com/google/gemini-cli/main/logo.png", "logo.png")

# 2. Functional Redirect
# starts a background server that redirects all traffic to Google
www.redirect("https://www.google.com", 9001)
sys.print("Redirect server running on http://localhost:9001")

# 3. WebApp (Flask-like)
# You can define a map of routes and their HTML content
_routes = {
    "/": "<h1>Gem App</h1><p>Home Page</p><a href='/info'>Info</a>",
    "/info": "<h1>Info</h1><p>Gem is a high-performance STEM language.</p><a href='/'>Back</a>"
}

sys.print("Starting Web App on port 9000...")
www.app(9000, _routes)

sys.print("Server is running in the background.")
sys.print("Visit http://localhost:9000 to see your app.")
