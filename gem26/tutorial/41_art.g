# 41_art.g — art builtin: ASCII art generation, SVG conversion, doc rendering
# Requires: figlet or toilet (text→art), rsvg-convert + jp2a (SVG→art), mmdc (mermaid)

# 1. Generate ASCII art banner from text
string banner = art.text_to_art("Gem Language", "big")
sys.print(banner)

# 2. Save ASCII art to a text file
art.art_to_file(banner, "banner.txt")
sys.print("Saved banner.txt")

# 3. Convert ASCII art to SVG
string svg = art.art_to_svg(banner, "banner.svg")
sys.print("Saved banner.svg")

# 4. Convert SVG back to ASCII art
string ascii = art.svg_to_art("banner.svg", 80)
sys.print(ascii)

# 5. Render the Gem mindmap as ASCII art
string mindmap = art.mindmap("docs/gem_mindmap.md")
sys.print(mindmap)
art.art_to_file(mindmap, "docs/gem_mindmap.txt")

# 6. Render README.md as ASCII art summary
string readme = art.readme("README.md")
sys.print(readme)

# 7. Render tutorial/README.md as ASCII art summary
string tut = art.tutorial("tutorial/README.md")
sys.print(tut)
