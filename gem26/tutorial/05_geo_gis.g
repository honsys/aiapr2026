# Gem Tutorial 05: Geo & GIS
# ---------------------------
# Covers: geo.lookup, geo.distance, geo.write_geojson, geo.plot2d, geo.plot3d

sys.print("--- Gem Geo & GIS Tutorial ---")

# 1. GeoIP Lookup
location = geo.lookup()
sys.print("City:", location.city)
sys.print("Country:", location.country)
sys.print("Coordinates:", location.lat, ",", location.lon)

# 2. Haversine Distance
double d = geo.distance(34.05, -118.24, 40.71, -74.00)
sys.print("LA → NYC:", d, "km")

# 3. Write GeoJSON
string feat1 = { "type": "Feature", "geometry": { "type": "Point", "coordinates": [-118.24, 34.05] }, "properties": { "name": "Los Angeles" } }
string feat2 = { "type": "Feature", "geometry": { "type": "Point", "coordinates": [-74.00, 40.71] }, "properties": { "name": "New York" } }
geo.write_geojson("cities.geojson", [feat1, feat2])
sys.print("GeoJSON written to cities.geojson")

# 4. 2D Interactive Map (Plotly open-street-map)
city_data = obj()
city_data.type = "scattermapbox"
city_data.lat = [34.05, 40.71, 51.50, 35.68, -33.86]
city_data.lon = [-118.24, -74.00, -0.12, 139.65, 151.20]
city_data.mode = "markers+text"
city_data.text = ["LA", "NYC", "London", "Tokyo", "Sydney"]
city_data.marker = obj()
city_data.marker.size = 12
city_data.marker.color = "red"

layout = obj()
layout.title = "Global Cities"
layout.mapbox = obj()
layout.mapbox.style = "open-street-map"
layout.mapbox.zoom = 1

path2d = geo.plot2d([city_data], layout)
sys.print("2D map →", path2d)

# 5. 3D Globe
path3d = geo.plot3d([city_data])
sys.print("3D globe →", path3d)

# 6. Tectonic History (AI-assisted)
# history = geo.history("Pacific")
# sys.print("Pacific plate history:", history)

sys.print("Tutorial 05 complete.")
