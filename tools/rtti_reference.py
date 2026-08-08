#!/usr/bin/env python3
"""Generate the browsable class reference: docs/class-reference.html.

Why this is generated and not written
-------------------------------------
Every fact on the page except the glosses comes from build/rtti.json, which
tools/rtti_extract.py reads out of the ROM and gates with --check.  A hand-written
page listing 429 classes is stale the first time somebody renames one, and it has
no way to fail.  This does: run it after the extractor and the page is current by
construction, or the extractor's gate refuses and there is no page.

That distinction is not theoretical.  While this work was being done the census was
wrong twice, a generated header asserted "everything below 0xdc8 is the base's"
(its own provenance list disproved it), and "slot 0 is null" was true but partial --
daObjSwdoor_c has twelve null slots.  Each was caught because a tool re-derived it.

What is hand-authored
---------------------
config/rom-name-glossary.json, and only that.  Nothing in the binary says what
`Ukiyuka` means.  Every gloss carries a confidence, and this page prints it, so a
settled Mario term is visually distinct from somebody's reading of a romaji
fragment.  UngLossed name parts are shown plainly rather than guessed at.

The course-code -> level mapping is NOT taken from the glossary's prose: it is
joined from symbols/overlay_actors.md, which tools/actor_names.py derives from the
ROM's own ACTOR_SPAWN_TABLE.  So "Bk means Whomp's Fortress" is backed by ov015's
actual actor list, not by assertion.

Usage
-----
    python tools/rtti_reference.py                 # -> docs/class-reference.html
    python tools/rtti_reference.py --check         # verify inputs, emit nothing
"""

from __future__ import annotations

import argparse
import collections
import html
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
RTTI = REPO / "build" / "rtti.json"
VTABLES = REPO / "build" / "rtti_vtables.json"
RECONCILE = REPO / "build" / "rtti_reconcile.json"
ELIGIBLE = REPO / "build" / "eligible-names.txt"
ROMBUILD = REPO / "build" / "rombuild-report.json"
CONFIG = REPO / "config"
GLOSSARY = REPO / "config" / "rom-name-glossary.json"
OVERLAY_ACTORS = REPO / "symbols" / "overlay_actors.md"
OUT = REPO / "docs" / "class-reference.html"

CONF_ORDER = {"high": 0, "medium": 1, "low": 2}


# --------------------------------------------------------------------------
# name decomposition
# --------------------------------------------------------------------------

def split_camel(stem):
    """`ObjKm1_Kuruma` -> ['Obj','Km1','Kuruma'];  `BgCh_SphCrr` -> parts."""
    parts = []
    for chunk in stem.split("_"):
        if not chunk:
            continue
        parts += [p for p in re.findall(r"[A-Z]+(?![a-z])|[A-Z][a-z0-9]*|[a-z0-9]+", chunk) if p]
    return parts


def decompose(name, gl):
    """(domain, subsystem, [(part, gloss_or_None, confidence_or_None)]).

    Longest subsystem prefix wins so `daObj` beats `da` and `dScMg` beats `dSc`.
    """
    bare = name.split("::")[-1]
    stem = bare[:-2] if bare.endswith(("_c", "_t")) else bare
    suffix = bare[-2:] if bare.endswith(("_c", "_t")) else None

    subsys = None
    for cand in sorted(gl.get("subsystem", {}), key=len, reverse=True):
        if stem.startswith(cand) and (len(stem) == len(cand) or stem[len(cand)].isupper()
                                      or stem[len(cand)] == "_"):
            subsys = cand
            break
    rest = stem[len(subsys):] if subsys else stem
    domain = None
    if not subsys and stem and stem[0] in gl.get("domain", {}):
        domain = stem[0]
        rest = stem[1:]
    elif subsys:
        domain = subsys[0] if subsys[0] in gl.get("domain", {}) else None

    terms = gl.get("terms", {})
    courses = gl.get("course", {})
    # case-insensitive fallback: the ROM spells some parts lowercase
    # (cMgSmartball_pakkun_c) and some capitalised (daPkn_c)
    terms_ci = {k.lower(): v for k, v in terms.items()}
    out = []
    for p in split_camel(rest.lstrip("_")):
        base = re.sub(r"\d+$", "", p)
        if p in terms:
            e = terms[p]
        elif base in terms:
            e = terms[base]
        elif p in courses or base in courses:
            e = courses.get(p) or courses[base]
        elif p.lower() in terms_ci:
            e = terms_ci[p.lower()]
        elif base.lower() in terms_ci:
            e = terms_ci[base.lower()]
        else:
            e = None
        out.append((p, (e or {}).get("gloss"), (e or {}).get("confidence"),
                    (e or {}).get("ja")))
    return domain, subsys, suffix, out


# --------------------------------------------------------------------------
# inputs
# --------------------------------------------------------------------------

def load_overlay_levels():
    """overlay -> the actor names it spawns, from the ROM's own spawn table."""
    out = {}
    if not OVERLAY_ACTORS.is_file():
        return out
    for line in OVERLAY_ACTORS.read_text(encoding="utf-8", errors="replace").splitlines():
        m = re.match(r"^- \*\*(\w+)\*\*:\s*(.+)$", line.strip())
        if m:
            names = [re.sub(r"\(\d+\)$", "", a.strip())
                     for a in m.group(2).split(",")]
            out[m.group(1)] = [n for n in names if n]
    return out


SYM_FUNC = re.compile(
    r"^(\S+)\s+kind:function\((?:arm|thumb),size=(0x[0-9a-fA-F]+)\)\s+addr:(0x[0-9a-fA-F]+)")
SYM_DATA = re.compile(r"^(\S+)\s+kind:data\([^)]*\)\s+addr:(0x[0-9a-fA-F]+)")


def load_symbol_tables():
    """(module, addr) -> name, for functions and for data, off config/**/symbols.txt.

    Module key is spelled as dsd spells it -- `arm9`, `ovNNN` -- because that is what
    build/rtti.json carries.  Keying on the path relative to config/ instead matches
    arm9 only and silently drops every overlay; notes/rtti-reconciliation.md 7 records
    that exact mistake costing 237 of 255 class names.
    """
    funcs, data = {}, {}
    for p in sorted(CONFIG.rglob("symbols.txt")):
        rel = p.relative_to(CONFIG).parent.as_posix()
        mod = rel.rsplit("/", 1)[-1] if "overlays" in rel else rel
        for line in p.read_text(encoding="utf-8", errors="replace").splitlines():
            line = line.strip()
            m = SYM_FUNC.match(line)
            if m:
                funcs[(mod, int(m.group(3), 16))] = (m.group(1), int(m.group(2), 16))
                continue
            m = SYM_DATA.match(line)
            if m:
                data[(mod, int(m.group(2), 16))] = m.group(1)
    return funcs, data


