#!/usr/bin/env python3
"""Candidate original filenames for the ROM's 429 RTTI classes, and what they imply.

EAD's class names encode a filesystem convention that the GameCube Zelda codebases
document: a lowercase prefix naming the layer, CamelCase for the rest, `_c` for
"class".  Later NSMBW source corroborates `dScBoot_c` -> `d_s_boot.cpp` and
`daObjFruitTree_c` -> `d_a_obj_fruit_tree.cpp`.  This pass applies the convention to
every RTTI class name and joins the result onto the recovered translation-unit map.

WHAT IS EVIDENCE HERE AND WHAT IS NOT
-------------------------------------
The filenames are a HYPOTHESIS.  This ROM carries no `__FILE__` evidence to confirm
them: a sweep of every printable string in all 104 modules finds exactly one source
filename, `isdoverlay.c` at 0x020868fc, which is Nintendo's IS-Debugger SDK and not
game code.  Everything else was built with asserts stripped.  So `d_a_krb.cpp` is a
proposal about naming, not a recovered fact, and this tool never writes it anywhere
the build can see.

What IS testable is the GROUPING the convention implies, and that is what `--check`
measures.  Two independent predictions:

  1. Classes sharing a derived stem should share a TU.
  2. A TU carrying several classes should carry RELATED ones -- same prefix family,
     usually a base and its derived, or a thing and its accessory.

Prediction 2 is where the signal is, because `tu_map.py` derives its boundaries from
address intervals and knows nothing about names.  Measured over the 608 recovered
TUs, 20 of the 21 multi-class TUs pair classes that are obviously related under
their ORIGINAL names -- and frequently more obviously than under the tree's coined
English ones:

    daChoropu_c + daChoro_Rock_c      (tree: MontyMole + MontyMoleRock)
    daStar_c    + daStarBase_c        (tree: PowerStar + PowerStarBase)
    daKpa_c     + daKpaTail_c         (tree: Bowser + BowserTail)
    daDossy_c   + daDossyCap_c        (tree: Dorrie + daDossyCap_c)
    daBook_c    + daBookGen_c         (tree: BookShot + BookShotSpawner)

That is an independent corroboration of the interval union-find, arrived at from
name evidence the clustering never saw.  The single exception is main's collapsed
unit, which is a known defect and is treated in `tu_map.py --split-swallowers`.

NESTED CLASSES TAKE THEIR OUTER CLASS'S FILE
--------------------------------------------
`dScMgBase_c::graphCallback_c` is not its own translation unit; it is a helper
declared inside `dScMgBase_c`.  Deriving a stem from the leaf alone gives
`graph_callback` for five different scene classes and collides them.  The outer
component is what names the file.

Usage:
    python tools/tu_names.py                 # write build/tu_names.json
    python tools/tu_names.py --report        # human table
    python tools/tu_names.py --check         # run the two grouping predictions
"""
from __future__ import annotations

import argparse
import collections
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
RTTI = REPO / "build" / "rtti.json"
RECONCILE = REPO / "build" / "rtti_reconcile.json"
TUMAP = REPO / "build" / "tu_map.json"
OUT = REPO / "build" / "tu_names.json"

# Class prefix -> path stem.  Order matters: specific prefixes precede `d`.
# Counts are over the 429 records, so an unlisted prefix is not merely unhandled,
# it does not occur.
PREFIX = [
    ("dSc", "d_s", "scene"),        # NSMBW: dScBoot_c -> d_s_boot.cpp
    ("da", "d_a", "actor"),          # 282
    ("d", "d", "game layer"),        # 101
    ("c", "c", "common/library"),    # 12
    ("f", "f", "framework"),         # 1  (fBase_c)
    ("m", "m", "memory"),            # mHeap::*
]


def snake(s):
    """CamelCase -> snake_case, preserving underscores already present."""
    s = re.sub(r"(?<=[a-z0-9])(?=[A-Z])", "_", s)
    s = re.sub(r"(?<=[A-Z])(?=[A-Z][a-z])", "_", s)
    return re.sub(r"_+", "_", s).lower().strip("_")


def candidate_stem(rom_name):
    """`daObjKaitendai_c` -> `d_a_obj_kaitendai`.

    Nested names resolve against their OUTER component: a nested helper shares its
    outer class's file rather than claiming one of its own."""
    outer = rom_name.split("::")[0]
    body = re.sub(r"_[ct]$", "", outer)
    for pre, path, _why in PREFIX:
        # `[A-Z0-9]` not `[A-Z]`: da1up_c is a real class and must not fall through
        # to the bare-snake branch, which would yield `da1up` and lose the family.
        if re.match(r"^%s(?=[A-Z0-9])" % pre, body):
            return "%s_%s" % (path, snake(body[len(pre):]))
    return snake(body)


def family(rom_name):
    """The lowercase prefix, or None when the name does not use the convention."""
    leaf = rom_name.split("::")[0]
    m = re.match(r"^([a-z]+)(?=[A-Z0-9])", leaf)
    return m.group(1) if m else None


