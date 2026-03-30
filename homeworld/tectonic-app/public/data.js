// ── Geological & Solar System reference data ──────────────────────────────

const TOTAL_MA = 4600; // 4.6 Ga

// Phase boundaries (Ma ago)
const PHASES = {
  SOLAR:    { start: 4600, end: 4500, label: 'Solar System Formation' },
  HADEAN:   { start: 4500, end: 4000, label: 'Hadean — Magma Ocean' },
  ARCHEAN:  { start: 4000, end: 2500, label: 'Archean' },
  PROTEROZOIC: { start: 2500, end: 541, label: 'Proterozoic' },
  PALEOZOIC:   { start: 541,  end: 252, label: 'Paleozoic' },
  MESOZOIC:    { start: 252,  end: 66,  label: 'Mesozoic' },
  CENOZOIC:    { start: 66,   end: 0,   label: 'Cenozoic' },
};

const EONS = [
  { s:4600,e:4500,name:'Solar Nebula',   color:'#ff8800' },
  { s:4500,e:4000,name:'Hadean',         color:'#cc2200' },
  { s:4000,e:2500,name:'Archean',        color:'#8B4513' },
  { s:2500,e:541, name:'Proterozoic',    color:'#DAA520' },
  { s:541, e:252, name:'Paleozoic',      color:'#228B22' },
  { s:252, e:66,  name:'Mesozoic',       color:'#20B2AA' },
  { s:66,  e:0,   name:'Cenozoic',       color:'#4169E1' },
];

const PERIODS = [
  {s:541,e:485,n:'Cambrian'},{s:485,e:444,n:'Ordovician'},{s:444,e:419,n:'Silurian'},
  {s:419,e:359,n:'Devonian'},{s:359,e:299,n:'Carboniferous'},{s:299,e:252,n:'Permian'},
  {s:252,e:201,n:'Triassic'},{s:201,e:145,n:'Jurassic'},{s:145,e:66,n:'Cretaceous'},
  {s:66,e:23,n:'Paleogene'},{s:23,e:2.6,n:'Neogene'},{s:2.6,e:0,n:'Quaternary'},
];

const EVENTS = [
  {ma:4600,t:'solar',  n:'Solar nebula collapses — Sun ignites'},
  {ma:4568,t:'solar',  n:'Planetesimals form — accretion begins'},
  {ma:4540,t:'solar',  n:'Giant Impact — Theia hits proto-Earth, Moon forms'},
  {ma:4500,t:'hadean', n:'Earth fully accreted — magma ocean'},
  {ma:4400,t:'hadean', n:'First liquid water (zircon evidence)'},
  {ma:4100,t:'impact', n:'Late Heavy Bombardment begins'},
  {ma:3900,t:'impact', n:'Late Heavy Bombardment ends'},
  {ma:3800,t:'life',   n:'First evidence of life (stromatolites)'},
  {ma:3500,t:'tect',   n:'Ur — first stable craton'},
  {ma:3100,t:'tect',   n:'Kenorland supercontinent assembles'},
  {ma:2700,t:'life',   n:'Great Oxygenation Event begins'},
  {ma:2500,t:'major',  n:'Archean ends — Proterozoic begins'},
  {ma:1800,t:'tect',   n:'Columbia/Nuna supercontinent'},
  {ma:1100,t:'tect',   n:'Rodinia assembles'},
  {ma:750, t:'tect',   n:'Rodinia breaks apart'},
  {ma:720, t:'climate',n:'Snowball Earth — Sturtian glaciation'},
  {ma:635, t:'climate',n:'Snowball Earth ends'},
  {ma:541, t:'life',   n:'Cambrian Explosion — Phanerozoic begins'},
  {ma:450, t:'life',   n:'Ordovician mass extinction'},
  {ma:375, t:'life',   n:'Late Devonian mass extinction'},
  {ma:335, t:'tect',   n:'Pangaea assembles'},
  {ma:252, t:'life',   n:'Permian–Triassic extinction (96% species lost)'},
  {ma:200, t:'life',   n:'Triassic–Jurassic extinction'},
  {ma:180, t:'tect',   n:'Pangaea rifts — Atlantic opens'},
  {ma:66,  t:'life',   n:'K-Pg extinction — dinosaurs extinct'},
  {ma:55,  t:'tect',   n:'India collides with Asia — Himalayas'},
  {ma:35,  t:'climate',n:'Antarctica glaciates'},
  {ma:5,   t:'tect',   n:'Panama land bridge closes'},
  {ma:2.6, t:'climate',n:'Pleistocene ice ages begin'},
  {ma:0.3, t:'life',   n:'Homo sapiens evolve'},
  {ma:0,   t:'major',  n:'Present day'},
];

