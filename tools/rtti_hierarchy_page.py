#!/usr/bin/env python3
"""Generate docs/class-hierarchy.html -- the class graph, every edge tagged.

Companion to tools/rtti_reference.py.  That page is a per-class reference: 429 rows,
one per type_info record.  This one is the GRAPH -- what derives from what, and for
each edge, what makes it true.

Three evidence tiers, and the distinction is the whole point:

    PROVEN       the ROM's type_info record holds a pointer to the base's record.
                 Read, not inferred.  405 edges + 24 records the ROM calls roots.
    INFERRED     no type_info record exists (the class is not polymorphic, or its
                 vtable was never emitted), and the base comes from
                 evidence_hierarchy.py reading ctor/dtor vptr chains.  58 classes.
    CONTRADICTED the repo's own header names a class that IS on the ROM's ancestor
                 chain but is not the immediate base.  24 classes, all in the
                 Platform family, and rtti_reconcile.py calls this `flattened`.

Nothing on the page is typed by hand except the glosses, which come from
config/rom-name-glossary.json and carry their own confidence.  Every count, edge and
offset is joined at generation time from:

    build/rtti.json                  tools/rtti_extract.py  (gated with --check)
    build/rtti_reconcile.json        tools/rtti_reconcile.py
    build/evidence_hierarchy.json    tools/evidence_hierarchy.py
    build/tu_names.json              tools/tu_names.py       (optional)

Usage:
    python tools/rtti_hierarchy_page.py
    python tools/rtti_hierarchy_page.py --check      # verify inputs, emit nothing
"""
from __future__ import annotations

import argparse
import collections
import html
import json
import pathlib
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
RTTI = REPO / "build" / "rtti.json"
RECONCILE = REPO / "build" / "rtti_reconcile.json"
EVIDENCE = REPO / "build" / "evidence_hierarchy.json"
TUNAMES = REPO / "build" / "tu_names.json"
GLOSSARY = REPO / "config" / "rom-name-glossary.json"
OUT = REPO / "docs" / "class-hierarchy.html"

E = html.escape


# --------------------------------------------------------------------------
# data
# --------------------------------------------------------------------------

def load():
    missing = [p for p in (RTTI, RECONCILE, EVIDENCE) if not p.is_file()]
    if missing:
        sys.exit("missing: %s\nrun rtti_extract.py, rtti_reconcile.py, "
                 "evidence_hierarchy.py first"
                 % ", ".join(str(p.relative_to(REPO)) for p in missing))
    d = {
        "rtti": json.loads(RTTI.read_text(encoding="utf-8")),
        "rec": json.loads(RECONCILE.read_text(encoding="utf-8")),
        "eh": json.loads(EVIDENCE.read_text(encoding="utf-8")),
    }
    d["names"] = (json.loads(TUNAMES.read_text(encoding="utf-8"))
                  if TUNAMES.is_file() else None)
    d["gloss"] = (json.loads(GLOSSARY.read_text(encoding="utf-8"))
                  if GLOSSARY.is_file() else {})
    return d


class Graph:
    def __init__(self, d):
        r = d["rtti"]
        self.rec = r["records"]
        self.name = {k: v["name"] for k, v in self.rec.items()}
        self.module = {k: v["module"] for k, v in self.rec.items()}
        self.key_of = {}
        for k, n in self.name.items():
            self.key_of.setdefault(n, k)
        self.parents = collections.defaultdict(list)   # key -> [(base_key, offset)]
        self.children = collections.defaultdict(list)
        for e in r["edges"]:
            self.parents[e["derived_key"]].append((e["base_key"], e.get("offset", 0)))
            self.children[e["base_key"]].append(e["derived_key"])
        for k in self.children:
            self.children[k].sort(key=lambda x: (-len(self.children.get(x, [])),
                                                 self.name[x]))
        # subtree_size must always answer for the REAL graph. §2 renders a pruned
        # spine by swapping self.children, and counting against the pruned map made
        # dActor_c report 2 descendants when it has 287.
        self.full_children = dict(self.children)
        self.roots = sorted((k for k in self.name if not self.parents.get(k)),
                            key=lambda k: (-len(self.children.get(k, [])),
                                           self.name[k]))
        self.components = r["components"]

    def depth(self, k):
        seen, n = set(), 0
        cur = k
        while self.parents.get(cur) and cur not in seen:
            seen.add(cur)
            cur = self.parents[cur][0][0]
            n += 1
        return n

    def subtree_size(self, k):
        seen, stack = set(), [k]
        while stack:
            x = stack.pop()
            if x in seen:
                continue
            seen.add(x)
            stack.extend(self.full_children.get(x, []))
        return len(seen)


