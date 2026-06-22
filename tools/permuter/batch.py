#!/usr/bin/env python3
"""Sweep the regperm pile with the permuter, no LLM tokens.

A "regperm" function is template-correct but register-allocation-blocked: our template
emits logically-correct C that differs from the ROM only by a consistent register
renaming (see tools/triage.py). The permuter brute-forces exactly that. This tool:

  1. finds regperm functions (template produces a regperm-tier candidate) and uses that
     candidate C as the seed,
  2. imports each into a permuter dir (tools/permuter/import_func.setup_dir),
  3. runs the permuter with --stop-on-zero under a per-function time budget,
  4. on a score-0 result, independently re-verifies with our oracle and banks it
     (writes src/<name>.c, appends progress/matched.jsonl tagged "permuter").

There are two seed sources:
  --module/--max  : auto-find template-regperm functions (free, but this pile is small
                    and currently exhausted -- the strict templates already take what
                    they can, leaving "no template" real-logic functions).
  --seeds FILE    : a JSONL of near-misses from ANY source (LLM fan-out, m2c, hand) --
                    {"name" or "module"+"addr", "c_source"}. This is the real fuel: a
                    draft that COMPILES but doesn't match is often only coloring/ordering
                    blocked, exactly what the permuter finishes. Turns fan-out misses
                    (the ~92% it currently discards) into matches for free.

Usage:
    python tools/permuter/batch.py --module main --max 0x40 --limit 5 --secs 90
    python tools/permuter/batch.py --seeds nearmisses.jsonl --secs 120
    python tools/permuter/batch.py --module main --max 0x80 --dry-run   # list the pile

Everything banked is oracle-verified, so it is safe to commit.
"""
import argparse
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent.parent
sys.path.insert(0, str(REPO / "tools"))
sys.path.insert(0, str(REPO / "tools" / "permuter"))
import swarm as S
import modules as MOD
import relocs as R
import sweep
import triage
import import_func as IMP

SRC = REPO / "src"
LEDGER = REPO / "progress" / "matched.jsonl"


def regperm_seed(name, ins, tgt, addr, relocs, syms):
    """Return the template C that classifies as regperm for this function, else None."""
    cands = []
    for rule in S.RULES:
        c = rule(name, ins, tgt)
        if c:
            cands.append(c)
    for rule in S.RELOC_RULES:
        c = rule(name, ins, tgt, addr, relocs, syms)
        if c:
            cands.append(c)
    for csrc, _label in cands:
        code, crel = triage.compile_candidate(csrc, name)
        if code is None:
            continue
        if triage.classify(tgt, code, crel) == "regperm":
            return csrc
    return None


def find_regperm(module, maxsize, limit_scan):
    """Yield (mod, label, name, addr, size, tgt, seed_src) for regperm functions."""
    done = sweep.load_done()
    syms = R.load_all_syms()
    found = []
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        if module and mod["name"] != module and label != module:
            continue
        relocs = R.load_relocs_file(mod["relocs"])
        data = mod["bin"].read_bytes()
        for name, addr, size in sweep.funcs(mod):
            if (label, addr) in done or size > maxsize:
                continue
            tgt = data[addr - mod["base"]:addr - mod["base"] + size]
            ins = list(S.md.disasm(tgt, 0))
            if not ins or S.is_thunk(ins):
                continue
            seed = regperm_seed(name, ins, tgt, addr, relocs, syms)
            if seed:
                found.append((mod, label, name, addr, size, tgt, seed))
                if limit_scan and len(found) >= limit_scan:
                    return found
    return found


def run_permuter(out, secs):
    """Run the permuter under a time budget; return the score-0 source.c text or None."""
    try:
        subprocess.run(
            [sys.executable, str(IMP.PERM_DIR / "permuter.py"), str(out),
             "--stop-on-zero", "-j", "4", "--quiet"],
            timeout=secs, cwd=str(IMP.PERM_DIR),
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
        )
    except subprocess.TimeoutExpired:
        pass
    except Exception:
        return None
    for od in sorted(out.glob("output-0-*")):
        src = od / "source.c"
        if src.exists():
            return src.read_text()
    return None


def bank(label, name, addr, size, src):
    (SRC / f"{name}.c").write_text(src)
    rec = {"addr": f"0x{addr:08x}", "name": name, "size": size,
           "module": label, "versions": ["permuter"]}
    with open(LEDGER, "a") as f:
        f.write(json.dumps(rec) + "\n")


def load_seeds(path):
    """Read a near-miss JSONL into the same tuple shape as find_regperm."""
    pile = []
    for line in open(path):
        line = line.strip()
        if not line:
            continue
        d = json.loads(line)
        addr = int(d["addr"], 0) if isinstance(d.get("addr"), str) else d.get("addr")
        found = IMP.find_func(d.get("module"), addr, d.get("name"))
        if not found:
            print(f"  (skip) function not found: {d.get('name') or hex(addr)}")
            continue
        mod, label, name, faddr, size = found
        tgt = mod["bin"].read_bytes()[faddr - mod["base"]:faddr - mod["base"] + size]
        pile.append((mod, label, name, faddr, size, tgt, d["c_source"]))
    return pile


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--module", default=None)
    ap.add_argument("--max", type=lambda x: int(x, 0), default=0x40)
    ap.add_argument("--limit", type=int, default=5, help="max functions to permute")
    ap.add_argument("--secs", type=int, default=90, help="permuter budget per function")
    ap.add_argument("--seeds", default=None, help="JSONL of near-misses to permute")
    ap.add_argument("--dry-run", action="store_true", help="just list the pile")
    args = ap.parse_args()

    if args.seeds:
        print(f"loading near-miss seeds from {args.seeds}...")
        pile = load_seeds(args.seeds)
    else:
        print(f"scanning for regperm functions (module={args.module or 'all'}, "
              f"size<=0x{args.max:x})...")
        pile = find_regperm(args.module, args.max, args.limit if args.dry_run else 0)
    print(f"found {len(pile)} candidate functions")
    for mod, label, name, addr, size, tgt, seed in pile:
        print(f"  {label} {name} @ 0x{addr:08x} (0x{size:x})")
    if args.dry_run:
        return

    banked = 0
    for mod, label, name, addr, size, tgt, seed in pile[:args.limit]:
        print(f"\n=== permuting {label} {name} @ 0x{addr:08x} (budget {args.secs}s) ===")
        seed = IMP.permutable_base(seed, name)   # convert C-compatible //cpp seeds to C
        out, _, _, _, nrel = IMP.setup_dir((mod, label, name, addr, size), seed)
        src = run_permuter(out, args.secs)
        if not src:
            print("  no score-0 found in budget")
            continue
        if S.oracle_ok(src, name, tgt):
            bank(label, name, addr, size, src)
            banked += 1
            print(f"  CRACKED + oracle-verified -> banked {name}")
        else:
            print("  permuter reported 0 but oracle disagreed (skipped)")
    print(f"\nbanked {banked}/{min(len(pile), args.limit)} via permuter")


if __name__ == "__main__":
    main()
