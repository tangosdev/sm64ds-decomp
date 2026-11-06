#!/usr/bin/env python3
"""Particle lab: a sandbox for exploring, cataloguing and naming the 321 SPA effects.

Exploration tooling, not evidence by itself and not part of any match.  Everything
it writes goes to a work directory OUTSIDE the repository (default C:/tmp/ptcl-lab),
because the shots are screenshots of the game and must not be committed.

  shoot    spawn effects beside the player in melonDS and record each as a GIF.
           A no-spawn baseline run of the same frames is diffed against every shot,
           so "draws nothing here" is measured (visible_pixels), not eyeballed.
  catalog  merge the table decode, call-site map, source names, shots and your
           labels into <work>/catalog.json
  gallery  write <work>/index.html: filter by texture/draw mode/shape/named/called,
           search, open an effect, type a name, export labels.json

  python tools/particle_lab.py shoot --range 0 321            # resumable; skips done ids
  python tools/particle_lab.py shoot 0x9c 0xa4 --force
  python tools/particle_lab.py catalog && python tools/particle_lab.py gallery

How a spawn works: func_0200fc44 (arm9 0x0200fc44) is the landing helper; its
`mov r0,#0xb3` at 0x0200fca4 feeds NewSimple.  The lab patches that instruction (and
0x0200fca8 for ids >= 256), stops at 0x0200fc9c after a jump, moves the position the
helper is about to use, and lets it run.  Reloading the savestate undoes every patch;
the lab then reads the original word back and refuses to finish if it differs.

Limits, so a blank row is not over-read: effects that need an actor handle or a
callback (e.g. 0xa6) draw nothing through NewSimple; one placement and one camera;
one savestate.  A shot says what an effect looked like HERE, not what it is for.

Needs: the melonDS AI controller fork running (tools/trace/ask.py serve --backend ctl,
or its start_control.ps1) with a Bob-omb Battlefield savestate; Pillow.
"""
import argparse
import hashlib
import html
import json
import os
import struct
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "trace"))
import spa_dump  # noqa: E402

DEFAULT_WORK = "C:/tmp/ptcl-lab"
DEFAULT_STATE = "C:/tmp/melonds-ai-controller/work/library/states/bob_king_bobomb_fight.ml"
ARM9 = os.path.join(HERE, "..", "extracted", "arm9_dec.bin")

# func_0200fc44 internals (see the module docstring)
MOV_ID = 0x0200FCA4        # mov r0,#0xb3   (0xE3A000B3)
ADD_Y = 0x0200FCA8         # add r1,r1,#0x19000 (+25.0 up)
HOOK = 0x0200FC9C          # stop here: r4 -> the Vector3 about to be used
CALL = 0x0200FCBC          # bl NewSimple
ORIG_MOV = 0xE3A000B3
NOP = 0xE1A00000
SHOT_BOX = (60, 20, 230, 110)   # crop of the top screen around the player
# Fix12 offsets from the helper's own spawn point (the player's feet, already +25.0 up).
# "beside" is the default view; the others change what scale/billboarding/gravity show.
PLACES = {
    "beside": (-0x50000, 0x30000, 0),       # head height, one body-width to the left of the player
    "feet": (0, -0x19000, 0),                # on the ground at the player's feet (helper's own point, un-lifted)
    "air": (-0x50000, 0x90000, 0),           # high above the ground, nothing behind it but sky/grass
    "far": (-0xB0000, 0x30000, 0),           # further out: shows scale and spread
}


def word(v):
    return struct.pack("<I", v)


def connect():
    import ctlclient
    c = ctlclient.CtlClient().connect()
    return c


