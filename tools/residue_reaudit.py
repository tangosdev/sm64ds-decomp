"""Re-audit the "never read" residue claims in notes/*-provenance.md.

Those claims were originally made by searching the class's own *mangled* source
files -- `src/_ZN<len><Class>*`. That misses the unmangled `func_ovNNN_*` state
handlers, which are where most of an actor's behaviour actually lives: they take
`this` as a bare `char *c` and poke it by raw offset, so a field the class methods
only ever write is very often read a few files away.

Two passes, because neither alone is a verdict:

  --overlay   every handler in the class's overlay mentions the offset. Wide: a
              different class in the same overlay has its own +0xNNN, so this
              over-reports.
  --owned     only handlers reachable from the class's own matched methods
              through the src/ call graph. Narrow, and it UNDER-reports: many
              actors dispatch their handlers through a data table
              (`data_ovNNN_...`) and are never named by a caller, so the graph
              cannot see them. KingBobOmb's 0x500 -- hit points, confirmed by
              hand -- does not show up in this pass.

So treat both outputs as a candidate list to confirm by reading the handler and
checking its pokes match the class's layout, not as an answer.

    python tools/residue_reaudit.py --owned
"""
import argparse
import collections
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"
NOTES = REPO / "notes"

HEAD = re.compile(
    r"^#{2,4}\s+`?(?P<cls>[A-Za-z_][\w:]*)`?\s*\(.*?,\s*(?P<mod>arm9|ov\d+)", re.I)
OFF = re.compile(r"0x([0-9a-f]{2,4})\b", re.I)
NOREAD = re.compile(
    r"never read|nothing reads|no matched body reads|nothing else reads"
    r"|never read back|nothing reads it back", re.I)
HANDLER = re.compile(r"\bfunc_(?:ov\d+_)?[0-9a-f]{8}\b")


def load_sources():
    text = {}
    for p in sorted(SRC.rglob("*")):
        if p.suffix in (".c", ".cpp"):
            text[p.name] = p.read_text(encoding="utf-8", errors="replace")
    return text


def by_overlay(text):
    out = collections.defaultdict(list)
    for name in text:
        m = re.match(r"func_(ov\d+)_", name)
        if m:
            out[m.group(1)].append(name)
        elif re.match(r"func_0[0-9a-f]{7}", name):
            out["arm9"].append(name)
    return out


def call_graph(text):
    handlers = {n.rsplit(".", 1)[0] for n in text if n.startswith("func_")}
    return {n: {h for h in HANDLER.findall(t)
                if h in handlers and h != n.rsplit(".", 1)[0]}
            for n, t in text.items()}


def owned_by(cls, text, calls, depth=3):
    """Handler files reachable from the class's own matched methods."""
    tag = "_ZN%d%s" % (len(cls), cls)
    frontier = {n for n in text if n.startswith(tag) or n.startswith(cls + "_")}
    seen = set()
    for _ in range(depth):
        nxt = set()
        for f in frontier:
            for h in calls.get(f, ()):
                for cand in (h + ".c", h + ".cpp"):
                    if cand in text and cand not in seen:
                        seen.add(cand)
                        nxt.add(cand)
        frontier = nxt
        if not frontier:
            break
    return seen


def claims():
    """(note, line, class, overlay, offset) for every 'never read' residue claim."""
    for note in sorted(NOTES.glob("*provenance*.md")):
        cls = mod = None
        for ln, line in enumerate(note.read_text(encoding="utf-8").splitlines(), 1):
            h = HEAD.match(line)
            if h:
                cls, mod = h.group("cls"), h.group("mod").lower()
                continue
            if cls and NOREAD.search(line):
                for off in sorted({o.lower() for o in OFF.findall(line)}):
                    yield note.name, ln, cls, mod, off


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    g = ap.add_mutually_exclusive_group()
    g.add_argument("--owned", action="store_true",
                   help="only handlers the call graph ties to the class (default)")
    g.add_argument("--overlay", action="store_true",
                   help="every handler in the class's overlay")
    args = ap.parse_args(argv)
    overlay_mode = args.overlay

    text = load_sources()
    pools = by_overlay(text) if overlay_mode else None
    calls = None if overlay_mode else call_graph(text)
    cache = {}

    rows = []
    for note, ln, cls, mod, off in claims():
        if overlay_mode:
            pool = pools.get(mod, [])
        else:
            if cls not in cache:
                cache[cls] = owned_by(cls, text, calls)
            pool = cache[cls]
        if not pool:
            continue
        pat = re.compile(r"[+]\s*0x0*" + off.lstrip("0") + r"\b", re.I)
        hit = sorted(f for f in pool if pat.search(text[f]))
        if hit:
            rows.append((note, ln, cls, "0x" + off, hit))

    scope = "the class's overlay" if overlay_mode else "a handler owned by the class"
    print("%d residue claim(s) with a candidate reader in %s\n" % (len(rows), scope))
    for note, ln, cls, off, hit in rows:
        print("%s:%d  %s %s <- %d: %s" %
              (note, ln, cls, off, len(hit), ", ".join(hit[:5])))
    print("\nCandidates, not verdicts -- read the handler and check its pokes "
          "match this class's layout before naming anything.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
