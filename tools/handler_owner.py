"""Which class does an unmangled func_ovNNN_* state handler belong to?

The handlers take `this` as a bare `char *c` and poke it by raw offset, so nothing
in the file says which actor it is. That is what makes
`tools/residue_reaudit.py --overlay` over-report: two classes in one overlay both
have a `+0x438`, and only one of them means what the handler means by it.

Scores every class in the handler's overlay by how well the offsets it pokes match
that class's declared layout, inherited fields included: an offset must fall inside
the struct, and where the access width is visible (`*(short*)`, `*(u8*)`,
`*(int*)`) it must agree with the width declared there. An offset the class does
not declare at all counts against it, because a class that owns the handler should
have every field the handler touches.

    python tools/handler_owner.py func_ov084_0212abd4.c

A decisive win (the top score far above the rest, and positive) is usable
evidence; a near tie is not -- fall back to reading the handler.

Measured against the call graph on ov084, 2026-08-24: agrees wherever the two
overlap, and settles cases the graph cannot see because the actor dispatches its
handlers through a data table. It resolved `+0x438` between daKrb_c (a
`ClosestPlayer()` pointer) and PiranhaPlant (the middle word of a Vector3).
"""
import argparse
import collections
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
INC, SRC = REPO / "include", REPO / "src"

STRUCT = re.compile(r"^struct\s+(\w+)\s*(?::\s*(\w+)\s*)?\{", re.M)
FIELD = re.compile(
    r"^\s{2,}([\w:<>\*\s]+?)\**(\w+)\s*(?:\[[^\]]*\])?;\s*/\*\s*(0x[0-9a-fA-F]+)", re.M)
SIZE = re.compile(r"sizeof\(\s*(?:struct\s+)?(\w+)\s*\)\s*==\s*(0x[0-9a-f]+)", re.I)
POKE = re.compile(r"\*\s*\(\s*(?:struct\s+)?([\w\s]*?)\s*\*\s*\)\s*\(*\s*"
                  r"(?:\(\s*int\s*\)\s*)?(?:self|c)\s*\+\s*(0x[0-9a-f]+)", re.I)

WIDTH = {'u8': 1, 's8': 1, 'char': 1, 'bool': 1, 'u16': 2, 's16': 2, 'short': 2,
         'u32': 4, 's32': 4, 'int': 4, 'long': 4, 'void': 4, 'float': 4}


def width(decl):
    for tok in decl.replace('unsigned', '').replace('signed', '').split():
        if tok in WIDTH:
            return WIDTH[tok]
    return None


def body(text, open_brace):
    i, d = open_brace, 0
    while i < len(text):
        if text[i] == '{':
            d += 1
        elif text[i] == '}':
            d -= 1
            if d == 0:
                return text[open_brace + 1:i]
        i += 1
    return ""


def load():
    occ = collections.defaultdict(list)
    sizes = {}
    for p in sorted(INC.glob("*.h")):
        t = p.read_text(encoding="utf-8", errors="replace")
        for n, s in SIZE.findall(t):
            sizes[n] = int(s, 16)
        for m in STRUCT.finditer(t):
            b = body(t, m.end() - 1)
            occ[m.group(1)].append(
                (m.group(2), {int(o, 16): width(dt) for dt, _, o in FIELD.findall(b)}))
    return occ, sizes


def layout(occ, name, seen=None):
    seen = seen or set()
    if name in seen:
        return {}
    seen.add(name)
    out = {}
    for base, fields in occ.get(name, []):
        if base:
            out.update(layout(occ, base, seen))
        out.update(fields)
    return out


def overlays_of_classes():
    """class -> overlays, from the module symbols its own mangled files reference."""
    out = collections.defaultdict(collections.Counter)
    for p in SRC.rglob("*"):
        if p.suffix not in ('.c', '.cpp'):
            continue
        m = re.match(r"_ZN(\d+)", p.name)
        if not m:
            continue
        n = int(m.group(1))
        cls = p.name[len(m.group(0)):][:n]
        t = p.read_text(encoding="utf-8", errors="replace")
        for ov in set(re.findall(r"\b(?:data|func)_(ov\d+)_", t)):
            out[cls][ov] += 1
    return out


def score(fname, occ, sizes, cls_ov):
    matches = list(SRC.rglob(fname))
    if not matches:
        raise SystemExit("no such handler in src/: " + fname)
    t = matches[0].read_text(encoding="utf-8", errors="replace")
    m = re.match(r"func_(ov\d+)_", fname)
    ov = m.group(1) if m else None
    pokes = [(int(o, 16), width(dt)) for dt, o in POKE.findall(t)]
    rows = []
    for cls, ovs in cls_ov.items():
        if ov not in ovs:
            continue
        L = layout(occ, cls)
        if not L:
            continue
        size = sizes.get(cls) or (max(L) + 4)
        good = bad = 0
        for off, w in pokes:
            if off >= size:
                bad += 2
            elif off in L:
                if w is None or L[off] is None or L[off] == w:
                    good += 2
                else:
                    bad += 1
            else:
                bad += 1
        rows.append((good - bad, good, bad, cls))
    rows.sort(reverse=True)
    return ov, len(pokes), rows


def main(argv=None):
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("handler", nargs="+", help="e.g. func_ov084_0212abd4.c")
    ap.add_argument("--top", type=int, default=3, help="candidates to show (default 3)")
    args = ap.parse_args(argv)

    occ, sizes = load()
    cls_ov = overlays_of_classes()
    for h in args.handler:
        ov, n, rows = score(h, occ, sizes, cls_ov)
        print("%s (%s, %d poke(s)):" % (h, ov, n))
        if not rows:
            print("    no candidate class found in that overlay")
            continue
        for sc, good, bad, cls in rows[:args.top]:
            print("    %+4d  (+%d/-%d)  %s" % (sc, good, bad, cls))
        if len(rows) > 1 and rows[0][0] - rows[1][0] < 6:
            print("    ^ near tie -- not evidence; read the handler")
    return 0


if __name__ == "__main__":
    sys.exit(main())