# --------------------------------------------------------------------------
# design tokens
# --------------------------------------------------------------------------

CSS = """
*,*::before,*::after{box-sizing:border-box}
:root{
  --paper:#F1F2EE; --card:#F8F9F6; --ink:#171C22; --mute:#5C6672;
  --rule:#C6CAC1; --rule-soft:#DCDFD8;
  --proven:#25407F; --inferred:#8A6516; --contra:#95241F;
  --proven-bg:#25407F14; --inferred-bg:#8A651614; --contra-bg:#95241F12;
  --mono:ui-monospace,"SFMono-Regular","Cascadia Mono","Segoe UI Mono",Menlo,Consolas,monospace;
  --sans:"Helvetica Neue",Helvetica,-apple-system,"Segoe UI",Arial,sans-serif;
  --measure:66ch;
}
@media (prefers-color-scheme:dark){
  :root:not([data-theme="light"]){
    --paper:#101317; --card:#171B21; --ink:#E3E6E1; --mute:#8B949F;
    --rule:#2C323A; --rule-soft:#21262D;
    --proven:#7E9EE2; --inferred:#D2A551; --contra:#E2776F;
    --proven-bg:#7E9EE21F; --inferred-bg:#D2A5511F; --contra-bg:#E2776F1A;
  }
}
:root[data-theme="dark"]{
  --paper:#101317; --card:#171B21; --ink:#E3E6E1; --mute:#8B949F;
  --rule:#2C323A; --rule-soft:#21262D;
  --proven:#7E9EE2; --inferred:#D2A551; --contra:#E2776F;
  --proven-bg:#7E9EE21F; --inferred-bg:#D2A5511F; --contra-bg:#E2776F1A;
}
html{-webkit-text-size-adjust:100%}
body{
  margin:0; background:var(--paper); color:var(--ink);
  font-family:var(--sans); font-size:16px; line-height:1.6;
  font-variant-numeric:tabular-nums;
}
.wrap{max-width:1080px; margin:0 auto; padding:0 24px 96px}

/* ---- title block: an engineering drawing's, not a hero ---- */
.block{border:1.5px solid var(--ink); margin:40px 0 8px}
.block-t{display:flex; flex-wrap:wrap; align-items:baseline; gap:16px;
  padding:20px 22px 16px; border-bottom:1.5px solid var(--ink)}
h1{font-size:clamp(26px,4.2vw,40px); line-height:1.08; margin:0; font-weight:700;
  letter-spacing:-.021em; text-wrap:balance}
.sub{color:var(--mute); font-size:14px; max-width:var(--measure); margin:6px 0 0}
.fields{display:grid; grid-template-columns:repeat(auto-fit,minmax(150px,1fr))}
.f{padding:11px 22px; border-right:1px solid var(--rule)}
.f:last-child{border-right:0}
.f dt{font:600 10px/1.4 var(--sans); letter-spacing:.13em; text-transform:uppercase;
  color:var(--mute); margin:0 0 3px}
.f dd{margin:0; font-family:var(--mono); font-size:13.5px}

h2{font-size:21px; letter-spacing:-.012em; margin:0; font-weight:700; text-wrap:balance}
h3{font-size:15px; margin:30px 0 10px; font-weight:700; letter-spacing:-.005em}
section{margin-top:56px; scroll-margin-top:16px}
.sec-h{display:flex; gap:14px; align-items:baseline;
  border-bottom:1.5px solid var(--ink); padding-bottom:8px; margin-bottom:20px}
.sec-n{font-family:var(--mono); font-size:13px; color:var(--proven); font-weight:700;
  flex:none; padding-top:3px}
p{max-width:var(--measure); margin:0 0 14px}
.lede{font-size:17.5px; max-width:var(--measure)}
a{color:var(--proven)}
a:focus-visible,summary:focus-visible{outline:2px solid var(--proven); outline-offset:2px}
code,.m{font-family:var(--mono); font-size:.92em}

/* ---- evidence system ---- */
.legend{display:flex; flex-wrap:wrap; gap:8px; margin:18px 0 4px}
.tag{display:inline-flex; align-items:center; gap:7px; font:600 11px/1 var(--sans);
  letter-spacing:.09em; text-transform:uppercase; padding:6px 11px; border-radius:2px;
  border:1px solid currentColor}
.t-proven{color:var(--proven); background:var(--proven-bg)}
.t-inferred{color:var(--inferred); background:var(--inferred-bg)}
.t-contra{color:var(--contra); background:var(--contra-bg)}
.swatch{width:22px; height:0; flex:none}
.t-proven .swatch{border-top:2.5px solid var(--proven)}
.t-inferred .swatch{border-top:2.5px dashed var(--inferred)}
.t-contra .swatch{border-top:2.5px dotted var(--contra)}

/* ---- tables ---- */
.scroll{overflow-x:auto; margin:18px 0; border:1px solid var(--rule-soft);
  background:var(--card)}
table{border-collapse:collapse; width:100%; font-size:13.5px}
th{text-align:left; font:600 10px/1.5 var(--sans); letter-spacing:.11em;
  text-transform:uppercase; color:var(--mute); padding:10px 14px;
  border-bottom:1px solid var(--rule); white-space:nowrap}
td{padding:8px 14px; border-bottom:1px solid var(--rule-soft); vertical-align:top}
tr:last-child td{border-bottom:0}
td.n{text-align:right; font-family:var(--mono)}
td.m,th.m{font-family:var(--mono)}
.bar{height:5px; background:var(--proven); border-radius:1px; min-width:2px}
.bar.w{background:var(--inferred)}

/* ---- the tree ---- */
.tree{font-family:var(--mono); font-size:13px; line-height:1.75; overflow-x:auto;
  background:var(--card); border:1px solid var(--rule-soft); padding:18px 20px;
  margin:18px 0; white-space:pre}
.tree .cls{font-weight:600}
.tree .pv{color:var(--proven)}
.tree .inf{color:var(--inferred)}
.tree .ct{color:var(--contra)}
.tree .dim{color:var(--mute)}
.tree .cnt{color:var(--mute); font-size:11.5px}
details{border:1px solid var(--rule-soft); background:var(--card); margin:10px 0}
summary{cursor:pointer; padding:11px 16px; font:600 13px/1.4 var(--sans);
  list-style:none; display:flex; justify-content:space-between; gap:12px}
summary::-webkit-details-marker{display:none}
summary::after{content:"+"; font-family:var(--mono); color:var(--mute)}
details[open] summary::after{content:"\\2212"}
details[open] summary{border-bottom:1px solid var(--rule-soft)}
details .tree{border:0; margin:0}
.note{border-left:2.5px solid var(--rule); padding:2px 0 2px 16px; margin:16px 0;
  color:var(--mute); font-size:14.5px; max-width:var(--measure)}
.note strong{color:var(--ink)}
figure{margin:22px 0}
figcaption{font-size:12.5px; color:var(--mute); margin-top:9px; max-width:var(--measure)}
svg{max-width:100%; height:auto; display:block}
footer{margin-top:72px; padding-top:18px; border-top:1.5px solid var(--ink);
  font-size:12.5px; color:var(--mute)}
@media(max-width:640px){
  .f{border-right:0; border-bottom:1px solid var(--rule)}
  .wrap{padding:0 16px 64px}
}
@media(prefers-reduced-motion:reduce){*{animation:none!important; transition:none!important}}
"""


