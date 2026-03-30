// ── Main controller ───────────────────────────────────────────────────────
const GPLATES_MAX = 750;
const canvas = document.getElementById('c');
const ctx = canvas.getContext('2d');

let W, H;
function resize(){
  const dpr=window.devicePixelRatio||1;
  const rect=canvas.parentElement.getBoundingClientRect();
  W=rect.width; H=rect.height;
  canvas.width=W*dpr; canvas.height=H*dpr;
  canvas.style.width=W+'px'; canvas.style.height=H+'px';
  ctx.scale(dpr,dpr);
}
resize();
window.addEventListener('resize',resize);

const solarR = new SolarRenderer(canvas);
const tecR   = new TectonicRenderer(canvas);

// State
let ma = 4600, playing = true, dir = -1, speed = 5;
let mode = 'auto'; // 'solar' | 'earth' | 'auto'
let geojson = null;
let lastTs = null;
let fetchTimer = null;

// Data cache
const cache = new Map();
const FETCH_TIMES = [
  0,5,10,15,20,25,30,35,40,50,60,66,70,80,90,100,
  110,120,130,140,150,160,170,180,190,200,210,220,230,
  240,250,260,270,280,290,300,310,320,330,340,350,360,
  370,380,390,400,410,420,430,440,450,460,470,480,490,
  500,510,520,530,540,550,560,570,580,590,600,620,640,
  660,680,700,720,740,750
];

async function fetchMa(t){
  const k=Math.round(t);
  if(cache.has(k))return cache.get(k);
  try{
    const [cr,br]=await Promise.allSettled([
      fetch(`https://gws.gplates.org/reconstruct/coastlines/?time=${k}&model=PALEOMAP&fmt=geojson`,{signal:AbortSignal.timeout(8000)}),
      fetch(`https://gws.gplates.org/topology/plate_boundaries/?time=${k}&model=PALEOMAP&fmt=geojson`,{signal:AbortSignal.timeout(8000)}),
    ]);
    const feats=[];
    if(cr.status==='fulfilled'&&cr.value.ok){const d=await cr.value.json();feats.push(...(d.features||[]));}
    if(br.status==='fulfilled'&&br.value.ok){const d=await br.value.json();feats.push(...(d.features||[]));}
    const gj=feats.length?{type:'FeatureCollection',features:feats}:null;
    cache.set(k,gj);return gj;
  }catch(e){cache.set(k,null);return null;}
}

function nearestFetchTime(t){
  return FETCH_TIMES.reduce((a,b)=>Math.abs(b-t)<Math.abs(a-t)?b:a);
}

function scheduleFetch(t){
  clearTimeout(fetchTimer);
  fetchTimer=setTimeout(async()=>{
    if(t>GPLATES_MAX){geojson=null;return;}
    const snap=nearestFetchTime(t);
    if(cache.has(snap)){geojson=cache.get(snap);return;}
    geojson=await fetchMa(snap);
    // prefetch neighbors
    const idx=FETCH_TIMES.indexOf(snap);
    [-2,-1,1,2].forEach(d=>{const nt=FETCH_TIMES[idx+d];if(nt!=null&&!cache.has(nt))fetchMa(nt);});
  },80);
}

// ── Render loop ───────────────────────────────────────────────────────────
function frame(ts){
  if(!lastTs)lastTs=ts;
  const dt=Math.min(ts-lastTs,100)/1000;
  lastTs=ts;

  if(playing){
    ma+=dir*speed*50*dt;
    ma=Math.max(0,Math.min(4600,ma));
    if(ma<=0||ma>=4600){playing=false;document.getElementById('bplay').textContent='▶ Play';}
  }

  updateUI();
  scheduleFetch(ma);
  draw();
  requestAnimationFrame(frame);
}

function draw(){
  const isSolar = mode==='solar' || (mode==='auto' && ma>4500);
  if(isSolar){
    solarR.render(ma,W,H);
  } else {
    tecR.render(ma,geojson,W,H);
  }
}

function updateUI(){
  document.getElementById('time-badge').textContent=formatMa(ma)+' ago';
  const eon=getEon(ma);
  const per=getPeriod(ma);
  document.getElementById('eon-badge').textContent=eon.name+(per?' · '+per.n:'');
  document.getElementById('eon-badge').style.color=eon.color;
  const ev=getNearestEvent(ma,ma>100?25:8);
  document.getElementById('event-badge').textContent=ev?'⚡ '+ev.n:'';
  const phase=Object.values(PHASES).find(p=>ma<=p.start&&ma>=p.end);
  document.getElementById('phase-badge').textContent=phase?phase.label:'';
  const pct=(4600-ma)/4600*100;
  document.getElementById('tl-fill').style.width=pct+'%';
  document.getElementById('tl-thumb').style.left=pct+'%';
}

