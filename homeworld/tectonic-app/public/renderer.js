// Projection utilities
const Projections = {
  equirect: {
    project(lon, lat, W, H) {
      return [(lon + 180) / 360 * W, (90 - lat) / 180 * H];
    },
    unproject(x, y, W, H) {
      return [x / W * 360 - 180, 90 - y / H * 180];
    }
  },
  robinson: {
    // Robinson projection lookup table
    _X: [1,0.9986,0.9954,0.9900,0.9822,0.9730,0.9600,0.9427,0.9216,0.8962,0.8679,0.8350,0.7986,0.7597,0.7186,0.6732,0.6213,0.5722,0.5322],
    _Y: [0,0.0620,0.1240,0.1860,0.2480,0.3100,0.3720,0.4340,0.4958,0.5571,0.6176,0.6769,0.7346,0.7903,0.8435,0.8936,0.9394,0.9761,1.0000],
    _lats: [0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90],
    _interp(lat) {
      const absLat = Math.abs(lat);
      const idx = Math.floor(absLat / 5);
      const t = (absLat % 5) / 5;
      const i = Math.min(idx, 17);
      const x = this._X[i] + (this._X[i+1] - this._X[i]) * t;
      const y = this._Y[i] + (this._Y[i+1] - this._Y[i]) * t;
      return { x, y: lat < 0 ? -y : y };
    },
    project(lon, lat, W, H) {
      const { x, y } = this._interp(lat);
      const px = (lon / 180) * x * 0.5 + 0.5;
      const py = 0.5 - y * 0.5;
      return [px * W, py * H];
    }
  },
  ortho: {
    _lon0: 0, _lat0: 20,
    project(lon, lat, W, H) {
      const l0 = this._lon0 * Math.PI / 180;
      const p0 = this._lat0 * Math.PI / 180;
      const l = lon * Math.PI / 180;
      const p = lat * Math.PI / 180;
      const cosc = Math.sin(p0)*Math.sin(p) + Math.cos(p0)*Math.cos(p)*Math.cos(l-l0);
      if (cosc < 0) return null; // behind globe
      const R = Math.min(W, H) * 0.45;
      const cx = W / 2, cy = H / 2;
      const x = R * Math.cos(p) * Math.sin(l - l0);
      const y = R * (Math.cos(p0)*Math.sin(p) - Math.sin(p0)*Math.cos(p)*Math.cos(l-l0));
      return [cx + x, cy - y];
    }
  }
};

// Canvas renderer
class TectonicRenderer {
  constructor(canvas) {
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');
    this.projection = 'equirect';
    this.resize();
  }

  resize() {
    const dpr = window.devicePixelRatio || 1;
    const rect = this.canvas.parentElement.getBoundingClientRect();
    this.canvas.width = rect.width * dpr;
    this.canvas.height = rect.height * dpr;
    this.canvas.style.width = rect.width + 'px';
    this.canvas.style.height = rect.height + 'px';
    this.ctx.scale(dpr, dpr);
    this.W = rect.width;
    this.H = rect.height;
  }

  project(lon, lat) {
    const proj = Projections[this.projection];
    return proj.project(lon, lat, this.W, this.H);
  }

  drawBackground(ma) {
    const ctx = this.ctx;
    // Ocean color shifts over time (early Earth was more reducing/different)
    let oceanColor;
    if (ma > 3800) oceanColor = '#0a0a0a'; // magma/no ocean
    else if (ma > 2500) oceanColor = '#0d1a2a'; // Archean ocean
    else if (ma > 541) oceanColor = '#0f2035';
    else oceanColor = '#0d2040';

    ctx.fillStyle = oceanColor;
    ctx.fillRect(0, 0, this.W, this.H);

    // Grid lines
    ctx.strokeStyle = 'rgba(255,255,255,0.04)';
    ctx.lineWidth = 0.5;
    for (let lon = -180; lon <= 180; lon += 30) {
      ctx.beginPath();
      for (let lat = -90; lat <= 90; lat += 2) {
        const pt = this.project(lon, lat);
        if (!pt) continue;
        lat === -90 ? ctx.moveTo(pt[0], pt[1]) : ctx.lineTo(pt[0], pt[1]);
      }
      ctx.stroke();
    }
    for (let lat = -90; lat <= 90; lat += 30) {
      ctx.beginPath();
      for (let lon = -180; lon <= 180; lon += 2) {
        const pt = this.project(lon, lat);
        if (!pt) continue;
        lon === -180 ? ctx.moveTo(pt[0], pt[1]) : ctx.lineTo(pt[0], pt[1]);
      }
      ctx.stroke();
    }

    // Equator
    ctx.strokeStyle = 'rgba(255,255,100,0.12)';
    ctx.lineWidth = 1;
    ctx.beginPath();
    for (let lon = -180; lon <= 180; lon += 2) {
      const pt = this.project(lon, 0);
      if (!pt) continue;
      lon === -180 ? ctx.moveTo(pt[0], pt[1]) : ctx.lineTo(pt[0], pt[1]);
    }
    ctx.stroke();
  }