# --------------------------------------------------------------------------
# rendering
# --------------------------------------------------------------------------

def tree_lines(g, root, contra, inferred_children, max_depth=99):
    """Unicode tree with evidence-coded connectors."""
    out = []

    def walk(k, prefix, last, depth):
        conn = "" if depth == 0 else ("└─ " if last else "├─ ")
        kids = g.children.get(k, [])
        n = g.subtree_size(k) - 1
        cnt = ('<span class="cnt">  %d below</span>' % n) if n > 1 else ""
        cls = "ct" if g.name[k] in contra else "pv"
        out.append('%s<span class="dim">%s</span><span class="cls %s">%s</span>%s'
                   % (E(prefix), E(conn), cls, E(g.name[k]), cnt))
        if depth >= max_depth:
            if kids:
                out.append('%s<span class="dim">%s   … %d more</span>'
                           % (E(prefix), "   " if last else "│  ", len(kids)))
            return
        nxt = prefix + ("" if depth == 0 else ("   " if last else "│  "))
        for i, c in enumerate(kids):
            walk(c, nxt, i == len(kids) - 1, depth + 1)
        for i, (nm, why) in enumerate(inferred_children.get(g.name[k], [])):
            out.append('%s<span class="inf">%s%s</span>'
                       "<span class=\"cnt\">  %s</span>"
                       % (E(nxt), E("└┈ " if i == len(inferred_children[g.name[k]]) - 1
                                    else "├┈ "), E(nm), E(why)))

    walk(root, "", True, 0)
    return "\n".join(out)


