"""Name a base-less struct's fields the way its real ancestors already name them.

`tools/twin_align.py` fixes the case where one header declares a class twice and
the flat half drifts from the based half. Most generated headers have no based
half at all: they declare `struct daObjPile_c { ... }` with every inherited field
restated flat and left as `unk_098`, even though the ROM's RTTI proves the class
derives from `dActor_c`, whose own header has called that offset `mHorzSpeed` all
along. The name is already known; the header just does not say so.

This resolves each base-less struct's offsets through the chain in
`build/evidence_hierarchy.json` -- reconstructed from ROM typeinfo and the
hand-verified reference headers -- and renames the `unk_` fields the chain names.
Because the struct has no base clause there is no inheritance to shadow: the
field stays exactly where it was, spelled the way the rest of the tree spells it.

    python tools/rtti_extract.py && python tools/evidence_hierarchy.py
    python tools/chain_align.py             # report
    python tools/chain_align.py --check     # exit 1 if any base-less struct has drifted
    python tools/chain_align.py --apply     # rewrite the headers

Guards, each of which cost a real candidate when it was added:

  * `pad_`/`field_` names are never touched. They are span arrays
    (`u8 pad_000[0x00c - 0x000];`) whose trailing comment marks where the span
    *ends*, so matching that comment against an ancestor field at that offset
    pairs a byte array with a scalar. 21 of the first 90 candidates were this.
  * an array declarator is never renamed to a scalar ancestor field.
  * the widths must agree where both are known, and a non-scalar must match the
    ancestor's type spelling -- a `Model` at 0x0d4 is only the base's `mModel` if
    it is still a `Model`.
  * only `high`-confidence chains are used.
  * a rename that would collide with a name the struct already declares is
    dropped, not applied: two members cannot share a spelling, and the collision
    means the offset story is wrong somewhere.

Renaming cannot change codegen, so `--apply` is byte-neutral by construction --
but it can strand the old spelling in a `.c` body, which only the full rombuild
catches. Always run `tools/rombuild.py` afterwards.
"""
import argparse
import collections
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
INC = REPO / "include"
HIER = REPO / "build" / "evidence_hierarchy.json"

STRUCT = re.compile(r"^struct\s+(\w+)\s*(?::\s*(\w+)\s*)?\{", re.M)
FIELD = re.compile(
    r"^\s{2,}([\w:<>\*\s]+?)(\**)\s*(\w+)\s*(\[[^\]]*\])?;\s*/\*\s*(0x[0-9a-fA-F]+)", re.M)
UNK = re.compile(r"^unk_[0-9a-fA-F]+$")
PLACEHOLDER = ("unk_", "pad_", "field_")

WIDTH = {'u8': 1, 's8': 1, 'char': 1, 'bool': 1, 'u16': 2, 's16': 2, 'short': 2,
         'u32': 4, 's32': 4, 'int': 4, 'long': 4, 'float': 4}


def width(decl):
    for tok in decl.replace('unsigned', '').replace('signed', '').split():
        if tok in WIDTH:
            return WIDTH[tok]
    return None


def struct_body(text, open_brace):
    i, depth = open_brace, 0
    while i < len(text):
        if text[i] == "{":
            depth += 1
        elif text[i] == "}":
            depth -= 1
            if depth == 0:
                return text[open_brace + 1:i]
        i += 1
    return ""


def parse():
    occ = collections.defaultdict(list)
    for p in sorted(INC.rglob("*.h")):
        t = p.read_text(encoding="utf-8", errors="replace")
        for m in STRUCT.finditer(t):
            fields = []
            for dt, stars, fn, arr, off in FIELD.findall(struct_body(t, m.end() - 1)):
                fields.append({"type": dt.strip(), "ptr": bool(stars), "name": fn,
                               "array": bool(arr), "off": int(off, 16)})
            occ[m.group(1)].append({"file": p, "base": m.group(2), "fields": fields})
    return occ


SHADOW_CAST = re.compile(r"struct\s+(\w+)\s*\*\s*\w+\s*=\s*\(\s*struct\s+(\w+)\s*\*\s*\)"
                         r"\s*\(\s*void\s*\*\s*\)\s*c\b")
THIS_C = re.compile(r"\bc\s*=\s*\(\s*char\s*\*\s*\)\s*this\b")


