# Gem Tutorial 34: GIS, Tectonics, and Geo-Visualizations
# ---------------------------------------------------------
# Covers: geo.lookup, geo.distance, geo.write_geojson,
#         geo.plot2d, geo.plot3d, geo.history

sys.print("--- Gem Geo & Tectonics Tutorial ---")

# 1. Current location
location = geo.lookup()
sys.print("You are in:", location.city, location.country)
sys.print("Lat/Lon:", location.lat, location.lon)

# 2. Haversine distance
double d = geo.distance(location.lat, location.lon, 51.50, -0.12)
sys.print("Distance to London:", d, "km")

# 3. Tectonic plate history (AI-assisted)
# history = geo.history("Pacific")
# sys.print("Pacific plate history:", history)

# 4. 2D Map — scattermapbox (open-street-map)
city_data = obj()
city_data.type = "scattermapbox"
city_data.lat = [34.05, 40.71, 51.50, 35.68, -33.86]
city_data.lon = [-118.24, -74.00, -0.12, 139.65, 151.20]
city_data.mode = "markers+text"
city_data.text = ["LA", "NYC", "London", "Tokyo", "Sydney"]
city_data.marker = obj()
city_data.marker.size = 12
city_data.marker.color = "crimson"

layout2d = obj()
layout2d.title = "Global Tech Hubs"
layout2d.mapbox = obj()
layout2d.mapbox.style = "open-street-map"
layout2d.mapbox.zoom = 1

path2d = geo.plot2d([city_data], layout2d)
sys.print("2D map →", path2d)

# 5. 3D Globe — scattergeo orthographic
globe_data = obj()
globe_data.type = "scattergeo"
globe_data.lat = city_data.lat
globe_data.lon = city_data.lon
globe_data.mode = "markers+text"
globe_data.text = city_data.text
globe_data.marker = city_data.marker

layout3d = obj()
layout3d.title = "3D Globe View"
layout3d.geo = obj()
layout3d.geo.projection = obj()
layout3d.geo.projection.type = "orthographic"

path3d = geo.plot3d([globe_data], layout3d)
sys.print("3D globe →", path3d)

# 6. Write GeoJSON
string feat = { "type": "Feature", "geometry": { "type": "Point", "coordinates": [-118.24, 34.05] }, "properties": { "name": "Los Angeles" } }
geo.write_geojson("tectonics.geojson", [feat])
sys.print("GeoJSON written.")

sys.print("Tutorial 34 complete.")