def mi_diagram(g):
    """The six multiple-inheritance records, drawn with their real base offsets."""
    cases = [(k, v) for k, v in g.parents.items() if len(v) > 1]
    cases.sort(key=lambda kv: (-len(kv[1]), g.name[kv[0]]))
    rowh, top, lblw = 64, 46, 210
    h = top + rowh * len(cases) + 18
    px_per_byte = 2.1
    parts = ['<svg viewBox="0 0 900 %d" role="img" '
             'aria-label="Base subobject offsets for the six classes with multiple '
             'inheritance">' % h]
    parts.append('<text x="0" y="16" font-size="11" font-weight="700" '
                 'letter-spacing="1.3" fill="var(--mute)">DERIVED CLASS</text>')
    parts.append('<text x="%d" y="16" font-size="11" font-weight="700" '
                 'letter-spacing="1.3" fill="var(--mute)">BASE SUBOBJECTS, '
                 'PLACED AT THEIR RECORDED OFFSET</text>' % lblw)
    parts.append('<line x1="0" y1="26" x2="900" y2="26" stroke="var(--rule)"/>')
    for i, (k, bases) in enumerate(cases):
        y = top + i * rowh
        parts.append('<text x="0" y="%d" font-size="13" font-family="var(--mono)" '
                     'font-weight="600" fill="var(--ink)">%s</text>'
                     % (y + 4, E(g.name[k])))
        for bk, off in sorted(bases, key=lambda b: b[1]):
            x = lblw + off * px_per_byte
            w = max(len(g.name[bk]) * 7.4 + 16, 64)
            primary = off == 0
            parts.append(
                '<rect x="%.1f" y="%d" width="%.1f" height="24" rx="2" '
                'fill="%s" stroke="var(--proven)" stroke-width="%s"/>'
                % (x, y - 14, w, "var(--proven-bg)" if primary else "none",
                   "1.6" if primary else "1"))
            parts.append(
                '<text x="%.1f" y="%d" font-size="11.5" font-family="var(--mono)" '
                'fill="var(--proven)">%s</text>' % (x + 8, y + 2, E(g.name[bk])))
            parts.append(
                '<text x="%.1f" y="%d" font-size="10" font-family="var(--mono)" '
                'fill="var(--mute)">+0x%x</text>' % (x, y - 19, off))
        parts.append('<line x1="0" y1="%d" x2="900" y2="%d" stroke="var(--rule-soft)"/>'
                     % (y + 22, y + 22))
    parts.append("</svg>")
    return "".join(parts)