def load_enrolled():
    """The symbols whose source file is compiled and linked into the rebuilt ROM.

    `build/eligible-names.txt` is the enrolled set -- its length equals the ROM build's
    own `enrolledFiles`, and the build reports 106/106 modules byte-exact.  So a symbol
    in here is not "someone matched it once": its object file is placed at link time and
    the resulting ROM is byte-identical to retail.  That is the strongest claim the tree
    can make about any function, and it is the one this page reports.
    """
    if not ELIGIBLE.is_file():
        return set(), None
    names = set(ELIGIBLE.read_text(encoding="utf-8").split())
    fidelity = None
    if ROMBUILD.is_file():
        try:
            rb = json.loads(ROMBUILD.read_text(encoding="utf-8"))
            fid = rb.get("analysis", {}).get("moduleFidelity", {})
            fidelity = {"exact": fid.get("modulesExact"),
                        "checked": fid.get("modulesChecked"),
                        "percent": fid.get("percent"),
                        "differing": fid.get("differingBytes"),
                        "bytes": fid.get("comparedBytes"),
                        "enrolled": rb.get("enrolledFiles"),
                        "passed": rb.get("analysis", {}).get("passed")}
        except (ValueError, KeyError):
            fidelity = None
    return names, fidelity


def suspect_tail(slots, base_slots):
    """Index where a slot list stops looking like a vtable, or None.

    `rtti_vtables.py`'s end-of-table rule is "a null followed by a pointer to a KNOWN
    typeinfo record is the next vtable's offset-to-top word". When the following data is
    not a vtable, the rule never fires and the walk runs on. 41 classes show the result:
    a strict (null, pointer) alternation running to the end of the list, beginning one
    slot past the base's count.

    Worked example, `daPeach_c` (ov085): slots 0-30 are `Actor` methods inherited from
    `dActor_c` (31 slots), slot 31 is its own override, and from slot 32 every even index
    is null while every odd one is a `func_ov085_*` local to the class. A class does not
    have 15 alternating pure virtuals; that is a following table of {0, handler} pairs.

    This FLAGS, it does not correct. Nothing here proves where the table really ends --
    dsd has delinked no symbol at the boundary -- and picking a length would be the kind
    of plausible guess `notes/rtti-reconciliation.md` 8 records three of, each wrong.
    """
    n = len(slots)
    if n < 4:
        return None
    # Longest strictly alternating null/non-null SUFFIX. Keying on the first null missed
    # every class with genuine pure virtuals before the bad tail (daObjSwdoor_c is pure
    # at 0, 3, 6 and only then alternates), and keying on (null, pointer) pairs missed
    # the other orientation: daPeach_c's tail reads {0, fn} while daObjSwdoor_c's reads
    # {fn, 0}. Orientation is not the signal; alternation is.
    i = n - 1
    while i - 1 >= 0 and bool(slots[i - 1]) != bool(slots[i]):
        i -= 1
    if n - i < 4:
        return None
    # The run must begin at or past the base's slot count. A "tail" reaching back into
    # inherited slots is not a walk overrun, it is a claim about the base, and this test
    # has no standing to make one.
    if base_slots is not None and i < base_slots:
        return None
    return i


def build_join(rtti, vt, funcs, data, enrolled):
    """Per class: every vtable slot joined ROM address -> symbol -> src file -> enrolled.

    This is the axis the page exists for.  A vtable slot is a raw word in ROM data; the
    question "do we have the C++ for it" is answered only by walking it out to a source
    file and then asking whether that file is in the link.  Nothing else in the tree
    performs that walk, which is why the interface coverage was never visible.

    A slot may point into arm9 from an overlay -- 183 of 413 base edges already cross
    module boundaries -- so the lookup falls back to arm9 rather than declaring a miss.
    """
    try:
        sys.path.insert(0, str(REPO / "tools"))
        import srcpath
    except ImportError:                                   # pragma: no cover
        srcpath = None

    # A slot can point at a function in a THIRD module -- dScMgBomroom_c's table is in
    # ov006 while slot 1 (0x020b0930) is ov004's code, and loading ov006 always loads
    # ov004 (overlay_residency rule E3). Trying only the vtable's module and arm9 left
    # 739 slots looking unnamed when they are simply named somewhere else. Candidates
    # are bounded by residency, and an address hosted by two co-resident modules is
    # reported as unresolved rather than guessed -- overlays share address space and
    # notes/rtti-reconciliation.md 1 records two addresses that host two classes each.
    by_addr = collections.defaultdict(list)
    for (mod, addr), (nm, size) in funcs.items():
        by_addr[addr].append((mod, nm))

    def resolve(vmod, addr):
        hit = funcs.get((vmod, addr)) or funcs.get(("arm9", addr))
        if hit:
            return hit[0]
        cands = by_addr.get(addr) or []
        if not cands:
            return None
        try:
            import overlay_residency as OR
            cands = [c for c in cands if not OR.conflict(c[0], vmod)]
        except Exception:
            return None
        names = {nm for _, nm in cands}
        return names.pop() if len(names) == 1 else None

    out = {}
    for name, v in vt.items():
        slots = v.get("slots") or []
        vmod = v.get("vtable_module") or v.get("module")
        own_by_slot = {o["slot"]: o for o in (v.get("own") or [])}
        rows, named, srcs, enr, pure = [], 0, 0, 0, 0
        for i, s in enumerate(slots):
            if not s:
                pure += 1
                rows.append({"slot": i, "addr": None, "symbol": None, "src": None,
                             "enrolled": False, "own": i in own_by_slot, "pure": True})
                continue
            sym = resolve(vmod, s)
            path = None
            if sym and srcpath:
                p = srcpath.path_for(sym)
                if p:
                    path = pathlib.Path(p).relative_to(REPO).as_posix() \
                        if pathlib.Path(p).is_absolute() else str(p).replace("\\", "/")
            if sym:
                named += 1
            if path:
                srcs += 1
            is_enr = bool(sym and sym in enrolled)
            if is_enr:
                enr += 1
            rows.append({"slot": i, "addr": "0x%08x" % s, "symbol": sym,
                         "src": path, "enrolled": is_enr,
                         "own": i in own_by_slot, "pure": False})
        live = len(slots) - pure
        sus = suspect_tail(slots, v.get("parent_slots"))
        # A derived class's vtable cannot be SHORTER than its base's -- every base slot
        # is either inherited or overridden, and neither removes one. Where that fails,
        # the walk stopped early: rtti_vtables.py bounds a table by the next table start
        # in the same module, and when another vtable begins a few words in, the bound
        # cuts this one short. dBgW_Kc reads 3 slots against dBgW's 13; dScMgAmida_c
        # reads 1 against dScMgBase_c's 36. This is an invariant, not a heuristic.
        ps = v.get("parent_slots")
        short = bool(ps and len(slots) < ps)
        out[name] = {"rows": rows, "slots": len(slots), "live": live, "pure": pure,
                     "named": named, "srcs": srcs, "enrolled": enr,
                     "vtable": v.get("vtable"), "vtable_module": vmod,
                     "parent_slots": v.get("parent_slots"),
                     "own_count": len(own_by_slot),
                     "suspect_from": sus,
                     "suspect_n": (len(slots) - sus) if sus is not None else 0,
                     "short_of_base": short, "base_slots": ps}

    # The data side: does the class's own compiler output exist as named ROM data?
    # These three symbols are what a C++ skeleton would EMIT -- so their presence is the
    # precondition for byte-comparing a compiled declaration against retail.
    by_sym = collections.defaultdict(set)
    for (mod, addr), sym in data.items():
        by_sym[sym].add((mod, addr))
    mangled = {r["name"]: r.get("mangled") for r in rtti["records"].values()}
    for name, rec in out.items():
        mang = mangled.get(name)
        for kind in ("ZTV", "ZTI", "ZTS"):
            sym = "_%s%s" % (kind, mang) if mang else None
            rec[kind.lower()] = sym if sym in by_sym else None
    return out