def shadow_pairs():
    """shadow struct -> the class it restates, evidenced by the cast site.

    A recovered body writes `char *c = (char *)this;` and then
    `struct daKpa3Bg_c *self = (struct daKpa3Bg_c *)(void *)c;` inside
    BowserSkyPlatform::CleanupResources. Casting `this` at offset zero to the
    shadow type IS the identity claim: daKpa3Bg_c is what the ROM's RTTI calls
    the class this tree named BowserSkyPlatform, restated flat so a C body can
    reach its fields by name. Such a shadow has no hierarchy entry of its own --
    the RTTI name never reaches build/rtti.json -- but it inherits exactly what
    the class it shadows inherits.

    A shadow cast from two different classes is dropped: the pairing has to be
    unambiguous to carry names.
    """
    seen = collections.defaultdict(collections.Counter)
    for p in (REPO / "src").rglob("*"):
        if p.suffix not in (".c", ".cpp"):
            continue
        m = re.match(r"_ZN(\d+)", p.name)
        if not m:
            continue
        real = p.name[len(m.group(0)):][:int(m.group(1))]
        t = p.read_text(encoding="utf-8", errors="replace")
        if not THIS_C.search(t):
            continue
        for a, b in SHADOW_CAST.findall(t):
            if a == b and a != real:
                seen[a][real] += 1
    return {s: next(iter(r)) for s, r in seen.items() if len(r) == 1}


def ancestor_names(occ, hier, cls, shadows):
    """offset -> field, from whatever the class inherits. Nearest source wins."""
    entry = hier.get(cls)
    if entry and entry.get("confidence") == "high":
        sources, chain = list(reversed(entry["chain"])), entry["chain"]
    else:
        real = shadows.get(cls)
        entry = hier.get(real) if real else None
        if not entry or entry.get("confidence") != "high":
            return {}, None
        # The shadow restates the WHOLE object, so the class it shadows supplies
        # names too -- and being nearest, its own fields win over its bases'.
        sources = list(reversed(entry["chain"])) + [real]
        chain = entry["chain"]
    out = {}
    for anc in sources:
        for e in occ.get(anc, []):
            for f in e["fields"]:
                if not f["name"].startswith(PLACEHOLDER):
                    out[f["off"]] = dict(f, owner=anc)
    return out, chain


def compatible(f, base):
    if f["array"] or base["array"]:
        return False
    if f["ptr"] != base["ptr"]:
        return False
    fw, bw = width(f["type"]), width(base["type"])
    if fw is not None and bw is not None:
        return fw == bw
    if fw is None and bw is None:
        return f["type"] == base["type"]
    return False


def drifted(occ, hier):
    shadows = shadow_pairs()
    rows = []
    for cls, entries in occ.items():
        cmap, chain = ancestor_names(occ, hier, cls, shadows)
        if not cmap:
            continue
        for e in entries:
            if e["base"]:
                continue
            taken = {f["name"] for f in e["fields"]}
            for f in e["fields"]:
                if not UNK.match(f["name"]) or f["off"] not in cmap:
                    continue
                base = cmap[f["off"]]
                if not compatible(f, base) or base["name"] in taken:
                    continue
                rows.append((e["file"], cls, f["off"], f["name"],
                             base["name"], base["owner"]))
                taken.add(base["name"])
    return rows


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    g = ap.add_mutually_exclusive_group()
    g.add_argument("--check", action="store_true", help="exit 1 if any struct has drifted")
    g.add_argument("--apply", action="store_true", help="rewrite the headers")
    args = ap.parse_args(argv)

    if not HIER.exists():
        print("chain_align: %s is missing -- run tools/rtti_extract.py then "
              "tools/evidence_hierarchy.py first." % HIER.relative_to(REPO))
        return 2
    hier = json.loads(HIER.read_text(encoding="utf-8"))["hierarchy"]

    rows = drifted(parse(), hier)
    if not rows:
        print("chain_align: every base-less struct agrees with its chain.")
        return 0

    by_file = collections.defaultdict(list)
    for p, cls, off, old, new, owner in rows:
        by_file[p].append((cls, off, old, new, owner))

    print("%d unk_ field(s) in a base-less struct that the class's own chain names\n"
          % len(rows))
    for p in sorted(by_file, key=lambda k: (-len(by_file[k]), k.name)):
        print("%s  (%d)" % (p.relative_to(INC), len(by_file[p])))
        for cls, off, old, new, owner in sorted(by_file[p], key=lambda r: r[1]):
            print("    0x%03x  %-14s -> %-24s (%s)" % (off, old, new, owner))

    if args.check:
        print("\nchain_align: FAIL -- run --apply, then the full rombuild.")
        return 1
    if not args.apply:
        print("\n(report only -- pass --apply to write)")
        return 0

    changed = 0
    for p, items in by_file.items():
        t = p.read_text(encoding="utf-8")
        for cls, off, old, new, owner in items:
            pat = re.compile(
                r"\b" + re.escape(old) + r"\b(?=\s*;\s*/\*\s*0x0*%x\b)" % off, re.I)
            t, n = pat.subn(new, t)
            changed += n
        p.write_text(t, encoding="utf-8", newline="\n")
    print("\napplied %d rename(s) across %d header(s)." % (changed, len(by_file)))
    print("A .c body may still spell the old name -- run tools/rombuild.py now.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
