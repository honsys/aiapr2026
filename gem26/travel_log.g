# travel_log.g — Cross-platform NLP Travel Log Server
# Runs on macOS, Linux, Windows 11 (WSL/native), Android (Termux), iOS (iSH)
# Any browser on any device connects to http://<host>:8080

use "mobl.gm"

int    port     = 8080
string log_path = "travel_log.geojson"
string ui_path  = "travel_log.html"

mobl phone("GemTravelLog")

# ── /log POST handler ─────────────────────────────────────────────────────────
# Browser POSTs: { lat, lon, text }
# Gem builds GeoJSON feature via AI, appends to log file, returns feature JSON
fun handle_log(req)
  double lat  = req.lat
  double lon  = req.lon
  string text = req.text

  string feat = phone.make_feature(lat, lon, text)

  # Append to running GeoJSON collection on disk
  string existing = "[]"
  if file.exists(log_path)
    existing = text.read(log_path)
  end
  # ai does the JSON array append to keep Gem code minimal
  string updated = ai.prompt("Given this JSON array: " + existing + " append this element and return only the updated JSON array: " + feat)
  file.write(log_path, updated)

  feat
end

# ── /data GET handler — serve raw GeoJSON ─────────────────────────────────────
fun handle_data(req)
  string fc = "{\"type\":\"FeatureCollection\",\"features\":[]}"
  if file.exists(log_path)
    string arr = text.read(log_path)
    fc = "{\"type\":\"FeatureCollection\",\"features\":" + arr + "}"
  end
  fc
end

# ── Route map ─────────────────────────────────────────────────────────────────
string routes = {
  "/"     : ui_path,
  "/log"  : handle_log,
  "/data" : handle_data
}

sys.print("🌍 Gem Travel Log — cross-platform")
sys.print("   Android / iPhone  → open browser to http://<your-ip>:" + port)
sys.print("   macOS / Linux / Win11 → http://localhost:" + port)
sys.print("   /        interactive map + dictation UI")
sys.print("   /data    raw GeoJSON feed")

sys.app(port, routes)
