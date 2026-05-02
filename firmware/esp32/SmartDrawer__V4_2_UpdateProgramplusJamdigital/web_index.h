const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Smart Drawer</title>
<!-- Google Fonts dihapus: bikin halaman hang saat AP mode (no internet) -->
<style>
*{box-sizing:border-box;margin:0;padding:0}
:root{
  --ink:#0a0a0a;--paper:#f5f0e8;--panel:#1a1a1a;--panel2:#242424;
  --accent:#e8ff00;--accent2:#ff6b35;--wire:#2a2a2a;--dim:#888;
  --border:#333;--red:#ff3b30;--green:#30d158;
  --mono:'DM Mono','Courier New',monospace;--display:'Syne','Arial Black',sans-serif;--body:'DM Sans','Segoe UI',Arial,sans-serif;
}
html,body{height:100%;overflow:hidden}
body{font-family:var(--body);background:var(--paper);color:var(--ink)}

/* ── TOPBAR ──────────────────────────── */
.topbar{
  position:fixed;top:0;left:0;right:0;height:48px;
  background:var(--ink);display:flex;align-items:center;
  padding:0 20px;gap:14px;z-index:100;
  border-bottom:2px solid var(--accent);
}
.logo{
  font-family:var(--display);font-size:.95rem;font-weight:800;
  color:var(--accent);letter-spacing:2px;text-transform:uppercase;
  display:flex;align-items:center;gap:8px;white-space:nowrap;
}
.logo span{color:#fff}
.topbar-sep{width:1px;height:18px;background:var(--border)}
.nav-tab{
  font-family:var(--mono);font-size:.65rem;color:var(--dim);
  padding:4px 10px;border-radius:2px;cursor:pointer;
  border:1px solid transparent;transition:.15s;text-transform:uppercase;letter-spacing:1px;
}
.nav-tab:hover{color:#fff}
.nav-tab.active{color:var(--accent);border-color:var(--border)}
.topbar-right{margin-left:auto;display:flex;gap:6px;align-items:center}
.chip{
  font-family:var(--mono);font-size:.62rem;
  background:var(--wire);color:var(--dim);
  border:1px solid var(--border);border-radius:2px;
  padding:3px 8px;cursor:pointer;transition:.15s;
  display:flex;align-items:center;gap:4px;
}
.chip:hover{color:#fff;background:var(--border)}
#sDot{width:7px;height:7px;border-radius:50%;background:var(--red);box-shadow:0 0 6px var(--red);flex-shrink:0}
#sDot.on{background:var(--green);box-shadow:0 0 8px var(--green)}
#sLbl{font-family:var(--mono);font-size:.62rem;color:var(--dim)}

/* ── PAGE WRAPPER ─────────────────────── */
.page{display:none;height:calc(100vh - 48px);margin-top:48px}
.page.active{display:flex}

/* ═══════════════════════════════════════
   PAGE: MAIN
═══════════════════════════════════════ */
#pgMain{flex-direction:row}

.sidebar{
  width:230px;background:var(--panel);
  overflow-y:auto;border-right:1px solid var(--border);
  display:flex;flex-direction:column;flex-shrink:0;
}
.sb-sec{border-bottom:1px solid var(--border);padding:14px 16px}
.sb-lbl{
  font-family:var(--mono);font-size:.58rem;color:var(--accent);
  text-transform:uppercase;letter-spacing:2px;margin-bottom:10px;
  display:flex;align-items:center;gap:6px;
}
.sb-lbl::after{content:'';flex:1;height:1px;background:var(--border)}

.ctrl{margin-bottom:10px}
.ctrl-n{font-family:var(--mono);font-size:.6rem;color:var(--dim);margin-bottom:4px;display:block}
input[type=color]{width:100%;height:32px;border:1px solid var(--border);border-radius:3px;cursor:pointer;padding:2px;background:#111}
select{width:100%;padding:6px 8px;background:var(--wire);color:#ccc;border:1px solid var(--border);border-radius:3px;font-family:var(--mono);font-size:.7rem;outline:none}
select:focus{border-color:var(--accent)}
.rng-row{display:flex;align-items:center;gap:8px}
input[type=range]{flex:1;-webkit-appearance:none;height:3px;background:var(--border);border-radius:2px;outline:none;cursor:pointer}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:14px;height:14px;border-radius:50%;background:var(--accent);cursor:pointer;border:2px solid var(--ink)}
.rng-v{font-family:var(--mono);font-size:.65rem;color:var(--accent);min-width:28px;text-align:right}
.pwr-row{display:grid;grid-template-columns:1fr 1fr;gap:6px;margin-top:4px}
.pwr-btn{padding:8px 0;border:1px solid var(--border);border-radius:3px;font-family:var(--mono);font-size:.7rem;cursor:pointer;transition:.15s;text-transform:uppercase;letter-spacing:1px}
.pwr-btn:active{transform:scale(.97)}
.pwr-off{background:transparent;color:#f87171;border-color:#7f1d1d}.pwr-off:hover{background:#7f1d1d}
.pwr-on{background:transparent;color:#4ade80;border-color:#14532d}.pwr-on:hover{background:#14532d}

.stats-grid{display:grid;grid-template-columns:1fr 1fr;gap:6px}
.stat{background:var(--wire);border:1px solid var(--border);border-radius:3px;padding:8px 10px}
.stat-n{font-family:var(--display);font-size:1.4rem;font-weight:800;color:#fff;line-height:1}
.stat-l{font-family:var(--mono);font-size:.54rem;color:var(--dim);margin-top:2px;text-transform:uppercase;letter-spacing:1px}

.add-form{display:flex;flex-direction:column;gap:6px}
.add-form input{padding:7px 10px;background:var(--wire);color:#ccc;border:1px solid var(--border);border-radius:3px;font-family:var(--mono);font-size:.7rem;outline:none}
.add-form input:focus{border-color:var(--accent)}
.add-form input::placeholder{color:#555}
.btn-add{padding:8px;background:var(--accent);color:var(--ink);border:none;border-radius:3px;font-family:var(--mono);font-size:.7rem;font-weight:500;cursor:pointer;text-transform:uppercase;letter-spacing:1px;transition:.15s}
.btn-add:hover{background:#d4eb00}

.main-content{flex:1;display:flex;flex-direction:column;overflow:hidden}
.toolbar{padding:12px 20px;border-bottom:1px solid #ddd;display:flex;gap:10px;align-items:center;flex-shrink:0;background:#fff}
.search-box{flex:1;display:flex;align-items:center;background:var(--paper);border:1px solid #d0c9b8;border-radius:3px;padding:0 12px;gap:8px}
#searchBox{flex:1;border:none;background:transparent;padding:8px 0;font-family:var(--body);font-size:.85rem;outline:none;color:var(--ink)}

.grid-wrap{flex:1;overflow-y:auto;padding:20px}
.card-grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(186px,1fr));gap:12px}

.card{background:#fff;border:1px solid #e0d9cf;border-radius:4px;overflow:hidden;position:relative;transition:.2s;display:flex;flex-direction:column}
.card:hover{border-color:#bbb;box-shadow:0 4px 16px rgba(0,0,0,.08);transform:translateY(-1px)}
.card-img{width:100%;height:116px;object-fit:cover;background:#f5f0e8;flex-shrink:0}
.card-body{padding:11px 12px;flex:1;display:flex;flex-direction:column;gap:5px}
.card-name{font-family:var(--display);font-weight:700;font-size:.85rem;color:var(--ink);white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.card-tags{display:flex;gap:4px;flex-wrap:wrap}
.tag{font-family:var(--mono);font-size:.56rem;padding:2px 6px;border-radius:2px}
.tag-laci{background:#e8f4f8;color:#0e7490;border:1px solid #bae6fd}
.stok-row{display:flex;align-items:center;justify-content:space-between;background:var(--paper);border:1px solid #e0d9cf;border-radius:3px;padding:3px 6px}
.stk-btn{background:none;border:none;cursor:pointer;font-size:1rem;color:#999;padding:0 4px;line-height:1;transition:.15s;font-family:var(--mono)}
.stk-btn:hover{color:var(--ink)}
.stok-val{font-family:var(--mono);font-weight:500;font-size:.85rem;color:var(--ink)}
.btn-cari{padding:7px;border:1px solid var(--ink);background:var(--ink);color:var(--accent);border-radius:3px;font-family:var(--mono);font-size:.62rem;font-weight:500;cursor:pointer;text-transform:uppercase;letter-spacing:1px;transition:.15s}
.btn-cari:hover{background:#222}
.btn-del{position:absolute;top:8px;right:8px;width:22px;height:22px;background:rgba(0,0,0,.5);color:#fff;border:none;border-radius:2px;cursor:pointer;font-size:.8rem;display:flex;align-items:center;justify-content:center;opacity:0;transition:.15s;backdrop-filter:blur(4px)}
.card:hover .btn-del{opacity:1}
.btn-del:hover{background:var(--red)}
.empty{grid-column:1/-1;text-align:center;padding:60px 20px;color:#aaa}
.empty-ico{font-size:2.5rem;margin-bottom:12px;opacity:.4}
.empty p{font-family:var(--mono);font-size:.8rem;line-height:1.8}

/* ═══════════════════════════════════════
   PAGE: SETTINGS
═══════════════════════════════════════ */
#pgSettings{flex-direction:row;background:var(--panel)}

.settings-nav{
  width:200px;background:#111;border-right:1px solid var(--border);
  padding:16px 0;flex-shrink:0;overflow-y:auto;
}
.sn-group{margin-bottom:20px}
.sn-group-label{font-family:var(--mono);font-size:.55rem;color:#444;text-transform:uppercase;letter-spacing:2px;padding:0 16px;margin-bottom:6px}
.sn-item{
  display:flex;align-items:center;gap:8px;
  padding:8px 16px;font-family:var(--mono);font-size:.7rem;
  color:var(--dim);cursor:pointer;transition:.15s;border-left:2px solid transparent;
}
.sn-item:hover{color:#ccc;background:var(--wire)}
.sn-item.active{color:var(--accent);border-left-color:var(--accent);background:rgba(232,255,0,.04)}
.sn-item svg{flex-shrink:0;opacity:.6}
.sn-item.active svg{opacity:1}

.settings-main{flex:1;overflow-y:auto;padding:32px}
.settings-section{display:none}
.settings-section.active{display:block}

.set-title{font-family:var(--display);font-size:1.1rem;font-weight:800;color:#fff;margin-bottom:4px}
.set-sub{font-family:var(--mono);font-size:.65rem;color:var(--dim);margin-bottom:24px}
.set-divider{height:1px;background:var(--border);margin:24px 0}

.set-card{background:var(--panel2);border:1px solid var(--border);border-radius:6px;padding:20px;margin-bottom:16px}
.set-card-title{font-family:var(--mono);font-size:.65rem;color:var(--accent);text-transform:uppercase;letter-spacing:1.5px;margin-bottom:14px}

.field{margin-bottom:14px}
.field:last-child{margin-bottom:0}
.field label{font-family:var(--mono);font-size:.65rem;color:var(--dim);display:block;margin-bottom:5px}
.field input[type=text],.field input[type=password],.field input[type=number]{
  width:100%;padding:8px 12px;background:var(--wire);color:#ddd;
  border:1px solid var(--border);border-radius:4px;
  font-family:var(--mono);font-size:.75rem;outline:none;
}
.field input:focus{border-color:var(--accent)}
.field input::placeholder{color:#444}
.field small{font-family:var(--mono);font-size:.6rem;color:#555;margin-top:4px;display:block}

.btn-set{
  padding:8px 18px;border-radius:3px;font-family:var(--mono);font-size:.7rem;
  font-weight:500;cursor:pointer;border:none;text-transform:uppercase;letter-spacing:1px;transition:.15s;
}
.btn-set:active{transform:scale(.97)}
.btn-primary{background:var(--accent);color:var(--ink)}.btn-primary:hover{background:#d4eb00}
.btn-secondary{background:var(--wire);color:var(--dim);border:1px solid var(--border)}.btn-secondary:hover{color:#fff}
.btn-danger{background:transparent;color:var(--red);border:1px solid #7f1d1d}.btn-danger:hover{background:#7f1d1d;color:#fff}

.info-grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.info-box{background:var(--wire);border:1px solid var(--border);border-radius:4px;padding:12px}
.info-val{font-family:var(--display);font-size:1rem;font-weight:800;color:#fff;margin-bottom:2px}
.info-lbl{font-family:var(--mono);font-size:.58rem;color:var(--dim);text-transform:uppercase;letter-spacing:1px}

.wifi-list{display:flex;flex-direction:column;gap:6px;margin-top:10px}
.wifi-item{
  display:flex;align-items:center;justify-content:space-between;
  background:var(--wire);border:1px solid var(--border);border-radius:4px;
  padding:8px 12px;cursor:pointer;transition:.15s;
}
.wifi-item:hover{border-color:var(--accent);background:#2f2f2f}
.wifi-ssid{font-family:var(--mono);font-size:.72rem;color:#ccc}
.wifi-rssi{font-family:var(--mono);font-size:.6rem;color:var(--dim);display:flex;align-items:center;gap:4px}
.wifi-lock{font-size:.65rem}

.status-row{display:flex;align-items:center;gap:10px;padding:10px 0;border-bottom:1px solid var(--border)}
.status-row:last-child{border:none}
.status-key{font-family:var(--mono);font-size:.65rem;color:var(--dim);width:130px;flex-shrink:0}
.status-val{font-family:var(--mono);font-size:.7rem;color:#ccc}
.badge-ok{color:var(--green)}.badge-err{color:var(--red)}.badge-warn{color:var(--accent2)}

/* ── MODALS ───────────────────────────── */
.overlay{position:fixed;inset:0;background:rgba(0,0,0,.75);z-index:200;display:none;align-items:center;justify-content:center;backdrop-filter:blur(4px)}
.overlay.open{display:flex}
.modal{background:var(--panel);border:1px solid var(--border);border-radius:4px;padding:24px;width:90%;max-width:400px;border-top:2px solid var(--accent)}
.modal h3{font-family:var(--display);color:#fff;font-size:1rem;margin-bottom:4px}
.modal p{font-family:var(--mono);font-size:.65rem;color:var(--dim);margin-bottom:14px}
.modal textarea{width:100%;height:150px;background:var(--wire);color:#ccc;border:1px solid var(--border);border-radius:3px;padding:10px;font-family:var(--mono);font-size:.65rem;resize:vertical;outline:none}
.modal-btns{display:flex;gap:8px;margin-top:12px}
.modal-btns button{flex:1;padding:8px;border-radius:3px;font-family:var(--mono);font-size:.68rem;font-weight:500;cursor:pointer;border:none;text-transform:uppercase;letter-spacing:1px}
.mb-p{background:var(--accent);color:var(--ink)}.mb-s{background:var(--wire);color:var(--dim);border:1px solid var(--border)}

#toast{
  position:fixed;bottom:80px;left:50%;transform:translateX(-50%) translateY(10px);
  background:var(--panel);color:#ccc;border:1px solid var(--border);border-left:3px solid var(--accent);
  border-radius:4px;padding:10px 18px;font-family:var(--mono);font-size:.72rem;
  opacity:0;transition:.25s;z-index:400;white-space:nowrap;pointer-events:none;
}
#toast.show{opacity:1;transform:translateX(-50%) translateY(0)}
#toast.err{border-left-color:var(--red);color:var(--red)}

::-webkit-scrollbar{width:5px}
::-webkit-scrollbar-track{background:transparent}
::-webkit-scrollbar-thumb{background:var(--border);border-radius:4px}

/* 📱 MOBILE MODE DARI PROGRAM 2 */
.mob-ctrl-btn{
  display:none;
  position:fixed;bottom:20px;right:20px;z-index:300;
  width:48px;height:48px;border-radius:50%;
  background:var(--accent);color:var(--ink);
  border:none;font-size:1.3rem;cursor:pointer;
  box-shadow:0 4px 16px rgba(232,255,0,.4);
  align-items:center;justify-content:center;
  transition:.2s;
}
.mob-ctrl-btn:active{transform:scale(.93)}

.mob-overlay{
  display:none;position:fixed;inset:0;
  background:rgba(0,0,0,.6);z-index:149;
  backdrop-filter:blur(2px);
}

@media (max-width: 768px){
  html,body{overflow:auto;height:auto}
  .topbar{ height:48px;padding:0 14px;gap:10px;flex-wrap:nowrap; }
  .logo{font-size:.8rem}
  .topbar-sep{display:none}

  .page{
    flex-direction:column;
    height:auto;
    min-height:calc(100vh - 48px);
    margin-top:48px;
  }

  .sidebar{
    position:fixed;
    top:48px;left:-260px;
    width:260px;height:calc(100vh - 48px);
    z-index:150;
    border-right:1px solid var(--border);
    overflow-y:auto;
    transition:left .25s cubic-bezier(.4,0,.2,1);
    box-shadow:4px 0 24px rgba(0,0,0,.4);
  }
  .sidebar.mob-open{left:0}
  .mob-overlay.mob-open{display:block}

  .main-content{
    width:100%;
    min-height:calc(100vh - 48px);
    padding-bottom:80px; 
  }

  .toolbar{padding:10px 12px}
  .grid-wrap{padding:10px}
  .card-grid{ grid-template-columns:1fr 1fr; gap:10px; }
  .card-img{height:120px}
  .card-name{font-size:.8rem}

  #pgSettings{flex-direction:column;height:auto}

  .settings-nav{
    width:100%;
    overflow-x:auto;
    display:flex;
    flex-direction:row;
    padding:8px 0;
    border-right:none;
    border-bottom:1px solid var(--border);
    -webkit-overflow-scrolling:touch;
  }
  .sn-group{
    display:flex;flex-direction:row;
    margin-bottom:0;white-space:nowrap;
  }
  .sn-group-label{display:none}
  .sn-item{
    padding:8px 14px;border-left:none;
    border-bottom:2px solid transparent;flex-shrink:0;
  }
  .sn-item.active{
    border-left-color:transparent;
    border-bottom-color:var(--accent);
  }

  .settings-main{padding:16px}
  .mob-ctrl-btn{display:flex}
}
</style>
</head>
<body>

<header class="topbar">
  <div class="logo">
    <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><rect x="2" y="7" width="20" height="14" rx="2"/><path d="M16 7V5a2 2 0 0 0-2-2h-4a2 2 0 0 0-2 2v2"/><line x1="12" y1="12" x2="12" y2="16"/><line x1="10" y1="14" x2="14" y2="14"/></svg>
    SMART<span>DRAWER</span>
  </div>
  <div class="topbar-sep"></div>
  <button class="nav-tab active" id="tabMain"     onclick="showPage('Main')">Inventori</button>
  <button class="nav-tab"        id="tabSettings" onclick="showPage('Settings')">Pengaturan</button>
  <div class="topbar-right">
    <div id="sDot"></div>
    <span id="sLbl">OFFLINE</span>
  </div>
</header>

<button class="mob-ctrl-btn" id="mobCtrlBtn" onclick="toggleSidebar()" title="Control LED">⚡</button>
<div class="mob-overlay" id="mobOverlay" onclick="closeSidebar()"></div>

<div class="page active" id="pgMain">
  <aside class="sidebar">
    <div class="sb-sec">
      <div class="sb-lbl">LED Control</div>
      <div class="ctrl"><span class="ctrl-n">Warna Utama</span><input type="color" id="col1" value="#ffffff"></div>
      <div class="ctrl"><span class="ctrl-n">Brightness</span><div class="rng-row"><input type="range" id="bri" min="1" max="255" value="128"><span class="rng-v" id="briV">128</span></div></div>
      <div class="ctrl"><span class="ctrl-n">Effect Speed</span><div class="rng-row"><input type="range" id="sx" min="1" max="255" value="128"><span class="rng-v" id="sxV">128</span></div></div>
      <div class="ctrl"><span class="ctrl-n">Effect Mode</span><select id="fx"></select></div>
      <div class="pwr-row">
        <button class="pwr-btn pwr-off" onclick="pwrToggle(false)">■ OFF</button>
        <button class="pwr-btn pwr-on"  onclick="pwrToggle(true)">▶ ON</button>
      </div>
    </div>
    <div class="sb-sec">
      <div class="sb-lbl">Statistik</div>
      <div class="stats-grid" style="grid-template-columns: 1fr;">
        <div class="stat"><div class="stat-n" id="stTotal">0</div><div class="stat-l">Komponen Total</div></div>
        <div class="stat"><div class="stat-n" id="stStok">0</div><div class="stat-l">Jumlah Stok</div></div>
        <div class="stat"><div class="stat-n" id="stLaci">0</div><div class="stat-l">Laci Terpakai</div></div>
      </div>
    </div>
    <div class="sb-sec" style="flex:1">
      <div class="sb-lbl">Tambah Komponen</div>
      <div class="add-form">
        <input type="text"   id="nNama" placeholder="Nama komponen *">
        <input type="number" id="nLaci" placeholder="Nomor laci *" min="1" max="99">
        <input type="number" id="nStok" placeholder="Stok awal" min="0">
        <input type="text"   id="nImg"  placeholder="URL gambar (opsional)">
        <button class="btn-add" onclick="addBarang()">+ Tambah</button>
      </div>
    </div>
  </aside>

  <div class="main-content">
    <div class="toolbar">
      <div class="search-box">
        <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="#999" stroke-width="2"><circle cx="11" cy="11" r="8"/><line x1="21" y1="21" x2="16.65" y2="16.65"/></svg>
        <input type="text" id="searchBox" placeholder="Cari komponen...">
      </div>
    </div>
    <div class="grid-wrap"><div class="card-grid" id="grid"></div></div>
  </div>
</div>

<div class="page" id="pgSettings">
  <nav class="settings-nav">
    <div class="sn-group">
      <div class="sn-group-label">Sistem</div>
      <div class="sn-item active" onclick="showSet('sStatus')" id="snStatus">Status</div>
      <div class="sn-item" onclick="showSet('sDevice')" id="snDevice">Perangkat</div>
      <div class="sn-item" onclick="showSet('sRTC')" id="snRTC">Jam Digital</div>
    </div>
    <div class="sn-group">
      <div class="sn-group-label">Jaringan</div>
      <div class="sn-item" onclick="showSet('sWifi')" id="snWifi">
        <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M5 12.55a11 11 0 0 1 14.08 0"/><path d="M1.42 9a16 16 0 0 1 21.16 0"/><path d="M8.53 16.11a6 6 0 0 1 6.95 0"/><line x1="12" y1="20" x2="12.01" y2="20"/></svg>WiFi
      </div>
      <div class="sn-item" onclick="showSet('sAP')" id="snAP">
        <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M12 2L2 7l10 5 10-5-10-5z"/><path d="M2 17l10 5 10-5"/><path d="M2 12l10 5 10-5"/></svg>Access Point
      </div>
    </div>
    <div class="sn-group">
      <div class="sn-group-label">Hardware</div>
      <div class="sn-item" onclick="showSet('sLED')" id="snLED">
        <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="5"/><line x1="12" y1="1" x2="12" y2="3"/><line x1="12" y1="21" x2="12" y2="23"/><line x1="4.22" y1="4.22" x2="5.64" y2="5.64"/><line x1="18.36" y1="18.36" x2="19.78" y2="19.78"/><line x1="1" y1="12" x2="3" y2="12"/><line x1="21" y1="12" x2="23" y2="12"/></svg>Konfigurasi LED
      </div>
    </div>
    <div class="sn-group">
      <div class="sn-group-label">Data</div>
      <div class="sn-item" onclick="showSet('sData')" id="snData">
        <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><ellipse cx="12" cy="5" rx="9" ry="3"/><path d="M21 12c0 1.66-4 3-9 3s-9-1.34-9-3"/><path d="M3 5v14c0 1.66 4 3 9 3s9-1.34 9-3V5"/></svg>Inventori
      </div>
      <div class="sn-item" onclick="showSet('sDanger')" id="snDanger">
        <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>Reset
      </div>
    </div>
  </nav>

  <div class="settings-main">
    <div class="settings-section active" id="sStatus">
      <div class="set-title">Status Sistem</div>
      <div class="set-sub">Informasi perangkat dan koneksi secara real-time.</div>
      <div class="set-card">
        <div class="set-card-title">Koneksi</div>
        <div id="statusRows">
          <div class="status-row"><span class="status-key">Alamat Web</span><span class="status-val badge-ok" id="iHostname">—</span></div>
          <div class="status-row"><span class="status-key">WiFi</span><span class="status-val" id="iStaStatus">—</span></div>
          <div class="status-row"><span class="status-key">IP WiFi</span><span class="status-val" id="iStaIP">—</span></div>
          <div class="status-row"><span class="status-key">Access Point</span><span class="status-val" id="iApStatus">—</span></div>
          <div class="status-row"><span class="status-key">IP Access Point</span><span class="status-val" id="iApIP">—</span></div>
          <div class="status-row"><span class="status-key">Jaringan (SSID)</span><span class="status-val" id="iSSID">—</span></div>
          <div class="status-row"><span class="status-key">Sinyal (RSSI)</span><span class="status-val" id="iRSSI">—</span></div>
        </div>
      </div>
      <div class="set-card">
        <div class="set-card-title">Perangkat</div>
        <div class="info-grid">
          <div class="info-box"><div class="info-val" id="iHeap">—</div><div class="info-lbl">Free Heap (KB)</div></div>
          <div class="info-box"><div class="info-val" id="iChip">—</div><div class="info-lbl">Chip Model</div></div>
          <div class="info-box"><div class="info-val" id="iFlash">—</div><div class="info-lbl">Flash (MB)</div></div>
          <div class="info-box"><div class="info-val" id="iLedCnt">—</div><div class="info-lbl">Jumlah LED</div></div>
        </div>
      </div>
      <button class="btn-set btn-secondary" onclick="loadCfg()">↻ Refresh</button>
    </div>
    
<div class="settings-section" id="sRTC">
  <div class="set-title">Jam Digital (ATmega)</div>
  <div class="set-sub">Kontrol waktu dan alarm jam digital lewat web via UART.</div>
  <div class="set-card">
    <div class="set-card-title">Status Realtime</div>
    <div class="info-grid">
      <div class="info-box">
        <div class="info-val" id="rtcTimeView">--:--:--</div>
        <div class="info-lbl">Waktu Sekarang</div>
      </div>
      <div class="info-box">
        <div class="info-val" id="rtcAlarmView">--:--</div>
        <div class="info-lbl">Alarm</div>
      </div>
      <div class="info-box">
        <div class="info-val" id="rtcAlarmOnView">--</div>
        <div class="info-lbl">Status Alarm</div>
      </div>
      <div class="info-box">
        <div class="info-val" id="rtcModeView">--</div>
        <div class="info-lbl">Mode Lampu</div>
      </div>
    </div>
  </div>
  <div class="set-card">
    <div class="set-card-title">Set Waktu</div>
    <div class="field">
      <label>Jam &amp; Menit</label>
      <input type="time" id="setTimeRTC">
      <small>Pilih waktu lalu tekan tombol Set Jam</small>
    </div>
    <button class="btn-set btn-primary" onclick="setRTC()">Set Jam</button>
  </div>
  <div class="set-card">
    <div class="set-card-title">Set Alarm</div>
    <div class="field">
      <label>Waktu Alarm</label>
      <input type="time" id="setAlarmRTC">
    </div>
    <div class="field">
      <label>Status Alarm</label>
      <select id="cfgAlarmOn">
        <option value="1">ON (Aktif)</option>
        <option value="0">OFF (Mati)</option>
      </select>
    </div>
    <div style="display:flex;gap:8px;flex-wrap:wrap">
    <button class="btn-set btn-primary" onclick="setAlarm()">Set Alarm</button>
    <button class="btn-set btn-secondary" onclick="setAlarmOn()">Simpan Status</button>
    </div>
  </div>
  <div class="set-card">
    <div class="set-card-title">Mode Lampu &amp; Nada Alarm</div>
    <div class="field">
      <label>Mode Lampu Jam</label>
      <select id="cfgMode">
        <option value="0">Mati</option>
        <option value="1">Putih</option>
        <option value="2">Kuning</option>
        <option value="3">Biru</option>
        <option value="4">Pink</option>
        <option value="5">Pelangi</option>
        <option value="6">Napas RGB</option>
        <option value="7">Ice Blue</option>
        <option value="8">Cyan</option>
        <option value="9">Hijau</option>
        <option value="10">Ungu</option>
        <option value="11">Warm Room</option>
      </select>
    </div>
    <div class="field">
      <label>Nada Alarm (1-5)</label>
      <select id="cfgTone">
        <option value="1">1 - Siren</option>
        <option value="2">2 - Rapid</option>
        <option value="3">3 - Emergency</option>
        <option value="4">4 - Kontinyu</option>
        <option value="5">5 - Triple</option>
      </select>
    </div>
    <div style="display:flex;gap:8px;flex-wrap:wrap">
      <button class="btn-set btn-primary" onclick="setLampMode()">Set Mode Lampu</button>
      <button class="btn-set btn-secondary" onclick="setTone()">Set Nada</button>
    </div>
  </div>
</div>

    <div class="settings-section" id="sDevice">
      <div class="set-title">Perangkat</div>
      <div class="set-sub">Nama dan identitas perangkat.</div>
      <div class="set-card">
        <div class="set-card-title">Identitas</div>
        <div class="field">
          <label>Nama Perangkat</label>
          <input type="text" id="cfgDevName" placeholder="SmartDrawer" maxlength="31">
          <small>Digunakan sebagai hostname OTA dan label jaringan.</small>
        </div>
      </div>
      <button class="btn-set btn-primary" onclick="saveSystem()">Simpan</button>
    </div>

    <div class="settings-section" id="sWifi">
      <div class="set-title">Koneksi WiFi</div>
      <div class="set-sub">Hubungkan ke jaringan WiFi rumah / kantor.</div>
      <div class="set-card">
        <div class="set-card-title">Jaringan WiFi</div>
        <div class="field">
          <label>Nama Jaringan (SSID)</label>
          <input type="text" id="cfgSSID" placeholder="Nama WiFi">
        </div>
        <div class="field">
          <label>Password</label>
          <input type="password" id="cfgPass" placeholder="Password WiFi">
        </div>
        <button class="btn-set btn-secondary" style="margin-bottom:10px" onclick="scanWifi()">⟳ Scan Jaringan</button>
        <div class="wifi-list" id="wifiList"></div>
      </div>
      <button class="btn-set btn-primary" onclick="saveWifi()">Simpan & Restart</button>
      <div style="background:#1a2a1a;border:1px solid #2d4a2d;border-radius:4px;padding:10px 12px;margin-top:10px">
        <div style="font-family:var(--mono);font-size:.6rem;color:#4ade80;margin-bottom:4px">ℹ Setelah restart:</div>
        <div style="font-family:var(--mono);font-size:.6rem;color:#666;line-height:1.7">
          • Jika WiFi berhasil → akses via <span style="color:#e8ff00" id="hintHostname">smartdrawer.local</span><br>
          • Jika WiFi gagal → AP tetap aktif, akses via 192.168.4.1
        </div>
      </div>
    </div>

    <div class="settings-section" id="sAP">
      <div class="set-title">Access Point</div>
      <div class="set-sub">Jaringan hotspot bawaan perangkat. Selalu aktif sebagai fallback.</div>
      <div class="set-card">
        <div class="set-card-title">Konfigurasi AP</div>
        <div class="field">
          <label>Nama Hotspot (SSID)</label>
          <input type="text" id="cfgApSSID" placeholder="SmartDrawer" maxlength="31">
        </div>
        <div class="field">
          <label>Password Hotspot</label>
          <input type="password" id="cfgApPass" placeholder="Kosongkan = tanpa password">
          <small>Minimal 8 karakter, atau kosongkan untuk open network.</small>
        </div>
      </div>
      <button class="btn-set btn-primary" onclick="saveAP()">Simpan & Restart</button>
    </div>

    <div class="settings-section" id="sLED">
      <div class="set-title">Konfigurasi LED</div>
      <div class="set-sub">Sesuaikan hardware LED strip yang terpasang.</div>
      <div class="set-card">
        <div class="set-card-title">Hardware</div>
        <div class="field">
          <label>Jumlah LED</label>
          <input type="number" id="cfgLedCount" placeholder="25" min="1" max="500">
          <small>Sesuaikan dengan panjang strip LED yang terpasang.</small>
        </div>
        <div class="field">
          <label>Pin Data LED</label>
          <input type="number" id="cfgLedPin" placeholder="14" min="0" max="39">
          <small>GPIO pin ESP32 yang terhubung ke data line LED strip.</small>
        </div>
      </div>
      <button class="btn-set btn-primary" onclick="saveLED()">Simpan</button>
      <small style="font-family:var(--mono);font-size:.6rem;color:#555;margin-top:8px;display:block">Perubahan jumlah LED & pin memerlukan restart manual.</small>
    </div>

    <div class="settings-section" id="sData">
      <div class="set-title">Data Inventori</div>
      <div class="set-sub">Backup, restore, dan manajemen data komponen.</div>
      <div class="set-card">
        <div class="set-card-title">Export</div>
        <p style="font-family:var(--mono);font-size:.65rem;color:var(--dim);margin-bottom:12px">Download backup seluruh data inventori dalam format JSON.</p>
        <div style="display:flex;gap:8px">
          <button class="btn-set btn-secondary" onclick="openExport()">Export JSON</button>
          <button class="btn-set btn-secondary" onclick="dlExport()">⬇ Download</button>
        </div>
      </div>
      <div class="set-card">
        <div class="set-card-title">Import</div>
        <p style="font-family:var(--mono);font-size:.65rem;color:var(--dim);margin-bottom:12px">Restore data dari file backup JSON. Data yang ada akan diganti.</p>
        <button class="btn-set btn-secondary" onclick="openImport()">Import JSON</button>
      </div>
    </div>

    <div class="settings-section" id="sDanger">
      <div class="set-title">Reset & Restart</div>
      <div class="set-sub">Tindakan yang tidak dapat dibatalkan.</div>
      <div class="set-card">
        <div class="set-card-title">Restart Perangkat</div>
        <p style="font-family:var(--mono);font-size:.65rem;color:var(--dim);margin-bottom:12px">Restart ESP32 tanpa menghapus data apapun.</p>
        <button class="btn-set btn-secondary" onclick="doRestart()">↺ Restart</button>
      </div>
      <div class="set-card" style="border-color:#7f1d1d">
        <div class="set-card-title" style="color:var(--red)">Factory Reset</div>
        <p style="font-family:var(--mono);font-size:.65rem;color:var(--dim);margin-bottom:12px">Hapus semua konfigurasi dan data inventori. Perangkat kembali ke setelan pabrik.</p>
        <button class="btn-set btn-danger" onclick="doFactory()">⚠ Factory Reset</button>
      </div>
    </div>

  </div>
</div>

<div class="overlay" id="mExport">
  <div class="modal">
    <h3>Export Data</h3><p>Salin atau download backup inventori.</p>
    <textarea id="expTxt" readonly></textarea>
    <div class="modal-btns">
      <button class="mb-p" onclick="copyExp()">Salin</button>
      <button class="mb-s" onclick="dlExport()">Download</button>
      <button class="mb-s" onclick="closeM('mExport')">Tutup</button>
    </div>
  </div>
</div>
<div class="overlay" id="mImport">
  <div class="modal">
    <h3>Import Data</h3><p>Paste JSON inventori. Data lama akan diganti.</p>
    <textarea id="impTxt" placeholder='[{"nama":"Resistor","laci":1,"stok":100}]'></textarea>
    <div class="modal-btns">
      <button class="mb-p" onclick="doImport()">Import</button>
      <button class="mb-s" onclick="closeM('mImport')">Batal</button>
    </div>
  </div>
</div>

<div id="toast"></div>

<script>
const ip = () => window.location.hostname;
const $ = id => document.getElementById(id);
const post = (url, d) => fetch(`http://${ip()}${url}`,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(d)});

let inventori = [], cfgData = {};
let rtcLock = false;   // 🔥 DARI PROGRAM 1

async function loadRTC(){
  if (rtcLock) return;
  try{
    const res = await fetch("/api/rtc");
    if (!res.ok) throw new Error();
    const d = await res.json();

    $('rtcTimeView').textContent  = d.time  || '--:--:--';
    $('rtcAlarmView').textContent = d.alarm || '--:--';
    if ($('rtcAlarmOnView')) $('rtcAlarmOnView').textContent = d.alarmon ? 'ON' : 'OFF';
    if ($('rtcModeView')) $('rtcModeView').textContent = modeNames[d.mode] || ('Mode ' + d.mode);

    const alarmInput = $('setAlarmRTC');
    if (alarmInput && d.alarm && d.alarm !== '--:--' && document.activeElement !== alarmInput && !rtcLock) {
      alarmInput.value = d.alarm;
    }

    const alarmOnSelect = $('cfgAlarmOn');
    if (!rtcLock && alarmOnSelect && document.activeElement !== alarmOnSelect) {
      alarmOnSelect.value = d.alarmon ? '1' : '0';
    }

    const modeSelect = $('cfgMode');
    if (modeSelect && d.mode != null && document.activeElement !== modeSelect && !rtcLock) {
      modeSelect.value = String(d.mode);
    }

    const toneSelect = $('cfgTone');
    if (toneSelect && d.tone != null && document.activeElement !== toneSelect && !rtcLock) {
      toneSelect.value = String(d.tone);
    }

  } catch(e){
    $('rtcTimeView').textContent = 'OFFLINE';
  }
}

const modeNames = ['Mati','Putih','Kuning','Biru','Pink','Pelangi','Napas RGB','Ice Blue','Cyan','Hijau','Ungu','Warm Room'];

async function setRTC(){
  const t = $('setTimeRTC').value; 
  if (!t) return toast('⚠ Pilih waktu dulu', true);
  try {
    await fetch('/api/settime',{
      method:'POST',
      headers:{'Content-Type':'application/x-www-form-urlencoded'},
      body:'t='+encodeURIComponent(t)
    });
    toast('✓ Jam berhasil diset ke ' + t);
  } catch { toast('✕ Gagal kirim', true); }
}

async function setAlarm(){
  const a = $('setAlarmRTC').value;
  if (!a) return toast('⚠ Pilih waktu alarm dulu', true);

  rtcLock = true;

  await fetch('/api/setalarm',{
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'a='+encodeURIComponent(a)
  });

  toast('✓ Alarm diset');

  setTimeout(()=>{
    rtcLock = false;
    loadRTC();
  },1500);
}

async function setAlarmOn(){
  rtcLock = true;
  const val = $('cfgAlarmOn').value;  
  await fetch('/api/setalarmon',{
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'on='+encodeURIComponent(val)
  });
  toast('✓ Status alarm diubah');
  setTimeout(()=>{
    rtcLock = false;
  },3000);
}

async function setLampMode(){
  rtcLock = true;
  const m = $('cfgMode').value;
  await fetch('/api/setmode',{
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'m='+m
  });
  toast('✓ Mode diubah');
  setTimeout(()=>{
    rtcLock = false;
  },1000);
}

async function setTone(){
  rtcLock = true;
  const t = $('cfgTone').value;
  await fetch('/api/settone',{
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'t='+t
  });
  toast('✓ Nada diubah');
  setTimeout(()=>{
    rtcLock = false;
  },1000);
}

setInterval(loadRTC,1000);

// FUNGSI MOBILE NAV & SIDEBAR DARI PROGRAM 2
function showPage(p) {
  ['Main','Settings'].forEach(x => {
    $('pg'+x).classList.toggle('active', x===p);
    $('tab'+x).classList.toggle('active', x===p);
  });
  if (p==='Settings') loadCfg();
  closeSidebar(); 
  const fab = $('mobCtrlBtn');
  if (fab) fab.style.display = (p==='Main') ? '' : 'none';
}

function toggleSidebar() {
  const sb  = document.querySelector('.sidebar');
  const ov  = $('mobOverlay');
  const fab = $('mobCtrlBtn');
  const isOpen = sb.classList.contains('mob-open');
  if (isOpen) {
    sb.classList.remove('mob-open');
    ov.classList.remove('mob-open');
    fab.textContent = '⚡';
  } else {
    sb.classList.add('mob-open');
    ov.classList.add('mob-open');
    fab.textContent = '✕';
  }
}
function closeSidebar() {
  const sb  = document.querySelector('.sidebar');
  const ov  = $('mobOverlay');
  const fab = $('mobCtrlBtn');
  if (sb)  sb.classList.remove('mob-open');
  if (ov)  ov.classList.remove('mob-open');
  if (fab) fab.textContent = '⚡';
}

function showSet(id) {
  document.querySelectorAll('.settings-section').forEach(s=>s.classList.remove('active'));
  document.querySelectorAll('.sn-item').forEach(s=>s.classList.remove('active'));
  $(id).classList.add('active');
  const navMap={sStatus:'snStatus',sDevice:'snDevice',sRTC:'snRTC',sWifi:'snWifi',sAP:'snAP',sLED:'snLED',sData:'snData',sDanger:'snDanger'};
  if(navMap[id]&&$(navMap[id]))$(navMap[id]).classList.add('active');
}

document.addEventListener('DOMContentLoaded', () => {
  initWLED();
  loadData();
  loadRTC();
  $('searchBox').oninput = render;
  $('col1').oninput = sendState;
  $('fx').onchange  = sendState;
  $('bri').oninput  = () => { $('briV').textContent=$('bri').value; sendState(); };
  $('sx').oninput   = () => { $('sxV').textContent=$('sx').value;   sendState(); };
  document.querySelectorAll('.overlay').forEach(el=>el.addEventListener('click',e=>{if(e.target===el)el.classList.remove('open')}));
});

async function initWLED() {
  try {
    const [fe,fs] = await Promise.all([
      fetch(`http://${ip()}/json/eff`),
      fetch(`http://${ip()}/json/state`)
    ]);
    const [fx,st] = await Promise.all([fe.json(),fs.json()]);
    $('fx').innerHTML  = fx.map((n,i)  => `<option value="${i}">${n}</option>`).join('');
    syncUI(st);
  } catch { setOffline(); }
}

function syncUI(st) {
  if (!st) return;
  if (st.bri!=null) { $('bri').value=st.bri; $('briV').textContent=st.bri; }
  const s=st.seg?.[0];
  if (s) {
    if (s.sx!=null)  { $('sx').value=s.sx;  $('sxV').textContent=s.sx; }
    if (s.fx!=null)    $('fx').value=s.fx;
    if (s.col?.[0]) {
      const [r,g,b]=s.col[0];
      $('col1').value='#'+[r,g,b].map(v=>v.toString(16).padStart(2,'0')).join('');
    }
  }
  setOnline(st.on);
}

function setOnline(on) {
  $('sDot').className=on?'on':'';
  $('sLbl').textContent=on?'ON':'OFF';
}
function setOffline() { $('sDot').className=''; $('sLbl').textContent='OFFLINE'; }

const deb=(fn,ms)=>{let t;return(...a)=>{clearTimeout(t);t=setTimeout(()=>fn(...a),ms);}};
const sendState=deb(()=>{
  const c=$('col1').value;
  const r=parseInt(c.slice(1,3),16),g=parseInt(c.slice(3,5),16),b=parseInt(c.slice(5,7),16);
  post('/json/state',{on:true,bri:+$('bri').value,seg:[{fx:+$('fx').value,sx:+$('sx').value,col:[[r,g,b]]}]}).catch(()=>{});
},200);

function pwrToggle(on){post('/json/state',{on}).then(()=>setOnline(on)).catch(()=>{});}

function cariBarang(laci){
  const i = parseInt(laci)-1;
  post('/json/state',{seg:[{start:i,stop:i+1}]})
  .catch(()=>toast('✕ Search gagal',true));
  toast('🔍 Laci '+laci);
}

async function loadData() {
  try {
    const r=await fetch(`http://${ip()}/data.json`);
    if (r.ok) { inventori=await r.json(); render(); updateStats(); }
  } catch {}
}

async function saveData() {
  try {
    const r=await post('/save',inventori);
    toast(r.ok?'✓ Tersimpan':'✕ Gagal simpan',!r.ok);
  } catch { toast('✕ Koneksi error',true); }
  render(); updateStats();
}

function updateStats() {
  $('stTotal').textContent=inventori.length;
  $('stStok').textContent=inventori.reduce((s,i)=>s+(i.stok||0),0);
  $('stLaci').textContent=new Set(inventori.map(i=>i.laci)).size;
}

function render() {
  const kw=$('searchBox').value.toLowerCase();
  let list = inventori
    .filter(i => i.nama.toLowerCase().includes(kw))
    .sort((a,b) => Number(a.laci) - Number(b.laci));
  if (!list.length) {
    $('grid').innerHTML='<div class="empty"><div class="empty-ico">📦</div><p>Tidak ada komponen.<br>Tambah komponen baru di sidebar.</p></div>';
    return;
  }
  $('grid').innerHTML=list.map(item=>{
    const img=item.img||"data:image/svg+xml;charset=utf-8,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 200 116'%3E%3Crect width='200' height='116' fill='%23f5f0e8'/%3E%3Ctext x='100' y='58' fill='%23888' font-family='sans-serif' font-weight='bold' text-anchor='middle' dominant-baseline='middle'%3ENo%20Image%3C/text%3E%3C/svg%3E";
    return `<div class="card">
      <button class="btn-del" onclick="hapus(${item.id})">✕</button>
      <img class="card-img" src="${img}" loading="lazy" onerror="this.src='data:image/svg+xml;charset=utf-8,%3Csvg xmlns=\\'http://www.w3.org/2000/svg\\' viewBox=\\'0 0 200 116\\'%3E%3Crect width=\\'200\\' height=\\'116\\' fill=\\'%23f5f0e8\\'/%3E%3Ctext x=\\'100\\' y=\\'58\\' fill=\\'%23888\\' font-family=\\'sans-serif\\' font-weight=\\'bold\\' text-anchor=\\'middle\\' dominant-baseline=\\'middle\\'%3EError%3C/text%3E%3C/svg%3E'">
      <div class="card-body">
        <div class="card-name" title="${item.nama}">${item.nama}</div>
        <div class="card-tags">
          <span class="tag tag-laci">Laci ${item.laci}</span>
        </div>
        <div class="stok-row">
          <button class="stk-btn" onclick="ubahStok(${item.id},-1)">−</button>
          <span class="stok-val">${item.stok||0}</span>
          <button class="stk-btn" onclick="ubahStok(${item.id},1)">+</button>
        </div>
        <button class="btn-cari" onclick="cariBarang(${item.laci})">▶ CARI LACI</button>
      </div></div>`;
  }).join('');
}

function addBarang() {
  const nama=$('nNama').value.trim(), laci=parseInt($('nLaci').value);
  const stok=parseInt($('nStok').value)||0, img=$('nImg').value.trim();
  if (!nama) return toast('⚠ Nama wajib diisi',true);
  if (!laci||laci<1) return toast('⚠ Nomor laci tidak valid',true);
  inventori.push({id:Date.now(),nama,laci,stok,img});
  inventori.sort((a,b) => Number(a.laci) - Number(b.laci));
  ['nNama','nLaci','nStok','nImg'].forEach(id=>$(id).value='');
  saveData();
  if (window.innerWidth <= 768) closeSidebar();
}

function ubahStok(id,d){const i=inventori.find(x=>x.id===id);if(i){i.stok=Math.max(0,(i.stok||0)+d);saveData();}}
function hapus(id){if(!confirm('Hapus komponen ini?'))return;inventori=inventori.filter(i=>i.id!==id);saveData();}

async function loadCfg() {
  try {
    const r=await fetch(`http://${ip()}/api/config`);
    if (!r.ok) return;
    cfgData=await r.json();
    $('iHostname').textContent='http://'+cfgData.hostname;
    if($('hintHostname')) $('hintHostname').textContent=cfgData.hostname;
    $('iStaStatus').textContent=cfgData.staOK?'Terhubung':'Tidak Terhubung';
    $('iStaStatus').className='status-val '+(cfgData.staOK?'badge-ok':'badge-err');
    $('iStaIP').textContent=cfgData.staIP||'—';
    $('iApStatus').textContent=cfgData.apActive?'Aktif':'Mati (STA terhubung)';
    $('iApStatus').className='status-val '+(cfgData.apActive?'badge-warn':'badge-ok');
    $('iApIP').textContent=cfgData.apActive?(cfgData.apIP||'—'):'—';
    $('iSSID').textContent=cfgData.ssid||'(belum diatur)';
    $('iRSSI').textContent=cfgData.staOK?(cfgData.rssi+' dBm'):'—';
    $('iHeap').textContent=Math.round(cfgData.freeHeap/1024);
    $('iChip').textContent=cfgData.chipModel||'—';
    $('iFlash').textContent=cfgData.flashMB||'—';
    $('iLedCnt').textContent=cfgData.ledCount||'—';
    $('cfgDevName').value=cfgData.devName||'';
    $('cfgSSID').value=cfgData.ssid||'';
    $('cfgApSSID').value=cfgData.apSSID||'';
    $('cfgApPass').value=cfgData.apPass||'';
    $('cfgLedCount').value=cfgData.ledCount||25;
    $('cfgLedPin').value=cfgData.ledPin||14;
  } catch { toast('Gagal load config',true); }
}

async function saveSystem() {
  const newName = $('cfgDevName').value.trim();
  if (!newName) return toast('⚠ Nama tidak boleh kosong', true);
  const nameChanged = newName.toLowerCase().replace(/[^a-z0-9]/g,'-') !== (cfgData.devName||'').toLowerCase().replace(/[^a-z0-9]/g,'-');
  if (nameChanged && !confirm('Nama perangkat berubah → hostname .local akan update.\nPerangkat akan restart. Lanjutkan?')) return;
  try {
    const r = await post('/api/config/system', { devName: newName });
    const j = await r.json();
    if (j.success) {
      if (j.restart) {
        toast('✓ Disimpan. Restarting... tunggu ~5 detik');
      } else {
        toast('✓ Tersimpan');
        loadCfg();
      }
    } else { toast('✕ Gagal simpan', true); }
  } catch { toast('✕ Koneksi error', true); }
}

async function saveWifi() {
  if (!confirm('Perangkat akan restart. Lanjutkan?')) return;
  const r=await post('/api/config/wifi',{
    ssid:$('cfgSSID').value,
    pass:$('cfgPass').value
  });
  if (r.ok) toast('Restarting...');
}

async function saveAP() {
  if (!confirm('Perangkat akan restart. Lanjutkan?')) return;
  const r=await post('/api/config/wifi',{
    apSSID:$('cfgApSSID').value,
    apPass:$('cfgApPass').value
  });
  if (r.ok) toast('Restarting...');
}

async function saveLED() {
  try {
    const r = await post('/api/config/system',{
      ledCount: parseInt($('cfgLedCount').value)||25,
      ledPin:   parseInt($('cfgLedPin').value)||14
    });
    const j = await r.json();
    toast(j.success ? '✓ Tersimpan' : '✕ Gagal', !j.success);
    if (j.success) loadCfg();
  } catch { toast('✕ Koneksi error', true); }
}

async function scanWifi() {
  $('wifiList').innerHTML='<div style="font-family:var(--mono);font-size:.65rem;color:var(--dim);padding:8px">Scanning...</div>';
  // Retry loop untuk async scan — ESP32 scan di background
  for (let attempt = 0; attempt < 8; attempt++) {
    try {
      const r=await fetch(`http://${ip()}/api/wifi/scan`);
      if (r.status === 202) {
        // masih scanning, tunggu 2 detik lalu retry
        await new Promise(res => setTimeout(res, 2000));
        continue;
      }
      const nets=await r.json();
      if (!nets.length) { $('wifiList').innerHTML='<div style="font-family:var(--mono);font-size:.65rem;color:var(--dim);padding:8px">Tidak ada jaringan ditemukan.</div>'; return; }
      $('wifiList').innerHTML=nets.map(n=>`
        <div class="wifi-item" onclick="selectWifi('${n.ssid}')">
          <span class="wifi-ssid">${n.ssid}</span>
          <span class="wifi-rssi">${n.rssi} dBm ${n.secure?'<span class="wifi-lock">🔒</span>':''}</span>
        </div>`).join('');
      return;
    } catch { $('wifiList').innerHTML=''; toast('Scan gagal',true); return; }
  }
  toast('Scan timeout, coba lagi',true);
}

function selectWifi(ssid) { $('cfgSSID').value=ssid; $('cfgPass').focus(); }

async function doRestart() {
  if (!confirm('Restart perangkat?')) return;
  await post('/api/restart',{});
  toast('Restarting...');
}

async function doFactory() {
  if (!confirm('⚠ Factory reset? Semua data akan dihapus!')) return;
  if (!confirm('Yakin? Tindakan ini tidak dapat dibatalkan.')) return;
  await post('/api/factory',{});
  toast('Factory reset...');
}

function openExport(){$('expTxt').value=JSON.stringify(inventori,null,2);$('mExport').classList.add('open');}
function copyExp(){navigator.clipboard.writeText($('expTxt').value).then(()=>toast('✓ Disalin'));}
function dlExport(){const a=document.createElement('a');a.href=URL.createObjectURL(new Blob([$('expTxt').value||JSON.stringify(inventori)],{type:'application/json'}));a.download='smart_drawer_backup.json';a.click();}
function openImport(){$('impTxt').value='';$('mImport').classList.add('open');}
function doImport(){
  try{
    const d=JSON.parse($('impTxt').value);
    if(!Array.isArray(d))throw 0;
    inventori=d.map(x=>({id:x.id||Date.now()+Math.random(),nama:x.nama||'?',laci:x.laci||1,stok:x.stok||0,img:x.img||''}));
    closeM('mImport');saveData();toast('✓ '+inventori.length+' item diimport');
  }catch{toast('✕ Format JSON tidak valid',true);}
}
function closeM(id){$(id).classList.remove('open');}

let tt;
function toast(msg,err=false){
  const t=$('toast');t.textContent=msg;
  t.className='show'+(err?' err':'');
  clearTimeout(tt);tt=setTimeout(()=>t.className='',3000);
}

// Fix: pakai AbortController supaya fetch lama tidak numpuk-numpuk
let _pollCtrl = null;
setInterval(async()=>{
  if (_pollCtrl) _pollCtrl.abort(); // batalkan fetch sebelumnya kalau belum selesai
  _pollCtrl = new AbortController();
  try{
    const r=await fetch(`http://${ip()}/json/state`,{signal:_pollCtrl.signal,cache:'no-store'});
    if(r.ok)setOnline((await r.json()).on);else setOffline();
  }catch(e){if(e.name!=='AbortError')setOffline();}
},5000);
</script>
</body>
</html>
)rawliteral";