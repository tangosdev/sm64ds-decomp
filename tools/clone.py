"""Free, no-LLM matcher: bank unmatched functions that are exact structural
clones of an already-matched one.

Two functions are matched by the same C source when their bytes are identical at
every position that is NOT a relocation slot (the oracle wildcards reloc slots,
so the call/data targets may differ). When that holds, the verified source of the
matched function, retargeted to the clone's name, matches the clone too. No model
is involved: we retarget and re-run the oracle, banking only what passes.

This compounds: every function the LLM loop lands becomes a template for its
byte-clones, so re-running clone.py after each bank harvests the new clones for
free. Run it as the cheap first pass before spending tokens on a fan-out.

Usage:
    python tools/clone.py                 # verify + bank all clones
    python tools/clone.py --dry-run       # report only, bank nothing
    python tools/clone.py --max 0x200     # limit by function size
"""
import argparse
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import modules as MOD
import relocs as R
import sweep as SW
import swarm as S

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"
LEDGER = REPO / "progress" / "matched.jsonl"


def norm_key(base, addr, size, raw, relocs):
    """Identity of a function modulo relocation slots. Equal keys => the same
    source matches both. Every 4-byte word that is a reloc slot is zeroed; the
    surviving bytes (opcodes, immediates, pool constants) must be identical."""
    if len(raw) != size or size == 0 or size % 4:
        return None
    b = bytearray(raw)
    offs = []
    for slot in range(0, size, 4):
        if relocs.get(addr + slot):
            b[slot:slot + 4] = b"\x00\x00\x00\x00"
            offs.append(slot)
    return (size, bytes(b), tuple(offs))


def retarget(src, old, new):
    """Rename the function identifier old -> new at token boundaries."""
    return re.sub(r"(?<![A-Za-z0-9_])" + re.escape(old) + r"(?![A-Za-z0-9_])",
                  new, src)


def read_src(name):
    for ext in ("c", "cpp"):
        p = SRC / f"{name}.{ext}"
        if p.exists():
            return p.read_text(encoding="utf-8")
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max", type=lambda x: int(x, 0), default=0x200)
    ap.add_argument("--min", type=lambda x: int(x, 0), default=0x0)
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    done = SW.load_done()

    # Index matched functions by structural key (only those we have source for).
    matched = {}          # key -> (name, src, target_bytes)
    unmatched = []        # (key, module, addr, size, name, target_bytes)
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        relocs = R.load_relocs_file(mod["relocs"])
        data = mod["bin"].read_bytes()
        for name, addr, size in SW.funcs(mod):
            if not (args.min <= size <= args.max):
                continue
            raw = data[addr - mod["base"]:addr - mod["base"] + size]
            k = norm_key(mod["base"], addr, size, raw, relocs)
            if k is None:
                continue
            ins = list(S.md.disasm(raw, 0))
            if not ins or S.is_thunk(ins):
                continue
            if (label, addr) in done:
                if k not in matched:
                    src = read_src(name)
                    if src:
                        matched[k] = (name, src, raw)
            else:
                unmatched.append((k, label, addr, size, name, raw))

    print(f"matched templates: {len(matched)}   unmatched in range: {len(unmatched)}")

    passed, rejects = [], []
    for k, label, addr, size, name, tb in unmatched:
        tpl = matched.get(k)
        if not tpl:
            continue
        old_name, old_src, _ = tpl
        cand = retarget(old_src, old_name, name)
        try:
            ok = S.oracle_ok(cand, name, tb)
        except Exception as e:
            rejects.append((name, f"error:{str(e)[:40]}"))
            continue
        if ok:
            passed.append((name, addr, size, label, cand))
        else:
            rejects.append((name, "oracle-False"))

    print(f"clones VERIFIED {len(passed)} (rejects {len(rejects)})")
    for r in rejects[:20]:
        print(f"  reject {r[0]}  {r[1]}")
    if args.dry_run:
        print("(dry-run: nothing banked)")
        return

    for name, addr, size, mod, src in passed:
        ext = "cpp" if src.startswith("//cpp") else "c"
        (SRC / f"{name}.{ext}").write_text(src if src.endswith("\n") else src + "\n",
                                           encoding="utf-8")
        with LEDGER.open("a") as f:
            f.write(json.dumps({"addr": f"0x{addr:08x}", "name": name, "size": size,
                                "module": mod, "versions": ["clone"]}) + "\n")
    print(f"banked {len(passed)}")


if __name__ == "__main__":
    main()
