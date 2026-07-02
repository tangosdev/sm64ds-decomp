"""Find idioms worth templatizing: shapes the agent tier already cracked that still
have many UNMATCHED siblings (especially on fresh modules). Every such sibling is a
function we would otherwise pay the LLM to re-derive -- turning the shape into a
free swarm.py template claws that spend back across the whole remainder.

For each agent-cracked function we take its mnemonic shape, then count how many
still-unmatched functions share that exact shape. Shapes with a high unmatched
count and a representative src example are the best templatization candidates.

Usage:
    python tools/recurring.py            # top recurring agent-cracked shapes
    python tools/recurring.py --min 4    # only shapes with >= N unmatched siblings
"""
import argparse
import collections
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import swarm as S
import modules as MOD
import sweep
import ledger as L

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"
AGENT_TAGS = {"agent-llm", "harvest"}


def shape_of(mod, addr, size):
    tgt = mod["bin"].read_bytes()[addr - mod["base"]:addr - mod["base"] + size]
    ins = list(S.md.disasm(tgt, 0))
    if not ins or S.is_thunk(ins):
        return None
    ins = S.code_insns(ins)              # drop the trailing literal pool
    return " ".join(i.mnemonic for i in ins)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--min", type=int, default=3)
    args = ap.parse_args()

    by_label = {("arm9" if m["name"] == "main" else m["name"]): m for m in MOD.modules()}
    # which (module,addr) were matched by the agent tier. "agent-llm" is the
    # older bank.py tag; "harvest" is the current coddog/bank_harvest path.
    agent_keys = set()
    for o in L.read_records(L.MATCHED):
        if AGENT_TAGS.intersection(o.get("versions", [])):
            agent_keys.add(L.key_of(o))

    # shapes the agent tier cracked, with a src example
    agent_shapes = {}
    for label, addr in agent_keys:
        mod = by_label.get(label)
        if not mod:
            continue
        for name, a, size in sweep.funcs(mod):
            if a == addr:
                sh = shape_of(mod, a, size)
                if sh:
                    agent_shapes.setdefault(sh, (name, label, a, size))
                break

    # count unmatched functions sharing each of those shapes (across ALL modules)
    done = L.load_done()
    unmatched = collections.Counter()
    fresh = collections.Counter()      # unmatched siblings outside arm9 (fresh modules)
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        for name, addr, size in sweep.funcs(mod):
            if (label, addr) in done:
                continue
            sh = shape_of(mod, addr, size)
            if sh in agent_shapes:
                unmatched[sh] += 1
                if label != "arm9":
                    fresh[sh] += 1

    rows = [(sh, n, fresh[sh], agent_shapes[sh]) for sh, n in unmatched.items() if n >= args.min]
    rows.sort(key=lambda r: -r[1])
    print(f"recurring agent-cracked shapes with >= {args.min} unmatched siblings:")
    print(f"{'unmatched':>9} {'fresh':>6}  example (src to copy the idiom from)")
    for sh, n, fr, (name, label, a, size) in rows:
        print(f"{n:9} {fr:6}  {name}  [{label} 0x{a:08x}]")
        print(f"          {sh[:76]}")
    total = sum(n for _, n, _, _ in rows)
    print(f"\n{len(rows)} shapes, {total} unmatched siblings recoverable by templatizing them"
          f" (src/<name>.c has the verified idiom for each).")


if __name__ == "__main__":
    main()
