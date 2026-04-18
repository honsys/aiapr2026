# 45_webgpu_mindmap.g — Serve the 3D WebGPU Mindmap
# This script starts a local server to visualize the Gem mindmap as a 3D dogwood tree.
# Requirements: A browser with WebGPU support (Chrome 113+, Edge 113+, Safari Tech Preview).

sys.print("********************************")
sys.print("* Gem 3D WebGPU Mindmap Server *")
sys.print("********************************")

# Serve the 'docs' directory so the HTML can access gem_mindmap.md
int port = 8085
sys.print("Starting server on http://localhost:" + tostr(port) + "/webgpu_mindmap.html")

# The www.app builtin takes a port and a map of routes.
# Since we want to serve files from 'docs/', we can just serve the content of the HTML file at the root,
# but the relative fetch in JS ('gem_mindmap.md') expects it to be in the same dir.
# Let's read the files and serve them.

string html = text.read("docs/webgpu_mindmap.html")
string md = text.read("docs/gem_mindmap.md")

www.app(port, {
    "/webgpu_mindmap.html": html,
    "/gem_mindmap.md": md,
    "/": "<h1>Gem 3D WebGPU</h1><p><a href='/webgpu_mindmap.html'>Open 3D Mindmap</a></p>"
})

# Keep the script alive
while true
    sys.exec("sleep 1")
end