def spawn_frames(c, state, effect_id, offset, nframes, stride, box, nop_call=False):
    """Run one spawn from a fresh state; return the cropped RGB frames."""
    from PIL import Image
    c.call("load_state", path=os.path.abspath(state), paused=True)
    c.call("breakpoint_clear")
    if not nop_call:
        if effect_id < 256:
            c.write(MOV_ID, word(0xE3A00000 | effect_id))
        else:
            c.write(MOV_ID, word(0xE3A00C01))                          # mov r0,#0x100
            c.write(ADD_Y, word(0xE2800000 | (effect_id - 256)))       # add r0,r0,#rest
            offset = (offset[0], offset[1] + 0x19000, offset[2])       # the add r1 we replaced
    else:
        c.write(CALL, word(NOP))
    c.call("input_sequence", steps=[{"buttons": [], "frames": 10}, {"buttons": ["B"], "frames": 2}], run=True)
    r = c.call("run_until", address=HOOK, max_frames=150)
    if not r.get("hit"):
        raise RuntimeError("the landing helper never ran (jump did not land)")
    regs = c.call("breakpoint_list")["last_breakpoint_hit"]["registers"]
    p = int(regs["r4"], 16)
    x, y, z = struct.unpack("<3i", c.read(p, 12))
    c.write(p, struct.pack("<3i", x + offset[0], y + offset[1], z + offset[2]))
    c.call("breakpoint_remove", address=HOOK)
    c.call("step", frames=1)
    frames = []
    for _ in range(nframes):
        w, h, rgb = c.screen_rgb("top")
        frames.append(Image.frombytes("RGB", (w, h), rgb).crop(box))
        c.call("step", frames=stride)
    return frames


def restored(c, state):
    c.call("load_state", path=os.path.abspath(state), paused=True)
    c.call("clear_input_override")
    return struct.unpack("<I", c.read(MOV_ID, 4))[0] == ORIG_MOV


def cmd_shoot(a):
    from PIL import Image, ImageChops
    work = os.path.abspath(a.work)
    tag = "" if a.place == "beside" else "_" + a.place
    shots = os.path.join(work, "shots" + tag)
    os.makedirs(shots, exist_ok=True)
    idx_path = os.path.join(work, "shots%s.json" % tag)
    idx = json.load(open(idx_path)) if os.path.exists(idx_path) else {}
    ids = list(range(a.range[0], a.range[1])) if a.range else [int(s, 0) for s in a.ids]
    if not ids:
        sys.exit("give ids or --range A B")
    c = connect()
    box = tuple(a.box)
    base_off = PLACES[a.place]
    off = tuple(base_off[k] if v is None else v for k, v in enumerate((a.dx, a.dy, a.dz)))
    meta = {"state": os.path.abspath(a.state), "offset": list(off), "frames": a.frames,
            "stride": a.stride, "box": list(box)}
    # baseline: same frames, no spawn
    bpath = os.path.join(work, "baseline%s.json" % tag)
    if (not os.path.exists(bpath)) or json.load(open(bpath)).get("meta") != meta or a.force:
        base = spawn_frames(c, a.state, 0, off, a.frames, a.stride, box, nop_call=True)
        for i, f in enumerate(base):
            f.save(os.path.join(work, "baseline%s_%02d.png" % (tag, i)))
        json.dump({"meta": meta}, open(bpath, "w"))
    base = [Image.open(os.path.join(work, "baseline%s_%02d.png" % (tag, i))).convert("RGB") for i in range(a.frames)]
    t0 = time.time()
    done = 0
    for n, i in enumerate(ids):
        key = "0x%03x" % i
        gif = os.path.join(shots, key + ".gif")
        if os.path.exists(gif) and idx.get(key, {}).get("meta") == meta and not a.force:
            continue
        fr = spawn_frames(c, a.state, i, off, a.frames, a.stride, box)
        vis = 0
        peak = 0
        for k in range(a.frames):
            d = ImageChops.difference(fr[k], base[k]).convert("L").point(lambda v: 255 if v > 24 else 0)
            cnt = sum(1 for v in d.getdata() if v)
            vis = max(vis, cnt)
            if cnt == vis:
                peak = k
        big = [f.resize((f.width * a.scale, f.height * a.scale), Image.NEAREST) for f in fr]
        big[0].save(gif, save_all=True, append_images=big[1:], duration=a.stride * 17 * 2, loop=0)
        fr[peak].resize((fr[peak].width * a.scale, fr[peak].height * a.scale), Image.NEAREST).save(
            os.path.join(shots, key + ".png"))
        idx[key] = {"meta": meta, "visible_pixels": vis, "peak_frame": peak, "shot_at": time.strftime("%Y-%m-%d %H:%M:%S")}
        json.dump(idx, open(idx_path, "w"), indent=1)
        done += 1
        print("%s  visible_pixels=%-6d peak=%d  (%d/%d, %.0fs)" % (key, vis, peak, n + 1, len(ids), time.time() - t0), flush=True)
    ok = restored(c, a.state)
    print("shot %d new effect(s); helper %s" % (done, "restored" if ok else "NOT RESTORED -- reload the state manually"))
    if not ok:
        sys.exit(1)


