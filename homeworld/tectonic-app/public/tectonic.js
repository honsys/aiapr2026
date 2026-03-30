// Tectonic / Earth renderer
const PROJ = {
  equirect:{
    p(lon,lat,W,H){return[(lon+180)/360*W,(90-lat)/180*H];},
  },
  robinson:{
    _X:[1,.9986,.9954,.99,.9822,.973,.96,.9427,.9216,.8962,.8679,.835,.7986,.7597,.7186,.6732,.6213,.5722,.5322],
    _Y:[0,.062,.124,.186,.248,.31,.372,.434,.4958,.5571,.6176,.6769,.7346,.7903,.8435,.8936,.9394,.9761,1],
    _i(lat){
      const a=Math.abs(lat),i=Math.min(Math.floor(a/5),17),t=(a%5)/5;
      const x=this._X[i]+(this._X[i+1]-this._X[i])*t;
      const y=this._Y[i]+(this._Y[i+1]-this._Y[i])*t;
      return{x,y:lat<0?-y:y};
    },
    p(lon,lat,W,H){const{x,y}=this._i(lat);return[(lon/180*x*.5+.5)*W,(.5-y*.5)*H];}
  },
  ortho:{
    _l0:0,_p0:20,
    p(lon,lat,W,H){
      const l0=this._l0*Math.PI/180,p0=this._p0*Math.PI/180;
      const l=lon*Math.PI/180,p=lat*Math.PI/180;
      const c=Math.sin(p0)*Math.sin(p)+Math.cos(p0)*Math.cos(p)*Math.cos(l-l0);
      if(c<0)return null;
      const R=Math.min(W,H)*.45;
      return[W/2+R*Math.cos(p)*Math.sin(l-l0),H/2-R*(Math.cos(p0)*Math.sin(p)-Math.sin(p0)*Math.cos(p)*Math.cos(l-l0))];
    }
  }
};

class TectonicRenderer {
  constructor(canvas){this.c=canvas;this.ctx=canvas.getContext('2d');this.proj='equirect';}

  pr(lon,lat,W,H){return PROJ[this.proj].p(lon,lat,W,H);}

  render(ma,geojson,W,H){
    const ctx=this.ctx;
    ctx.clearRect(0,0,W,H);
    this._bg(ctx,ma,W,H);
    this._grid(ctx,W,H);
    if(this.proj==='ortho')this._globe(ctx,W,H);
    this._ice(ctx,ma,W,H);
    if(geojson) this._geojson(ctx,geojson,ma,W,H);
    else this._synth(ctx,ma,W,H);
  }

  _bg(ctx,ma,W,H){
    let c=ma>4000?'#0a0000':ma>2500?'#050a10':ma>541?'#071520':'#0a1828';
    ctx.fillStyle=c;ctx.fillRect(0,0,W,H);
  }

  _grid(ctx,W,H){
    ctx.strokeStyle='rgba(255,255,255,0.04)';ctx.lineWidth=.5;
    for(let lon=-180;lon<=180;lon+=30){
      ctx.beginPath();
      for(let lat=-90;lat<=90;lat+=3){const p=this.pr(lon,lat,W,H);if(!p)continue;lat===-90?ctx.moveTo(p[0],p[1]):ctx.lineTo(p[0],p[1]);}
      ctx.stroke();
    }
    for(let lat=-90;lat<=90;lat+=30){
      ctx.beginPath();
      for(let lon=-180;lon<=180;lon+=3){const p=this.pr(lon,lat,W,H);if(!p)continue;lon===-180?ctx.moveTo(p[0],p[1]):ctx.lineTo(p[0],p[1]);}
      ctx.stroke();
    }
    // Equator
    ctx.strokeStyle='rgba(255,255,80,0.1)';ctx.lineWidth=1;
    ctx.beginPath();
    for(let lon=-180;lon<=180;lon+=3){const p=this.pr(lon,0,W,H);if(!p)continue;lon===-180?ctx.moveTo(p[0],p[1]):ctx.lineTo(p[0],p[1]);}
    ctx.stroke();
  }

  _globe(ctx,W,H){
    const R=Math.min(W,H)*.45;
    ctx.strokeStyle='rgba(80,120,255,0.25)';ctx.lineWidth=1;
    ctx.beginPath();ctx.arc(W/2,H/2,R,0,Math.PI*2);ctx.stroke();
  }

