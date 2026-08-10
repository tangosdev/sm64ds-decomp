"""Turn a hand-spelt D0 destructor file into a real C++ one, or leave it alone.

D0 is the DELETING destructor -- vtable slot 17 for an actor: destroy through
the class and its bases, then return the object to its heap. Nobody writes that
body; the compiler generates it. That is why `src/_ZN5EnemyD0Ev.c` argues
against renaming these files, and why they were counted out of the backlog.

Counting them out was wrong. You do not write D0 -- you write `X::~X()`, and
mwcc emits D2, D0 and D1 together, and objisolate keeps whichever one the file
is bound to. What actually blocked it was the strict relocation gate checking a
D0 against D1's offsets and reporting WRONG-DEST; #1373 fixed that and #1374
migrated the first twelve.

So a D0 needs exactly what a D1 needs: a header where the class is real. This
walks every D0 still spelt by hand, tries it against its own header, verifies,
and restores the original if the bytes differ.
"""
import argparse
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import build_pin as BP  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parents[1]

BODY = """//cpp
// @symbol _ZN{n}{cls}D0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why more than one vptr store appears -- then return the object to
 * its heap. Nobody writes that; declaring `~{cls}()` is enough, because mwcc
 * emits D2, D0 and D1 together and objisolate keeps the one this file is bound
 * to.
 *
 * The deallocation is an inline operator delete, which is why nothing below
 * mentions a heap.
 */
#include "{cls}.h"

{cls}::~{cls}()
{{
}}
"""


def symbol_index():
    idx = {}
    for p in REPO.glob("config/**/symbols.txt"):
        lab = p.parent.name
        for line in p.read_text(errors="replace").splitlines():
            m = re.match(r"\s*(\S+)\s+kind:function\([^)]*size=(0x[0-9a-f]+)\)\s+addr:(0x[0-9a-f]+)", line)
            if m:
                idx[m.group(1)] = (int(m.group(3), 16), int(m.group(2), 16), lab)
    return idx


def candidates():
    """Hand-spelt D0 files whose class has a real C++ header with a real base."""
    out = []
    for f in sorted((REPO / "src").glob("*D0Ev.c")):
        m = re.match(r"_ZN(\d+)(\w+?)D0Ev$", f.stem)
        if not m:
            continue
        n = int(m.group(1))
        cls = m.group(2)[:n]
        if len(cls) != n:
            continue
        h = REPO / "include" / f"{cls}.h"
        if not h.exists():
            continue
        ht = h.read_text(errors="replace")
        if not re.search(r"^\s*struct\s+" + re.escape(cls) + r"\s*:\s*\w+\s*\{", ht, re.M):
            continue                      # needs a real base
        if not re.search(r"virtual\s+~" + re.escape(cls), ht):
            continue                      # needs a declared virtual destructor
        out.append((f, cls))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--list", action="store_true", help="show candidates, change nothing")
    ap.add_argument("--only", help="comma-separated class names")
    ap.add_argument("--limit", type=int, default=0)
    args = ap.parse_args()

    idx = symbol_index()
    cands = candidates()
    if args.only:
        want = set(args.only.split(","))
        cands = [c for c in cands if c[1] in want]
    if args.limit:
        cands = cands[:args.limit]

    if args.list:
        for _, cls in cands:
            print(cls)
        print(f"\n{len(cands)} candidates")
        return

    kept, failed = [], []
    for f, cls in cands:
        sym = f.stem
        if sym not in idx:
            continue
        addr, size, lab = idx[sym]
        original = f.read_text(errors="replace")
        new = f.with_suffix(".cpp")
        new.write_text(BODY.format(n=len(cls), cls=cls))
        f.unlink()
        try:
            ok, why = BP.verify(str(new.relative_to(REPO)).replace("\\", "/"), sym, addr, size, lab)
        except Exception as e:                                   # noqa: BLE001
            ok, why = False, f"{type(e).__name__}: {e}"[:90]
        if ok:
            kept.append(cls)
            print(f"OK   {cls:30s} {lab}")
        else:
            new.unlink()
            f.write_text(original)
            failed.append((cls, why))
            print(f"--   {cls:30s} {why[:70]}")

    print(f"\nkept {len(kept)} / tried {len(kept) + len(failed)}")
    if kept:
        print("kept: " + " ".join(kept))


if __name__ == "__main__":
    main()
