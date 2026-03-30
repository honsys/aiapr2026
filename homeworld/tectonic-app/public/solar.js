// Solar system formation renderer (4600–4500 Ma)
class SolarRenderer {
  constructor(canvas){this.c=canvas;this.ctx=canvas.getContext('2d');}

  render(ma, W, H){
    const ctx=this.ctx;
    ctx.clearRect(0,0,W,H);

    // Phase within solar formation: 0=nebula, 1=fully formed
    // 4600 Ma = start, 4500 Ma = end of this phase
    const t = Math.max(0, Math.min(1, (4600-ma)/100));

    this._drawStarfield(ctx,W,H,ma);

    if(ma>4568){
      // Nebula phase
      this._drawNebula(ctx,W,H,1-(ma-4568)/32);
    }

    if(ma<=4568){
      // Protoplanetary disk + planets forming
      const diskT=Math.max(0,Math.min(1,(4568-ma)/68));
      this._drawDisk(ctx,W,H,diskT,ma);
      this._drawPlanets(ctx,W,H,diskT,ma);
    }

    this._drawLabels(ctx,W,H,ma);
  }

  _drawStarfield(ctx,W,H,ma){
    // Seed-stable stars
    ctx.fillStyle='#000';
    ctx.fillRect(0,0,W,H);
    const rng=this._rng(42);
    for(let i=0;i<300;i++){
      const x=rng()*W, y=rng()*H;
      const r=rng()*1.2+0.2;
      const a=0.3+rng()*0.7;
      ctx.beginPath();
      ctx.arc(x,y,r,0,Math.PI*2);
      ctx.fillStyle=`rgba(255,255,255,${a})`;
      ctx.fill();
    }
  }

  _drawNebula(ctx,W,H,intensity){
    const cx=W/2,cy=H/2;
    // Rotating gas cloud
    const now=Date.now()/4000;
    for(let i=0;i<6;i++){
      const angle=now+i*Math.PI/3;
      const r=Math.min(W,H)*0.35;
      const gx=cx+Math.cos(angle)*r*0.3, gy=cy+Math.sin(angle)*r*0.3;
      const grad=ctx.createRadialGradient(gx,gy,0,cx,cy,r);
      const colors=['rgba(255,120,20','rgba(200,80,180','rgba(80,120,255','rgba(255,200,50','rgba(100,200,100','rgba(200,100,255'];
      grad.addColorStop(0,colors[i]+`,${0.15*intensity})`);
      grad.addColorStop(1,colors[i]+',0)');
      ctx.fillStyle=grad;
      ctx.fillRect(0,0,W,H);
    }
    // Central proto-sun glow
    const sg=ctx.createRadialGradient(cx,cy,0,cx,cy,Math.min(W,H)*0.2);
    sg.addColorStop(0,`rgba(255,220,100,${0.8*intensity})`);
    sg.addColorStop(0.3,`rgba(255,150,30,${0.4*intensity})`);
    sg.addColorStop(1,'rgba(255,100,0,0)');
    ctx.fillStyle=sg;
    ctx.fillRect(0,0,W,H);
  }

  _drawDisk(ctx,W,H,diskT,ma){
    const cx=W/2,cy=H/2;
    const maxR=Math.min(W,H)*0.46;
    // Protoplanetary disk (ellipse for perspective)
    ctx.save();
    ctx.translate(cx,cy);
    // Disk gradient
    for(let r=maxR;r>20;r-=2){
      const frac=r/maxR;
      const alpha=(1-frac)*0.12*diskT;
      const hue=30+frac*20;
      ctx.beginPath();
      ctx.ellipse(0,0,r,r*0.18,0,0,Math.PI*2);
      ctx.strokeStyle=`hsla(${hue},80%,60%,${alpha})`;
      ctx.lineWidth=2;
      ctx.stroke();
    }
    ctx.restore();

    // Sun
    const sunR=Math.max(8,18*(1-diskT*0.3));
    const sg=ctx.createRadialGradient(cx,cy,0,cx,cy,sunR*3);
    sg.addColorStop(0,'rgba(255,240,150,1)');
    sg.addColorStop(0.3,'rgba(255,180,30,0.8)');
    sg.addColorStop(1,'rgba(255,100,0,0)');
    ctx.fillStyle=sg;
    ctx.beginPath();ctx.arc(cx,cy,sunR*3,0,Math.PI*2);ctx.fill();
    ctx.fillStyle='#FFE080';
    ctx.beginPath();ctx.arc(cx,cy,sunR,0,Math.PI*2);ctx.fill();
  }

