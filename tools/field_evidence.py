"""Show every use of a class's unk_ fields, in its methods AND in its handlers.

A provenance pass that only reads a class's own mangled methods will call almost
everything "written once, never read", because that is only two or three files.
The behaviour lives in unmangled `func_ovNNN_*` handlers that take `this` as a
bare `char *` and poke it by raw offset, so they never mention the field name --
or the class name -- and no grep for the field finds them.

This gathers the three routes to a handler and prints what each one reads:

  called    the class's methods call it with `this` unadjusted, transitively.
            A call passing `this + 0x124` is NOT followed: that hands over a
            member subobject, and what it pokes belongs to that member's class.
  scored    tools/handler_owner.py attributes it to this class decisively --
            the offsets it pokes fit this class's layout and no rival in the
            overlay comes within the tie margin.
  table     neither, but it lives in an overlay the class uses. Actors dispatch
            state handlers through a data table, which names no caller, so this
            is where those land. Weakest route: confirm before believing it.

    python tools/field_evidence.py Bowser
    python tools/field_evidence.py Dorrie --route called,scored

Prints evidence, never verdicts. Read the handler before naming anything.
"""
import argparse
import collections
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
INC, SRC = REPO / "include", REPO / "src"
sys.path.insert(0, str(REPO / "tools"))

STRUCT = re.compile(r"^struct\s+(\w+)\s*(?::\s*(\w+)\s*)?\{", re.M)
FIELD = re.compile(
    r"^\s{2,}([\w:<>\*\s]+?)\**(\w+)\s*(?:\[[^\]]*\])?;\s*/\*\s*(0x[0-9a-fA-F]+)", re.M)
UNK = re.compile(r"^unk_[0-9a-fA-F]+$")
SELF = r"(?:\(\s*(?:unsigned\s+)?char\s*\*\s*\)\s*)?(?:this|c|thiz|self|p)"
CALL = re.compile(r"\b(func_ov\d+_[0-9a-f]+)\s*\(\s*\(*\s*" + SELF + r"\s*\)*\s*[,)]")


def struct_body(text, ob):
    i, d = ob, 0
    while i < len(text):
        if text[i] == "{":
            d += 1
        elif text[i] == "}":
            d -= 1
            if d == 0:
                return text[ob + 1:i]
        i += 1
    return ""


def load_sources():
    out = {}
    for p in list(SRC.rglob("*")) + list((REPO / "src_tu").rglob("*")):
        if p.suffix in (".c", ".cpp"):
            out[p.name] = (p, p.read_text(encoding="utf-8", errors="replace"))
    return out


def unk_fields(cls):
    """{name: (type, offset)} for every unk_ this class declares, either half."""
    out = {}
    for p in sorted(INC.rglob("*.h")):
        t = p.read_text(encoding="utf-8", errors="replace")
        for m in STRUCT.finditer(t):
            if m.group(1) != cls:
                continue
            for dt, fn, off in FIELD.findall(struct_body(t, m.end() - 1)):
                if UNK.match(fn):
                    out[fn] = (dt.strip(), off.lower(), p.name)
    return out


def routes(cls, files):
    own = [n for n in files
           if n.startswith("_ZN%d%s" % (len(cls), cls)) or n[:-4] == cls]
    called, queue = [], list(own)
    seen = set()
    while queue:
        n = queue.pop(0)
        if n in seen or n not in files:
            continue
        seen.add(n)
        if n.startswith("func_"):
            called.append(n)
        for cal in CALL.findall(files[n][1]):
            for ext in (".c", ".cpp"):
                if cal + ext in files:
                    queue.append(cal + ext)

    overlays = set()
    for n in own:
        overlays |= set(re.findall(r"\b(?:data|func)_(ov\d+)_", files[n][1]))

    scored, table = [], []
    try:
        import handler_owner as HO
        occ, sizes = HO.load()
        cls_ov = HO.overlays_of_classes()
    except Exception:
        occ = None
    for n, (p, _t) in files.items():
        m = re.match(r"func_(ov\d+)_", n)
        if not m or m.group(1) not in overlays or n in called:
            continue
        if occ is None:
            table.append(n)
            continue
        try:
            _ov, _np, rows = HO.score(n, occ, sizes, cls_ov)
        except SystemExit:
            continue
        decisive = (rows and rows[0][3] == cls and rows[0][0] > 0
                    and (len(rows) == 1 or rows[0][0] - rows[1][0] >= 6))
        (scored if decisive else table).append(n)
    return own, called, scored, table


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("cls", nargs="+")
    ap.add_argument("--route", default="called,scored,table",
                    help="which handler routes to search (default all)")
    ap.add_argument("--quiet-unused", action="store_true",
                    help="omit fields nothing reads")
    args = ap.parse_args(argv)
    want = {r.strip() for r in args.route.split(",")}

    files = load_sources()
    for cls in args.cls:
        fields = unk_fields(cls)
        own, called, scored, table = routes(cls, files)
        print("=" * 76)
        print("%s -- %d unk_ field(s) | %d method file(s) | handlers: %d called, "
              "%d scored, %d table" % (cls, len(fields), len(own), len(called),
                                       len(scored), len(table)))
        print("=" * 76)
        if not fields:
            continue
        pool = [("method", n) for n in own]
        pool += [("called", n) for n in called if "called" in want]
        pool += [("scored", n) for n in scored if "scored" in want]
        pool += [("table", n) for n in table if "table" in want]

        for fn, (dt, off, hdr) in sorted(fields.items(), key=lambda k: k[1][1]):
            bare = off[2:].lstrip("0") or "0"
            byoff = re.compile(r"\+\s*0x0*%s\b" % bare, re.I)
            byname = re.compile(r"\b%s\b" % fn)
            hits = []
            for route, n in pool:
                for i, line in enumerate(files[n][1].splitlines(), 1):
                    if byname.search(line) or (route != "method" and byoff.search(line)):
                        hits.append((route, n, i, line.strip()))
            if args.quiet_unused and not hits:
                continue
            print("\n--- %s  %-6s %s   (%s) ---" % (off, dt, fn, hdr))
            if not hits:
                print("      (nothing reads or writes it)")
            for route, n, i, line in hits[:14]:
                print("  %-7s %s:%d: %s" % (route, n, i, line[:120]))
            if len(hits) > 14:
                print("      ... %d more" % (len(hits) - 14))
        print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
