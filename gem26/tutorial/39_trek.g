# Gem Tutorial 39: Trek — Travel Log with OSM/geo/GIS
# ----------------------------------------------------
# Covers: trek builtin, OpenStreetMap display, GeoJSON editing, GPX export

sys.print("--- Gem Trek Tutorial ---")

# 1. Create a new travel log
string log = "my_trip.geojson"
trek.new(log)
sys.print("Created:", log)

# 2. Add waypoints
trek.add(log, 48.8566, 2.3522, "Paris", "Eiffel Tower visit")
trek.add(log, 51.5074, -0.1278, "London", "Tower Bridge")
trek.add(log, 52.3676, 4.9041, "Amsterdam", "Rijksmuseum")
sys.print("Added 3 waypoints")

# 3. Edit a waypoint
trek.edit(log, 0, "Paris", "Eiffel Tower + Louvre")
sys.print("Edited waypoint 0")

# 4. Stats
string s = trek.stats(log)
sys.print("Waypoints:", s.waypoints, "  Distance:", s.distance_km, "km")

# 5. Display interactive OSM map (opens browser, live edit UI)
string url = trek.show(log)
sys.print("OSM map →", url)

# 6. Export to GPX
trek.export_gpx(log, "my_trip.gpx")
sys.print("GPX exported → my_trip.gpx")

# 7. Remove a waypoint
trek.remove(log, 1)
sys.print("Removed waypoint 1 (London)")

sys.print("Tutorial 39 complete.")
