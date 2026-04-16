# Gem Tutorial 36: Mobile & Cross-Platform Travel Log (mobl)
# -----------------------------------------------------------
# Covers: mobl object, NLP dictation, GeoJSON, sys.app PWA server
#
# Architecture:
#   Browser (any device) → Web Speech API + Geolocation API
#       → POST /log → Gem server → ai.prompt NLP → GeoJSON feature
#       → GET /     → travel_log.html (Plotly map PWA)
#
# Supported platforms (browser connects to Gem server):
#   Android Chrome, iPhone Safari, macOS, Linux, Windows 11

# mobl is now a builtin — no use statement needed

sys.print("--- Gem Mobile Travel Log Tutorial ---")

# 1. Instantiate the mobl object
mobl phone("MyPhone")

# 2. NLP dictation — parse spoken text into structured JSON
string spoken = "Stopped at Yosemite Valley, incredible granite cliffs, nature hiking"
string parsed = phone.dictate(spoken)
sys.print("AI parsed note:", parsed)

# 3. Build a GeoJSON feature from GPS + dictation
# In production, lat/lon come from the browser's Geolocation API via POST /log
double lat = 37.7456
double lon = -119.5936
string feature = phone.make_feature(lat, lon, spoken)
sys.print("GeoJSON feature:", feature)

# 4. Write a GeoJSON file
geo.write_geojson("demo_log.geojson", [feature])
sys.print("GeoJSON written → demo_log.geojson")

# 5. Render an interactive map
layout = obj()
layout.title = "Demo Travel Log"
layout.mapbox = obj()
layout.mapbox.style = "open-street-map"
layout.mapbox.zoom = 10
string map_path = geo.plot2d([feature], layout)
sys.print("Map →", map_path)

# 6. Full server mode — run travel_log.g for the complete PWA
# ./gem travel_log.g
# Then open http://localhost:8080 on any device (same WiFi for mobile)
#
# Routes served by travel_log.g:
#   /       → travel_log.html  (PWA: mic button + live Plotly map)
#   /log    → POST handler     (receives {lat, lon, text}, returns GeoJSON feature)
#   /data   → GET handler      (returns full FeatureCollection JSON)

sys.print("Tutorial 36 complete.")
sys.print("Run './gem travel_log.g' for the full cross-platform PWA.")
