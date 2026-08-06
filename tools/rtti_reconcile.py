#!/usr/bin/env python3
"""Pass 4: what the ROM says about the class graph vs. what the tree believes.

Reads build/rtti.json (tools/rtti_extract.py -- the ROM's own type_info records) and
build/evidence_hierarchy.json (tools/evidence_hierarchy.py -- inheritance reconstructed
from ctor/dtor vptr chains), and reports every place they disagree.

This pass proposes nothing and edits nothing.  Its whole job is to size the problem:
before it existed, nobody knew whether the tree's hierarchy was wrong in three places
or three hundred, and every downstream decision -- which headers to re-parent, whether
a rename is worth it -- depends on that number.

Joining the two
---------------
The two sides do not share a vocabulary.  The ROM knows `dActor_c`; the tree knows
`Actor`.  The join is the **vtable address**, which both sides can name:

    rtti.json record  ->  vtable VA  ->  config/**/symbols.txt  ->  _ZTV<TreeName>

A record whose vtable is still a `data_` placeholder means the tree has no name for
that class at all -- it is not a disagreement, it is an absence, and the two are
counted separately.  Conflating them would inflate the disagreement count with
classes nobody has ever looked at.

Why the vtable join alone is not enough
---------------------------------------
The first draft joined on vtables only and reported 60 disagreements.  Most were not
disagreements.  The tree calls `dEnemyBase_c` "Enemy" -- but `dEnemyBase_c`'s vtable is
still an unnamed placeholder, so the vtable join could not see that "Enemy" and
`dEnemyBase_c` are the same class, and every one of its 55 children looked like a
conflict.  Reporting that number would have been worse than reporting nothing.

So the name map is **extended structurally, then corroborated**.  If the vtable join
already proves Goomba == `daKrb_c`, and the tree believes `Goomba : CapEnemy` while the
ROM says `daKrb_c : dCapEnemy_c`, that is one vote for CapEnemy == `dCapEnemy_c`.  An
alias is accepted only when every vote agrees; contradicted ones are reported, never
silently picked.  Aliases carry their vote count so a 1-vote alias can be told from a
55-vote one.

Verdicts
--------
agree              tree's base == ROM's base (after name translation)
agree_primary_only ROM has several bases, tree has the first right and omits the rest
disagree           tree names a different base -- the actionable bucket
no_belief          tree knows the class but evidence_hierarchy placed no base for it
unknown_class      no _ZTV symbol: the tree has never named this class
root_both          ROM says root, tree says root
root_rom_only      ROM says root, tree claims a base -- the tree invented an edge
"""

from __future__ import annotations

import argparse
import collections
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
SYM_RE = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:(0x[0-9a-fA-F]+)")
_LEN = re.compile(r"(\d+)")


def split_nested(s):
    parts, i = [], 0
    while i < len(s):
        m = _LEN.match(s, i)
        if not m:
            return None
        n = int(m.group(1))
        i = m.end()
        if i + n > len(s):
            return None
        parts.append(s[i:i + n])
        i += n
    return parts or None


def ztv_to_class(sym):
    """_ZTV5Actor -> Actor;  _ZTVN5dPa_c7level_cE -> dPa_c::level_c."""
    body = sym[4:]
    if body.startswith("N") and body.endswith("E"):
        body = body[1:-1]
    p = split_nested(body)
    return "::".join(p) if p else None


def load_symbols():
    """module -> addr -> [names].  Module name matches rtti_extract's."""
    out = collections.defaultdict(lambda: collections.defaultdict(list))
    for p in (REPO / "config").rglob("symbols.txt"):
        mod = p.parent.name
        for line in p.read_text(encoding="utf-8", errors="replace").splitlines():
            m = SYM_RE.match(line.strip())
            if m:
                out[mod][int(m.group(3), 16)].append(m.group(1))
    return out


