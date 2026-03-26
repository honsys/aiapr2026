# Gem Tutorial 34: GIS, Tectonics, and Geo-Visualizations
# This tutorial demonstrates the new 'geo' builtin features for plate history and mapping.

sys.print("--- Gem Geo & Tectonics Tutorial ---")

# 1. Tectonic Plate History
# Uses AI to retrieve geological history for a specific plate.
# sys.setKey("YOUR_API_KEY") # Ensure AI is configured
plate = "Pacific"
sys.print("\nGeological history of the", plate, "plate:")
# history = geo.history(plate)
# sys.print(history)

# 2. 2D Geo Visualization
# Generates a Plotly-based HTML map using the Robinson projection.
city_data = obj()
city_data.type = "scattergeo"
city_data.lat = [34.05, 40.71, 51.50, 35.68, -33.86]
city_data.lon = [-118.24, -74.00, -0.12, 139.65, 151.20]
city_data.mode = "markers+text"
city_data.text = ["LA", "NYC", "London", "Tokyo", "Sydney"]
city_data.marker = obj()
city_data.marker.size = 10
city_data.marker.color = "red"

map2d_path = geo.plot2d([city_data])
sys.print("\n2D Map generated at:", map2d_path)

# 3. 3D Globe Visualization
# Generates an interactive 3D globe using the Orthographic projection.
map3d_path = geo.plot3d([city_data])
sys.print("3D Globe generated at:", map3d_path)

# 4. Customizing Layout
# You can pass an optional layout object to further customize the visualization.
layout = obj()
layout.title = "Global Tech Hubs"
layout.geo = obj()
layout.geo.projection = obj()
layout.geo.projection.type = "natural earth"

custom_map = geo.plot2d([city_data], layout)
sys.print("Custom 2D Map generated at:", custom_map)

sys.print("\nTutorial 34: Geo-tectonics and visualizations documented.")