// ── Controls ──────────────────────────────────────────────────────────────
document.getElementById('bplay').addEventListener('click',()=>{
  playing=!playing;
  document.getElementById('bplay').textContent=playing?'⏸ Pause':'▶ Play';
  if(playing)lastTs=null;
});
document.getElementById('brev').addEventListener('click',()=>{
  dir*=-1;
  document.getElementById('brev').textContent=dir<0?'⏮ Rev':'⏭ Fwd';
});
document.getElementById('bsolar').addEventListener('click',()=>{
  mode='solar';ma=4580;geojson=null;lastTs=null;
  document.getElementById('bsolar').classList.add('on');
  document.getElementById('bearth').classList.remove('on');
});
document.getElementById('bearth').addEventListener('click',()=>{
  mode='earth';if(ma>4500)ma=4500;geojson=null;lastTs=null;
  document.getElementById('bearth').classList.add('on');
  document.getElementById('bsolar').classList.remove('on');
});
document.getElementById('spd').addEventListener('input',e=>{
  speed=parseInt(e.target.value);
  document.getElementById('speed-lbl').textContent=speed+'×';
});

// Timeline scrub
const tl=document.getElementById('tl');
let scrub=false;
function scrubTo(x){
  const r=tl.getBoundingClientRect();
  const pct=Math.max(0,Math.min(1,(x-r.left)/r.width));
  ma=4600-pct*4600;geojson=null;lastTs=null;
}
tl.addEventListener('mousedown',e=>{scrub=true;scrubTo(e.clientX);});
document.addEventListener('mousemove',e=>{if(scrub)scrubTo(e.clientX);});
document.addEventListener('mouseup',()=>scrub=false);
// Touch support
tl.addEventListener('touchstart',e=>{scrub=true;scrubTo(e.touches[0].clientX);},{passive:true});
document.addEventListener('touchmove',e=>{if(scrub)scrubTo(e.touches[0].clientX);},{passive:true});
document.addEventListener('touchend',()=>scrub=false);

// Projection
['eq','rob','orth'].forEach(p=>{
  document.getElementById('proj-'+p).addEventListener('click',()=>{
    const map={eq:'equirect',rob:'robinson',orth:'ortho'};
    tecR.proj=map[p];
    document.querySelectorAll('[id^=proj-]').forEach(b=>b.classList.remove('on'));
    document.getElementById('proj-'+p).classList.add('on');
  });
});

// Keyboard
document.addEventListener('keydown',e=>{
  if(e.key===' '){e.preventDefault();document.getElementById('bplay').click();}
  if(e.key==='ArrowLeft'){ma=Math.min(4600,ma+100);geojson=null;}
  if(e.key==='ArrowRight'){ma=Math.max(0,ma-100);geojson=null;}
  if(e.key==='r')document.getElementById('brev').click();
  if(e.key==='s')document.getElementById('bsolar').click();
  if(e.key==='e')document.getElementById('bearth').click();
});

// Tooltip on hover (show nearest event)
canvas.addEventListener('mousemove',e=>{
  const tip=document.getElementById('tooltip');
  const ev=getNearestEvent(ma,ma>500?50:15);
  if(ev){
    tip.style.display='block';
    tip.style.left=(e.clientX+12)+'px';
    tip.style.top=(e.clientY-10)+'px';
    tip.innerHTML=`<b>${formatMa(ev.ma)}</b><br>${ev.n}`;
  } else {
    tip.style.display='none';
  }
});
canvas.addEventListener('mouseleave',()=>document.getElementById('tooltip').style.display='none');

// ── Init ──────────────────────────────────────────────────────────────────
async function init(){
  const lf=document.getElementById('lf');
  const ls=document.getElementById('ls');
  // Preload key frames
  const preload=[0,50,100,200,300,400,500,600,750];
  for(let i=0;i<preload.length;i++){
    ls.textContent=`Fetching ${preload[i]} Ma reconstruction…`;
    lf.style.width=((i+1)/preload.length*100)+'%';
    await fetchMa(preload[i]);
  }
  geojson=cache.get(750)||null;
  document.getElementById('loading').style.display='none';
  requestAnimationFrame(frame);
  // Background prefetch
  setTimeout(()=>FETCH_TIMES.forEach(t=>fetchMa(t)),3000);
}
init();
