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
    return rtti, vt, gl, rec, load_overlay_levels()


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
"""


def conf_span(c):
    if not c:
        return ""
    return '<span class="pill c-%s">%s</span>' % (c, c)


def render(rtti, vt, gl, rec, ovl):
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

    A("<div class='stats'>")
    for label, val in (("classes", st.get("records_total")),
                       ("inheritance edges", st.get("edges_resolved")),
                       ("unresolved", st.get("edges_unresolved_ambiguous", 0)),
                       ("separate hierarchies", st.get("components")),
                       ("multiple inheritance", st.get("classes_with_multiple_bases")),
                       ("roots", st.get("roots"))):
        A("<div class='stat'><b>%s</b><span>%s</span></div>" % (val, e(label)))
    A("</div>")

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

    lines = []

    def walk(n, depth, maxd=2):
        tot = subtree_size(n) - 1
        lines.append("%s%-26s %3d direct, %4d below" %
                     ("  " * depth, n, len(kids[n]), tot))
        if depth < maxd:
            for c in sorted(kids[n], key=lambda x: -subtree_size(x))[:6]:
                if subtree_size(c) > 1:
                    walk(c, depth + 1, maxd)
    walk("fBase_c", 0)
    A("<div class='tree'><pre>%s</pre></div>" % e("\n".join(lines)))

    A("<h2>All %d hierarchies</h2>" % st.get("components", 0))
    A("<div class='scroll'><table><tr><th>size</th><th>root(s)</th></tr>")
    for comp in rtti.get("components", [])[:20]:
        roots = [recs[m]["name"] for m in comp["members"]
                 if recs[m]["name"] not in parent]
        A("<tr><td>%d</td><td class='mono'>%s</td></tr>"
          % (comp["size"], e(", ".join(sorted(roots)) or "(cycle)")))
    A("</table></div>")

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

    for root in sorted(groups, key=lambda r: (-len(groups[r]), r)):
        A("<details%s><summary>%s — %d class(es)</summary>"
          % (" open" if len(groups[root]) > 300 else "", e(root), len(groups[root])))
        for name in sorted(groups[root]):
            v = by_name[name]
            t = vt.get(name, {})
            r = rec.get(name, {})
            dom, sub, suf, parts = decompose(name, gl)
            A("<div class='cls'><h3>%s</h3>" % e(name))
            bits = []
            if r.get("tree_name"):
                bits.append("tree name <b>%s</b>" % e(r["tree_name"]))
            bits.append("module <code>%s</code>" % e(v["module"]))
            if parent.get(name):
                bits.append("base <code>%s</code>" % e(parent[name]))
            else:
                bits.append("<b>root</b>")
            if kids.get(name):
                bits.append("%d child(ren)" % len(kids[name]))
            if t.get("slots"):
                nulls = [i for i, s in enumerate(t["slots"]) if s is None]
                bits.append("%d vtable slots%s" % (len(t["slots"]),
                            (", %d pure-virtual" % len(nulls)) if nulls else ""))
            bits.append("typeinfo <code>%s</code>" % e(v["addr"]))
            A("<div class='meta'>%s</div>" % " · ".join(bits))
            glossed = [p for p in parts if p[1]]
            if glossed:
                A("<div class='parts'>" + "".join(
                    "<span class='p'><code>%s</code> = %s %s</span>"
                    % (e(p[0]), e(p[1]), conf_span(p[2])) for p in glossed) + "</div>")
            A("</div>")
        A("</details>")

    A("<h2>What this does not tell you</h2>")
    A("<p class='sub'>RTTI carries class names, the inheritance graph, and "
      "multiple-inheritance base offsets. It does <b>not</b> carry field names, field types, "
      "field offsets, or method names. A class with no virtual functions emits no record and "
      "is invisible here. See <code>notes/rtti-reconciliation.md</code> for the full "
      "derivation, the traps, and what was deliberately not landed.</p>")
    A("</div></body></html>")
    return "\n".join(out) + "\n"


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--out", default=str(OUT))
    ap.add_argument("--check", action="store_true",
                    help="validate inputs and glossary, write nothing")
    a = ap.parse_args()

    rtti, vt, gl, rec, ovl = load_all()

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
    if a.check:
        print("CHECK OK")
        return 0

    page = render(rtti, vt, gl, rec, ovl)
    p = pathlib.Path(a.out)
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(page, encoding="utf-8", newline="\n")
    print("wrote %s (%.0f KB)" % (a.out, len(page) / 1024))
    return 0


if __name__ == "__main__":
    sys.exit(main())
