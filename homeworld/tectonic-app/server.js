const http = require('http');
const fs = require('fs');
const path = require('path');
const https = require('https');
const url = require('url');

const PORT = 3000;
const PUBLIC = path.join(__dirname, 'public');

const MIME = {
  '.html': 'text/html',
  '.js': 'application/javascript',
  '.css': 'text/css',
  '.json': 'application/json',
};

const server = http.createServer(async (req, res) => {
  const parsed = url.parse(req.url, true);

  // CORS proxy for GPlates API
  if (parsed.pathname.startsWith('/gplates/')) {
    const target = 'https://gws.gplates.org' + parsed.pathname.replace('/gplates', '') + (parsed.search || '');
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Content-Type', 'application/json');
    https.get(target, (upstream) => {
      upstream.pipe(res);
    }).on('error', (e) => {
      res.writeHead(502);
      res.end(JSON.stringify({ error: e.message }));
    });
    return;
  }

  // Static files
  let filePath = path.join(PUBLIC, parsed.pathname === '/' ? 'index.html' : parsed.pathname);
  const ext = path.extname(filePath);
  if (!fs.existsSync(filePath)) { res.writeHead(404); res.end('Not found'); return; }

  res.setHeader('Content-Type', MIME[ext] || 'text/plain');
  res.setHeader('Cache-Control', 'no-cache');
  fs.createReadStream(filePath).pipe(res);
});

server.listen(PORT, () => {
  console.log(`\n🌍 Tectonic History Webapp running at http://localhost:${PORT}\n`);
  console.log('Controls:');
  console.log('  Space     — Play/Pause');
  console.log('  ← →       — Jump 50 Ma');
  console.log('  R         — Reverse direction');
  console.log('  Timeline  — Click/drag to scrub');
  console.log('\nData source: GPlates Web Service (PALEOMAP model, Scotese 2016)');
  console.log('Coverage: 0–750 Ma (real data) + 750 Ma–4.5 Ga (synthetic reconstructions)\n');
});
