"""
FoFworld - Prehistoric Earth-like world generator
60% water / 40% land, large jungles, rivers, lakes, mountains, small deserts/grasslands
Uses simplex noise (same as worldengine's generation.py) without platec dependency.
"""
import numpy as np
from noise import snoise2
import json

WIDTH = 512
HEIGHT = 256

BIOME_COLORS = {
    "deep_ocean":    [0,   20,  80],
    "ocean":         [0,   60,  130],
    "shallow_ocean": [30,  100, 170],
    "lake":          [60,  140, 200],
    "river":         [80,  160, 220],
    "beach":         [210, 200, 140],
    "jungle":        [10,  90,  20],
    "rainforest":    [20,  110, 30],
    "forest":        [40,  130, 50],
    "grassland":     [130, 180, 60],
    "savanna":       [190, 170, 80],
    "desert":        [220, 200, 120],
    "mountain":      [140, 120, 100],
    "snow_mountain": [230, 230, 240],
    "tundra":        [160, 170, 150],
    "ice":           [220, 235, 245],
}


def fbm(x, y, octaves=8, persistence=0.5, lacunarity=2.0, scale=1.0, seed=0):
    """Fractional Brownian Motion using simplex noise."""
    val = 0.0
    amp = 1.0
    freq = scale
    max_val = 0.0
    for _ in range(octaves):
        val += snoise2(x * freq + seed * 0.1, y * freq + seed * 0.1, 1) * amp
        max_val += amp
        amp *= persistence
        freq *= lacunarity
    return val / max_val