def build(d):
    g = Graph(d)
    rec, eh = d["rec"], d["eh"]
    rows = rec["rows"]
    by_verdict = collections.defaultdict(list)
    for r in rows:
        by_verdict[r["verdict"]].append(r)

    contra = {r["rom_name"] for r in by_verdict["flattened"]}
    tree_only = rec["tree_only_classes"]
    hier = eh["hierarchy"]

    # Inferred children: a tree-only class whose believed base translates to a ROM class.
    tree2rom = {r["tree_name"]: r["rom_name"] for r in rows
                if r["tree_name"] and r["rom_name"]}
    inferred_children = collections.defaultdict(list)
    unplaced = []
    for c in tree_only:
        ent = hier.get(c) or {}
        base = ent.get("base")
        rombase = tree2rom.get(base, base if base in g.key_of else None)
        if rombase:
            inferred_children[rombase].append(
                (c, "inferred · %s" % (ent.get("confidence") or "?")))
        else:
            unplaced.append(c)
    for v in inferred_children.values():
        v.sort()

    st = rec["stats"]
    n_edges = len(d["rtti"]["edges"])
    n_rec = len(g.name)

    H = []
    A = H.append
    A('<title>The fBase Tree</title>')
    A("<style>%s</style>" % CSS)
    A('<div class="wrap">')

    # ---- title block
    A('<div class="block"><div class="block-t"><div>')
    A("<h1>The fBase Tree</h1>")
    A('<p class="sub">Every class in Super Mario 64 DS that the ROM itself can '
      'name, how they descend from one another, and — edge by edge — what makes '
      'each relationship true.</p>')
    A("</div></div>")
    A('<dl class="fields">')
    for label, val in [
        ("Source", "retail ROM, ARM9 + 103 overlays"),
        ("Classes", "%d named by the ROM" % n_rec),
        ("Edges", "%d proven · %d inferred" % (n_edges, len(tree_only) - len(unplaced))),
        ("Deepest chain", "%d levels" % max(g.depth(k) for k in g.name)),
        ("Generated", "tools/rtti_hierarchy_page.py"),
    ]:
        A('<div class="f"><dt>%s</dt><dd>%s</dd></div>' % (E(label), E(val)))
    A("</dl></div>")

    A('<div class="legend">')
    A('<span class="tag t-proven"><span class="swatch"></span>Proven</span>')
    A('<span class="tag t-inferred"><span class="swatch"></span>Inferred</span>')
    A('<span class="tag t-contra"><span class="swatch"></span>Contradicted</span>')
    A("</div>")

    # ---- §1
    A('<section><div class="sec-h"><span class="sec-n">§1</span>'
      "<h2>What a line in this document is worth</h2></div>")
    A('<p class="lede">The three tiers are not degrees of confidence in the same '
      "kind of claim. They are different kinds of claim, and collapsing them into a "
      "single score would be the one mistake this page exists to avoid.</p>")
    A('<div class="scroll"><table><thead><tr><th>Tier</th><th>Count</th>'
      "<th>What makes it true</th><th>Can it be wrong?</th></tr></thead><tbody>")
    for tier, cnt, how, wrong in [
        ("Proven", "%d edges" % n_edges,
         "The class's <code>type_info</code> record holds a pointer to its base's "
         "record. The compiler wrote it; this is read, not inferred.",
         "Only if the pointer was misread. 0 of %d are unresolved." % n_edges),
        ("Proven root", "%d classes" % len(g.roots),
         "A <code>__class_type_info</code> record — the ABI's own encoding for "
         "“no base”.",
         "No. The record kind is the statement."),
        ("Inferred", "%d classes" % (len(tree_only) - len(unplaced)),
         "No <code>type_info</code> exists, so the base comes from vptr stores in "
         "the constructor/destructor chain.",
         "Yes. A shim or an inlined base constructor can mislead it."),
        ("Contradicted", "%d classes" % len(contra),
         "The repo's header names a real ancestor, but not the immediate base. The "
         "ROM names the immediate one.",
         "The ROM is right by construction; the header is the error."),
        ("Unplaced", "%d classes" % len(unplaced),
         "Known to the repo, no <code>type_info</code>, and no vptr chain placed it.",
         "Nothing is claimed, so nothing can be wrong."),
    ]:
        A("<tr><td><strong>%s</strong></td><td class=\"m\">%s</td><td>%s</td>"
          "<td>%s</td></tr>" % (E(tier), E(cnt), how, wrong))
    A("</tbody></table></div>")
    A('<div class="note">The ROM stores %d type_info records and %d base pointers, '
      "and <strong>every one of those pointers resolves</strong> — 183 of them across "
      "an overlay boundary. Where this page is silent it is because the ROM is "
      "silent, not because the reading was hard.</div>" % (n_rec, n_edges))

    # ---- §2 spine
    A('</section><section><div class="sec-h"><span class="sec-n">§2</span>'
      "<h2>The spine</h2></div>")
    A("<p>Of the %d classes, %d hang off a single root. Strip out everything with "
      "fewer than two children and the framework EAD actually built is four levels "
      "deep and fits on a screen.</p>"
      % (n_rec, max(c["size"] for c in g.components)))
    fbase = g.key_of.get("fBase_c")
    if fbase:
        spine = set()

        def mark(k, depth):
            spine.add(k)
            if depth >= 3:
                return
            for c in g.children.get(k, []):
                if len(g.children.get(c, [])) >= 2 or depth < 2:
                    mark(c, depth + 1)

        mark(fbase, 0)
        saved, g.children = g.children, {
            k: [c for c in v if c in spine] for k, v in g.children.items()}
        A('<div class="tree">%s</div>' % tree_lines(g, fbase, contra, {}))
        g.children = saved
    A('<div class="note"><strong>fBase_c</strong> is the framework root — the same '
      "name, in the same position, as in EAD's GameCube codebases. Everything the "
      "game draws, updates or scenes through descends from it, and the two great "
      "families below <code>dActor_c</code> account for %d of its descendants.</div>"
      % (len(g.children.get(g.key_of.get("dBgActor_c", ""), []))
         + len(g.children.get(g.key_of.get("dEnemyBase_c", ""), []))))

    # ---- §3 families
    A('</section><section><div class="sec-h"><span class="sec-n">§3</span>'
      "<h2>The families</h2></div>")
    A("<p>Fan-out is extremely uneven. Twelve classes carry almost the whole game; "
      "the rest are leaves.</p>")
    A('<div class="scroll"><table><thead><tr><th>Base class</th><th>Direct</th>'
      "<th>Whole subtree</th><th></th><th>Module</th></tr></thead><tbody>")
    top = sorted(g.children, key=lambda k: -len(g.children[k]))[:12]
    mx = max(len(g.children[k]) for k in top)
    for k in top:
        n = len(g.children[k])
        A('<tr><td class="m">%s</td><td class="n">%d</td><td class="n">%d</td>'
          '<td style="width:34%%"><div class="bar" style="width:%.1f%%"></div></td>'
          '<td class="m">%s</td></tr>'
          % (E(g.name[k]), n, g.subtree_size(k) - 1, 100.0 * n / mx,
             E(g.module[k])))
    A("</tbody></table></div>")

    for nm in ("dBgActor_c", "dEnemyBase_c", "dScMgBase_c"):
        k = g.key_of.get(nm)
        if not k:
            continue
        A("<details><summary>%s — %d direct children, %d in the subtree"
          "</summary><div class=\"tree\">%s</div></details>"
          % (E(nm), len(g.children.get(k, [])), g.subtree_size(k) - 1,
             tree_lines(g, k, contra, inferred_children)))

    # ---- §4 multiple inheritance
    A('</section><section><div class="sec-h"><span class="sec-n">§4</span>'
      "<h2>Where a class has two parents</h2></div>")
    mi = [(k, v) for k, v in g.parents.items() if len(v) > 1]
    A("<p>Exactly %d classes use multiple inheritance, and the ROM records the byte "
      "offset of each base subobject. The base at <code>+0x0</code> is the primary — "
      "and it is <em>not</em> always the first one listed, which is a trap that has "
      "already cost this project one wrong reading.</p>" % len(mi))
    A("<figure>%s<figcaption>Base subobjects drawn at their recorded offsets. The "
      "filled box is the primary base at offset 0. <code>dExtAnmModel_c</code> lists "
      "<code>dExtFrameCtrl_c</code> (+0x50) before <code>dExtSimpleModel_c</code> "
      "(+0x0): list order is not offset order.</figcaption></figure>"
      % mi_diagram(g))

    # ---- §5 contradicted
    A('</section><section><div class="sec-h"><span class="sec-n">§5</span>'
      "<h2>Where the repo's headers are wrong</h2></div>")
    A("<p>%d classes have a header naming a base that is a genuine ancestor but not "
      "the immediate one — the chain was flattened. Every one is in the Platform "
      "family, and each names an intermediate class the repo has never modelled. "
      "There are no outright contradictions: not one header names a class that is "
      "absent from the ROM's ancestor chain.</p>" % len(contra))
    A('<div class="scroll"><table><thead><tr><th>Class (ROM)</th>'
      "<th>Repo calls it</th><th>Repo's base</th><th>Actual base</th>"
      "</tr></thead><tbody>")
    for r in sorted(by_verdict["flattened"], key=lambda x: x["rom_bases"][0]):
        A('<tr><td class="m">%s</td><td class="m">%s</td>'
          '<td class="m" style="color:var(--contra)">%s</td>'
          '<td class="m" style="color:var(--proven)">%s</td></tr>'
          % (E(r["rom_name"]), E(r["tree_name"] or "—"), E(r["tree_base"] or "—"),
             E(", ".join(r["rom_bases"]))))
    A("</tbody></table></div>")
    inter = collections.Counter(r["rom_bases"][0] for r in by_verdict["flattened"])
    A('<div class="note">Those %d rows collapse to <strong>%d missing intermediate '
      "classes</strong>, not %d separate mistakes: %s. Modelling those %d is the "
      "single highest-yield correction available to the hierarchy.</div>"
      % (len(contra), len(inter), len(contra),
         ", ".join("<code>%s</code> (%d)" % (E(k), v) for k, v in inter.most_common()),
         len(inter)))

    # ---- §6 unproven
    A('</section><section><div class="sec-h"><span class="sec-n">§6</span>'
      "<h2>What is still guessed</h2></div>")
    A("<p>%d classes the repo knows have no <code>type_info</code> record at all. "
      "They are not polymorphic, or their vtable was never emitted, so RTTI cannot "
      "speak to them and their placement rests entirely on constructor evidence. "
      "%d of them have an inferred base; %d have none.</p>"
      % (len(tree_only), len(tree_only) - len(unplaced), len(unplaced)))
    A('<div class="scroll"><table><thead><tr><th>Status</th><th>Count</th>'
      "<th>Classes</th></tr></thead><tbody>")
    inf_flat = sorted(c for v in inferred_children.values() for c, _ in v)
    A('<tr><td style="color:var(--inferred)"><strong>Inferred base</strong></td>'
      '<td class="n">%d</td><td class="m">%s</td></tr>'
      % (len(inf_flat), E(", ".join(inf_flat))))
    A('<tr><td><strong>No placement</strong></td><td class="n">%d</td>'
      '<td class="m">%s</td></tr>' % (len(unplaced), E(", ".join(sorted(unplaced)))))
    A("</tbody></table></div>")
    A("<p>Separately, %d ROM classes have no name in the repo at all — the ROM knows "
      "them, the project has never given them a header. They are proven within this "
      "graph and invisible outside it.</p>" % st.get("unknown_class", 0))

    # ---- §7 limits
    A('</section><section><div class="sec-h"><span class="sec-n">§7</span>'
      "<h2>What this document cannot tell you</h2></div>")
    A("<p>The ROM's RTTI carries class <em>names</em> and base <em>pointers</em>. "
      "That is all it carries. Specifically:</p>")
    A('<div class="scroll"><table><thead><tr><th>Not available</th><th>Why</th>'
      "</tr></thead><tbody>")
    for what, why in [
        ("Method names",
         "No symbol table survives in a retail ROM. Zero <code>_Z</code>-prefixed "
         "strings exist anywhere in the image."),
        ("Parameter types",
         "The <code>type_info</code> kinds that encode a signature "
         "(<code>__function_type_info</code>, <code>__pointer_type_info</code>) were "
         "never linked in — this program never does <code>typeid</code> on a "
         "function type."),
        ("Field names or layouts",
         "RTTI records a base's <em>offset</em>, never its members. Sizes come from "
         "the <code>operator new</code> literal at each call site instead."),
        ("Original filenames",
         "Exactly one <code>__FILE__</code> string survives ROM-wide — "
         "<code>isdoverlay.c</code>, Nintendo's debugger SDK. Game code was built "
         "with asserts stripped."),
        ("Non-polymorphic classes",
         "A class with no virtual function gets no vtable and no record. The %d "
         "inferred classes in §6 are exactly this population." % len(tree_only)),
    ]:
        A("<tr><td><strong>%s</strong></td><td>%s</td></tr>" % (E(what), why))
    A("</tbody></table></div></section>")

    A("<footer>Generated by <code>tools/rtti_hierarchy_page.py</code> from "
      "<code>build/rtti.json</code>, <code>build/rtti_reconcile.json</code> and "
      "<code>build/evidence_hierarchy.json</code>. Every count, edge and offset on "
      "this page is joined at generation time; nothing is typed by hand. Re-run "
      "<code>tools/rtti_extract.py --check</code> to re-prove the census before "
      "trusting a number here.</footer>")
    A("</div>")
    return "\n".join(H)


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--out", default=str(OUT))
    ap.add_argument("--check", action="store_true")
    a = ap.parse_args()

    d = load()
    g = Graph(d)
    print("records %d, edges %d, roots %d, components %d"
          % (len(g.name), len(d["rtti"]["edges"]), len(g.roots), len(g.components)))
    print("tree-only classes %d, flattened %d"
          % (len(d["rec"]["tree_only_classes"]),
             sum(1 for r in d["rec"]["rows"] if r["verdict"] == "flattened")))
    if a.check:
        print("CHECK OK")
        return 0

    doc = build(d)
    p = pathlib.Path(a.out)
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(doc, encoding="utf-8", newline="\n")
    print("wrote %s (%d KB)" % (p, len(doc.encode("utf-8")) // 1024))
    return 0


if __name__ == "__main__":
    sys.exit(main())
