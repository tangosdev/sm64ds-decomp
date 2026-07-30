"""Match-rate-aware target selection: score unmatched functions by how likely
they are to match, and CLUSTER ones that share callees so a single agent builds
struct/callee understanding once and reuses it across siblings.

Scoring per function (from its disassembly + the verified symbol map):
  + named callees (we know what they do)      -> easier
  - unknown callees                            -> harder
  - large size                                 -> harder
Functions sharing a callee are unioned into a cluster; output is ordered
cluster-by-cluster (best clusters first) so contiguous chunking keeps a cluster
with one agent.

Usage:
    python tools/cluster_targets.py --lo 0x02007000 --hi 0x0200f000 --max 0x90 --count 20
"""
import argparse
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
import ledger as L
import worklist as WL

REPO = pathlib.Path(__file__).resolve().parent.parent
ARM9 = REPO / "extracted" / "arm9_dec.bin"
SYMS = REPO / "config" / "arm9" / "symbols.txt"
VERIFIED = REPO / "symbols" / "verified.tsv"
MATCHED = REPO / "progress" / "matched.jsonl"
BASE = 0x02004000
md = Cs(CS_ARCH_ARM, CS_MODE_ARM)

LINE_RE = re.compile(r"^(\S+)\s+kind:function\((arm|thumb),size=0x([0-9a-fA-F]+)\)\s+addr:0x([0-9a-fA-F]+)")


def load_named():
    named = set()
    if VERIFIED.is_file():
        for l in VERIFIED.read_text(errors="ignore").splitlines():
            p = l.split("\t")
            if len(p) >= 1 and p[0].startswith("0x"):
                named.add(int(p[0], 16))
    return named


def load_matched():
    """arm9 addresses this tool must not hand to an agent.

    matched.jsonl alone is not enough: it is gitignored, so on a fresh clone this
    filtered nothing and the tool offered functions that were already matched, plus any
    carrying a verified floor mark. schedule_skip_set() supplies matched + locally parked
    + committed floors; a committed src/ file is the portable is-it-done signal on top."""
    done = {addr for mod, addr in L.schedule_skip_set() if mod == "arm9"}
    # Also skip known-hard functions that repeatedly fail (one hex addr per line).
    skip = REPO / "progress" / "skip.txt"
    if skip.is_file():
        for l in skip.read_text(errors="ignore").splitlines():
            l = l.split("#")[0].strip()
            if l:
                try:
                    done.add(int(l, 0))
                except Exception:
                    pass
    return done


def callees(code, fn_addr, size):
    """Set of call/tail-call target addresses outside the function body."""
    out = set()
    for insn in md.disasm(code, fn_addr):
        if insn.mnemonic in ("bl", "b") and insn.op_str.startswith("#"):
            try:
                t = int(insn.op_str[1:], 0)
            except ValueError:
                continue
            if not (fn_addr <= t < fn_addr + size):  # external = call/tailcall
                out.add(t)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--lo", type=lambda x: int(x, 0), default=BASE)
    ap.add_argument("--hi", type=lambda x: int(x, 0), default=0xFFFFFFFF)
    ap.add_argument("--min", type=lambda x: int(x, 0), default=0x10)
    ap.add_argument("--max", type=lambda x: int(x, 0), default=0x90)
    ap.add_argument("--count", type=int, default=20)
    args = ap.parse_args()

    named, done = load_named(), load_matched()
    a9 = ARM9.read_bytes()
    limit = BASE + len(a9)

    funcs = []
    for line in SYMS.read_text(errors="ignore").splitlines():
        m = LINE_RE.match(line)
        if not m:
            continue
        name, mode, sz, ad = m.group(1), m.group(2), int(m.group(3), 16), int(m.group(4), 16)
        if mode != "arm" or not (args.min <= sz <= args.max):
            continue
        if not (args.lo <= ad < args.hi) or not (BASE <= ad < limit) or ad in done:
            continue
        # A committed src/ file is the portable is-it-matched signal (matched.jsonl is
        # gitignored). A "// NONMATCHING" draft is NOT matched and stays a valid target,
        # but one settled under the asm-primitive policy is done and must not be offered.
        src = WL.read_src_text(name)
        if src is not None and ("// NONMATCHING" not in src[:200] or WL.is_policy_done(src)):
            continue
        code = a9[ad - BASE: ad - BASE + sz]
        cs = callees(code, ad, sz)
        known = sum(1 for t in cs if t in named)
        unknown = len(cs) - known
        score = known * 2 - unknown - (1 if sz > 0x80 else 0)
        funcs.append({"name": name, "addr": f"0x{ad:08x}", "size": sz,
                      "offset": f"0x{ad - BASE:x}", "_cs": cs,
                      "_score": score, "_known": known, "_unknown": unknown})

    # Union-find clustering by shared callee.
    parent = {f["addr"]: f["addr"] for f in funcs}
    def find(a):
        while parent[a] != a:
            parent[a] = parent[parent[a]]; a = parent[a]
        return a
    def union(a, b):
        parent[find(a)] = find(b)
    by_callee = {}
    for f in funcs:
        for c in f["_cs"]:
            by_callee.setdefault(c, []).append(f["addr"])
    for addrs in by_callee.values():
        for other in addrs[1:]:
            union(addrs[0], other)

    clusters = {}
    for f in funcs:
        clusters.setdefault(find(f["addr"]), []).append(f)
    # rank clusters by total score, functions within by score
    ranked = sorted(clusters.values(), key=lambda g: sum(x["_score"] for x in g), reverse=True)
    ordered = []
    for g in ranked:
        ordered.extend(sorted(g, key=lambda x: x["_score"], reverse=True))

    chosen = ordered[:args.count]
    print(f"candidates: {len(funcs)}  clusters: {len(clusters)}  -> picking {len(chosen)}")
    for f in chosen:
        print(f"  {f['name']:18} {f['addr']} sz {f['size']:<4} score {f['_score']:+d} "
              f"(known {f['_known']}/unknown {f['_unknown']})")
    out = [{k: f[k] for k in ("name", "addr", "size", "offset")} for f in chosen]
    (REPO / "match" / "targets_cluster.json").write_text(json.dumps(out, indent=2))
    print(f"\nwrote match/targets_cluster.json ({len(out)} targets)")


if __name__ == "__main__":
    main()
