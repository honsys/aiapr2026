#!/usr/bin/env python3
"""
Rescale gem_mindmap.svg:
  - All geometry scaled 2x from center (1600,1600)  → nodes 100% larger, spacing 2x
  - All font-size values tripled (3x)
  - Canvas expanded to fit; viewBox and pan-zoom script updated
"""
import re, sys

INPUT  = "docs/gem_mindmap.svg"
OUTPUT = "docs/gem_mindmap.svg"

SCALE  = 2.0
CX, CY = 1600.0, 1600.0   # original center of the mindmap

def sc(v):   # scale a coordinate
    return CX + SCALE * (v - CX)

def sn(v):   # scale a plain number (radius, width, stroke-width, etc.)
    return v * SCALE

# ── path d= ──────────────────────────────────────────────────────────────────
def scale_path_d(d):
    """All numbers in path d are absolute coords — scale each from center."""
    return re.sub(r'-?\d+(?:\.\d+)?', lambda m: f"{sc(float(m.group())):.3f}", d)

# ── attribute dispatch ────────────────────────────────────────────────────────
# Maps attr-name → transform function
COORD_ATTRS  = {"cx", "cy", "x", "y"}   # absolute coords
PLAIN_ATTRS  = {"r", "width", "height", "rx", "ry", "stroke-width"}  # plain scale
TRIPLE_ATTRS = {"font-size"}             # triple
REL_ATTRS    = {"dy"}                    # relative offset → plain scale

def transform_attr(attr, val):
    v = float(val)
    if attr in COORD_ATTRS:
        coord = CX if attr in ("cx", "x") else CY
        return f"{coord + SCALE * (v - coord):.3f}"
    if attr in PLAIN_ATTRS:
        return f"{v * SCALE:.3f}"
    if attr in TRIPLE_ATTRS:
        return f"{v * 3.0:.3f}"
    if attr in REL_ATTRS:
        return f"{v * SCALE:.3f}"
    return val

ALL_ATTRS = COORD_ATTRS | PLAIN_ATTRS | TRIPLE_ATTRS | REL_ATTRS
ATTR_PAT  = re.compile(
    r'\b(' + '|'.join(re.escape(a) for a in ALL_ATTRS) + r')=(["\'])(-?[\d.]+)\2'
)

def repl_attr(m):
    return f'{m.group(1)}={m.group(2)}{transform_attr(m.group(1), m.group(3))}{m.group(2)}'

# ── main ──────────────────────────────────────────────────────────────────────
with open(INPUT, "r", encoding="utf-8") as f:
    svg = f.read()

# 1. Scale path d= (before attribute pass so numbers aren't double-touched)
svg = re.sub(r'd="([^"]+)"', lambda m: f'd="{scale_path_d(m.group(1))}"', svg)

# 2. Scale / triple all relevant attributes in one pass
svg = ATTR_PAT.sub(repl_attr, svg)

# 3. Compute new canvas / viewBox
# Original content spans roughly x/y ∈ [-136, 3200] after padding.
# After 2x scale from 1600: new_min = 1600 + 2*(-136-1600) = 1600-3472 = -1872
# new_max = 1600 + 2*(3200-1600) = 1600+3200 = 4800
# Add 400px padding each side.
PAD    = 500
new_min = int(sc(-200)) - PAD   # generous lower bound
new_max = int(sc(3400)) + PAD   # generous upper bound
VB_W   = new_max - new_min
VB_H   = VB_W
CANVAS = VB_W   # square canvas

# 4. Update SVG root
svg = re.sub(r'viewBox="[^"]*"',  f'viewBox="{new_min} {new_min} {VB_W} {VB_H}"', svg)
svg = re.sub(r'\bwidth="\d+"',    f'width="{CANVAS}"',  svg)
svg = re.sub(r'\bheight="\d+"',   f'height="{CANVAS}"', svg)

# 5. Update pan-zoom JS
svg = re.sub(r'var vb = \[[\d,\s-]+\];',
             f'var vb = [{new_min},{new_min},{VB_W},{VB_H}];', svg)
svg = re.sub(r'var dx=\(e\.clientX-sx\)\*vb\[2\]/\d+;',
             f'var dx=(e.clientX-sx)*vb[2]/{CANVAS};', svg)
svg = re.sub(r'var dy=\(e\.clientY-sy\)\*vb\[3\]/\d+;',
             f'var dy=(e.clientY-sy)*vb[3]/{CANVAS};', svg)
svg = re.sub(r'var mx=e\.offsetX/\d+\*vb\[2\]\+vb\[0\];',
             f'var mx=e.offsetX/{CANVAS}*vb[2]+vb[0];', svg)
svg = re.sub(r'var my=e\.offsetY/\d+\*vb\[3\]\+vb\[1\];',
             f'var my=e.offsetY/{CANVAS}*vb[3]+vb[1];', svg)

with open(OUTPUT, "w", encoding="utf-8") as f:
    f.write(svg)

print(f"Done. Canvas: {CANVAS}x{CANVAS}  viewBox: {new_min} {new_min} {VB_W} {VB_H}")