  _drawPlanets(ctx,W,H,diskT,ma){
    const cx=W/2,cy=H/2;
    const maxR=Math.min(W,H)*0.46;
    const now=Date.now()/1000;

    SOLAR_BODIES.forEach((body,i)=>{
      if(body.au===0)return; // Sun drawn separately
      if(body.belt){
        // Asteroid belt
        const bR=this._auToR(body.au,maxR);
        const rng=this._rng(99);
        for(let j=0;j<80;j++){
          const a=rng()*Math.PI*2;
          const r=bR+(rng()-0.5)*this._auToR(0.3,maxR);
          const x=cx+Math.cos(a)*r, y=cy+Math.sin(a)*r*0.18;
          ctx.fillStyle=`rgba(150,140,120,${0.3*diskT})`;
          ctx.beginPath();ctx.arc(x,y,0.8,0,Math.PI*2);ctx.fill();
        }
        return;
      }

      const orbitR=this._auToR(body.au,maxR);
      // Orbit ring
      ctx.beginPath();
      ctx.ellipse(cx,cy,orbitR,orbitR*0.18,0,0,Math.PI*2);
      ctx.strokeStyle=`rgba(255,255,255,0.06)`;
      ctx.lineWidth=0.5;ctx.stroke();

      // Planet formation progress: inner planets form faster
      const formT=Math.max(0,Math.min(1,diskT*2-body.au*0.15));
      if(formT<=0)return;

      // Orbital speed (Kepler: T∝a^1.5)
      const period=Math.pow(body.au,1.5)*8; // seconds per orbit (visual)
      const angle=now/period*Math.PI*2;
      const px=cx+Math.cos(angle)*orbitR;
      const py=cy+Math.sin(angle)*orbitR*0.18;

      // Planet radius on screen
      const pr=Math.max(2,Math.log(body.r/1000+1)*2.5*formT);

      if(body.rings){
        ctx.save();ctx.translate(px,py);
        ctx.beginPath();ctx.ellipse(0,0,pr*2.5,pr*0.5,0,0,Math.PI*2);
        ctx.strokeStyle=`rgba(200,180,120,${0.5*formT})`;ctx.lineWidth=1.5;ctx.stroke();
        ctx.restore();
      }

      // Glow
      const glow=ctx.createRadialGradient(px,py,0,px,py,pr*3);
      glow.addColorStop(0,body.color+'cc');
      glow.addColorStop(1,body.color+'00');
      ctx.fillStyle=glow;
      ctx.beginPath();ctx.arc(px,py,pr*3,0,Math.PI*2);ctx.fill();

      ctx.fillStyle=body.color;
      ctx.beginPath();ctx.arc(px,py,pr,0,Math.PI*2);ctx.fill();

      if(body.highlight){
        ctx.strokeStyle='rgba(100,200,255,0.6)';ctx.lineWidth=1;
        ctx.beginPath();ctx.arc(px,py,pr+3,0,Math.PI*2);ctx.stroke();
      }

      // Label
      if(pr>3){
        ctx.fillStyle='rgba(200,220,255,0.7)';
        ctx.font='9px Courier New';ctx.textAlign='center';
        ctx.fillText(body.name,px,py-pr-4);
      }
    });
  }

  _drawLabels(ctx,W,H,ma){
    ctx.fillStyle='rgba(255,200,100,0.6)';
    ctx.font='11px Courier New';ctx.textAlign='left';
    if(ma>4568) ctx.fillText('Solar nebula collapsing…',12,H-20);
    else if(ma>4540) ctx.fillText('Protoplanetary disk forming…',12,H-20);
    else if(ma>4500) ctx.fillText('Giant Impact — Moon forming',12,H-20);
  }

  _auToR(au,maxR){return au/32*maxR;}
  _rng(seed){let s=seed;return()=>{s=(s*16807+0)%2147483647;return(s-1)/2147483646;};}
}
