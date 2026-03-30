// Main app controller
const MAX_MA = 4500;
const MIN_MA = 0;
const GPLATES_MAX_MA = 750; // GPlates PALEOMAP coverage limit

let currentMa = 750;
let playing = true;
let direction = -1; // -1 = forward in time (decreasing Ma), +1 = reverse
let speedMultiplier = 5;
let animFrame = null;
let lastTimestamp = null;
let currentGeojson = null;
let pendingFetch = null;

const loader = new DataLoader();
let renderer = null;

// UI elements
const canvas = document.getElementById('canvas');
const timeDisplay = document.getElementById('time-display');
const eonDisplay = document.getElementById('eon-display');
const eventDisplay = document.getElementById('event-display');
const timelineFill = document.getElementById('timeline-fill');
const timelineThumb = document.getElementById('timeline-thumb');
const loadingEl = document.getElementById('loading');
const loadingFill = document.getElementById('loading-fill');
const loadingStatus = document.getElementById('loading-status');

function formatMa(ma) {
  if (ma >= 1000) return (ma / 1000).toFixed(2) + ' Ga';
  if (ma < 1) return (ma * 1000).toFixed(0) + ' ka';
  return ma.toFixed(1) + ' Ma';
}

function updateUI(ma) {
  timeDisplay.textContent = formatMa(ma) + ' ago';
  const eon = getEon(ma);
  const period = getPeriod(ma);
  eonDisplay.textContent = eon.name + (period ? ' · ' + period.name : '');
  eonDisplay.style.color = eon.color;

  const event = getNearestEvent(ma, ma > 100 ? 20 : 5);
  eventDisplay.textContent = event ? '⚡ ' + event.name : '';

  const pct = (MAX_MA - ma) / MAX_MA * 100;
  timelineFill.style.width = pct + '%';
  timelineThumb.style.left = pct + '%';
}

async function fetchAndRender(ma) {
  const roundedMa = Math.round(ma / 5) * 5; // snap to 5 Ma grid

  if (roundedMa <= GPLATES_MAX_MA) {
    // Use real GPlates data
    const snapTimes = FETCH_TIMES.filter(t => t <= GPLATES_MAX_MA);
    const nearest = snapTimes.reduce((a, b) => Math.abs(b - roundedMa) < Math.abs(a - roundedMa) ? b : a);

    if (!loader.cache.has(nearest)) {
      pendingFetch = loader.fetchTime(nearest);
      const data = await pendingFetch;
      if (data) currentGeojson = data;
    } else {
      currentGeojson = loader.cache.get(nearest);
    }
    loader.prefetch(roundedMa, FETCH_TIMES);
  } else {
    currentGeojson = null;
  }
}

function render(ma) {
  const eon = getEon(ma);
  const period = getPeriod(ma);
  const event = getNearestEvent(ma, 15);
  const syntheticConfig = ma > GPLATES_MAX_MA ? getSyntheticConfig(ma) : null;

  renderer.render({
    ma,
    geojson: currentGeojson,
    syntheticConfig,
    eon, period, event
  });
}

let fetchScheduled = false;
function scheduleFetch(ma) {
  if (fetchScheduled) return;
  fetchScheduled = true;
  setTimeout(async () => {
    await fetchAndRender(ma);
    fetchScheduled = false;
  }, 50);
}

function animate(timestamp) {
  if (!lastTimestamp) lastTimestamp = timestamp;
  const dt = Math.min(timestamp - lastTimestamp, 100); // cap at 100ms
  lastTimestamp = timestamp;

  if (playing) {
    // Ma per second: at speed 1 = 50 Ma/s, speed 20 = 1000 Ma/s
    const maPerSec = speedMultiplier * 50;
    currentMa += direction * maPerSec * (dt / 1000);
    currentMa = Math.max(MIN_MA, Math.min(MAX_MA, currentMa));

    if (currentMa <= MIN_MA || currentMa >= MAX_MA) {
      playing = false;
      document.getElementById('btn-play').textContent = '▶ Play';
    }
  }

  updateUI(currentMa);
  scheduleFetch(currentMa);
  render(currentMa);

  animFrame = requestAnimationFrame(animate);
}

// Preload initial batch of data
async function preload() {
  const initialTimes = [0, 50, 100, 200, 300, 400, 500, 600, 750];
  let loaded = 0;
  loadingStatus.textContent = 'Fetching paleogeographic reconstructions…';

  // Fetch first few in parallel
  const promises = initialTimes.slice(0, 4).map(async (t) => {
    const data = await loader.fetchTime(t);
    loaded++;
    loadingFill.style.width = (loaded / 4 * 100) + '%';
    loadingStatus.textContent = `Loaded ${t} Ma reconstruction…`;
    return data;
  });

  await Promise.allSettled(promises);

  // Set initial state
  currentGeojson = loader.cache.get(750) || loader.cache.get(700) || null;
  loadingEl.style.display = 'none';

  // Start animation
  animFrame = requestAnimationFrame(animate);

  // Continue prefetching in background
  setTimeout(() => {
    FETCH_TIMES.forEach(t => loader.fetchTime(t).catch(() => {}));
  }, 2000);
}

// Controls
document.getElementById('btn-play').addEventListener('click', () => {
  playing = !playing;
  document.getElementById('btn-play').textContent = playing ? '⏸ Pause' : '▶ Play';
  if (playing) lastTimestamp = null;
});

document.getElementById('btn-rev').addEventListener('click', () => {
  direction *= -1;
  document.getElementById('btn-rev').textContent = direction < 0 ? '⏮ Reverse' : '⏭ Forward';
});

document.getElementById('speed').addEventListener('input', (e) => {
  speedMultiplier = parseInt(e.target.value);
  document.getElementById('speed-label').textContent = `Speed: ${speedMultiplier}×`;
});

// Timeline scrubbing
const timeline = document.getElementById('timeline');
let scrubbing = false;

function scrubToX(x) {
  const rect = timeline.getBoundingClientRect();
  const pct = Math.max(0, Math.min(1, (x - rect.left) / rect.width));
  currentMa = MAX_MA - pct * MAX_MA;
  currentGeojson = null; // force refetch
  lastTimestamp = null;
}

timeline.addEventListener('mousedown', (e) => { scrubbing = true; scrubToX(e.clientX); });
document.addEventListener('mousemove', (e) => { if (scrubbing) scrubToX(e.clientX); });
document.addEventListener('mouseup', () => { scrubbing = false; });

// Projection buttons
['equirect','robinson','ortho'].forEach(proj => {
  document.getElementById('proj-' + proj).addEventListener('click', () => {
    renderer.projection = proj;
    document.querySelectorAll('#projection-btns button').forEach(b => b.classList.remove('active'));
    document.getElementById('proj-' + proj).classList.add('active');
  });
});

// Resize
window.addEventListener('resize', () => {
  renderer.resize();
});

// Keyboard shortcuts
document.addEventListener('keydown', (e) => {
  if (e.key === ' ') { e.preventDefault(); document.getElementById('btn-play').click(); }
  if (e.key === 'ArrowLeft') { currentMa = Math.min(MAX_MA, currentMa + 50); currentGeojson = null; }
  if (e.key === 'ArrowRight') { currentMa = Math.max(MIN_MA, currentMa - 50); currentGeojson = null; }
  if (e.key === 'r') document.getElementById('btn-rev').click();
});

// Init
renderer = new TectonicRenderer(canvas);
preload();