def generate_world(seed=42):
    rng = np.random.RandomState(seed)
    s = int(rng.randint(0, 9999))

    # --- Elevation ---
    elev = np.zeros((HEIGHT, WIDTH))
    for y in range(HEIGHT):
        for x in range(WIDTH):
            nx = x / WIDTH
            ny = y / HEIGHT
            # Base continent shape
            e = fbm(nx, ny, octaves=8, persistence=0.55, lacunarity=2.1, scale=3.0, seed=s)
            # Add mountain ridges
            ridge = abs(fbm(nx, ny, octaves=6, persistence=0.4, lacunarity=2.5, scale=5.0, seed=s + 100))
            e += ridge * 0.6
            elev[y, x] = e

    # Normalize to [0,1]
    elev = (elev - elev.min()) / (elev.max() - elev.min())

    # Fade borders to ocean (ensures wraparound ocean edges)
    border = 20
    for i in range(border):
        f = i / border
        elev[i, :] *= f
        elev[HEIGHT - 1 - i, :] *= f
        elev[:, i] *= f
        elev[:, WIDTH - 1 - i] *= f

    # Re-normalize
    elev = (elev - elev.min()) / (elev.max() - elev.min())

    # Set sea level so ~60% is ocean
    sea_level = float(np.percentile(elev, 60))

    # --- Temperature (latitude-based + altitude) ---
    temp = np.zeros((HEIGHT, WIDTH))
    for y in range(HEIGHT):
        lat_factor = 1.0 - abs((y / HEIGHT) - 0.5) * 2.0  # 1 at equator, 0 at poles
        for x in range(WIDTH):
            alt_penalty = max(0, (elev[y, x] - sea_level) / (1.0 - sea_level)) * 0.5
            noise_t = fbm(x / WIDTH, y / HEIGHT, octaves=4, scale=2.0, seed=s + 200) * 0.1
            temp[y, x] = lat_factor - alt_penalty + noise_t
    temp = np.clip(temp, 0, 1)

    # --- Humidity (high near equator/coasts, low inland) ---
    humid = np.zeros((HEIGHT, WIDTH))
    for y in range(HEIGHT):
        for x in range(WIDTH):
            lat_h = 1.0 - abs((y / HEIGHT) - 0.5) * 1.5
            noise_h = fbm(x / WIDTH, y / HEIGHT, octaves=5, scale=4.0, seed=s + 300) * 0.4 + 0.3
            humid[y, x] = np.clip(lat_h * 0.6 + noise_h, 0, 1)

    # Boost humidity near water
    is_water = elev <= sea_level
    from scipy.ndimage import distance_transform_edt
    dist_to_water = distance_transform_edt(~is_water)
    dist_norm = np.clip(dist_to_water / 40.0, 0, 1)
    humid = np.clip(humid - dist_norm * 0.3, 0, 1)

    # --- Rivers: trace downhill from high points ---
    rivers = np.zeros((HEIGHT, WIDTH), dtype=bool)
    mountain_mask = elev > (sea_level + (1 - sea_level) * 0.6)
    mountain_pts = list(zip(*np.where(mountain_mask)))
    rng.shuffle(mountain_pts)
    num_rivers = 80
    for ry, rx in mountain_pts[:num_rivers]:
        cy, cx = ry, rx
        for _ in range(300):
            if elev[cy, cx] <= sea_level:
                break
            rivers[cy, cx] = True
            # Move to lowest neighbor
            best = None
            best_e = elev[cy, cx]
            for dy in [-1, 0, 1]:
                for dx in [-1, 0, 1]:
                    ny2, nx2 = cy + dy, cx + dx
                    if 0 <= ny2 < HEIGHT and 0 <= nx2 < WIDTH:
                        if elev[ny2, nx2] < best_e:
                            best_e = elev[ny2, nx2]
                            best = (ny2, nx2)
            if best is None:
                break
            cy, cx = best

    # --- Lakes: low inland basins above sea level ---
    lakes = np.zeros((HEIGHT, WIDTH), dtype=bool)
    lake_level = sea_level + (1 - sea_level) * 0.05
    inland_low = (elev > sea_level) & (elev < lake_level)
    # Only keep inland basins (not touching map border)
    from scipy.ndimage import label
    labeled, num = label(inland_low)
    for i in range(1, num + 1):
        region = labeled == i
        if region.sum() > 50:
            lakes |= region

    # --- Biome assignment ---
    biome = np.full((HEIGHT, WIDTH), "", dtype=object)
    for y in range(HEIGHT):
        for x in range(WIDTH):
            e = elev[y, x]
            t = temp[y, x]
            h = humid[y, x]

            if lakes[y, x]:
                biome[y, x] = "lake"
            elif rivers[y, x] and e > sea_level:
                biome[y, x] = "river"
            elif e <= sea_level * 0.5:
                biome[y, x] = "deep_ocean"
            elif e <= sea_level * 0.85:
                biome[y, x] = "ocean"
            elif e <= sea_level:
                biome[y, x] = "shallow_ocean"
            elif e <= sea_level + (1 - sea_level) * 0.03:
                biome[y, x] = "beach"
            else:
                land_frac = (e - sea_level) / (1 - sea_level)
                if land_frac > 0.65:
                    biome[y, x] = "snow_mountain" if t < 0.3 else "mountain"
                elif t < 0.2:
                    biome[y, x] = "ice" if land_frac < 0.3 else "tundra"
                elif t > 0.75 and h > 0.65:
                    biome[y, x] = "jungle" if h > 0.8 else "rainforest"
                elif t > 0.65 and h < 0.25:
                    biome[y, x] = "desert"
                elif t > 0.5 and h < 0.4:
                    biome[y, x] = "savanna"
                elif h > 0.55:
                    biome[y, x] = "forest"
                else:
                    biome[y, x] = "grassland"

    # Build pixel color array
    img = np.zeros((HEIGHT, WIDTH, 3), dtype=np.uint8)
    for name, color in BIOME_COLORS.items():
        mask = biome == name
        img[mask] = color

    # Encode as flat list for JSON transfer
    return {
        "width": WIDTH,
        "height": HEIGHT,
        "pixels": img.reshape(-1, 3).tolist(),
        "sea_level": sea_level,
        "seed": seed,
        "biome_colors": BIOME_COLORS,
    }


if __name__ == "__main__":
    import time
    t = time.time()
    data = generate_world(42)
    print(f"Generated in {time.time()-t:.1f}s, sea_level={data['sea_level']:.3f}")
