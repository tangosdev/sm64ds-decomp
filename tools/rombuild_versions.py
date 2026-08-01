"""Find which mwccarm version reproduces each function the ROM build got wrong.

A function counts as matched in this repo if *some* toolchain version reproduces it -
`tools/match.py` sweeps a list. A ROM link, though, compiles everything with one
compiler, so a function matched under a different version links to the wrong bytes even
though its source is perfectly correct.

This takes the names `tools/rombuild_check.py` reported as mismatching, sweeps them
through the versions `match.py` knows about, and writes the ones that match under a
non-default version to config/rombuild-versions.txt. `rombuild.py` and `eligible.py`
then compile those files with the named version.

Anything that matches under no version is a genuine problem with the source (or with
its relocation destinations) and is left out of the override file.

Usage:
    python tools/rombuild_check.py --out build/bad.txt
    python tools/rombuild_versions.py build/bad.txt          # report
    python tools/rombuild_versions.py build/bad.txt --write   # update the config
"""
import argparse
import collections
import concurrent.futures
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import match as M           # noqa: E402
import modules as MOD       # noqa: E402
import srcpath as SP        # noqa: E402
from rombuild import CFLAGS, REPO, VERSION, VERSIONS_FILE, versions  # noqa: E402

SYM = re.compile(
    r"^(\S+)\s+kind:function\((arm|thumb),size=0x([0-9a-fA-F]+)\)\s+addr:0x([0-9a-fA-F]+)")

HEADER = """\
# Per-file mwccarm version overrides for the ROM build, as: <symbol> <version>.
# tools/rombuild.py and tools/eligible.py compile these with the named version instead
# of the default (see rombuild.VERSION).
#
# A function counts as matched if SOME toolchain version reproduces it, and match.py
# sweeps a list - so a single-version link produces the wrong bytes for the ones
# matched elsewhere, even though the source is correct.
#
# Regenerate with:
#   python tools/rombuild_check.py --out build/bad.txt
#   python tools/rombuild_versions.py build/bad.txt --write
"""


def symbol_info():
    info = {}
    for sp in sorted((REPO / "config").rglob("symbols.txt")):
        rel = sp.parent.relative_to(REPO / "config").as_posix()
        label = "arm9" if rel == "arm9" else rel.split("/")[-1]
        for line in sp.read_text(encoding="utf-8", errors="ignore").splitlines():
            m = SYM.match(line)
            if m:
                info.setdefault(m.group(1),
                                (label, int(m.group(4), 16), int(m.group(3), 16)))
    return info


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("names", help="file of symbol names (from rombuild_check --out)")
    ap.add_argument("--write", action="store_true", help="update config/rombuild-versions.txt")
    ap.add_argument("-j", "--jobs", type=int, default=10)
    args = ap.parse_args()

    info = symbol_info()
    mods = {("arm9" if m["name"] == "main" else m["name"]): m for m in MOD.modules()}
    names = [l.strip() for l in pathlib.Path(args.names).read_text().split() if l.strip()]
    # Try the default first so a file that already works never gets an override.
    order = [VERSION] + [v for v in M.SWEEP if v != VERSION]

    def sweep(n):
        if n not in info:
            return n, None
        label, addr, size = info[n]
        src = SP.path_for(n)
        if src is None:
            return n, None
        # The build's flags, not match.py's defaults: sweeping with different flags
        # than the link compiles with can bless a version the build then breaks on.
        flags = CFLAGS
        if src.read_text(encoding="utf-8", errors="ignore").startswith("//cpp"):
            flags = flags.replace("-lang c99", "-lang c++")
        if label == "arm9":
            tgt = M.target_bytes(addr, size)
        elif label in mods:
            tgt = M.target_bytes(addr, size, mods[label]["bin"], mods[label]["base"])
        else:
            return n, None
        for v in order:
            obj = M.compile_c(src, v, flags)
            if obj is None:
                continue
            code, relocs = M.extract_func(obj, n)
            if code is None:
                continue
            ok, _ = M.compare(tgt, code, relocs, verbose=False)
            if ok:
                return n, v
        return n, None

    found, counts = {}, collections.Counter()
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for n, v in ex.map(sweep, names):
            counts[v or "no version reproduces it"] += 1
            if v and v != VERSION:
                found[n] = v

    print(f"swept {len(names)} name(s); default is {VERSION}")
    for k, v in counts.most_common():
        print(f"  {v:5d}  {k}")

    if args.write:
        keep = dict(versions())
        keep.update(found)
        # Drop stale overrides that now build correctly under the default.
        keep = {k: v for k, v in keep.items() if v != VERSION}
        VERSIONS_FILE.write_text(
            HEADER + "\n".join(f"{k} {v}" for k, v in sorted(keep.items())) + "\n",
            encoding="utf-8", newline="\n")
        print(f"\nwrote {VERSIONS_FILE.relative_to(REPO)} with {len(keep)} override(s)")
    elif found:
        print(f"\n{len(found)} would get an override (pass --write):")
        for k, v in list(found.items())[:10]:
            print(f"  {k} {v}")


if __name__ == "__main__":
    main()
