// Geological time scale and key events
const GEO_EVENTS = [
  { ma: 4500, name: "Hadean: Earth forms", type: "major" },
  { ma: 4400, name: "First liquid water", type: "major" },
  { ma: 4100, name: "Late Heavy Bombardment begins", type: "impact" },
  { ma: 3800, name: "First evidence of life (stromatolites)", type: "life" },
  { ma: 3500, name: "Ur — first stable craton", type: "tectonic" },
  { ma: 3100, name: "Kenorland supercontinent assembles", type: "tectonic" },
  { ma: 2700, name: "Great Oxygenation Event begins", type: "life" },
  { ma: 2500, name: "Archean ends — Proterozoic begins", type: "major" },
  { ma: 2100, name: "Columbia/Nuna supercontinent assembles", type: "tectonic" },
  { ma: 1800, name: "Columbia fully assembled", type: "tectonic" },
  { ma: 1500, name: "Columbia breaks apart", type: "tectonic" },
  { ma: 1100, name: "Rodinia supercontinent assembles", type: "tectonic" },
  { ma: 900,  name: "Rodinia fully assembled", type: "tectonic" },
  { ma: 750,  name: "Rodinia begins to break apart", type: "tectonic" },
  { ma: 720,  name: "Snowball Earth — Sturtian glaciation", type: "climate" },
  { ma: 635,  name: "Snowball Earth — Marinoan glaciation ends", type: "climate" },
  { ma: 600,  name: "Pannotia supercontinent (brief)", type: "tectonic" },
  { ma: 541,  name: "Cambrian Explosion — Phanerozoic begins", type: "life" },
  { ma: 490,  name: "Gondwana dominant southern landmass", type: "tectonic" },
  { ma: 450,  name: "Ordovician mass extinction (glaciation)", type: "life" },
  { ma: 420,  name: "Laurussia begins forming", type: "tectonic" },
  { ma: 375,  name: "Late Devonian mass extinction", type: "life" },
  { ma: 335,  name: "Pangaea assembles", type: "tectonic" },
  { ma: 300,  name: "Pangaea fully assembled", type: "tectonic" },
  { ma: 252,  name: "Permian–Triassic extinction (96% species lost)", type: "life" },
  { ma: 230,  name: "Pangaea begins rifting", type: "tectonic" },
  { ma: 200,  name: "Triassic–Jurassic extinction", type: "life" },
  { ma: 180,  name: "Gondwana separates from Laurasia", type: "tectonic" },
  { ma: 150,  name: "Atlantic Ocean opens", type: "tectonic" },
  { ma: 130,  name: "India separates from Antarctica", type: "tectonic" },
  { ma: 100,  name: "South America separates from Africa", type: "tectonic" },
  { ma: 66,   name: "K-Pg extinction — dinosaurs extinct", type: "life" },
  { ma: 55,   name: "India collides with Asia — Himalayas begin", type: "tectonic" },
  { ma: 35,   name: "Antarctica isolates — glaciation begins", type: "climate" },
  { ma: 20,   name: "Arabian plate collides with Eurasia", type: "tectonic" },
  { ma: 15,   name: "East African Rift begins", type: "tectonic" },
  { ma: 5,    name: "Panama land bridge closes", type: "tectonic" },
  { ma: 2.6,  name: "Pleistocene ice ages begin", type: "climate" },
  { ma: 0.3,  name: "Homo sapiens evolve", type: "life" },
  { ma: 0,    name: "Present day", type: "major" },
];

const EONS = [
  { start: 4500, end: 4000, name: "Hadean", color: "#8B0000" },
  { start: 4000, end: 2500, name: "Archean", color: "#8B4513" },
  { start: 2500, end: 541,  name: "Proterozoic", color: "#DAA520" },
  { start: 541,  end: 252,  name: "Paleozoic", color: "#228B22" },
  { start: 252,  end: 66,   name: "Mesozoic", color: "#20B2AA" },
  { start: 66,   end: 0,    name: "Cenozoic", color: "#4169E1" },
];

const PERIODS = [
  { start: 541, end: 485, name: "Cambrian" },
  { start: 485, end: 444, name: "Ordovician" },
  { start: 444, end: 419, name: "Silurian" },
  { start: 419, end: 359, name: "Devonian" },
  { start: 359, end: 299, name: "Carboniferous" },
  { start: 299, end: 252, name: "Permian" },
  { start: 252, end: 201, name: "Triassic" },
  { start: 201, end: 145, name: "Jurassic" },
  { start: 145, end: 66,  name: "Cretaceous" },
  { start: 66,  end: 23,  name: "Paleogene" },
  { start: 23,  end: 2.6, name: "Neogene" },
  { start: 2.6, end: 0,   name: "Quaternary" },
];

function getEon(ma) {
  for (const e of EONS) if (ma <= e.start && ma >= e.end) return e;
  return EONS[0];
}
function getPeriod(ma) {
  for (const p of PERIODS) if (ma <= p.start && ma >= p.end) return p;
  return null;
}
function getNearestEvent(ma, threshold = 15) {
  let best = null, bestDist = Infinity;
  for (const e of GEO_EVENTS) {
    const d = Math.abs(e.ma - ma);
    if (d < bestDist && d < threshold) { bestDist = d; best = e; }
  }
  return best;
}

