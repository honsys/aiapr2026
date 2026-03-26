# Gem Tutorial 05: Geo & GIS
# ---------------------------

# 1. GeoIP Lookup
sys.print("Looking up current IP location...")
location = geo.lookup()
sys.print("City:", location.city)
sys.print("Country:", location.country)
sys.print("Coordinates:", location.lat, ",", location.lon)

# 2. GeoJSON Handling
# Create defined objects for features
obj Point(lat, lon)
  .lat = lat
  .lon = lon
end

p1 = Point(34.05, -118.24)
p2 = Point(40.71, -74.00)

# Write to file
geo.write_geojson("points.geojson", [p1, p2])
sys.print("GeoJSON file 'points.geojson' created.")

# 3. 2D Map Rendering (Mapnik)
# Render the GeoJSON directly to an image
www.map2d("points.geojson", "map_view.png")
sys.print("Map rendered to 'map_view.png'")