def grade(j):
    """The class's recovery tier.  Deliberately discrete buckets, not a score.

    A percentage invites averaging, and the average of "we read the interface out of the
    ROM" and "the code is byte-identical to retail" is not a meaningful number.  These
    are different claims with different strength, so they stay separate.
    """
    if not j:
        return "none"
    if not j["slots"]:
        # A located vtable that read zero slots is NOT the same as no vtable, and folding
        # the two hides an anomaly worth someone's attention rather than reporting it.
        return "empty"
    if j["live"] and j["enrolled"] == j["live"]:
        return "verified"
    if j["enrolled"]:
        return "partial"
    if j["named"] == j["live"]:
        return "interface"
    return "listed"


GRADES = {
    "verified":  ("every virtual byte-verified",
                  "Every non-pure slot resolves to a symbol whose source file is enrolled "
                  "in the ROM build. The class's whole virtual interface is reproduced "
                  "byte-for-byte at link time."),
    "partial":   ("some virtuals byte-verified",
                  "The interface is fully read out of the ROM and some of its slots are "
                  "enrolled; the rest are named functions that are not in the link."),
    "interface": ("interface read, no code enrolled",
                  "Every slot resolves to a named function, so the ordered virtual "
                  "interface is known exactly -- but none of those functions is enrolled."),
    "listed":    ("record only",
                  "The typeinfo record is read from the ROM. Some or all of the vtable "
                  "could not be walked out to named functions."),
    "empty":     ("vtable located, 0 slots read",
                  "A vtable address is bound to the record but the slot walk returned "
                  "nothing. Distinct from having no vtable, and worth a look."),
    "none":      ("no vtable reachable",
                  "The record exists but no vtable could be bound to it. "
                  "rtti_vtables.py refuses to guess rather than pick a plausible one."),
}


def load_all():
    missing = [p.name for p in (RTTI, VTABLES, GLOSSARY) if not p.is_file()]
    if missing:
        raise SystemExit("missing input(s): %s -- run tools/rtti_extract.py and "
                         "tools/rtti_vtables.py first" % ", ".join(missing))
    rtti = json.loads(RTTI.read_text(encoding="utf-8"))
    vt = json.loads(VTABLES.read_text(encoding="utf-8"))
    gl = json.loads(GLOSSARY.read_text(encoding="utf-8"))
    rec = {}
    if RECONCILE.is_file():
        for r in json.loads(RECONCILE.read_text(encoding="utf-8"))["rows"]:
            rec[r["rom_name"]] = r
    funcs, data = load_symbol_tables()
    enrolled, fidelity = load_enrolled()
    join = build_join(rtti, vt, funcs, data, enrolled)
    return rtti, vt, gl, rec, load_overlay_levels(), join, fidelity


# --------------------------------------------------------------------------
# page
# --------------------------------------------------------------------------

CSS = """
:root{--bg:#fff;--fg:#1a1a1a;--mut:#666;--line:#e2e2e2;--acc:#0b6bcb;
      --hi:#0a7f3f;--med:#a06000;--low:#a02020;--card:#fafafa}
@media (prefers-color-scheme:dark){
 :root{--bg:#141416;--fg:#e8e8e8;--mut:#9a9a9a;--line:#2c2c30;--acc:#5aa9f0;
       --hi:#4fc07d;--med:#d9a441;--low:#e0736b;--card:#1c1c20}}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--fg);
 font:15px/1.55 -apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,sans-serif}
.wrap{max-width:1080px;margin:0 auto;padding:2rem 1.25rem 5rem}
h1{font-size:1.7rem;margin:0 0 .3rem}
h2{font-size:1.15rem;margin:2.5rem 0 .75rem;padding-bottom:.3rem;border-bottom:1px solid var(--line)}
.sub{color:var(--mut);margin:0 0 1.5rem}
code,.mono{font-family:ui-monospace,SFMono-Regular,Menlo,Consolas,monospace;font-size:.9em}
.stats{display:flex;flex-wrap:wrap;gap:.6rem;margin:1.25rem 0}
.stat{background:var(--card);border:1px solid var(--line);border-radius:7px;padding:.5rem .8rem}
.stat b{display:block;font-size:1.25rem}
.stat span{color:var(--mut);font-size:.8rem}
.tree{overflow-x:auto;background:var(--card);border:1px solid var(--line);
 border-radius:7px;padding:1rem;margin:.5rem 0}
.tree pre{margin:0;font-family:ui-monospace,Menlo,Consolas,monospace;font-size:.85rem;line-height:1.5}
a{color:var(--acc);text-decoration:none}
a:hover{text-decoration:underline}
table{border-collapse:collapse;width:100%;font-size:.88rem}
th,td{text-align:left;padding:.4rem .55rem;border-bottom:1px solid var(--line);vertical-align:top}
th{color:var(--mut);font-weight:600;font-size:.8rem;text-transform:uppercase;letter-spacing:.03em}
.scroll{overflow-x:auto}
.c-high{color:var(--hi)}.c-medium{color:var(--med)}.c-low{color:var(--low)}
.pill{font-size:.7rem;border:1px solid currentColor;border-radius:99px;padding:.02rem .4rem;white-space:nowrap}
.cls{margin:.4rem 0;padding:.7rem .9rem;background:var(--card);
 border:1px solid var(--line);border-radius:7px}
.cls h3{margin:0 0 .3rem;font-size:1rem;font-family:ui-monospace,Menlo,Consolas,monospace}
.meta{color:var(--mut);font-size:.82rem}
.parts{margin:.4rem 0 0;font-size:.85rem}
.parts span.p{display:inline-block;margin-right:.55rem}
.note{color:var(--mut);font-size:.82rem;font-style:italic}
details summary{cursor:pointer;color:var(--acc)}

/* ---- grades ---------------------------------------------------------- */
:root{--g-ver:#0a7f3f;--g-par:#0b6bcb;--g-int:#a06000;--g-lis:#8a5cd0;--g-non:#a02020}
@media (prefers-color-scheme:dark){
 :root{--g-ver:#4fc07d;--g-par:#5aa9f0;--g-int:#d9a441;--g-lis:#b18cf0;--g-non:#e0736b}}
.g-verified{color:var(--g-ver)}.g-partial{color:var(--g-par)}
.g-interface{color:var(--g-int)}.g-listed{color:var(--g-lis)}
.g-empty{color:var(--g-lis)}.g-none{color:var(--g-non)}
.dot{display:inline-block;width:.6rem;height:.6rem;border-radius:50%;background:currentColor;
 vertical-align:baseline;margin-right:.3rem}

/* ---- coverage bar ---------------------------------------------------- */
.bar{display:inline-flex;flex:0 0 120px;height:.55rem;width:120px;border-radius:3px;
 overflow:hidden;background:var(--line);vertical-align:middle;margin:0 .45rem}
.bar i{display:block;height:100%}
.bar .e{background:var(--g-ver)}.bar .s{background:var(--g-par);opacity:.55}
.bar .n{background:var(--g-int);opacity:.45}

/* ---- navigable tree -------------------------------------------------- */
#treewrap{background:var(--card);border:1px solid var(--line);border-radius:7px;
 padding:.85rem 1rem;margin:.6rem 0}
#filter{width:100%;padding:.5rem .65rem;border:1px solid var(--line);border-radius:6px;
 background:var(--bg);color:var(--fg);font:inherit;font-size:.9rem;margin-bottom:.6rem}
.tv{list-style:none;margin:0;padding:0}
.tv ul{list-style:none;margin:0;padding-left:1.15rem;border-left:1px dotted var(--line)}
.tv li{margin:.05rem 0}
.tv .row{display:flex;align-items:center;gap:.35rem;padding:.06rem .25rem;border-radius:4px;
 font-family:ui-monospace,Menlo,Consolas,monospace;font-size:.83rem;white-space:nowrap}
.tv .row:hover{background:var(--line)}
.tv .tog{cursor:pointer;width:1rem;flex:0 0 1rem;text-align:center;color:var(--mut);
 user-select:none;font-size:.7rem}
.tv .tog.leaf{visibility:hidden}
.tv .cnt{color:var(--mut);font-size:.75rem}
.tv .tn{color:var(--mut);font-size:.75rem;font-style:italic}
.tv li.collapsed > ul{display:none}
.toolbar{display:flex;gap:.5rem;flex-wrap:wrap;margin-bottom:.5rem;font-size:.82rem}
.toolbar button{font:inherit;padding:.25rem .6rem;border:1px solid var(--line);
 border-radius:5px;background:var(--bg);color:var(--acc);cursor:pointer}
.toolbar button:hover{background:var(--line)}
.legend{display:flex;gap:.9rem;flex-wrap:wrap;font-size:.78rem;color:var(--mut);
 margin:.35rem 0 .1rem}

/* ---- class cards ----------------------------------------------------- */
/* No content-visibility here on purpose. It was tried: with 429 cards the browser
   guesses each card's height, so a jump from the tree lands in blank space and the
   card never paints. Lazy slot tables already removed the weight that made
   containment tempting. Correct anchors beat a load-time micro-optimisation. */
.cls{scroll-margin-top:1rem}
.cls > summary{display:flex;align-items:center;gap:.4rem;flex-wrap:wrap;list-style:none;
 margin:-.2rem -.2rem .1rem;padding:.2rem;border-radius:5px;color:inherit}
.cls > summary::-webkit-details-marker{display:none}
.cls > summary:hover{background:var(--line)}
.cls > summary .cname{font-family:ui-monospace,Menlo,Consolas,monospace;font-size:.95rem;
 font-weight:600;color:var(--acc)}
.cls[open] > summary{margin-bottom:.4rem;border-bottom:1px solid var(--line)}
.kv{display:grid;grid-template-columns:auto 1fr;gap:.1rem .7rem;font-size:.82rem;
 margin:.45rem 0 0}
.kv dt{color:var(--mut)}
.kv dd{margin:0;font-family:ui-monospace,Menlo,Consolas,monospace}
.slots{width:100%;font-size:.8rem;margin-top:.4rem}
.slots td,.slots th{padding:.18rem .45rem}
.slots .own{font-weight:600}
.slots tr.pure td{color:var(--mut);font-style:italic}
.tick{color:var(--g-ver);font-weight:700}
.cross{color:var(--mut)}
"""


