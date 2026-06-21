"""Re-verify every banked match against the ROM.

For each entry in progress/matched.jsonl: recompile its committed source and
reloc-aware byte-compare against the module's ROM bytes. An entry PASSES if it
reproduces under the canonical compiler (1.2/sp2p3) OR any other available
mwccarm version (some legit matches were banked under a different version).
Entries failing every version are FALSE MATCHES (banked but non-reproducing).

Usage:
  python tools/reverify_corpus.py              # full sweep, writes report
  python tools/reverify_corpus.py --module ov007
"""
import argparse
import json
import pathlib
import sys
from concurrent.futures import ThreadPoolExecutor

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import match as M
import modules as MOD
import probe_versions as PV
import swarm as S

SRC = REPO / "src"
AUTO = REPO / "match" / "auto"
ALL_VERSIONS = ["1.2/sp2p3", "1.2/base", "1.2/sp2", "1.2/sp3", "1.2/sp4",
                "2.0/base", "2.0/sp1", "2.0/sp1p2", "2.0/sp2", "2.0/sp2p2", "2.0/sp2p3"]

_modcache = {}        # module name -> dict
_bincache = {}        # module name -> full bytes


def mod_for(name):
    if not _modcache:
        for m in MOD.modules():
            _modcache[m["name"]] = m
        # matched.jsonl labels the main ARM9 module "arm9"; the registry calls it "main"
        if "main" in _modcache:
            _modcache["arm9"] = _modcache["main"]
    return _modcache.get(name)


def rom_bytes(modname, addr, size):
    m = mod_for(modname)
    if m is None:
        return None
    if modname not in _bincache:
        _bincache[modname] = m["bin"].read_bytes()
    off = addr - m["base"]
    return _bincache[modname][off:off + size]


def src_texts(name, addr):
    """All candidate committed sources for an entry: the human-named src/<name> file and
    the address-keyed match/auto/0x<addr> file. src/<name>.c is sometimes a STUB whose only
    content is a path into match/auto -- skip those. A match counts if ANY candidate
    reproduces it (the two dirs can hold different revisions of the same function)."""
    out = []
    for base in (SRC / name, AUTO / f"0x{addr:08x}"):
        for ext in (".c", ".cpp"):
            p = base.with_suffix(ext)
            if p.exists():
                t = p.read_text(encoding="utf-8", errors="replace")
                if not t.strip().startswith("C:") and "match\\auto" not in t and "match/auto" not in t:
                    out.append(t)
    return out


def compiles_to(src, name, target):
    """First (suffix, version) that reproduces target, else None. Tries BOTH C and C++
    flags (committed sources don't always carry the //cpp marker), and matches ANY symbol
    in the object (compiler-generated thunks live under mangled names like _ZThn16_...)."""
    import tempfile, os
    attempts = ([(S.CPP_FLAGS, ".cpp")] if src.startswith("//cpp")
                else [(M.DEFAULT_FLAGS, ".c"), (S.CPP_FLAGS, ".cpp")])
    for flags, suf in attempts:
        fd, tmp = tempfile.mkstemp(suffix=suf)
        os.close(fd)
        pathlib.Path(tmp).write_text(src)
        try:
            for v in ALL_VERSIONS:
                obj = M.compile_c(pathlib.Path(tmp), v, flags)
                if obj is None:
                    continue
                try:
                    syms = list(PV.funcs_in(obj).keys())
                except Exception:
                    syms = [name]
                if name not in syms:
                    syms = [name] + syms
                for sym in syms:
                    code, relocs = M.extract_func(obj, sym)
                    if code is None or len(code) != len(target):
                        continue
                    ok, _ = M.compare(target, code, relocs, verbose=False)
                    if ok:
                        # 1.2/sp2p3 is canonical for BOTH C and C++ (the .cpp flag is not a
                        # different compiler version); only flag genuinely different versions.
                        return "1.2/sp2p3" if v == "1.2/sp2p3" else f"{v}/{suf[1:]}"
        finally:
            pathlib.Path(tmp).unlink(missing_ok=True)
    return None


def check(entry):
    name = entry["name"]
    try:
        addr = int(entry["addr"], 16)
    except Exception:
        addr = int(entry["addr"], 0)
    size = entry["size"]
    mod = entry["module"]
    target = rom_bytes(mod, addr, size)
    if target is None:
        return (name, mod, "NO-MODULE-BIN")
    if len(target) != size:
        return (name, mod, "BYTES-OOB")
    srcs = src_texts(name, addr)
    if not srcs:
        return (name, mod, "NO-SOURCE")
    for src in srcs:
        v = compiles_to(src, name, target)
        if v is not None:
            return (name, mod, "OK" if v == "1.2/sp2p3" else f"OK:{v}")
    return (name, mod, "FALSE")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--module", default=None)
    ap.add_argument("-j", "--jobs", type=int, default=8)
    ap.add_argument("--out", default="progress/reverify_report.txt")
    args = ap.parse_args()

    entries = [json.loads(l) for l in (REPO / "progress" / "matched.jsonl").read_text().splitlines() if l.strip()]
    if args.module:
        entries = [e for e in entries if e["module"] == args.module]
    print(f"re-verifying {len(entries)} matches with {args.jobs} workers ...", flush=True)

    results = []
    done = 0
    with ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for r in ex.map(check, entries):
            results.append(r)
            done += 1
            if done % 250 == 0:
                bad = sum(1 for x in results if x[2] in ("FALSE", "NO-SOURCE", "NO-MODULE-BIN", "BYTES-OOB"))
                print(f"  {done}/{len(entries)}  ({bad} problems so far)", flush=True)

    from collections import Counter
    cat = Counter(r[2].split(":")[0] for r in results)
    problems = [r for r in results if r[2] in ("FALSE", "NO-SOURCE", "NO-MODULE-BIN", "BYTES-OOB")]
    altver = [r for r in results if r[2].startswith("OK:")]

    lines = []
    lines.append(f"corpus re-verification: {len(results)} entries")
    for k, v in cat.most_common():
        lines.append(f"  {k:16} {v}")
    lines.append("")
    lines.append(f"FALSE MATCHES (banked but reproduce under NO compiler version): {len(problems)}")
    for n, m, s in sorted(problems):
        lines.append(f"  {s:14} {m:7} {n}")
    lines.append("")
    lines.append(f"OK only under a NON-canonical version: {len(altver)}")
    for n, m, s in sorted(altver):
        lines.append(f"  {s:14} {m:7} {n}")
    report = "\n".join(lines)
    (REPO / args.out).write_text(report)
    print("\n" + report)
    print(f"\nreport written to {args.out}")


if __name__ == "__main__":
    main()
