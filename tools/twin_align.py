"""Keep a flat C twin spelling its fields the way the real C++ chain spells them.

Several headers carry the same class twice: a real `struct X : Base` for the C++
half, and a base-less `struct X` in the `#else` branch that restates the inherited
fields flat, because some C translation unit (usually a compiler-generated D0)
reads them. Nothing keeps the two halves in step, so the flat side drifts -- it
leaves a field `unk_098` that `dActor_c` has called `mHorzSpeed` all along, or
worse, invents a second name for it (`mMoveSpeed`). Two headers describing one
class under two sets of names is a trap for whoever reads them next.

This walks every base-less struct that shares its name with a based one, resolves
each offset through the based one's inheritance chain, and reports (or fixes) the
`unk_NNN` fields the chain already names. Renaming a field cannot change codegen,
so `--apply` is byte-neutral by construction -- but it can strand the old spelling
in a `.c` body, so run the full rombuild afterwards either way.

    python tools/twin_align.py             # report
    python tools/twin_align.py --check     # exit 1 if any twin has drifted
    python tools/twin_align.py --apply     # rewrite the headers
"""
import argparse
import collections
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
INC = REPO / "include"

STRUCT = re.compile(r"^struct\s+(\w+)\s*(?::\s*(\w+)\s*)?\{", re.M)
FIELD = re.compile(
    r"^\s{2,}[\w:<>\*\s]+?\**(\w+)\s*(?:\[[^\]]*\])?;\s*/\*\s*(0x[0-9a-fA-F]+)", re.M)
PLACEHOLDER = ("unk_", "pad_", "field_")


def struct_body(text, open_brace):
    """Text between the struct's braces, matching nesting."""
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
    for p in sorted(INC.glob("*.h")):
        t = p.read_text(encoding="utf-8", errors="replace")
        for m in STRUCT.finditer(t):
            body = struct_body(t, m.end() - 1)
            occ[m.group(1)].append({
                "file": p.name,
                "base": m.group(2),
                "fields": {off.lower(): fn for fn, off in FIELD.findall(body)},
            })
    return occ


def chain_map(occ, name, seen=None):
    """offset -> name, walking the class and its bases. Nearest definition wins."""
    seen = seen or set()
    if name in seen:
        return {}
    seen.add(name)
    out = {}
    for e in occ.get(name, []):
        if e["base"]:
            out.update(chain_map(occ, e["base"], seen))
        for off, fn in e["fields"].items():
            if not fn.startswith(PLACEHOLDER):
                out[off] = fn
    return out


def drifted(occ):
    rows = []
    for name, entries in occ.items():
        based = [e for e in entries if e["base"]]
        flat = [e for e in entries if not e["base"]]
        if not based or not flat:
            continue
        cmap = chain_map(occ, name)
        for e in flat:
            for off, fn in sorted(e["fields"].items()):
                if fn.startswith("unk_") and off in cmap:
                    rows.append((e["file"], name, off, fn, cmap[off]))
    return rows


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    g = ap.add_mutually_exclusive_group()
    g.add_argument("--check", action="store_true", help="exit 1 if any twin has drifted")
    g.add_argument("--apply", action="store_true", help="rewrite the headers")
    args = ap.parse_args(argv)

    rows = drifted(parse())
    if not rows:
        print("twin_align: every flat twin agrees with its chain.")
        return 0

    by_file = collections.defaultdict(list)
    for f, name, off, old, new in rows:
        by_file[f].append((name, off, old, new))

    print("%d unk_ field(s) in a flat twin that the real chain already names\n" % len(rows))
    for f in sorted(by_file, key=lambda k: (-len(by_file[k]), k)):
        print("%s  (%d)" % (f, len(by_file[f])))
        for _, off, old, new in by_file[f]:
            print("    %s  %-12s -> %s" % (off, old, new))

    if args.check:
        print("\ntwin_align: FAIL -- run --apply, then the full rombuild.")
        return 1
    if not args.apply:
        print("\n(report only -- pass --apply to write)")
        return 0

    changed = 0
    for f, items in by_file.items():
        p = INC / f
        t = p.read_text(encoding="utf-8")
        for _, off, old, new in items:
            pat = re.compile(
                r"\b" + re.escape(old) + r"\b(?=\s*(?:\[[^\]]*\])?;\s*/\*\s*"
                + re.escape(off) + r")", re.I)
            t, n = pat.subn(new, t)
            changed += n
        p.write_text(t, encoding="utf-8", newline="\n")
    print("\napplied %d rename(s) across %d header(s)." % (changed, len(by_file)))
    print("A .c body may still spell the old name -- run tools/rombuild.py now.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
