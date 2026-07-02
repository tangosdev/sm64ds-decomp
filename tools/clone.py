"""Free, no-LLM matcher: bank unmatched functions that are exact structural
clones of an already-matched one.

Two functions are matched by the same C source when their bytes are identical at
every position that is NOT a relocation slot (the oracle wildcards reloc slots,
so the call/data targets may differ). When that holds, the verified source of the
matched function, retargeted to the clone's name, matches the clone too. No model
is involved: we retarget and re-run the oracle, banking only what passes.

One more check, because the oracle wildcards reloc slots: the retargeted source
still names the TEMPLATE's callees and globals, so the clone only genuinely
matches when config/**/relocs.txt records the SAME destination at every slot for
both functions. Byte-identical PMF/table dispatchers fail exactly there (each
sibling reads its own table) -- 24 of them byte-passed and banked wrong across
two machines on 2026-07-02. The gate compares the two config reloc maps directly
(no compile) and rejects mismatches before the oracle ever runs.

This compounds: every function the LLM loop lands becomes a template for its
byte-clones, so re-running clone.py after each bank harvests the new clones for
free. Run it as the cheap first pass before spending tokens on a fan-out.

Dry-run by default; pass --apply to bank.

Usage:
    python tools/clone.py                 # report only, bank nothing
    python tools/clone.py --apply         # verify + bank all clones
    python tools/clone.py --max 0x200     # limit by function size
"""
import argparse
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import modules as MOD
import relocs as R
import sweep as SW
import swarm as S
import ledger as L

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src"


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
    ap.add_argument("--apply", action="store_true", help="bank the verified clones")
    ap.add_argument("--dry-run", action="store_true",
                    help="no-op; dry-run is the default (kept for compatibility)")
    args = ap.parse_args()
    do_apply = args.apply and not args.dry_run

    matched_keys = L.matched_set()       # byte-exact matches: template candidates
    done = L.load_done()                 # matched + parked: not re-attempted

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
            # config-recorded destination address at each reloc slot, in slot
            # order (k[2] slots exist in the map by norm_key construction)
            dests = tuple(relocs[addr + o][1] for o in k[2])
            if (label, addr) in matched_keys:
                if k not in matched:
                    src = read_src(name)
                    if src:
                        matched[k] = (name, src, raw, dests)
            elif (label, addr) not in done:
                unmatched.append((k, label, addr, size, name, raw, dests))

    print(f"matched templates: {len(matched)}   unmatched in range: {len(unmatched)}")

    passed, rejects = [], []
    for k, label, addr, size, name, tb, dests in unmatched:
        tpl = matched.get(k)
        if not tpl:
            continue
        old_name, old_src, _, tpl_dests = tpl
        if dests != tpl_dests:
            # the retargeted source would link to the template's destinations,
            # not this function's -- byte-"match" with wrong relocs. Refuse.
            bad = next((o, d, td) for o, d, td in
                       zip(k[2], dests, tpl_dests) if d != td)
            rejects.append((name, f"wrong-dest vs template {old_name}: "
                                  f"+0x{bad[0]:x} needs 0x{bad[1]:08x}, "
                                  f"template links 0x{bad[2]:08x}"))
            continue
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
    if not do_apply:
        print("(dry-run: nothing banked; re-run with --apply)")
        return

    landed = 0
    for name, addr, size, mod, src in passed:
        landed += L.bank({"addr": addr, "name": name, "size": size, "module": mod,
                          "versions": ["clone"]}, src) == "banked"
    print(f"banked {landed}/{len(passed)}")


if __name__ == "__main__":
    main()