def source_names():
    """id -> source name, from include/ParticleIds.h when present and from local enums."""
    import re
    out = {}
    root = os.path.join(HERE, "..")
    paths = [os.path.join(root, "include", "ParticleIds.h")]
    for dp, dn, fn in os.walk(os.path.join(root, "src")):
        for f in fn:
            if f.endswith((".c", ".cpp", ".h")):
                paths.append(os.path.join(dp, f))
    rx = re.compile(r"\b(kPtcl\w+|PARTICLE_\w+)\s*=\s*(0[xX][0-9a-fA-F]+|\d+)")
    for p in paths:
        try:
            t = open(p, encoding="utf-8", errors="replace").read()
        except OSError:
            continue
        if "kPtcl" not in t and "PARTICLE_" not in t:
            continue
        for m in rx.finditer(t):
            out.setdefault(int(m.group(2), 0), set()).add(m.group(1))
    return {k: sorted(v) for k, v in out.items()}


def cmd_catalog(a):
    import particle_id_sites as pis
    work = os.path.abspath(a.work)
    table = spa_dump.parse(open(ARM9, "rb").read())
    sites = {}
    for s in pis.scan(os.path.join(HERE, "..", "src")):
        sites.setdefault(s["id"], []).append({k: s[k] for k in ("file", "line", "symbol", "via", "name")})
    shots_p = os.path.join(work, "shots.json")
    shots = json.load(open(shots_p)) if os.path.exists(shots_p) else {}
    lab_p = os.path.join(work, "labels.json")
    labels = json.load(open(lab_p)) if os.path.exists(lab_p) else {}
    names = source_names()
    judged_p = os.path.join(work, "judged.json")
    judged = json.load(open(judged_p)) if os.path.exists(judged_p) else {}
    runtime = {}
    wd = os.path.join(work, "watch")
    for f in sorted(os.listdir(wd)) if os.path.isdir(wd) else []:
        w = json.load(open(os.path.join(wd, f)))
        for k, v in w["effects"].items():
            r = runtime.setdefault(k, {"count": 0, "callers": {}, "watches": []})
            r["count"] += v["count"]
            r["watches"].append(f[:-5])
            for who, n in v["callers"].items():
                r["callers"][who] = r["callers"].get(who, 0) + n
    places = {}
    for f in os.listdir(work):
        if f.startswith("shots_") and f.endswith(".json"):
            places[f[6:-5]] = json.load(open(os.path.join(work, f)))
    cat = []
    for d in table["definitions"]:
        i, key = d["id"], "0x%03x" % d["id"]
        cat.append({"id": i, "key": key, "texture": d["texture"], "optional": d["optional"],
                    "behaviors": d["behaviors"], "flags": "0x%08x" % d["flags"], "decoded": d["decoded"],
                    "records": d.get("records"), "calls": sites.get(i, []), "source_names": names.get(i, []),
                    "shot": shots.get(key), "label": labels.get(key),
                    "judged": judged.get(key), "runtime": runtime.get(key),
                    "places": {pl: v[key] for pl, v in places.items() if key in v}})
    out = {"count": len(cat), "textures": len(table["textures"]), "effects": cat}
    json.dump(out, open(os.path.join(work, "catalog.json"), "w"), indent=1)
    n = lambda f: sum(1 for e in cat if f(e))
    print("catalog: %d effects | %d called by a literal | %d with a source name | %d shot (%d visible) | %d labelled | %d seen at runtime | %d judged" % (
        len(cat), n(lambda e: e["calls"]), n(lambda e: e["source_names"]), n(lambda e: e["shot"]),
        n(lambda e: e["shot"] and e["shot"]["visible_pixels"] > 0), n(lambda e: e["label"]),
        n(lambda e: e["runtime"]), n(lambda e: e["judged"])))

