# Test Gem Interactive Charting
sys.print("Testing 2D and 3D Charting...")

# Create 2D data
double x = [1, 2, 3, 4, 5]
double y = [10, 15, 13, 17, 22]

trace1 = obj()
trace1.x = x
trace1.y = y
trace1.type = "scatter"
trace1.mode = "lines+markers"
trace1.name = "Sample 2D Data"

layout = obj()
layout.title = "Gem 2D Chart Test"

# Generate chart
path2d = chart.plot([trace1], layout)
sys.print("2D Chart generated: " + path2d)

# Create 3D data
x3 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
y3 = [10, 11, 12, 13, 14, 15, 16, 17, 18, 19]
z3 = [1, 4, 9, 16, 25, 36, 49, 64, 81, 100]

trace2 = obj()
trace2.x = x3
trace2.y = y3
trace2.z = z3
trace2.type = "scatter3d"
trace2.mode = "lines+markers"
trace2.name = "Sample 3D Data"

layout3d = obj()
layout3d.title = "Gem 3D Chart Test"

path3d = chart.plot([trace2], layout3d)
sys.print("3D Chart generated: " + path3d)

# Test multiple axes (traces)
trace3 = obj()
trace3.x = x
trace3.y = [5, 4, 3, 2, 1]
trace3.type = "bar"
trace3.name = "Overlay Bar"

pathMulti = chart.plot([trace1, trace3], layout)
sys.print("Multi-trace Chart generated: " + pathMulti)

sys.print("All chart tests passed.")
