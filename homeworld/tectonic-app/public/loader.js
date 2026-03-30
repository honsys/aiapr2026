// Data loader — fetches from GPlates Web Service
class DataLoader {
  constructor() {
    this.cache = new Map();
    this.pending = new Map();
  }

  // GPlates API endpoints
  _coastlinesUrl(ma) {
    const t = Math.round(ma);
    return `https://gws.gplates.org/reconstruct/coastlines/?time=${t}&model=PALEOMAP&fmt=geojson`;
  }
  _platesUrl(ma) {
    const t = Math.round(ma);
    return `https://gws.gplates.org/reconstruct/plate_polygons/?time=${t}&model=PALEOMAP&fmt=geojson`;
  }
  _boundariesUrl(ma) {
    const t = Math.round(ma);
    return `https://gws.gplates.org/topology/plate_boundaries/?time=${t}&model=PALEOMAP&fmt=geojson`;
  }

  async fetchTime(ma) {
    const key = Math.round(ma);
    if (this.cache.has(key)) return this.cache.get(key);
    if (this.pending.has(key)) return this.pending.get(key);

    const promise = this._doFetch(key);
    this.pending.set(key, promise);
    const result = await promise;
    this.cache.set(key, result);
    this.pending.delete(key);
    return result;
  }

  async _doFetch(ma) {
    try {
      const [coastRes, boundRes] = await Promise.allSettled([
        fetch(this._coastlinesUrl(ma), { signal: AbortSignal.timeout(8000) }),
        fetch(this._boundariesUrl(ma), { signal: AbortSignal.timeout(8000) }),
      ]);

      let coastlines = null, boundaries = null;

      if (coastRes.status === 'fulfilled' && coastRes.value.ok) {
        coastlines = await coastRes.value.json();
      }
      if (boundRes.status === 'fulfilled' && boundRes.value.ok) {
        boundaries = await boundRes.value.json();
      }

      if (!coastlines && !boundaries) return null;

      // Merge into single GeoJSON
      const features = [
        ...(coastlines?.features || []),
        ...(boundaries?.features || []),
      ];
      return { type: 'FeatureCollection', features };
    } catch (e) {
      console.warn(`Failed to fetch data for ${ma} Ma:`, e.message);
      return null;
    }
  }

  // Prefetch nearby times
  prefetch(ma, times) {
    const nearby = times.filter(t => Math.abs(t - ma) < 100 && !this.cache.has(Math.round(t)));
    for (const t of nearby.slice(0, 3)) {
      this.fetchTime(t).catch(() => {});
    }
  }
}

// Interpolate between two GeoJSON frames (simple morph)
function interpolateGeoJSON(a, b, t) {
  if (!a || !b) return a || b;
  // For now return nearest frame — true morphing requires plate ID matching
  return t < 0.5 ? a : b;
}

// Get synthetic config for pre-750 Ma
function getSyntheticConfig(ma) {
  const keys = Object.keys(SYNTHETIC_CONFIGS).map(Number).sort((a,b) => b-a);
  for (let i = 0; i < keys.length - 1; i++) {
    if (ma <= keys[i] && ma > keys[i+1]) {
      // Interpolate between two synthetic configs
      const t = (keys[i] - ma) / (keys[i] - keys[i+1]);
      // Return the closer one
      return t < 0.5 ? SYNTHETIC_CONFIGS[keys[i]] : SYNTHETIC_CONFIGS[keys[i+1]];
    }
  }
  return SYNTHETIC_CONFIGS[keys[keys.length-1]];
}