  drawGeoJSON(geojson, ma) {
    if (!geojson) return;
    const ctx = this.ctx;
    const features = geojson.features || [];

    for (const f of features) {
      const geom = f.geometry;
      if (!geom) continue;
      const type = f.properties?.type || f.properties?.PLATEID1 || '';
      const isOcean = f.properties?.FEATURE_CLASS === 'COB' || false;

      ctx.fillStyle = this._landColor(ma, f.properties);
      ctx.strokeStyle = 'rgba(200,200,150,0.3)';
      ctx.lineWidth = 0.5;

      if (geom.type === 'Polygon') {
        this._drawPolygon(geom.coordinates, true);
      } else if (geom.type === 'MultiPolygon') {
        for (const poly of geom.coordinates) this._drawPolygon(poly, true);
      } else if (geom.type === 'LineString') {
        this._drawLine(geom.coordinates, f.properties);
      } else if (geom.type === 'MultiLineString') {
        for (const line of geom.coordinates) this._drawLine(line, f.properties);
      }
    }
  }

  _landColor(ma, props) {
    // Color based on age and feature type
    if (ma > 4000) return 'rgba(180,60,20,0.7)'; // magma
    if (ma > 3000) return 'rgba(120,80,30,0.8)'; // early crust
    const base = [58, 100, 58];
    // Slight variation by plate
    const id = (props?.PLATEID1 || 0) % 7;
    const offsets = [[0,0,0],[5,-5,10],[-5,10,-5],[10,5,-10],[-10,-5,5],[8,-8,3],[-3,8,-8]];
    const o = offsets[id];
    return `rgba(${base[0]+o[0]},${base[1]+o[1]},${base[2]+o[2]},0.85)`;
  }

  _drawPolygon(rings, fill) {
    const ctx = this.ctx;
    ctx.beginPath();
    for (const ring of rings) {
      let first = true;
      for (const [lon, lat] of ring) {
        const pt = this.project(lon, lat);
        if (!pt) continue;
        first ? ctx.moveTo(pt[0], pt[1]) : ctx.lineTo(pt[0], pt[1]);
        first = false;
      }
      ctx.closePath();
    }
    if (fill) ctx.fill();
    ctx.stroke();
  }

  _drawLine(coords, props) {
    const ctx = this.ctx;
    const type = props?.type || props?.FEATURE_CLASS || '';
    if (type.includes('ridge') || type.includes('diverge') || type.includes('MOR')) {
      ctx.strokeStyle = '#ff4444'; ctx.lineWidth = 1.5;
    } else if (type.includes('trench') || type.includes('subduct') || type.includes('converge')) {
      ctx.strokeStyle = '#ffaa00'; ctx.lineWidth = 1.5;
    } else if (type.includes('transform') || type.includes('fault')) {
      ctx.strokeStyle = '#44ffaa'; ctx.lineWidth = 1;
    } else {
      ctx.strokeStyle = 'rgba(255,200,100,0.4)'; ctx.lineWidth = 0.8;
    }
    ctx.beginPath();
    let first = true;
    for (const [lon, lat] of coords) {
      const pt = this.project(lon, lat);
      if (!pt) continue;
      first ? ctx.moveTo(pt[0], pt[1]) : ctx.lineTo(pt[0], pt[1]);
      first = false;
    }
    ctx.stroke();
  }