NEWSIMPLE = 0x02022E98     # Particle::System::NewSimple(id, x, y, z)
# A short loop of inputs for `watch`: idle, walk, jump, attack, walk back.  Anything else
# goes in a JSON file: a list of {"buttons": [...], "frames": N}.
DEFAULT_SCRIPT = [
    {"buttons": [], "frames": 30}, {"buttons": ["Right"], "frames": 60}, {"buttons": ["B"], "frames": 2},
    {"buttons": [], "frames": 50}, {"buttons": ["Y"], "frames": 4}, {"buttons": [], "frames": 40},
    {"buttons": ["Left"], "frames": 60}, {"buttons": ["B"], "frames": 2}, {"buttons": [], "frames": 60},
]


def symbol_index():
    """Sorted (addr, size, name) for arm9 functions, from config/arm9/symbols.txt."""
    import re
    rx = re.compile(r"^(\S+) kind:function\((?:arm|thumb),size=(0x[0-9a-fA-F]+)\) addr:(0x[0-9a-fA-F]+)")
    out = []
    try:
        for ln in open(os.path.join(HERE, "..", "config", "arm9", "symbols.txt"), encoding="utf-8"):
            m = rx.match(ln)
            if m:
                out.append((int(m.group(3), 16), int(m.group(2), 16), m.group(1)))
    except OSError:
        pass
    return sorted(out)


def resolve(index, addr):
    """name+offset of the arm9 function containing addr, else None (overlay or unknown)."""
    import bisect
    i = bisect.bisect_right(index, (addr, 1 << 40, "")) - 1
    if i >= 0 and index[i][0] <= addr < index[i][0] + index[i][1]:
        return "%s+0x%x" % (index[i][2], addr - index[i][0])
    return None


def cmd_watch(a):
    """Log every NewSimple the game itself makes while a scripted input plays."""
    work = os.path.abspath(a.work)
    steps = json.load(open(a.script)) if a.script else DEFAULT_SCRIPT
    c = connect()
    c.call("load_state", path=os.path.abspath(a.state), paused=True)
    c.call("breakpoint_clear")
    c.call("breakpoint_add", address=NEWSIMPLE, action="log", registers="args", log_limit=65536)
    c.call("input_sequence", steps=steps, run=True, timeout=600)
    bps = c.call("breakpoint_list", include_log=True)["breakpoints"]
    bp = next(b for b in bps if int(str(b["address"]), 0) == NEWSIMPLE)
    c.call("breakpoint_clear")
    c.call("clear_input_override")
    counts = {}
    sym = symbol_index()
    for h in bp.get("log", []):
        r = h["registers"]
        i = int(r["r0"], 16)
        lr = int(r["r14"], 16)
        who = "0x%08x" % lr
        nm = resolve(sym, lr - 4)
        if nm:
            who += " " + nm
        e = counts.setdefault("0x%03x" % i, {"count": 0, "callers": {}})
        e["count"] += 1
        e["callers"][who] = e["callers"].get(who, 0) + 1
    out = {"state": os.path.abspath(a.state), "frames": sum(x["frames"] for x in steps), "steps": steps,
           "hits": bp.get("log_seen", len(bp.get("log", []))), "effects": counts}
    os.makedirs(os.path.join(work, "watch"), exist_ok=True)
    path = os.path.join(work, "watch", "%s.json" % a.name)
    json.dump(out, open(path, "w"), indent=1)
    print("watch %s: %d NewSimple hit(s), %d distinct effect(s) over %d frames -> %s" % (
        a.name, out["hits"], len(counts), out["frames"], path))
    for k, v in sorted(counts.items(), key=lambda kv: -kv[1]["count"])[:25]:
        print("  %s x%-4d from %s" % (k, v["count"], ", ".join("%s(%d)" % kv for kv in sorted(v["callers"].items()))))
    if not restored(c, a.state):
        sys.exit("helper not restored (load the state again)")


