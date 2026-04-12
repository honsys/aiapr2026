# Gem Tutorial 06: Web Apps & Routing
# -------------------------------------
# Covers: sys.app with string routes, file routes, and handler functions

sys.print("--- Gem Web App Tutorial ---")

# 1. Simple string routes
string routes = {
  "/":     "<h1>Gem App</h1><p>Welcome. <a href='/info'>Info</a></p>",
  "/info": "<h1>Info</h1><p>Gem is a high-performance STEM language. <a href='/'>Back</a></p>"
}
sys.print("Starting web app on port 9000...")
sys.app(9000, routes)
sys.print("Visit http://localhost:9000")

# 2. File route — serve an HTML file directly
# string file_routes = { "/map": "travel_map.html" }
# sys.app(9001, file_routes)

# 3. Handler function route — dynamic response
fun handle_status(req)
  "{\"status\": \"ok\", \"time\": \"" + algo.now() + "\"}"
end

string api_routes = {
  "/":       "<h1>API Server</h1>",
  "/status": handle_status
}
# sys.app(9002, api_routes)

# 4. HTTP Redirect server
# sys.redirect("https://www.google.com", 9003)
# sys.print("Redirect server on http://localhost:9003")

sys.print("Tutorial 06 complete.")