// GPlates Web Service — real paleogeographic reconstructions
// Uses the PALEOMAP model (Scotese) via the public GPlates API
const GPLATES_BASE = "https://gws.gplates.org";

// Time steps we'll fetch (Ma) — covers full Earth history
// Pre-Rodinia we use synthetic data since GPlates coverage starts ~1100 Ma
const FETCH_TIMES = [
  0, 5, 10, 15, 20, 25, 30, 35, 40, 50, 60, 66, 70, 80, 90, 100,
  110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 210, 220, 230,
  240, 250, 260, 270, 280, 290, 300, 310, 320, 330, 340, 350, 360,
  370, 380, 390, 400, 410, 420, 430, 440, 450, 460, 470, 480, 490,
  500, 510, 520, 530, 540, 550, 560, 570, 580, 590, 600, 620, 640,
  660, 680, 700, 720, 740, 750
];

// Synthetic plate configurations for pre-GPlates era (>750 Ma)
// Based on published paleogeographic reconstructions
const SYNTHETIC_CONFIGS = {
  900:  { name: "Rodinia", plates: buildRodinia() },
  1100: { name: "Rodinia assembling", plates: buildRodiniaAssembling() },
  1500: { name: "Columbia breaking", plates: buildColumbiaBreaking() },
  1800: { name: "Columbia", plates: buildColumbia() },
  2500: { name: "Kenorland", plates: buildKenorland() },
  3500: { name: "Ur craton", plates: buildUr() },
  4000: { name: "Early Archean", plates: buildEarlyArchean() },
  4500: { name: "Magma ocean", plates: buildMagmaOcean() },
};

function buildRodinia() {
  // Rodinia: single supercontinent centered ~10°S, 0°E
  return [{
    id: 0, name: "Rodinia", color: "#5a7a3a",
    outline: makeEllipse(0, -10, 80, 55, 20),
    boundaries: [
      { type: "divergent", points: [[-80,-10],[-60,20],[-40,30],[-20,10],[0,-5],[20,-15],[40,-10],[60,5],[80,-10]] }
    ]
  }];
}
function buildRodiniaAssembling() {
  return [
    { id: 0, name: "Proto-Rodinia A", color: "#5a7a3a", outline: makeEllipse(-30, 5, 50, 35, 15), boundaries: [] },
    { id: 1, name: "Proto-Rodinia B", color: "#4a6a2a", outline: makeEllipse(40, -15, 45, 30, 12), boundaries: [] },
  ];
}
function buildColumbiaBreaking() {
  return [
    { id: 0, name: "Laurentia", color: "#5a7a3a", outline: makeEllipse(-40, 20, 35, 25, 10), boundaries: [] },
    { id: 1, name: "Baltica", color: "#4a8a4a", outline: makeEllipse(10, 30, 25, 20, 8), boundaries: [] },
    { id: 2, name: "Siberia", color: "#6a7a2a", outline: makeEllipse(60, 15, 30, 22, 10), boundaries: [] },
    { id: 3, name: "S. Gondwana", color: "#3a6a5a", outline: makeEllipse(20, -30, 55, 35, 15), boundaries: [] },
  ];
}
function buildColumbia() {
  return [{
    id: 0, name: "Columbia/Nuna", color: "#5a7a3a",
    outline: makeEllipse(10, 10, 75, 50, 18),
    boundaries: [
      { type: "convergent", points: [[-70,10],[-40,30],[0,20],[40,10],[70,5]] }
    ]
  }];
}
function buildKenorland() {
  return [{
    id: 0, name: "Kenorland", color: "#6a6a3a",
    outline: makeEllipse(-10, 15, 60, 40, 16),
    boundaries: []
  }];
}
function buildUr() {
  return [
    { id: 0, name: "Ur craton", color: "#7a6a2a", outline: makeEllipse(30, -20, 30, 20, 8), boundaries: [] },
    { id: 1, name: "Vaalbara", color: "#6a5a1a", outline: makeEllipse(-20, -10, 25, 18, 7), boundaries: [] },
  ];
}
function buildEarlyArchean() {
  return [
    { id: 0, name: "Proto-craton A", color: "#8a5a1a", outline: makeEllipse(-30, 10, 20, 15, 6), boundaries: [] },
    { id: 1, name: "Proto-craton B", color: "#7a4a0a", outline: makeEllipse(20, -5, 18, 12, 5), boundaries: [] },
    { id: 2, name: "Proto-craton C", color: "#9a6a2a", outline: makeEllipse(60, 20, 15, 10, 5), boundaries: [] },
  ];
}
function buildMagmaOcean() {
  return []; // No solid plates — magma ocean
}

function makeEllipse(cx, cy, rx, ry, steps) {
  const pts = [];
  for (let i = 0; i <= steps; i++) {
    const a = (i / steps) * Math.PI * 2;
    pts.push([cx + rx * Math.cos(a), cy + ry * Math.sin(a)]);
  }
  return pts;
}
