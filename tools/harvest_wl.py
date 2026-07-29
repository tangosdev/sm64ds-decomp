"""Build a harvest worklist: floor-filtered, smallest-first, untried.

PREFER tools/coddog.py for the fan-out. A 2026-07-01 head-to-head (same band, same model,
same effort) landed 71% with coddog's fuzzy opcode-similarity scheduling vs 50% with this
smallest-first builder, at ~2.3x fewer tokens per landed function -- coddog picks functions
that have a close already-matched sibling and attaches it as the scaffold. Use this builder
for the floor-filtered coverage sweep / when coddog finds no good siblings in a band.

The proven high-yield recipe (2026-06-21: 82% hit vs the 7% mis-targeted floor). Generates
a worklist of small unmatched functions with the base-materialization RMW shape removed
(those are the unmatchable floor -- they only waste fan-out tokens), sorted smallest-first.

Usage:
  python tools/harvest_wl.py --max 0x48 --limit 120 --out progress/wl.jsonl
Then fan out over it (agents write oracle-verified {name,c_source}), bank with independent
re-verify, then clone.py + paramclone.py for free siblings.
"""
import argparse, json, subprocess, sys, pathlib
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
import swarm as S
import ledger as L
md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
REPO = pathlib.Path(__file__).resolve().parent.parent


def _adj_thunk(ins):
    """this-adjusting veneer (multiple-inheritance dtor/method): ends in a direct `b`
    and adjusts `this` through `ip` (a scratch reg loaded from a pooled offset) --
    `add/sub r0, r0, ip`. Only binds under its mangled _ZThn* name, so it is unmatchable
    from C under a func_* symbol. Restricted to the `ip` form on purpose: an immediate
    adjust (`add r0,r0,#k; b f`) can be a MATCHABLE tail-call `return f(this+k)`, so we
    must not flag that."""
    if not ins or ins[-1].mnemonic != "b":
        return False
    body = " ".join(i.mnemonic + " " + i.op_str for i in ins[:-1])
    return "add r0, r0, ip" in body or "sub r0, r0, ip" in body


def is_floor(target_hex):
    """Known-unmatchable shapes to keep out of the fan-out: the leaf materialize-RMW
    (`add (ldr*) ... (str*)`, base folds from C) and this-adjusting thunks. Kept STRICT
    on purpose -- a filter that drops a matchable function is worse than one that lets a
    little floor through (agents identify the subtler mid-band/guarded floor cheaply, and
    those feed the NONMATCHING hatch). Do not broaden this to guarded/mid-band RMW: tested,
    it false-positives on matchable functions."""
    ins = list(S.code_insns(list(md.disasm(bytes.fromhex(target_hex), 0))))
    if not ins:
        return False
    mn = [i.mnemonic for i in ins]
    leaf_rmw = (mn[0] == "add" and len(mn) >= 4 and mn[1] in ("ldr", "ldrb", "ldrh")
                and any(m in ("str", "strb", "strh") for m in mn[-3:]))
    return leaf_rmw or _adj_thunk(ins)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min", default="0x8")
    ap.add_argument("--max", default="0x48")
    ap.add_argument("--limit", type=int, default=120)
    ap.add_argument("--pool", type=int, default=500, help="raw pool size to draw from")
    ap.add_argument("--module", default=None, help="target one module (use 'main' for arm9) "
                    "instead of spreading across all modules -- aims at a rich vein")
    ap.add_argument("--out", default="progress/wl.jsonl")
    ap.add_argument("--floor-out", default=None, help="also write the filtered floor funcs here")
    args = ap.parse_args()

    # draw a pool from the existing worklist generator (does callee/pool resolution).
    # --module targets one rich module; otherwise --spread round-robins all modules.
    cmd = [sys.executable, str(REPO / "tools" / "worklist.py"), "--no-template",
           "--min", args.min, "--max", args.max, "--limit", str(args.pool)]
    cmd += ["--module", args.module] if args.module else ["--spread"]
    pool_path = REPO / "progress" / "_hpool.jsonl"
    with open(pool_path, "w") as f:
        subprocess.run(cmd, stdout=f, check=True)
    rows = [json.loads(l) for l in open(pool_path) if l.strip()]
    pool_path.unlink(missing_ok=True)

    # exclude functions already parked NONMATCHING and the floor skip-list (confirmed
    # floor/regperm that agents keep re-hitting but can't match from C -- don't re-attempt).
    # Keys go through L.key_of, which normalizes module and address. These files disagree
    # on the addr type -- nonmatching.jsonl carries ints, nearmiss/db.jsonl mostly hex
    # strings -- so a raw (module, addr) tuple silently failed to match across them and let
    # excluded functions back into the pool.
    parked = set()
    for fn in ("nonmatching.jsonl", "floor_skip.jsonl"):
        p = REPO / "progress" / fn
        if p.exists():
            for r in L.read_records(p):
                parked.add(L.key_of(r))
    # also exclude functions already in the near-miss DB: they were attempted and their best
    # compiling attempt is stored (nearmiss/db.jsonl); re-fanning them out wastes tokens. Finish
    # those through the permuter / by hand from the DB, not by re-harvesting.
    for r in L.read_records(REPO / "nearmiss" / "db.jsonl"):
        parked.add(L.key_of(r))
    rows = [r for r in rows if L.key_of(r) not in parked]

    sz = lambda r: int(r["size"], 16) if isinstance(r["size"], str) else r["size"]
    floor = [r for r in rows if is_floor(r["target_hex"])]
    cand = sorted((r for r in rows if not is_floor(r["target_hex"])), key=sz)
    keep = cand[:args.limit]
    with open(args.out, "w") as f:
        for r in keep:
            f.write(json.dumps(r) + "\n")
    if args.floor_out:
        with open(args.floor_out, "w") as f:
            for r in floor:
                f.write(json.dumps({"name": r["name"], "module": r["module"],
                                    "addr": r["addr"], "size": r["size"]}) + "\n")
    rng = (sz(keep[0]), sz(keep[-1])) if keep else (0, 0)
    print(f"pool={len(rows)}  floor-filtered={len(floor)}  kept(smallest {len(keep)})  "
          f"size {rng[0]:#x}-{rng[1]:#x}  modules={len(set(r['module'] for r in keep))}  -> {args.out}")


if __name__ == "__main__":
    main()