  drawSyntheticPlates(config, ma) {
    if (!config || !config.plates) return;
    const ctx = this.ctx;

    if (ma > 4000) {
      // Magma ocean — draw glowing effect
      const grad = ctx.createRadialGradient(this.W/2, this.H/2, 0, this.W/2, this.H/2, Math.min(this.W,this.H)*0.4);
      grad.addColorStop(0, 'rgba(255,100,0,0.6)');
      grad.addColorStop(0.5, 'rgba(200,50,0,0.4)');
      grad.addColorStop(1, 'rgba(100,20,0,0.1)');
      ctx.fillStyle = grad;
      ctx.fillRect(0, 0, this.W, this.H);
      return;
    }

    for (const plate of config.plates) {
      ctx.fillStyle = plate.color + 'cc';
      ctx.strokeStyle = 'rgba(200,200,150,0.4)';
      ctx.lineWidth = 1;
      ctx.beginPath();
      let first = true;
      for (const [lon, lat] of plate.outline) {
        const pt = this.project(lon, lat);
        if (!pt) continue;
        first ? ctx.moveTo(pt[0], pt[1]) : ctx.lineTo(pt[0], pt[1]);
        first = false;
      }
      ctx.closePath();
      ctx.fill();
      ctx.stroke();

      // Plate label
      if (plate.outline.length > 0) {
        const mid = plate.outline[Math.floor(plate.outline.length / 2)];
        const pt = this.project(mid[0], mid[1]);
        if (pt) {
          ctx.fillStyle = 'rgba(255,255,200,0.7)';
          ctx.font = '10px Courier New';
          ctx.textAlign = 'center';
          ctx.fillText(plate.name, pt[0], pt[1]);
        }
      }
    }
  }

  drawOrthoGlobe() {
    if (this.projection !== 'ortho') return;
    const ctx = this.ctx;
    const R = Math.min(this.W, this.H) * 0.45;
    ctx.strokeStyle = 'rgba(100,150,255,0.3)';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.arc(this.W/2, this.H/2, R, 0, Math.PI*2);
    ctx.stroke();
  }

  drawIceAge(ma) {
    // Snowball Earth events
    const iceEvents = [{start:720,end:635},{start:2400,end:2100}];
    for (const ev of iceEvents) {
      if (ma <= ev.start && ma >= ev.end) {
        const ctx = this.ctx;
        ctx.fillStyle = 'rgba(200,230,255,0.15)';
        ctx.fillRect(0, 0, this.W, this.H);
        // Ice caps
        for (const lat of [75, -75]) {
          ctx.fillStyle = 'rgba(220,240,255,0.4)';
          ctx.beginPath();
          let first = true;
          for (let lon = -180; lon <= 180; lon += 5) {
            const pt = this.project(lon, lat);
            if (!pt) continue;
            first ? ctx.moveTo(pt[0], pt[1]) : ctx.lineTo(pt[0], pt[1]);
            first = false;
          }
          const pole = this.project(0, lat > 0 ? 90 : -90);
          if (pole) ctx.lineTo(pole[0], pole[1]);
          ctx.closePath();
          ctx.fill();
        }
      }
    }
  }

  drawHUD(ma, eon, period, event) {
    const ctx = this.ctx;
    // Eon color bar at top
    ctx.fillStyle = eon.color + '44';
    ctx.fillRect(0, 0, this.W, 3);

    // Scale bar
    ctx.fillStyle = 'rgba(0,0,0,0.5)';
    ctx.fillRect(10, this.H - 30, 200, 20);
    ctx.fillStyle = '#aaa';
    ctx.font = '10px Courier New';
    ctx.textAlign = 'left';
    ctx.fillText('← 10,000 km →', 15, this.H - 16);
  }

  render(state) {
    const { ma, geojson, syntheticConfig, eon, period, event } = state;
    this.drawBackground(ma);
    this.drawOrthoGlobe();
    this.drawIceAge(ma);

    if (geojson) {
      this.drawGeoJSON(geojson, ma);
    } else if (syntheticConfig) {
      this.drawSyntheticPlates(syntheticConfig, ma);
    }

    this.drawHUD(ma, eon, period, event);
  }
}