TAGS = ["fire", "smoke", "dust/puff", "cloud", "sparkle", "star", "ring/shockwave", "splash/water", "trail",
        "glow", "debris/chunks", "electric", "text/number", "other", "nothing visible"]
QUIZ = r"""<!doctype html><meta charset=utf-8><title>Particle quiz</title>
<style>body{font:14px system-ui;background:#1b1d22;color:#ddd;margin:0;padding:16px;max-width:760px}
img{width:100%;image-rendering:pixelated;background:#444}label{display:inline-block;margin:3px 10px 3px 0}
.q{background:#23262d;border:1px solid #333;padding:10px;margin:12px 0}input[type=text]{width:100%;background:#2c3038;color:#ddd;border:1px solid #444}
button{background:#2c3038;color:#ddd;border:1px solid #444;padding:6px 12px}</style>
<h2>Particle quiz</h2><p>Each clip is one effect beside the player. Tick every tag that fits what you SEE, add words if you like.
Some clips repeat. Nothing here says what an effect is for.</p><div id=qs></div><button id=ex>export answers.json</button>
<script>
const Q=__Q__,TAGS=__TAGS__;let A=JSON.parse(localStorage.getItem('ptcl-quiz-__SEED__')||'{}');
const save=()=>localStorage.setItem('ptcl-quiz-__SEED__',JSON.stringify(A));
Q.forEach((code,n)=>{const d=document.createElement('div');d.className='q';
 d.innerHTML=`<b>${n+1}/${Q.length}</b><img src="g/${code}.gif">`+TAGS.map(t=>`<label><input type=checkbox data-c=${code} value="${t}" ${(A[code]&&A[code].tags.includes(t))?'checked':''}>${t}</label>`).join('')+
 `<input type=text placeholder="in your words" data-t=${code} value="${(A[code]&&A[code].text)||''}">`;document.getElementById('qs').append(d)});
document.addEventListener('input',e=>{const c=e.target.dataset.c||e.target.dataset.t;if(!c)return;
 const box=[...document.querySelectorAll(`[data-c=${c}]`)].filter(x=>x.checked).map(x=>x.value);
 A[c]={tags:box,text:document.querySelector(`[data-t=${c}]`).value};save()});
document.getElementById('ex').onclick=()=>{const a=document.createElement('a');a.href=URL.createObjectURL(new Blob([JSON.stringify(A,null,1)],{type:'application/json'}));a.download='answers.json';a.click()};
</script>"""