def load():
    for p in (RTTI, RECONCILE, TUMAP):
        if not p.is_file():
            sys.exit("missing %s -- run rtti_extract.py, rtti_reconcile.py, tu_map.py"
                     % p.relative_to(REPO))
    rtti = json.loads(RTTI.read_text(encoding="utf-8"))
    rec = json.loads(RECONCILE.read_text(encoding="utf-8"))
    tumap = json.loads(TUMAP.read_text(encoding="utf-8"))
    return rtti, rec, tumap


def build():
    rtti, rec, tumap = load()

    tree2rom = {r["tree_name"]: r["rom_name"] for r in rec["rows"]
                if r["tree_name"] and r["rom_name"]}

    names = sorted({r["name"] for r in rtti["records"].values()})
    classes = {}
    for n in names:
        classes[n] = {
            "stem": candidate_stem(n),
            "family": family(n),
            "nested_in": n.split("::")[0] if "::" in n else None,
            "tree_name": next((t for t, r in tree2rom.items() if r == n), None),
        }

    # Which classes land in which recovered TU, translated into ROM names.
    units = []
    for modname, mod in sorted(tumap["modules"].items()):
        for u in mod["units"]:
            # Two tree names can join to one ROM class (a class and its `_Spawn`
            # veneer share a vtable address), so dedupe while keeping order.
            roms, seen = [], set()
            for c in u.get("classes", []):
                r = tree2rom.get(c)
                if r and r not in seen:
                    seen.add(r)
                    roms.append(r)
            units.append({
                "module": modname,
                "start": u["start"],
                "end": u["end"],
                "functions": len(u["functions"]),
                "tree_classes": u.get("classes", []),
                "rom_classes": roms,
                "stems": sorted({classes[r]["stem"] for r in roms if r in classes}),
                "families": sorted({classes[r]["family"] for r in roms
                                    if r in classes and classes[r]["family"]}),
            })

    stats = collections.Counter()
    stats["rtti_classes"] = len(classes)
    stats["distinct_stems"] = len({c["stem"] for c in classes.values()})
    stats["recovered_tus"] = len(units)
    stats["tus_with_a_rom_named_class"] = sum(1 for u in units if u["rom_classes"])
    stats["tus_namable_from_rtti"] = sum(1 for u in units if len(u["stems"]) == 1)
    return {"stats": dict(sorted(stats.items())), "classes": classes, "units": units}


def check(res):
    """The two grouping predictions.  Returns the number of failures."""
    bad = 0

    print("== prediction 1: classes sharing a stem share a TU ==")
    by_stem = collections.defaultdict(list)
    for n, c in res["classes"].items():
        by_stem[c["stem"]].append(n)
    shared = {s: v for s, v in by_stem.items() if len(v) > 1}
    loc = {}
    for i, u in enumerate(res["units"]):
        for r in u["rom_classes"]:
            loc[r] = (u["module"], u["start"])
    if not shared:
        print("  no stem is claimed by more than one class -- nothing to test")
    for s, v in sorted(shared.items()):
        where = {loc.get(x) for x in v if x in loc}
        where.discard(None)
        verdict = "same TU" if len(where) == 1 else ("SPLIT" if len(where) > 1
                                                     else "not placed")
        if len(where) > 1:
            bad += 1
        print("  %-24s %-11s %s" % (s, verdict, ", ".join(v)))

    print("\n== prediction 2: a multi-class TU carries related classes ==")
    multi = [u for u in res["units"] if len(u["rom_classes"]) > 1]
    same_fam = [u for u in multi if len(u["families"]) == 1]
    print("  multi-class TUs: %d;  single prefix family: %d;  mixed: %d"
          % (len(multi), len(same_fam), len(multi) - len(same_fam)))
    for u in multi:
        if len(u["families"]) != 1:
            bad += 1
            print("  MIXED  %-7s %-11s %d classes: %s"
                  % (u["module"], u["start"], len(u["rom_classes"]),
                     ", ".join(u["rom_classes"][:8])))
    for u in same_fam:
        print("  ok     %-7s %-11s %s"
              % (u["module"], u["start"], ", ".join(u["rom_classes"])))
    return bad


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--out", default=str(OUT))
    ap.add_argument("--report", action="store_true")
    ap.add_argument("--check", action="store_true")
    a = ap.parse_args()

    res = build()

    if a.report:
        print("== stats ==")
        for k, v in res["stats"].items():
            print("  %-34s %d" % (k, v))
        print("\n== derived filenames (first 30) ==")
        for n in sorted(res["classes"])[:30]:
            c = res["classes"][n]
            print("  %-30s -> %s.cpp%s"
                  % (n, c["stem"], "  [nested]" if c["nested_in"] else ""))

    rc = 0
    if a.check:
        rc = 1 if check(res) else 0

    pathlib.Path(a.out).parent.mkdir(parents=True, exist_ok=True)
    pathlib.Path(a.out).write_text(
        json.dumps(res, indent=1, sort_keys=False) + "\n",
        encoding="utf-8", newline="\n")
    print("\nwrote %s: %d classes, %d stems, %d TUs"
          % (a.out, res["stats"]["rtti_classes"], res["stats"]["distinct_stems"],
             res["stats"]["recovered_tus"]))
    return rc


if __name__ == "__main__":
    sys.exit(main())
