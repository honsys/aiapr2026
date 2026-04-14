# globe_tz.g — Earth Globe Timezone App
# PWA server: 3D globe + geolocation + current/selected timezone date-time
# Platforms: Android, iPhone, macOS, Linux, Windows 11
# Run: ./gem globe_tz.g   → http://localhost:8080  (mobile: http://<host-ip>:8080)

int    port    = 8080
string ui_path = "globe_tz.html"

# ── /tz POST handler ──────────────────────────────────────────────────────────
# Browser POSTs: { lat, lon }
# Returns: { timezone, offset, datetime, city, country }
fun handle_tz(req)
  double lat = req.lat
  double lon = req.lon

  # Derive timezone from coordinates via AI
  string prompt = "Given latitude " + lat + " and longitude " + lon + " return ONLY valid JSON with keys: timezone (IANA string), offset_hours (number), city (string), country (string). No explanation."
  string info = ai.prompt(prompt)

  # Compute current datetime in that timezone
  string now_prompt = "Given IANA timezone from this JSON: " + info + " return ONLY valid JSON adding key datetime (ISO 8601 local time string for right now). No explanation."
  ai.prompt(now_prompt)
end

# ── /tzname POST handler ──────────────────────────────────────────────────────
# Browser POSTs: { timezone }  (IANA name from globe click)
# Returns: { timezone, offset_hours, datetime, city, country }
fun handle_tzname(req)
  string tz = req.timezone
  string prompt = "For IANA timezone " + tz + " return ONLY valid JSON with keys: timezone (string), offset_hours (number), city (string), country (string), datetime (ISO 8601 local time string for right now). No explanation."
  ai.prompt(prompt)
end

# ── routes ────────────────────────────────────────────────────────────────────
string routes = {
  "/"       : ui_path,
  "/tz"     : handle_tz,
  "/tzname" : handle_tzname
}

sys.print("🌍 Globe Timezone App")
sys.print("   Desktop  → http://localhost:" + port)
sys.print("   Mobile   → http://<your-ip>:" + port)
sys.print("   /tz      POST {lat,lon}      → timezone + datetime")
sys.print("   /tzname  POST {timezone}     → timezone + datetime")

sys.app(port, routes)