def cmd_quiz(a):
    """Blind round: shuffled clips under random codes; the key stays outside the quiz folder."""
    import random
    import shutil
    work = os.path.abspath(a.work)
    shots = json.load(open(os.path.join(work, "shots.json")))
    pool = sorted(k for k, v in shots.items() if v["visible_pixels"] > 0)
    if a.ids:
        pool = [("0x%03x" % int(x, 0)) for x in a.ids]
    rnd = random.Random(a.seed)
    pick = rnd.sample(pool, min(a.n, len(pool))) if not a.ids else pool
    reps = rnd.sample(pick, max(1, len(pick) // 8)) if len(pick) > 4 else []
    seq = pick + reps
    rnd.shuffle(seq)
    qdir = os.path.join(work, "quiz_%s" % a.seed)
    shutil.rmtree(qdir, ignore_errors=True)
    os.makedirs(os.path.join(qdir, "g"))
    key, codes = {}, []
    for k in seq:
        code = hashlib.sha1(("%s%s%d" % (a.seed, k, len(codes))).encode()).hexdigest()[:8]
        shutil.copy(os.path.join(work, "shots", k + ".gif"), os.path.join(qdir, "g", code + ".gif"))
        key[code] = k
        codes.append(code)
    page = QUIZ.replace("__Q__", json.dumps(codes)).replace("__TAGS__", json.dumps(TAGS)).replace("__SEED__", str(a.seed))
    open(os.path.join(qdir, "index.html"), "w", encoding="utf-8").write(page)
    json.dump(key, open(os.path.join(work, "quiz_%s_key.json" % a.seed), "w"), indent=1)
    print("quiz %s: %d clips (%d repeats) -> %s" % (a.seed, len(codes), len(reps), os.path.join(qdir, "index.html")))
    print("key (do not open until scored): %s" % os.path.join(work, "quiz_%s_key.json" % a.seed))


def cmd_quizscore(a):
    """Unblind: per-effect tags, and how consistently the repeated clips were tagged."""
    work = os.path.abspath(a.work)
    key = json.load(open(os.path.join(work, "quiz_%s_key.json" % a.seed)))
    ans = json.load(open(a.answers))
    per = {}
    for code, k in key.items():
        if code in ans and (ans[code]["tags"] or ans[code]["text"]):
            per.setdefault(k, []).append(ans[code])
    jp = os.path.join(work, "judged.json")
    judged = json.load(open(jp)) if os.path.exists(jp) else {}
    agree = []
    for k, rs in per.items():
        tags = sorted({t for r in rs for t in r["tags"]})
        if len(rs) > 1:
            a1, a2 = set(rs[0]["tags"]), set(rs[1]["tags"])
            agree.append(len(a1 & a2) / max(1, len(a1 | a2)))
        judged.setdefault(k, {"rounds": []})["rounds"].append(
            {"seed": a.seed, "tags": tags, "text": [r["text"] for r in rs if r["text"]]})
        judged[k]["tags"] = sorted({t for rd in judged[k]["rounds"] for t in rd["tags"]})
    json.dump(judged, open(jp, "w"), indent=1)
    cons = ("%.0f%% over %d repeats (100%% = identical tags both times)" % (100 * sum(agree) / len(agree), len(agree))
            if agree else "n/a (no repeats answered)")
    print("scored %d effects from quiz %s; repeat-consistency %s" % (len(per), a.seed, cons))
    for k in sorted(per):
        print("  %s  %s" % (k, ", ".join(judged[k]["tags"]) or "-"))



GALLERY = r"""<!doctype html><meta charset=utf-8><title>Particle lab</title>
<style>
body{font:13px system-ui;margin:0;background:#1b1d22;color:#ddd}
header{position:sticky;top:0;background:#23262d;padding:8px 12px;display:flex;gap:10px;flex-wrap:wrap;align-items:center;z-index:2}
select,input,button{background:#2c3038;color:#ddd;border:1px solid #444;padding:3px 6px}
#grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(190px,1fr));gap:8px;padding:10px}
.c{background:#23262d;border:1px solid #333;cursor:pointer;padding:4px}
.c.sel{border-color:#fc0}.c img{width:100%;image-rendering:pixelated;display:block;background:#444}
.c b{color:#fc0}.n{color:#7d7}.u{color:#888}.bl{color:#c77}
#d{position:fixed;right:0;top:0;bottom:0;width:480px;background:#23262d;border-left:1px solid #444;padding:12px;overflow:auto;display:none;z-index:3}
#d img{width:100%;image-rendering:pixelated}pre{white-space:pre-wrap;font-size:11px;color:#9ab}
</style>
<header>
<input id=q placeholder="search id / name / file" size=22>
<select id=tex><option value="">texture: any</option></select>
<select id=draw><option value="">draw mode: any</option></select>
<select id=shape><option value="">shape: any</option></select>
<select id=st><option value="">status: any</option><option>unlabelled</option><option>labelled</option><option>source-named</option><option>called</option><option>never-called</option><option>draws-nothing</option></select>
<span id=cnt></span><button id=exp>export labels.json</button><button id=imp>import</button>
</header>
<div id=grid></div>
<div id=d></div>
<script>
const CAT=__CATALOG__;
const LS='ptcl-labels';let L=JSON.parse(localStorage.getItem(LS)||'{}');
for(const e of CAT.effects){if(e.label&&!L[e.key])L[e.key]=e.label}
const E=CAT.effects;const $=s=>document.querySelector(s);
const opt=(el,vals)=>[...new Set(vals)].sort((a,b)=>a-b).forEach(v=>{const o=document.createElement('option');o.textContent=v;el.append(o)});
opt($('#tex'),E.map(e=>e.texture));opt($('#draw'),E.map(e=>e.decoded.drawModeName));opt($('#shape'),E.map(e=>e.decoded.shapeName));
const vis=e=>e.shot&&e.shot.visible_pixels>0;
function match(e){
 const q=$('#q').value.toLowerCase(),t=$('#tex').value,d=$('#draw').value,sh=$('#shape').value,st=$('#st').value;
 if(t!==''&&String(e.texture)!==t)return 0;if(d&&e.decoded.drawModeName!==d)return 0;if(sh&&e.decoded.shapeName!==sh)return 0;
 const l=L[e.key];
 if(st==='unlabelled'&&l&&l.name)return 0;if(st==='labelled'&&!(l&&l.name))return 0;
 if(st==='source-named'&&!e.source_names.length)return 0;if(st==='called'&&!e.calls.length)return 0;
 if(st==='never-called'&&e.calls.length)return 0;if(st==='draws-nothing'&&!(e.shot&&!vis(e)))return 0;
 if(q){const h=(e.key+' '+e.id+' '+e.source_names.join(' ')+' '+(l&&l.name||'')+' '+e.calls.map(c=>c.file+' '+(c.symbol||'')).join(' ')).toLowerCase();if(!h.includes(q))return 0}
 return 1}
function render(){
 const g=$('#grid');g.innerHTML='';let n=0;
 for(const e of E){if(!match(e))continue;n++;const l=L[e.key];
  const c=document.createElement('div');c.className='c';c.dataset.k=e.key;
  const img=e.shot?`<img loading=lazy src="shots/${e.key}.png" data-gif="shots/${e.key}.gif">`:`<div style="height:90px;background:#333"></div>`;
  const nm=l&&l.name?`<span class=n>${l.name}</span>`:e.source_names.length?`<span class=n>${e.source_names[0]}</span>`:'<span class=u>unnamed</span>';
  c.innerHTML=`${img}<b>${e.key}</b> tex${e.texture} ${e.calls.length?e.calls.length+' call(s)':'<span class=u>no call</span>'}${e.shot&&!vis(e)?' <span class=bl>blank</span>':''}<br>${nm}`;
  c.onmouseenter=()=>{const i=c.querySelector('img');if(i)i.src=i.dataset.gif};c.onmouseleave=()=>{const i=c.querySelector('img');if(i)i.src='shots/'+e.key+'.png'};
  c.onclick=()=>open_(e);g.append(c)}
 $('#cnt').textContent=n+' / '+E.length}
function open_(e){
 const l=L[e.key]||{};const d=$('#d');d.style.display='block';
 const sim=E.filter(x=>x.texture===e.texture&&x.id!==e.id).map(x=>x.key).join(' ');
 d.innerHTML=`<h3>${e.key} (${e.id}) <button onclick="$('#d').style.display='none'">x</button></h3>
 ${e.shot?`<img src="shots/${e.key}.gif">`:'<i>not shot yet</i>'}
 <p>name <input id=ln value="${l.name||''}" size=28>
 status <select id=ls><option>guess</option><option>seen</option><option>confirmed</option></select></p>
 <p>evidence / notes<br><textarea id=lv rows=4 cols=56>${l.notes||''}</textarea></p>
 <button id=sv>save label</button>
 <pre>${JSON.stringify(e.decoded,null,1)}
optional: ${e.optional.join(', ')||'-'}   behaviors: ${e.behaviors.join(', ')||'-'}
flags ${e.flags}   texture ${e.texture}
records: ${JSON.stringify(e.records)}
same texture: ${sim||'-'}
source names: ${e.source_names.join(', ')||'-'}
calls:
${e.calls.map(c=>'  '+c.file+':'+c.line+' '+(c.symbol||'')+' ['+c.via+']'+(c.name?' '+c.name:'')).join('\n')||'  none (no literal reaches it)'}
visible pixels: ${e.shot?e.shot.visible_pixels:'-'}</pre>`;
 $('#ls').value=l.status||'guess';
 $('#sv').onclick=()=>{L[e.key]={name:$('#ln').value.trim(),status:$('#ls').value,notes:$('#lv').value};
  if(!L[e.key].name&&!L[e.key].notes)delete L[e.key];localStorage.setItem(LS,JSON.stringify(L));render()}}
['q','tex','draw','shape','st'].forEach(i=>$('#'+i).oninput=render);
$('#exp').onclick=()=>{const a=document.createElement('a');a.href=URL.createObjectURL(new Blob([JSON.stringify(L,null,1)],{type:'application/json'}));a.download='labels.json';a.click()};
$('#imp').onclick=()=>{const i=document.createElement('input');i.type='file';i.onchange=async()=>{L=Object.assign(L,JSON.parse(await i.files[0].text()));localStorage.setItem(LS,JSON.stringify(L));render()};i.click()};
render();
</script>"""


def cmd_gallery(a):
    work = os.path.abspath(a.work)
    cp = os.path.join(work, "catalog.json")
    if not os.path.exists(cp):
        cmd_catalog(a)
    cat = open(cp, encoding="utf-8").read().replace("</", "<\\/")
    with open(os.path.join(work, "index.html"), "w", encoding="utf-8") as f:
        f.write(GALLERY.replace("__CATALOG__", cat))
    print("wrote %s" % os.path.join(work, "index.html"))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--work", default=DEFAULT_WORK)
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("shoot")
    s.add_argument("ids", nargs="*")
    s.add_argument("--range", nargs=2, type=lambda v: int(v, 0))
    s.add_argument("--state", default=DEFAULT_STATE)
    s.add_argument("--frames", type=int, default=14)
    s.add_argument("--stride", type=int, default=3)
    s.add_argument("--scale", type=int, default=2)
    s.add_argument("--place", choices=sorted(PLACES), default="beside")
    s.add_argument("--dx", type=int)
    s.add_argument("--dy", type=int)
    s.add_argument("--dz", type=int)
    s.add_argument("--box", nargs=4, type=int, default=list(SHOT_BOX))
    s.add_argument("--force", action="store_true")
    s.set_defaults(fn=cmd_shoot)
    w = sub.add_parser("watch", help="log the effects the game spawns itself during a scripted input")
    w.add_argument("name")
    w.add_argument("--state", default=DEFAULT_STATE)
    w.add_argument("--script", help="JSON list of {buttons, frames}")
    w.set_defaults(fn=cmd_watch)
    q = sub.add_parser("quiz", help="build a blind tagging round")
    q.add_argument("seed")
    q.add_argument("ids", nargs="*")
    q.add_argument("-n", type=int, default=40)
    q.set_defaults(fn=cmd_quiz)
    qs = sub.add_parser("quizscore", help="unblind a finished quiz")
    qs.add_argument("seed")
    qs.add_argument("answers")
    qs.set_defaults(fn=cmd_quizscore)
    sub.add_parser("catalog").set_defaults(fn=cmd_catalog)
    sub.add_parser("gallery").set_defaults(fn=cmd_gallery)
    a = ap.parse_args()
    a.fn(a)


if __name__ == "__main__":
    main()