def build(rtti_path, eh_path):
    rtti = json.loads(pathlib.Path(rtti_path).read_text(encoding="utf-8"))
    eh = json.loads(pathlib.Path(eh_path).read_text(encoding="utf-8"))
    hier = eh["hierarchy"]
    syms = load_symbols()

    # ---- ROM class -> tree name, via the vtable address --------------------
    tree_name = {}          # rtti record key -> tree's class name
    for key, r in rtti["records"].items():
        if not r["vtable"]:
            continue
        for n in sorted(syms.get(r["module"], {}).get(int(r["vtable"], 16), [])):
            if n.startswith("_ZTV"):
                c = ztv_to_class(n)
                if c:
                    tree_name[key] = c
                    break

    rom_name = {k: r["name"] for k, r in rtti["records"].items()}
    stats_alias = collections.Counter()
    stats_alias["aliases_from_vtable_symbol"] = len(set(tree_name.values()))

    parents = collections.defaultdict(list)
    for e in rtti["edges"]:
        parents[e["derived_key"]].append(e)

    # ---- extend the name map structurally, then corroborate ---------------
    # A tree name that is not itself a ROM name, sitting where the ROM names a
    # specific base, is a vote that the two are the same class.
    votes = collections.defaultdict(collections.Counter)
    rom_names_all = set(rom_name.values())
    for key, tn in sorted(tree_name.items()):
        ent = hier.get(tn)
        if not ent or not ent.get("base"):
            continue
        believed = ent["base"]
        if believed in rom_names_all:
            continue                      # already speaks the ROM's vocabulary
        edges = parents.get(key, [])
        if len(edges) != 1:
            continue                      # ambiguous under multiple inheritance
        votes[believed][rom_name[edges[0]["base_key"]]] += 1

    # Plurality with a margin, not unanimity.  A unanimity rule rejected the two
    # most important aliases: "Enemy" got 34 votes for dEnemyBase_c and 2 for
    # daOts_c, "Platform" 46 for dBgActor_c against 8 rivals with 1-3 each.  The
    # minority votes are not noise disproving the alias -- they are the finding.
    # Each one is a class where the tree named an ancestor instead of the
    # immediate base, which is what evidence_hierarchy.py's docstring predicts
    # shims do ("routinely flatten the chain").  Take the plurality, then let the
    # ancestor test below classify the minority.
    alias = {}                            # tree name -> (ROM name, votes)
    contested = {}
    for tn, c in sorted(votes.items()):
        ranked = c.most_common()
        top, n = ranked[0]
        runner = ranked[1][1] if len(ranked) > 1 else 0
        if n >= 3 and n > 3 * runner:
            alias[tn] = (top, n)
        elif len(ranked) == 1:
            alias[tn] = (top, n)
        else:
            contested[tn] = dict(c)
    stats_alias["aliases_inferred_from_edges"] = len(alias)
    stats_alias["aliases_contested_and_rejected"] = len(contested)

    # ---- ROM ancestor sets, for telling "flattened" from "wrong" -----------
    rom_parents = {k: [e["base_key"] for e in v] for k, v in parents.items()}

    def rom_ancestors(key):
        out, stack, seen = set(), list(rom_parents.get(key, [])), set()
        while stack:
            k = stack.pop()
            if k in seen:
                continue
            seen.add(k)
            out.add(rom_name[k])
            stack.extend(rom_parents.get(k, []))
        return out

    def names_for(key):
        """Every spelling that means this ROM class."""
        out = {rom_name[key]}
        if key in tree_name:
            out.add(tree_name[key])
        return out

    # every tree-side name we can translate into the ROM's vocabulary
    translatable = set(tree_name.values()) | set(alias) | rom_names_all

    def matches(believed, base_key):
        if believed in names_for(base_key):
            return True
        a = alias.get(believed)
        return bool(a and a[0] == rom_name[base_key])

    stats = collections.Counter()
    rows = []
    for key in sorted(rtti["records"]):
        rn = rom_name[key]
        tn = tree_name.get(key)
        edges = parents.get(key, [])
        rom_bases = [e["base_key"] for e in edges]
        rom_base_names = [rom_name[b] for b in rom_bases]

        if tn is None:
            verdict = "unknown_class"
            believed = None
        else:
            ent = hier.get(tn)
            believed = ent["base"] if ent else None
            if not rom_bases:
                verdict = "root_both" if believed is None else "root_rom_only"
            elif believed is None:
                verdict = "no_belief"
            elif matches(believed, rom_bases[0]) and len(rom_bases) > 1:
                verdict = "agree_primary_only"
            elif any(matches(believed, b) for b in rom_bases):
                verdict = "agree"
            elif believed not in translatable:
                # The tree's base name has no proven ROM counterpart, so nothing
                # can be concluded either way.  `SphereClsn : BgCh` lands here:
                # include/BgCh.h exists, but dBgCh's vtable (0x020991d8) is still
                # an unnamed data_ placeholder, so no evidence chain reaches from
                # "BgCh" to `dBgCh`.  Calling that a disagreement would be a
                # false positive dressed as a finding; Phase 1 naming the vtable
                # resolves it either way.
                verdict = "unproven_name"
            else:
                # Is the tree's base a true ancestor, just not the immediate one?
                anc = rom_ancestors(key)
                spelled = {believed}
                a = alias.get(believed)
                if a:
                    spelled.add(a[0])
                verdict = "flattened" if (spelled & anc) else "disagree"
        stats[verdict] += 1
        rows.append({
            "key": key,
            "module": rtti["records"][key]["module"],
            "rom_name": rn,
            "tree_name": tn,
            "rom_bases": rom_base_names,
            "tree_base": believed,
            "tree_confidence": (hier.get(tn) or {}).get("confidence") if tn else None,
            "tree_sources": (hier.get(tn) or {}).get("sources") if tn else None,
            "verdict": verdict,
            "vtable": rtti["records"][key]["vtable"],
        })

    # classes the tree believes in that the ROM has no record for
    known_tree = {v for v in tree_name.values()}
    orphan = sorted(c for c in hier if c not in known_tree)
    stats["tree_classes_with_no_rom_record"] = len(orphan)
    stats["rom_records"] = len(rtti["records"])
    stats["rom_edges"] = len(rtti["edges"])
    stats["tree_classes_named_by_ztv"] = len(tree_name)
    stats.update(stats_alias)

    return {"stats": dict(sorted(stats.items())), "rows": rows,
            "tree_only_classes": orphan,
            "aliases": {k: {"rom_name": v[0], "votes": v[1]}
                        for k, v in sorted(alias.items())},
            "aliases_contested": contested}


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--rtti", default=str(REPO / "build" / "rtti.json"))
    ap.add_argument("--evidence", default=str(REPO / "build" / "evidence_hierarchy.json"))
    ap.add_argument("--out", default=str(REPO / "build" / "rtti_reconcile.json"))
    ap.add_argument("--report", action="store_true")
    a = ap.parse_args()

    res = build(a.rtti, a.evidence)
    pathlib.Path(a.out).write_text(
        json.dumps(res, indent=1) + "\n", encoding="utf-8", newline="\n")

    st = res["stats"]
    if a.report:
        print("== verdicts ==")
        for k in sorted(st):
            print("  %-40s %d" % (k, st[k]))
        dis = [r for r in res["rows"] if r["verdict"] == "disagree"]
        print("\n== disagreements (%d) ==" % len(dis))
        for r in dis:
            print("  %-22s tree:%-20s ROM base:%-22s tree base:%-20s (%s, %s)"
                  % (r["rom_name"], r["tree_name"], ",".join(r["rom_bases"]),
                     r["tree_base"], r["tree_confidence"],
                     ",".join(r["tree_sources"] or [])))
        rro = [r for r in res["rows"] if r["verdict"] == "root_rom_only"]
        print("\n== tree invented a base for a ROM root (%d) ==" % len(rro))
        for r in rro:
            print("  %-22s tree:%-20s tree base:%s (%s)"
                  % (r["rom_name"], r["tree_name"], r["tree_base"],
                     r["tree_confidence"]))
    print("wrote %s" % a.out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