// Solar system bodies (semi-major axis AU, radius km, color, name)
const SOLAR_BODIES = [
  {name:'Sun',     au:0,     r:696000, color:'#FFD700', glow:true},
  {name:'Mercury', au:0.387, r:2439,   color:'#aaa'},
  {name:'Venus',   au:0.723, r:6051,   color:'#e8c060'},
  {name:'Earth',   au:1.0,   r:6371,   color:'#4488ff', highlight:true},
  {name:'Mars',    au:1.524, r:3389,   color:'#c04020'},
  {name:'Asteroid Belt', au:2.7, r:0, color:'#888', belt:true},
  {name:'Jupiter', au:5.203, r:69911,  color:'#c8a060'},
  {name:'Saturn',  au:9.537, r:58232,  color:'#e0c080', rings:true},
  {name:'Uranus',  au:19.19, r:25362,  color:'#80d0e0'},
  {name:'Neptune', au:30.07, r:24622,  color:'#4060e0'},
];

// Synthetic plate configs for pre-GPlates era (>750 Ma)
// Coordinates: [lon, lat]
function ellipse(cx,cy,rx,ry,n=20){
  const p=[];
  for(let i=0;i<=n;i++){const a=i/n*Math.PI*2;p.push([cx+rx*Math.cos(a),cy+ry*Math.sin(a)]);}
  return p;
}

const SYNTH = {
  750:  [{id:0,name:'Rodinia',       color:'#5a7a3a',pts:ellipse(0,-10,80,55,24)}],
  900:  [{id:0,name:'Rodinia',       color:'#5a7a3a',pts:ellipse(0,-10,80,55,24)}],
  1100: [
    {id:0,name:'Proto-Rodinia A',    color:'#5a7a3a',pts:ellipse(-30,5,50,35,18)},
    {id:1,name:'Proto-Rodinia B',    color:'#4a6a2a',pts:ellipse(40,-15,45,30,16)},
  ],
  1500: [
    {id:0,name:'Laurentia',          color:'#5a7a3a',pts:ellipse(-40,20,35,25,14)},
    {id:1,name:'Baltica',            color:'#4a8a4a',pts:ellipse(10,30,25,20,12)},
    {id:2,name:'Siberia',            color:'#6a7a2a',pts:ellipse(60,15,30,22,14)},
    {id:3,name:'S.Gondwana',         color:'#3a6a5a',pts:ellipse(20,-30,55,35,18)},
  ],
  1800: [{id:0,name:'Columbia/Nuna', color:'#5a7a3a',pts:ellipse(10,10,75,50,22)}],
  2500: [{id:0,name:'Kenorland',     color:'#6a6a3a',pts:ellipse(-10,15,60,40,20)}],
  3500: [
    {id:0,name:'Ur craton',          color:'#7a6a2a',pts:ellipse(30,-20,30,20,12)},
    {id:1,name:'Vaalbara',           color:'#6a5a1a',pts:ellipse(-20,-10,25,18,10)},
  ],
  4000: [
    {id:0,name:'Proto-craton A',     color:'#8a5a1a',pts:ellipse(-30,10,20,15,8)},
    {id:1,name:'Proto-craton B',     color:'#7a4a0a',pts:ellipse(20,-5,18,12,8)},
    {id:2,name:'Proto-craton C',     color:'#9a6a2a',pts:ellipse(60,20,15,10,8)},
  ],
  4500: [], // magma ocean — no plates
};

function getEon(ma){return EONS.find(e=>ma<=e.s&&ma>=e.e)||EONS[0];}
function getPeriod(ma){return PERIODS.find(p=>ma<=p.s&&ma>=p.e)||null;}
function getNearestEvent(ma,thr=20){
  return EVENTS.reduce((b,e)=>{const d=Math.abs(e.ma-ma);return d<thr&&d<(b?Math.abs(b.ma-ma):Infinity)?e:b;},null);
}
function getSynth(ma){
  const keys=Object.keys(SYNTH).map(Number).sort((a,b)=>b-a);
  for(let i=0;i<keys.length-1;i++){
    if(ma<=keys[i]&&ma>keys[i+1]){
      const t=(keys[i]-ma)/(keys[i]-keys[i+1]);
      return t<0.5?SYNTH[keys[i]]:SYNTH[keys[i+1]];
    }
  }
  return SYNTH[keys[keys.length-1]]||[];
}
function formatMa(ma){
  if(ma>=1000)return(ma/1000).toFixed(2)+' Ga';
  if(ma<1)return(ma*1000).toFixed(0)+' ka';
  return ma.toFixed(1)+' Ma';
}
