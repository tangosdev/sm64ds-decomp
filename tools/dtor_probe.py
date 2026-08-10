"""Probe which unmigrated D1 destructors reproduce from an empty real method.

For each candidate the probe writes `Class::~Class() {}` over the file, asks
build_pin to compile it at the file's own pin and compare against the ROM, and
puts the original back if the bytes differ. Nothing is kept that did not match.

Run with --list to see candidates without touching the tree.
"""
import argparse
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import build_pin as BP  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent


def symbol_index():
    """symbol -> (addr, size, module label) from every config/**/symbols.txt."""
    idx = {}
    for p in REPO.glob("config/**/symbols.txt"):
        parts = p.relative_to(REPO / "config").parts
        label = parts[-2] if parts[-2] != "arm9" else "arm9"
        for line in p.read_text(errors="replace").splitlines():
            m = re.match(
                r"\s*(\S+)\s+kind:function\([^)]*size=(0x[0-9a-fA-F]+)\)\s+addr:(0x[0-9a-fA-F]+)",
                line)
            if m:
                idx[m.group(1)] = (int(m.group(3), 16), int(m.group(2), 16), label)
    return idx


def cpp_struct(header_text, cls):
    """The class's C++ declaration line, or None."""
    m = re.search(r"^\s*(?:struct|class)\s+" + re.escape(cls) + r"\s*(:[^{;]*)?\{",
                  header_text, re.M)
    return m.group(0) if m else None


def candidates():
    """Unmigrated D1 files whose class has a real C++ header."""
    out = []
    for f in sorted((REPO / "src").glob("*D1Ev.c")) + sorted((REPO / "src").glob("*D1Ev.cpp")):
        text = f.read_text(errors="replace")
        if text.lstrip().startswith("//cpp"):
            continue
        m = re.match(r"_ZN(\d+)(\w+?)D1Ev$", f.stem)
        if not m:
            continue
        n, rest = int(m.group(1)), m.group(2)
        cls = rest[:n]
        if len(cls) != n:
            continue
        h = REPO / "include" / f"{cls}.h"
        if not h.exists():
            continue
        ht = h.read_text(errors="replace")
        decl = cpp_struct(ht, cls)
        if not decl or ":" not in decl:
            continue                      # needs a real base
        if not re.search(r"virtual\s+~" + re.escape(cls), ht):
            continue                      # needs a declared virtual destructor
        out.append((f, cls, decl.strip().rstrip("{").strip()))
    return out


BODY = """//cpp
// @symbol _ZN{n}{cls}D1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The hand-written version spelled out what a complete-object destructor does
 * anyway: store this class's vtable over the one the base constructor left,
 * destroy the members in reverse declaration order, then run the base
 * subobject destructor. All of it follows from `{decl}` and the member types
 * in the header, so the body is empty and the bytes still reproduce.
 */
#include "{cls}.h"

{cls}::~{cls}()
{{
}}
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--list", action="store_true", help="show candidates, change nothing")
    ap.add_argument("--only", help="comma-separated class names to try")
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
        for f, cls, decl in cands:
            sym = f.stem
            print(f"{cls:32s} {idx.get(sym, ('?',))[0] if sym in idx else '?'}  {decl}")
        print(f"\n{len(cands)} candidates")
        return

    kept, failed, skipped = [], [], []
    for f, cls, decl in cands:
        sym = f.stem
        if sym not in idx:
            skipped.append((cls, "no symbol")); continue
        addr, size, label = idx[sym]
        original, orig_path = f.read_text(errors="replace"), f
        new_path = f.with_suffix(".cpp")
        new_path.write_text(BODY.format(n=len(cls), cls=cls, decl=decl))
        if orig_path != new_path:
            orig_path.unlink()
        try:
            ok, ver = BP.verify(str(new_path.relative_to(REPO)).replace("\\", "/"),
                                sym, addr, size, label)
        except Exception as e:                       # noqa: BLE001
            ok, ver = False, f"{type(e).__name__}: {e}"[:120]
        # match.compare returns the literal 999 for ANY size mismatch -- it is a
        # sentinel, not a count. Say so, or a candidate that is one instruction
        # short reads as though it were unrelated to the target.
        if not ok and "999 word(s) differ" in str(ver):
            ver = "size differs from the ROM (run tools/dtor_diff.py to see how)"
        if ok:
            kept.append((cls, ver, size))
            print(f"OK   {cls:32s} {ver}  0x{size:x}  {label}")
        else:
            new_path.unlink()
            orig_path.write_text(original)
            failed.append((cls, ver))
            print(f"--   {cls:32s} {ver}")

    print(f"\nkept {len(kept)} / tried {len(kept) + len(failed)}   skipped {len(skipped)}")
    if kept:
        print("kept: " + " ".join(c for c, _, _ in kept))


if __name__ == "__main__":
    main()
