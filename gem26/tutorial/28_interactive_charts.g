# Gem Tutorial 28: Interactive Charting (2D, 3D, Multi-Axes)
# -----------------------------------------------------------

sys.print("Tutorial 28: Interactive Charting with Gem")

# 1. Basic 2D Scatter/Line Chart
sys.print("Creating basic 2D chart...")
x = [1, 2, 3, 4, 5]
y = [10, 15, 13, 17, 22]

trace1 = obj()
trace1.x = x
trace1.y = y
trace1.name = "Growth Data"
trace1.type = "scatter"
trace1.mode = "lines+markers"

layout = obj()
layout.title = "Interactive 2D Growth Chart"

# chart.plot returns the path to the generated HTML file
path2d = chart.plot([trace1], layout)
sys.print("2D Chart generated:", path2d)


# 2. 3D Scatter Chart
sys.print("Creating 3D scatter chart...")
x3 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
y3 = [10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
z3 = [1, 4, 9, 16, 25, 36, 49, 64, 81, 100]

trace3d = obj()
trace3d.x = x3
trace3d.y = y3
trace3d.z = z3
trace3d.type = "scatter3d"
trace3d.mode = "markers"
trace3d.name = "3D Power Distribution"

layout3d = obj()
layout3d.title = "Gem 3D Visualization"

path3d = chart.plot([trace3d], layout3d)
sys.print("3D Chart generated:", path3d)


# 3. Multi-Axes Chart (Dual Y-Axes)
sys.print("Creating Multi-Axes chart...")
y_alt = [100, 150, 120, 180, 210]

# Primary trace (uses default Y axis)
t_primary = obj()
t_primary.x = x
t_primary.y = y
t_primary.name = "Temp (C)"
t_primary.type = "scatter"

# Secondary trace (uses second Y axis)
t_secondary = obj()
t_secondary.x = x
t_secondary.y = y_alt
t_secondary.name = "Pressure (hPa)"
t_secondary.type = "bar"
t_secondary.yaxis = "y2"

# Layout with dual axis configuration
layout_multi = obj()
layout_multi.title = "Dual Axis: Temperature & Pressure"

yax1 = obj()
yax1.title = "Temperature (C)"
layout_multi.yaxis = yax1

yax2 = obj()
yax2.title = "Pressure (hPa)"
yax2.overlaying = "y"
yax2.side = "right"
layout_multi.yaxis2 = yax2

path_multi = chart.plot([t_primary, t_secondary], layout_multi)
sys.print("Multi-axes chart generated:", path_multi)


# 4. Serving the Chart
# You can serve the chart via a background web server
# chart.server(path_multi, 8080)
# sys.print("Multi-axes chart is being served at http://localhost:8080")

# 5. Opening the Chart
# chart.show(path2d)

sys.print("Tutorial completed. Check the generated .html files in the current directory.")