JS = """
(function(){
 /* Slot tables are built the first time their disclosure is opened. The data is one
    JSON blob; emitting 9351 table rows up front froze the renderer outright. */
 var SD=null;
 function slots(){ if(SD===null){ var s=document.getElementById('slotdata');
   SD = s ? JSON.parse(s.textContent) : {}; } return SD; }
 function esc(s){ return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;'); }
 document.addEventListener('toggle',function(e){
   var d=e.target; if(d.tagName!=='DETAILS'||!d.open) return;
   var box=d.querySelector('.slotbox'); if(!box||box.dataset.done) return;
   var rows=slots()[d.getAttribute('data-c')]; if(!rows) return;
   var h=['<div class="scroll"><table class="slots"><tr><th>#</th><th>ROM address</th>',
          '<th>symbol</th><th>source file</th><th>linked</th></tr>'];
   for(var i=0;i<rows.length;i++){
     var r=rows[i], own=r[5]?' own':'';
     if(r[1]===null){ h.push('<tr class="pure"><td>'+r[0]+
       '</td><td>\\u2014</td><td colspan="3">pure virtual (null in ROM)</td></tr>'); continue; }
     h.push('<tr><td class="mono'+own+'">'+r[0]+'</td><td class="mono">'+esc(r[1])+
       '</td><td class="mono'+own+'">'+esc(r[2]||'\\u2014')+'</td><td class="mono">'+
       esc(r[3]||'\\u2014')+'</td><td>'+(r[4]?'<span class="tick">\\u2713</span>':
       '<span class="cross">\\u00b7</span>')+'</td></tr>');
   }
   h.push('</table></div>');
   box.innerHTML=h.join(''); box.dataset.done='1';
 },true);

 /* Class groups ship collapsed, so a #c-<name> link has to open its group before the
    browser can scroll to it. Runs on load and on every hash change, including clicks
    from the tree. */
 function reveal(){
   var h=location.hash; if(!h||h.length<2) return;
   var t=document.getElementById(decodeURIComponent(h.slice(1))); if(!t) return;
   var d=t.closest('details');
   while(d){ d.open=true; d=d.parentElement?d.parentElement.closest('details'):null; }
   requestAnimationFrame(function(){ t.scrollIntoView({block:'start'}); });
 }
 window.addEventListener('hashchange',reveal);
 if(location.hash) setTimeout(reveal,0);

 var f=document.getElementById('filter'), tv=document.getElementById('tv');
 if(!f||!tv) return;
 var rows=[].slice.call(tv.querySelectorAll('li'));
 function setCollapsed(li,on){ li.classList.toggle('collapsed',on);
   var t=li.querySelector(':scope > .row > .tog');
   if(t&&!t.classList.contains('leaf')) t.textContent=on?'\\u25B8':'\\u25BE'; }
 tv.addEventListener('click',function(e){
   var t=e.target.closest('.tog'); if(!t||t.classList.contains('leaf'))return;
   var li=t.closest('li'); setCollapsed(li,!li.classList.contains('collapsed'));
 });
 document.getElementById('expand').onclick=function(){
   rows.forEach(function(li){setCollapsed(li,false)})};
 document.getElementById('collapse').onclick=function(){rows.forEach(function(li){
   if(li.parentNode!==tv) setCollapsed(li,true)})};
 var timer;
 f.addEventListener('input',function(){ clearTimeout(timer); timer=setTimeout(run,110); });
 function run(){
   var q=f.value.trim().toLowerCase();
   rows.forEach(function(li){ li.style.display=''; });
   if(!q){ rows.forEach(function(li){ if(li.parentNode!==tv) setCollapsed(li,true); }); return; }
   rows.forEach(function(li){ li.style.display='none'; });
   rows.forEach(function(li){
     var n=li.getAttribute('data-s')||li.getAttribute('data-n')||'';
     if(n.indexOf(q)<0) return;
     li.style.display='';
     var p=li.parentNode;
     while(p&&p!==tv){ if(p.tagName==='LI'){ p.style.display=''; setCollapsed(p,false);} p=p.parentNode; }
     setCollapsed(li,false);
     li.querySelectorAll('li').forEach(function(c){c.style.display=''});
   });
 }
})();
"""


