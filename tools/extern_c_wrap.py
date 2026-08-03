"""Give ROM symbol declarations C linkage in C++ translation units.

A C++ TU that declares a ROM symbol like this:

    extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);

gets it mangled *again*, because the declaration has C++ linkage. mwccarm emits a
reference to `_Z35_ZN5Model8LoadFileER13SharedFilePtrPv`, which exists nowhere. Plain
names suffer the same way: `extern void Deallocate(void *)` becomes `_Z10DeallocatePv`.

The file still byte-matches, which is why this survives. `match.py` compares the
relocated word as a wildcard, so the *name* a call resolves to never enters the byte
verdict -- only the full ROM link cares, and until a function is enrolled the link
never sees it either. So these read as finished work while referencing symbols that
do not exist.

`eligible.py` counts them as `unresolvable` and refuses to enroll them, which is the
visible symptom: 1,454 of them at the time of writing, the largest single blocker to
building more of the ROM from source.

The fix is linkage, not naming -- the identifiers in the source are already the exact
symbols the ROM uses. Contiguous runs of file-scope `extern` declarations are wrapped
in `extern "C" { ... }`.

Every file is byte-verified after the edit and reverted if it stops reproducing, so a
declaration that genuinely wanted C++ linkage cannot be broken silently.

Usage:
    python tools/extern_c_wrap.py                 # report only
    python tools/extern_c_wrap.py --apply
    python tools/extern_c_wrap.py --apply -j 16
"""
import argparse
import concurrent.futures
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import match as M          # noqa: E402
import modules as MOD      # noqa: E402
from enroll import candidates  # noqa: E402

DECL = re.compile(r"^extern\s+[^;{]*;\s*$")


def is_cpp(path, text):
    return path.suffix == ".cpp" or text.startswith("//cpp")


def wrap(text):
    """Wrap contiguous file-scope `extern ...;` runs in extern "C". Returns None if
    nothing to do."""
    lines = text.splitlines()
    out, run, depth, changed = [], [], 0, False

    def flush():
        nonlocal changed
        if run:
            out.append('extern "C" {')
            out.extend(run)
            out.append("}")
            run.clear()
            changed = True

    for ln in lines:
        s = ln.strip()
        # Only at file scope, and never inside an existing extern "C" block.
        if depth == 0 and DECL.match(s):
            run.append(ln)
            continue
        flush()
        out.append(ln)
        depth += ln.count("{") - ln.count("}")
    flush()
    return "\n".join(out) + "\n" if changed else None


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("-j", "--jobs", type=int, default=10)
    args = ap.parse_args()

    info = {}
    for (d, name, rel, addr, size, sec) in candidates()[0]:
        info[rel] = (name, addr, size, d)
    mods = {("arm9" if m["name"] == "main" else m["name"]): m for m in MOD.modules()}

    targets = []
    for f in sorted((REPO / "src").rglob("*")):
        if not f.is_file() or f.suffix not in (".c", ".cpp"):
            continue
        t = f.read_text(encoding="utf-8", errors="ignore")
        if not is_cpp(f, t) or 'extern "C"' in t:
            continue
        if wrap(t):
            targets.append(f)

    print(f"C++ TUs whose ROM declarations lack C linkage: {len(targets)}")
    if not args.apply:
        print("\n(report only -- pass --apply to wrap and byte-verify)")
        return 0

    def one(f):
        rel = f.relative_to(REPO).as_posix()
        original = f.read_text(encoding="utf-8", errors="ignore")
        new = wrap(original)
        if new is None:
            return rel, "unchanged"
        f.write_text(new, encoding="utf-8", newline="\n")
        if rel not in info:
            return rel, "wrapped (not enrolled, unverified)"
        name, addr, size, d = info[rel]
        label = d.relative_to(REPO / "config").as_posix()
        label = "arm9" if label == "arm9" else label.split("/")[-1]
        flags = M.DEFAULT_FLAGS.replace("-lang c99", "-lang c++")
        tgt = (M.target_bytes(addr, size) if label == "arm9"
               else M.target_bytes(addr, size, mods[label]["bin"], mods[label]["base"]))
        for v in M.SWEEP:
            obj = M.compile_c(f, v, flags)
            if obj is None:
                continue
            code, rl = M.extract_func(obj, name)
            if code is None:
                continue
            ok, _ = M.compare(tgt, code, rl, verbose=False)
            if ok:
                return rel, "wrapped"
        f.write_text(original, encoding="utf-8", newline="\n")
        return rel, "reverted (stopped matching)"

    counts = {}
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for rel, verdict in ex.map(one, targets):
            counts[verdict] = counts.get(verdict, 0) + 1
            if verdict.startswith("reverted"):
                print(f"  {verdict}: {rel}")
    print()
    for k, v in sorted(counts.items(), key=lambda kv: -kv[1]):
        print(f"  {v:5d}  {k}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