  _ice(ctx,ma,W,H){
    const iceRanges=[{s:720,e:635},{s:2400,e:2100}];
    for(const r of iceRanges){
      if(ma>r.s||ma<r.e)continue;
      ctx.fillStyle='rgba(180,220,255,0.12)';ctx.fillRect(0,0,W,H);
      for(const lat of[72,-72]){
        ctx.fillStyle='rgba(210,235,255,0.35)';
        ctx.beginPath();
        let f=true;
        for(let lon=-180;lon<=180;lon+=5){const p=this.pr(lon,lat,W,H);if(!p)continue;f?ctx.moveTo(p[0],p[1]):ctx.lineTo(p[0],p[1]);f=false;}
        const pole=this.pr(0,lat>0?90:-90,W,H);
        if(pole)ctx.lineTo(pole[0],pole[1]);
        ctx.closePath();ctx.fill();
      }
    }
  }

  _geojson(ctx,gj,ma,W,H){
    for(const f of(gj.features||[])){
      const g=f.geometry;if(!g)continue;
      const props=f.properties||{};
      const fc=props.FEATURE_CLASS||props.type||'';
      const isLine=g.type.includes('Line');

      if(isLine){
        this._lineStyle(ctx,fc);
        const rings=g.type==='LineString'?[g.coordinates]:g.coordinates;
        for(const r of rings)this._line(ctx,r,W,H);
      } else {
        ctx.fillStyle=this._landColor(ma,props);
        ctx.strokeStyle='rgba(200,200,150,0.25)';ctx.lineWidth=.5;
        const polys=g.type==='Polygon'?[g.coordinates]:g.coordinates;
        for(const poly of polys)this._poly(ctx,poly,W,H);
      }
    }
  }

  _lineStyle(ctx,fc){
    if(/ridge|diverge|MOR/i.test(fc)){ctx.strokeStyle='#f84';ctx.lineWidth=1.5;}
    else if(/trench|subduct|converge/i.test(fc)){ctx.strokeStyle='#fc4';ctx.lineWidth=1.5;}
    else if(/transform|fault/i.test(fc)){ctx.strokeStyle='#4fa';ctx.lineWidth=1;}
    else{ctx.strokeStyle='rgba(255,200,100,0.35)';ctx.lineWidth=.8;}
  }

  _poly(ctx,rings,W,H){
    ctx.beginPath();
    for(const ring of rings){
      let f=true;
      for(const[lon,lat]of ring){const p=this.pr(lon,lat,W,H);if(!p)continue;f?ctx.moveTo(p[0],p[1]):ctx.lineTo(p[0],p[1]);f=false;}
      ctx.closePath();
    }
    ctx.fill();ctx.stroke();
  }

  _line(ctx,coords,W,H){
    ctx.beginPath();let f=true;
    for(const[lon,lat]of coords){const p=this.pr(lon,lat,W,H);if(!p)continue;f?ctx.moveTo(p[0],p[1]):ctx.lineTo(p[0],p[1]);f=false;}
    ctx.stroke();
  }

  _landColor(ma,props){
    if(ma>4000)return'rgba(180,60,20,0.7)';
    if(ma>3000)return'rgba(120,80,30,0.8)';
    const id=(props.PLATEID1||0)%7;
    const off=[[0,0,0],[5,-5,10],[-5,10,-5],[10,5,-10],[-10,-5,5],[8,-8,3],[-3,8,-8]][id];
    return`rgba(${58+off[0]},${100+off[1]},${58+off[2]},0.85)`;
  }

  _synth(ctx,ma,W,H){
    if(ma>4000){
      // Magma ocean
      const g=ctx.createRadialGradient(W/2,H/2,0,W/2,H/2,Math.min(W,H)*.45);
      g.addColorStop(0,'rgba(255,100,0,0.55)');
      g.addColorStop(.5,'rgba(200,40,0,0.35)');
      g.addColorStop(1,'rgba(80,10,0,0.05)');
      ctx.fillStyle=g;ctx.fillRect(0,0,W,H);
      ctx.fillStyle='rgba(255,150,50,0.5)';
      ctx.font='14px Courier New';ctx.textAlign='center';
      ctx.fillText('🌋 Global Magma Ocean',W/2,H/2);
      return;
    }
    const plates=getSynth(ma);
    for(const pl of plates){
      ctx.fillStyle=pl.color+'cc';
      ctx.strokeStyle='rgba(200,200,150,0.35)';ctx.lineWidth=1;
      ctx.beginPath();
      let f=true;
      for(const[lon,lat]of pl.pts){const p=this.pr(lon,lat,W,H);if(!p)continue;f?ctx.moveTo(p[0],p[1]):ctx.lineTo(p[0],p[1]);f=false;}
      ctx.closePath();ctx.fill();ctx.stroke();
      if(pl.pts.length>0){
        const mid=pl.pts[Math.floor(pl.pts.length/2)];
        const p=this.pr(mid[0],mid[1],W,H);
        if(p){ctx.fillStyle='rgba(255,255,200,0.65)';ctx.font='10px Courier New';ctx.textAlign='center';ctx.fillText(pl.name,p[0],p[1]);}
      }
    }
  }
}
