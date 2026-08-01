"""Byte-diff every source-built function against the ROM, one function at a time.

`dsd check modules` answers "is this module identical?"; when the answer is no it does
not say which function broke it. This does. Because an enrolled function's object is
required to be exactly its declared size, nothing can shift its neighbours, so every
mismatch is confined to the function that caused it and all of them can be found from a
single build instead of bisecting.

Usage:
    python tools/rombuild_check.py                 # summary + the failing names
    python tools/rombuild_check.py --out bad.txt   # write failures for enroll.py

Feed the failures back with:
    python tools/enroll.py --complete-list <eligible minus bad>
"""
import argparse
import collections
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from enroll import read_delinks, sections, CONFIG, REPO  # noqa: E402

BUILD = REPO / "build" / "build"
EXTRACTED = REPO / "extracted" / "dsd"
ENTRY_SEC = re.compile(r"^\s+(\.\S+)\s+start:0x([0-9a-fA-F]+)\s+end:0x([0-9a-fA-F]+)")


def module_binaries(d):
    """(built, retail) binary paths for a module config directory, or (None, None)."""
    rel = d.relative_to(CONFIG).as_posix()
    if rel == "arm9":
        return BUILD / "arm9.bin", EXTRACTED / "arm9" / "arm9.bin"
    if rel in ("arm9/itcm", "arm9/dtcm"):
        n = rel.split("/")[-1]
        return BUILD / f"{n}.bin", EXTRACTED / "arm9" / f"{n}.bin"
    m = re.fullmatch(r"arm9/overlays/(ov\d+)", rel)
    if m:
        return BUILD / f"arm9_{m.group(1)}.bin", EXTRACTED / "arm9_overlays" / f"{m.group(1)}.bin"
    return None, None


def complete_entries(path):
    """[(srcpath, addr, end)] for entries marked `complete`."""
    out, cur, done, sec = [], None, False, None
    for line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        if not line.strip():
            continue
        if not line[0].isspace():
            if cur and done and sec:
                out.append((cur, *sec))
            cur, done, sec = line.strip().rstrip(":"), False, None
        elif cur is not None:
            if line.strip() == "complete":
                done = True
            else:
                m = ENTRY_SEC.match(line)
                if m:
                    sec = (int(m.group(2), 16), int(m.group(3), 16))
    if cur and done and sec:
        out.append((cur, *sec))
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--out", help="write failing symbol names here")
    ap.add_argument("--show", type=int, default=12)
    args = ap.parse_args()

    checked = bad = 0
    intentional = []
    failures, per_module = [], collections.Counter()
    missing_bins = []

    for sym in sorted(CONFIG.rglob("symbols.txt")):
        d = sym.parent
        dl = d / "delinks.txt"
        if not dl.is_file():
            continue
        entries = complete_entries(dl)
        if not entries:
            continue
        built_p, retail_p = module_binaries(d)
        if not built_p or not built_p.is_file() or not retail_p.is_file():
            missing_bins.append(d.relative_to(CONFIG).as_posix())
            continue
        header, _ = read_delinks(dl)
        secs = sections(header)
        if not secs:
            continue
        base = min(s[1] for s in secs)
        built, retail = built_p.read_bytes(), retail_p.read_bytes()
        label = d.relative_to(CONFIG).as_posix()
        for (rel, addr, end) in entries:
            lo, hi = addr - base, end - base
            if hi > len(retail) or hi > len(built):
                continue
            # A mods/ entry is a deliberate divergence; differing from the ROM is the
            # whole point, so it is reported separately and never counted as a failure.
            if rel.startswith("mods/"):
                if built[lo:hi] != retail[lo:hi]:
                    intentional.append((pathlib.Path(rel).stem, label, addr,
                                        sum(1 for x, y in zip(built[lo:hi], retail[lo:hi]) if x != y)))
                else:
                    intentional.append((pathlib.Path(rel).stem, label, addr, 0))
                continue
            checked += 1
            if built[lo:hi] != retail[lo:hi]:
                bad += 1
                name = pathlib.Path(rel).stem
                nd = sum(1 for x, y in zip(built[lo:hi], retail[lo:hi]) if x != y)
                failures.append((label, name, addr, hi - lo, nd))
                per_module[label] += 1

    if intentional:
        print(f"intentional divergences (mods/): {len(intentional)}")
        for (n, label, addr, nd) in intentional:
            state = f"{nd} byte(s) differ from the ROM" if nd else "!! identical to the ROM - the mod is NOT in the build"
            print(f"  {n} ({label}, 0x{addr:08x}): {state}")
        print()
    print(f"source-built functions checked: {checked}")
    print(f"                    reproducing: {checked - bad}")
    print(f"                    mismatching: {bad}")
    if missing_bins:
        print(f"(no built binary for {len(missing_bins)} module(s): {missing_bins[:4]})")
    if per_module:
        print("\nmismatches by module:")
        for k, v in per_module.most_common(12):
            print(f"  {v:5d}  {k}")
        print(f"\nfirst {min(args.show, len(failures))} mismatches:")
        for (label, name, addr, size, nd) in failures[:args.show]:
            print(f"  {label:26s} {name:44s} 0x{addr:08x} size 0x{size:<5x} {nd} bad bytes")
    if args.out:
        pathlib.Path(args.out).write_text("\n".join(sorted(n for _, n, _, _, _ in failures)) + "\n")
        print(f"\nwrote {args.out}")


if __name__ == "__main__":
    main()