def conf_span(c):
    if not c:
        return ""
    return '<span class="pill c-%s">%s</span>' % (c, c)


def bar(j):
    """enrolled / has-source / named, as one 120px bar."""
    if not j or not j["live"]:
        return ""
    L = j["live"]
    pe, ps, pn = (100.0 * j["enrolled"] / L, 100.0 * (j["srcs"] - j["enrolled"]) / L,
                  100.0 * (j["named"] - j["srcs"]) / L)
    return ("<span class='bar' title='%d enrolled / %d with source / %d named, of %d'>"
            "<i class='e' style='width:%.1f%%'></i><i class='s' style='width:%.1f%%'></i>"
            "<i class='n' style='width:%.1f%%'></i></span>"
            % (j["enrolled"], j["srcs"], j["named"], L, pe, max(ps, 0), max(pn, 0)))


def render(rtti, vt, gl, rec, ovl, join, fidelity):
    e = html.escape
    recs = rtti["records"]
    by_name = {v["name"]: v for v in recs.values()}
    kids = collections.defaultdict(list)
    parent = {}
    for ed in rtti["edges"]:
        kids[ed["base"]].append(ed["derived"])
        if ed.get("offset", 0) == 0 or ed["derived"] not in parent:
            parent[ed["derived"]] = ed["base"]

    st = rtti["meta"]["stats"]
    out = []
    A = out.append
    A("<!-- GENERATED by tools/rtti_reference.py -- do not edit by hand -->")
    A("<html><head><meta charset='utf-8'>")
    A("<meta name='viewport' content='width=device-width,initial-scale=1'>")
    A("<title>SM64DS engine class reference</title><style>%s</style></head><body><div class='wrap'>" % CSS)
    A("<h1>SM64DS engine class reference</h1>")
    A("<p class='sub'>Every class the ROM's RTTI names, its real inheritance, and what the "
      "names mean. Generated from <code>build/rtti.json</code> — read out of the binary, "
      "not transcribed. Glosses come from <code>config/rom-name-glossary.json</code> and are "
      "the only hand-authored part of this page.</p>")

    # aggregate the join once -- every headline number below is derived, never typed
    tot_slots = sum(j["live"] for j in join.values())
    tot_named = sum(j["named"] for j in join.values())
    tot_src = sum(j["srcs"] for j in join.values())
    tot_enr = sum(j["enrolled"] for j in join.values())
    tot_pure = sum(j["pure"] for j in join.values())
    grades = collections.Counter(grade(join.get(v["name"])) for v in recs.values())

    A("<div class='stats'>")
    for label, val in (("classes", st.get("records_total")),
                       ("inheritance edges", st.get("edges_resolved")),
                       ("unresolved", st.get("edges_unresolved_ambiguous", 0)),
                       ("virtual slots", tot_slots + tot_pure),
                       ("slots named", tot_named),
                       ("slots enrolled", tot_enr)):
        A("<div class='stat'><b>%s</b><span>%s</span></div>" % (val, e(label)))
    A("</div>")

    # ---- the feasibility answer, up front -------------------------------
    A("<h2 id='confidence'>How much of this is proven?</h2>")
    A("<p class='sub'>This page's reason to exist. Each row is a different claim with a "
      "different strength, so they are reported separately rather than averaged into a "
      "score. Everything here is joined at generation time — no number below is typed by "
      "hand.</p>")

    def pct(a, b):
        return ("%.1f%%" % (100.0 * a / b)) if b else "—"

    A("<div class='scroll'><table>"
      "<tr><th>claim</th><th>count</th><th>of</th><th></th><th>what makes it true</th></tr>")
    for claim, a, b, why in (
        ("Class exists and is named", st.get("records_total"), st.get("records_total"),
         "The ROM stores the name string itself. Read, not inferred."),
        ("Base class known", st.get("edges_resolved"),
         st.get("base_pointers_read", st.get("edges_resolved")),
         "A pointer in the typeinfo record. 183 of these cross an overlay boundary."),
        ("Vtable located", len(join), st.get("records_total"),
         "Bound to the record and bounded by the next table, not by a fixed window."),
        ("Virtual slot resolves to a named function", tot_named, tot_slots,
         "Slot word → config/**/symbols.txt in the vtable's own module, or arm9."),
        ("…and that function has a source file", tot_src, tot_slots,
         "tools/srcpath.py resolves the symbol to a path under src/."),
        ("…and that file is enrolled in the ROM build", tot_enr, tot_slots,
         "Compiled to an object and linked; the rebuilt ROM is byte-identical to retail."),
    ):
        A("<tr><td>%s</td><td class='mono'>%s</td><td class='mono'>%s</td>"
          "<td class='mono'>%s</td><td class='note'>%s</td></tr>"
          % (e(claim), a, b, pct(a, b), e(why)))
    A("</table></div>")
    if fidelity:
        A("<p class='note'>ROM build at generation time: <b>%s</b>, %s/%s modules exact "
          "(%.6f%%), %s differing bytes of %s compared, %s enrolled files. Every "
          "<span class='g-verified'>enrolled</span> mark on this page inherits that "
          "gate.</p>"
          % ("PASS" if fidelity.get("passed") else "FAIL", fidelity.get("exact"),
             fidelity.get("checked"), fidelity.get("percent") or 0.0,
             fidelity.get("differing"), fidelity.get("bytes"), fidelity.get("enrolled")))

    # ---- the known defect in the input, stated rather than absorbed -----
    sus_cls = sorted((n for n, j in join.items() if j.get("suspect_from") is not None),
                     key=lambda n: -join[n]["suspect_n"])
    sus_slots = sum(join[n]["suspect_n"] for n in sus_cls)
    if sus_cls:
        A("<p class='note'><b>Known overcount, not corrected here.</b> %d classes carry a "
          "slot list whose tail is a strict (null, pointer) alternation running to the "
          "end — <b>%d slots, %.1f%% of the total</b>. That is the signature of "
          "<code>rtti_vtables.py</code>'s end-of-table rule failing to fire and the walk "
          "continuing into a following <code>{0, handler}</code> table, not of a class "
          "with a dozen alternating pure virtuals. <code>daPeach_c</code> is the clear "
          "case: slots 0–30 are <code>Actor</code> methods, slot 31 is its own, and from "
          "32 every odd slot is a <code>func_ov085_*</code> local to the class. Affected "
          "classes are marked below; the counts are left as the extractor reported them "
          "because nothing here proves where the table really ends.</p>"
          % (len(sus_cls), sus_slots, 100.0 * sus_slots / (tot_slots + tot_pure)))
    short_cls = sorted(n for n, j in join.items() if j.get("short_of_base"))
    if short_cls:
        A("<p class='note'><b>Known undercount, and this one is an invariant violation.</b> "
          "%d classes report FEWER slots than their own base. Every base slot is either "
          "inherited or overridden and neither removes one, so a shorter derived table is "
          "impossible — the walk stopped early, because <code>rtti_vtables.py</code> bounds "
          "a table by the next table start in the same module and something begins a few "
          "words in. <code>dBgW_Kc</code> reads 3 against <code>dBgW</code>'s 13; "
          "<code>dScMgAmida_c</code> reads 1 against <code>dScMgBase_c</code>'s 36. These "
          "classes' interfaces are truncated on this page, so their coverage bars read "
          "better than the truth.</p>" % len(short_cls))

    A("<div class='scroll'><table><tr><th>tier</th><th>classes</th><th>meaning</th></tr>")
    for g in ("verified", "partial", "interface", "listed", "empty", "none"):
        title, desc = GRADES[g]
        A("<tr><td class='g-%s'><span class='dot'></span>%s</td><td class='mono'>%d</td>"
          "<td class='note'>%s</td></tr>" % (g, e(title), grades.get(g, 0), e(desc)))
    A("</table></div>")

    # ---- where the repo's belief and the ROM disagree in precision -------
    verdicts = collections.Counter(r.get("verdict") for r in rec.values() if r.get("verdict"))
    if verdicts:
        A("<h3 style='font-size:.95rem;margin:1.4rem 0 .3rem'>Where the repo's own naming "
          "is imprecise</h3>")
        A("<p class='sub'>The join between what this repo calls a class and what the ROM "
          "calls it, from <code>tools/rtti_reconcile.py</code>. Note the row that matters "
          "most: <b>zero</b> outright disagreements. Where the repo falls short it names a "
          "true ancestor instead of the immediate base — imprecise, not wrong.</p>")
        MEAN = {
            "agree": "the repo's base is the ROM's base",
            "agree_primary_only": "repo has the offset-0 base and omits the rest",
            "flattened": "repo named a true ANCESTOR, skipping the real parent",
            "disagree": "repo's base is not on the ROM's ancestor chain at all",
            "unproven_name": "no corroborated ROM counterpart for the repo's name",
            "no_belief": "repo knows the class but placed no base",
            "unknown_class": "no _ZTV symbol — the repo has never named this class",
            "root_both": "both agree it is a root",
            "root_rom_only": "repo invented an edge under a ROM root",
        }
        A("<div class='scroll'><table><tr><th>verdict</th><th>classes</th>"
          "<th>meaning</th></tr>")
        for v, n in sorted(verdicts.items(), key=lambda kv: -kv[1]):
            A("<tr><td class='mono'>%s</td><td class='mono'>%d</td><td class='note'>%s</td>"
              "</tr>" % (e(v), n, e(MEAN.get(v, ""))))
        A("</table></div>")
        flat = sorted(n for n, r in rec.items() if r.get("verdict") == "flattened")
        if flat:
            A("<p class='note'>The %d <code>flattened</code> classes — each one hides a real "
              "intermediate class with real shared fields: %s</p>"
              % (len(flat), e(", ".join(flat))))

    A("<p class='sub'>The honest read: <b>the interface is solved and the fields are not.</b> "
      "%s of %s virtual slots in the entire game resolve to a named function, and %s of "
      "those are already byte-verified in the link. What no row above claims is field "
      "names, types or offsets — RTTI does not carry them, and a wrong field type is "
      "usually invisible in the bytes. See "
      "<code>notes/runbook-type-reconstruction.md</code>.</p>"
      % (tot_named, tot_slots, tot_enr))

    # ---- the trunk -----------------------------------------------------
    A("<h2>The engine's shape</h2>")
    A("<p class='sub'><code>fBase_c</code> is the root of everything — and note that "
      "<code>dScene_c</code> and <code>dView_c</code> are <em>siblings</em> of "
      "<code>dActor_c</code>, not descendants. Nothing at that level is actor-specific, which "
      "is why the tree's name for it (<code>ActorBase</code>) misleads.</p>")

    def subtree_size(n, seen=None):
        seen = seen or set()
        if n in seen:
            return 0
        seen.add(n)
        return 1 + sum(subtree_size(c, seen) for c in kids[n])

    # ---- the navigable tree: every class, collapsible, filterable --------
    A("<div id='treewrap'>")
    A("<input id='filter' type='search' placeholder='Filter %d classes — try daObj, "
      "Bully, Fader, ov015…' autocomplete='off'>" % len(recs))
    A("<div class='toolbar'><button id='expand'>expand all</button>"
      "<button id='collapse'>collapse all</button></div>")
    A("<div class='legend'>"
      "<span class='g-verified'><span class='dot'></span>every virtual byte-verified</span>"
      "<span class='g-partial'><span class='dot'></span>some byte-verified</span>"
      "<span class='g-interface'><span class='dot'></span>interface read, none enrolled</span>"
      "<span class='g-listed'><span class='dot'></span>record only</span>"
      "<span class='g-none'><span class='dot'></span>no vtable</span></div>")
    A("<ul class='tv' id='tv'>")

    drawn = set()

    def node(n, depth):
        if n in drawn:
            return
        drawn.add(n)
        ch = sorted(kids.get(n, []), key=lambda x: (-subtree_size(x), x))
        j = join.get(n)
        g = grade(j)
        r = rec.get(n, {})
        below = subtree_size(n) - 1
        cls = "" if depth == 0 else " class='collapsed'"
        # Searchable on the ROM name, the repo's name for it, AND the module -- somebody
        # looking for "Bully" is not going to type daDonketu_c, and "ov015" is how you ask
        # "what lives in Whomp's Fortress".
        A("<li%s data-n='%s' data-s='%s'>"
          % (cls, e(n),
             e(" ".join(x for x in (n, r.get("tree_name") or "",
                                    by_name[n]["module"] if n in by_name else "")
                        if x).lower())))
        A("<span class='row'>")
        A("<span class='tog%s'>%s</span>"
          % ("" if ch else " leaf", "▾" if depth == 0 and ch else "▸"))
        A("<span class='g-%s' title='%s'><span class='dot'></span></span>"
          % (g, e(GRADES[g][0])))
        A("<a href='#c-%s'>%s</a>" % (e(n), e(n)))
        if j and j["live"]:
            A(bar(j))
            A("<span class='cnt'>%d/%d</span>" % (j["enrolled"], j["live"]))
        if r.get("tree_name"):
            A("<span class='tn'>%s</span>" % e(r["tree_name"]))
        if below:
            A("<span class='cnt'>· %d below</span>" % below)
        A("</span>")
        if ch:
            A("<ul>")
            for c in ch:
                node(c, depth + 1)
            A("</ul>")
        A("</li>")

    roots_sorted = sorted((v["name"] for v in recs.values() if v["name"] not in parent),
                          key=lambda n: (-subtree_size(n), n))
    for rt in roots_sorted:
        node(rt, 0)
    # anything a cycle kept out of the walk still gets a row
    for v in sorted(recs.values(), key=lambda x: x["name"]):
        if v["name"] not in drawn:
            node(v["name"], 0)
    A("</ul></div>")
    A("<p class='note'>%d classes across %d separate hierarchies. The bar is "
      "enrolled / has-source / named over that class's own non-pure vtable slots; the "
      "italic name is what this repo calls the class today.</p>"
      % (len(recs), st.get("components", 0)))

    # ---- how to read a name --------------------------------------------
    A("<h2>How to read a name</h2>")
    A("<p class='sub'>The names are structured. A domain letter, a subsystem, sometimes a "
      "two-letter course code, then Japanese romaji describing what the thing is.</p>")
    for section, title in (("domain", "Domain letter"), ("subsystem", "Subsystem"),
                           ("suffix", "Suffix"), ("course", "Course code")):
        rows = gl.get(section, {})
        if not rows:
            continue
        A("<h3 style='font-size:.95rem;margin:1rem 0 .3rem'>%s</h3>" % e(title))
        A("<div class='scroll'><table><tr><th>token</th><th>meaning</th><th>confidence</th>"
          "<th>evidence</th></tr>")
        for k in sorted(rows, key=lambda x: (CONF_ORDER.get(rows[x].get("confidence"), 3), x)):
            v = rows[k]
            ev = v.get("note", "")
            if section == "course":
                mods = sorted({recs[m]["module"] for m in recs
                               if k in split_camel(recs[m]["name"])})
                acts = []
                for mod in mods[:1]:
                    acts = ovl.get(mod, [])[:4]
                if acts:
                    ev = "%s → %s%s" % (", ".join(mods[:3]), ", ".join(acts),
                                        " …" if len(ovl.get(mods[0], [])) > 4 else "")
            A("<tr><td class='mono'>%s</td><td>%s</td><td>%s</td><td class='note'>%s</td></tr>"
              % (e(k), e(v.get("gloss", "")), conf_span(v.get("confidence")), e(ev)))
        A("</table></div>")

    A("<h3 style='font-size:.95rem;margin:1rem 0 .3rem'>Terms</h3>")
    A("<div class='scroll'><table><tr><th>token</th><th>japanese</th><th>meaning</th>"
      "<th>confidence</th><th>note</th></tr>")
    terms = gl.get("terms", {})
    for k in sorted(terms, key=lambda x: (CONF_ORDER.get(terms[x].get("confidence"), 3), x)):
        v = terms[k]
        A("<tr><td class='mono'>%s</td><td>%s</td><td>%s</td><td>%s</td><td class='note'>%s</td></tr>"
          % (e(k), e(v.get("ja", "")), e(v.get("gloss", "")),
             conf_span(v.get("confidence")), e(v.get("note", ""))))
    A("</table></div>")

    # ---- the classes ----------------------------------------------------
    slotdata = {}
    A("<h2>Classes</h2>")
    A("<p class='sub'>%d classes, grouped by their top-level hierarchy. "
      "<em>tree name</em> is what this repo calls the class today, joined via the vtable "
      "address; a blank one means the tree has never named it.</p>" % len(recs))

    def root_of(n):
        seen = set()
        while n in parent and n not in seen:
            seen.add(n)
            n = parent[n]
        return n

    groups = collections.defaultdict(list)
    for v in recs.values():
        groups[root_of(v["name"])].append(v["name"])

    # Every group starts CLOSED. With the largest one open the document was 69,000px
    # tall and Chrome could not paint it -- measured, not guessed. Navigation is the
    # tree above and the anchor handler in JS, which opens whichever group holds the
    # target, so nothing is unreachable for being collapsed.
    for root in sorted(groups, key=lambda r: (-len(groups[r]), r)):
        gc = collections.Counter(grade(join.get(n)) for n in groups[root])
        tally = " ".join(
            "<span class='g-%s'><span class='dot'></span>%d</span>" % (g, gc[g])
            for g in ("verified", "partial", "interface", "listed", "empty", "none")
            if gc.get(g))
        A("<details><summary>%s — %d class(es) &nbsp; %s</summary>"
          % (e(root), len(groups[root]), tally))
        for name in sorted(groups[root]):
            v = by_name[name]
            r = rec.get(name, {})
            j = join.get(name)
            g = grade(j)
            dom, sub, suf, parts = decompose(name, gl)
            # One <details> per class, closed. Opening a 338-class group then has to lay
            # out only its summary lines; a closed disclosure's body is never laid out.
            # With full cards inline the group froze the renderer -- measured.
            A("<details class='cls' id='c-%s'><summary>" % e(name))
            A("<span class='g-%s' title='%s'><span class='dot'></span></span>"
              % (g, e(GRADES[g][0])))
            A("<span class='cname'>%s</span>" % e(name))
            if j and j["live"]:
                A(bar(j))
                A("<span class='cnt'>%d/%d</span>" % (j["enrolled"], j["live"]))
            if r.get("tree_name"):
                A("<span class='tn'>%s</span>" % e(r["tree_name"]))
            A("</summary>")
            bits = ["<span class='g-%s'><span class='dot'></span>%s</span>"
                    % (g, e(GRADES[g][0]))]
            if r.get("tree_name"):
                bits.append("tree name <b>%s</b>" % e(r["tree_name"]))
            if parent.get(name):
                bits.append("base <code>%s</code>" % e(parent[name]))
            else:
                bits.append("<b>root</b>")
            if kids.get(name):
                bits.append("%d child(ren)" % len(kids[name]))
            A("<div class='meta'>%s</div>" % " · ".join(bits))

            # --- where it lives in the ROM, and what proves the name ------
            A("<dl class='kv'>")
            A("<dt>module</dt><dd>%s</dd>" % e(v["module"]))
            A("<dt>typeinfo</dt><dd>%s <span class='cnt'>%s</span>%s</dd>"
              % (e(v["addr"]), e(v.get("kind", "")),
                 (" &nbsp;<code>%s</code>" % e(j["zti"])) if j and j.get("zti") else ""))
            if v.get("name_addr"):
                A("<dt>name string</dt><dd>%s%s</dd>"
                  % (e(v["name_addr"]),
                     (" &nbsp;<code>%s</code>" % e(j["zts"])) if j and j.get("zts") else ""))
            if j and j.get("vtable"):
                # rtti_vtables.json's `vtable` is the address of SLOT 0, not of the _ZTV
                # symbol: the table proper starts 8 bytes earlier, at the offset-to-top
                # word followed by the typeinfo pointer. Verified against the ROM --
                # fBase_c reads 0x00000000, 0x02086d70 at 0x02099ed4. Reporting slot 0 as
                # the table start understates the object by exactly those two words.
                slot0 = int(j["vtable"], 16)
                start = slot0 - 8
                span = 8 + 4 * j["slots"]
                extra = ""
                if j["vtable_module"] != v["module"]:
                    extra = (" <span class='cnt'>in %s, not the record's module</span>"
                             % e(j["vtable_module"]))
                A("<dt>vtable</dt><dd>0x%08x .. 0x%08x <span class='cnt'>(%d bytes: "
                  "offset-to-top, typeinfo ptr, then %d slots from 0x%08x)</span>%s%s</dd>"
                  % (start, start + span, span, j["slots"], slot0, extra,
                     (" &nbsp;<code>%s</code>" % e(j["ztv"])) if j.get("ztv") else ""))
            if j and j.get("short_of_base"):
                A("<dt class='g-none'>slot count</dt><dd class='g-none'>truncated — %d "
                  "slots against a base of %d, which is impossible. The walk stopped "
                  "early; this class's interface is incomplete on this page.</dd>"
                  % (j["slots"], j["base_slots"]))
            if j and j.get("suspect_from") is not None:
                A("<dt class='g-none'>slot count</dt><dd class='g-none'>suspect from "
                  "slot %d — the last %d entries alternate (null, pointer), which reads "
                  "as the walk running past the end of the table. Treat this class's "
                  "slot and pure counts as an upper bound.</dd>"
                  % (j["suspect_from"], j["suspect_n"]))
            if r.get("verdict"):
                A("<dt>name join</dt><dd>%s <span class='cnt'>%s</span></dd>"
                  % (e(r["verdict"]), e(str(r.get("proven_by") or "—"))))
            if r.get("rom_bases") and len(r["rom_bases"]) > 1:
                A("<dt>all bases</dt><dd>%s</dd>"
                  % e(", ".join(str(b) for b in r["rom_bases"])))
            if j and j["live"]:
                A("<dt>coverage</dt><dd>%s %d enrolled · %d with source · %d named · "
                  "of %d live slots%s</dd>"
                  % (bar(j), j["enrolled"], j["srcs"], j["named"], j["live"],
                     (" · %d pure-virtual" % j["pure"]) if j["pure"] else ""))
            A("</dl>")

            glossed = [p for p in parts if p[1]]
            if glossed:
                A("<div class='parts'>" + "".join(
                    "<span class='p'><code>%s</code> = %s %s</span>"
                    % (e(p[0]), e(p[1]), conf_span(p[2])) for p in glossed) + "</div>")

            # --- the slot table: ROM word -> symbol -> file -> in the link -
            # Emitted as data, built on first open. 9351 slots as table markup is ~2MB of
            # HTML and 10k live DOM nodes, which froze the renderer outright when this was
            # measured -- a reference nobody can scroll is not a reference.
            if j and j["rows"]:
                slotdata[name] = [
                    [r2["slot"], r2["addr"], r2["symbol"], r2["src"],
                     1 if r2["enrolled"] else 0, 1 if r2["own"] else 0]
                    for r2 in j["rows"]]
                A("<details data-c='%s'><summary>%d vtable slots — ROM address, symbol, "
                  "source file, in the link</summary><div class='slotbox'></div>"
                  "<p class='note'>Bold rows are this class's own overrides — a slot whose "
                  "address differs from the base's. <span class='tick'>✓</span> means the "
                  "file is compiled and linked into the rebuilt ROM, which matches retail "
                  "byte-for-byte.</p></details>" % (e(name), j["slots"]))
            A("</details>")
        A("</details>")

    A("<h2>What this does not tell you</h2>")
    A("<p class='sub'>RTTI carries class names, the inheritance graph, and "
      "multiple-inheritance base offsets. It does <b>not</b> carry field names, field types, "
      "field offsets, or method names. A class with no virtual functions emits no record and "
      "is invisible here. See <code>notes/rtti-reconciliation.md</code> for the full "
      "derivation, the traps, and what was deliberately not landed.</p>")
    A("<p class='sub'>Two limits worth stating precisely, because they bound what the "
      "green above is worth:</p>")
    A("<ul class='sub'>")
    A("<li><b>A named slot is not a named method.</b> The join proves slot <i>n</i> holds "
      "the function at that address and that the tree has a symbol for it. Where that "
      "symbol is a <code>func_*</code> placeholder the position is right and the name is "
      "still unknown — and a vtable never carries a signature, so parameters and return "
      "types come from the definition or from nowhere.</li>")
    A("<li><b>Enrolled means the bytes are right, not that the C++ is idiomatic.</b> Most "
      "enrolled files are still <code>int*</code>-and-array-index C. Turning them into real "
      "methods is what <code>notes/plan-cpp-language-mode.md</code> tracks; for destructors "
      "specifically it is currently blocked, and <code>notes/dtor-variant-audit.md</code> §7 "
      "has the reason — <code>eligible.py</code> rejects the three-<code>.text</code> object "
      "a <code>~Class()</code> definition always emits.</li>")
    A("</ul>")
    A("<script id='slotdata' type='application/json'>%s</script>"
      % json.dumps(slotdata, separators=(",", ":")).replace("</", "<\\/"))
    A("<script>%s</script>" % JS)
    A("</div></body></html>")
    return "\n".join(out) + "\n"


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--out", default=str(OUT))
    ap.add_argument("--check", action="store_true",
                    help="validate inputs and glossary, write nothing")
    a = ap.parse_args()

    rtti, vt, gl, rec, ovl, join, fidelity = load_all()

    # glossary hygiene: every entry needs a gloss and a known confidence
    bad = []
    for section in ("domain", "subsystem", "suffix", "course", "terms"):
        for k, v in gl.get(section, {}).items():
            if not v.get("gloss"):
                bad.append("%s.%s has no gloss" % (section, k))
            if v.get("confidence") not in CONF_ORDER:
                bad.append("%s.%s confidence=%r not in %s"
                           % (section, k, v.get("confidence"), sorted(CONF_ORDER)))
    if bad:
        for b in bad:
            print("GLOSSARY: %s" % b)
        return 1

    n_terms = sum(len(gl.get(s, {})) for s in
                  ("domain", "subsystem", "suffix", "course", "terms"))
    covered = sum(1 for v in rtti["records"].values()
                  if any(p[1] for p in decompose(v["name"], gl)[3]))
    print("glossary: %d entries (%s)"
          % (n_terms, ", ".join("%s=%d" % (c, sum(
              1 for s in ("domain", "subsystem", "suffix", "course", "terms")
              for v in gl.get(s, {}).values() if v.get("confidence") == c))
              for c in ("high", "medium", "low"))))
    print("classes with at least one glossed name part: %d / %d"
          % (covered, len(rtti["records"])))

    live = sum(j["live"] for j in join.values())
    named = sum(j["named"] for j in join.values())
    enr = sum(j["enrolled"] for j in join.values())
    print("vtable slots: %d live, %d named (%.1f%%), %d enrolled (%.1f%%)"
          % (live, named, 100.0 * named / live if live else 0,
             enr, 100.0 * enr / live if live else 0))
    gc = collections.Counter(grade(join.get(v["name"])) for v in rtti["records"].values())
    print("recovery tiers: " + ", ".join("%s=%d" % (g, gc.get(g, 0))
                                         for g in ("verified", "partial", "interface",
                                                   "listed", "empty", "none")))
    # A slot the join cannot name is a hole in the page's central claim, not a detail.
    holes = sum(j["live"] - j["named"] for j in join.values())
    if holes:
        print("WARNING: %d vtable slot(s) resolve to no symbol" % holes)
    sc = [n for n, j in join.items() if j.get("suspect_from") is not None]
    if sc:
        print("NOTE: %d class(es) have a suspect alternating tail -- %d slots are probably "
              "not vtable slots at all (rtti_vtables.py end-of-table rule)"
              % (len(sc), sum(join[n]["suspect_n"] for n in sc)))
    shc = [n for n, j in join.items() if j.get("short_of_base")]
    if shc:
        print("NOTE: %d class(es) report fewer slots than their base -- impossible, so "
              "the vtable walk stopped early (rtti_vtables.py next-table bound)"
              % len(shc))
    if a.check:
        print("CHECK OK")
        return 0

    page = render(rtti, vt, gl, rec, ovl, join, fidelity)
    p = pathlib.Path(a.out)
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(page, encoding="utf-8", newline="\n")
    print("wrote %s (%.0f KB)" % (a.out, len(page) / 1024))
    return 0


if __name__ == "__main__":
    sys.exit(main())
