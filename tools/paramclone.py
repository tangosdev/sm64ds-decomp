"""Parameterized free matcher: a step beyond clone.py.

clone.py only banks functions that are byte-identical to a matched one at every
non-relocation position. This relaxes that: it pairs an unmatched function with a
matched one that has the SAME instruction skeleton (same mnemonics and operand
structure, differing only in immediate values), then rewrites the matched source
by substituting M's immediates with U's and re-checks against the ROM.

The immediates that vary across an idiom family are field offsets, shift amounts,
and non-relocation pool constants -- exactly what distinguishes, say, one actor's
cleanup from another's. Relocation slots (call/data symbol references) are already
wildcarded by the oracle, so symbol names need no substitution -- which is exactly
why the rewritten source can name a WRONG callee or table and still byte-pass.
Before compiling, a template is skipped unless config/**/relocs.txt records the
same destination at every reloc slot the two functions share (the clone.py gate,
same incident: 24 wrong-table PMF paramclones on 2026-07-02).

Nothing is trusted: every rewritten candidate is compiled and byte-compared. A bad
substitution simply fails the oracle and is dropped, so the pass is always safe.
Like clone.py it compounds -- each newly matched representative becomes a template
for its whole skeleton family -- so run it after each LLM batch.

Dry-run by default; pass --apply to bank.

Usage:
    python tools/paramclone.py                 # report only, bank nothing
    python tools/paramclone.py --apply         # verify + bank
    python tools/paramclone.py --max 0x200     # size cap (default 0x200)
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
import clone as CL
import ledger as L

REPO = pathlib.Path(__file__).resolve().parent.parent

PCREL = re.compile(r"\[pc,\s*#(-?(?:0x[0-9a-fA-F]+|[0-9]+))\]")
IMM = re.compile(r"#(-?(?:0x[0-9a-fA-F]+|[0-9]+))")
# a numeric literal in C source that is not part of an identifier or a float
NUM = re.compile(r"(?<![A-Za-z0-9_])(-?0x[0-9a-fA-F]+|-?\d+)(?![A-Za-z0-9_.])")


def analyze(addr, ins, raw, relocs):
    """(skeleton, values): skeleton is the operand structure with every
    substitutable immediate blanked; values are those immediates in order, so
    two functions with equal skeletons have positionally-aligned value lists.
    Non-reloc pool constants are captured as values; reloc pool slots are not."""
    skel, vals = [], []
    for i in ins:
        if i.mnemonic in ("b", "bl", "blx"):
            skel.append(i.mnemonic)            # branch target: reloc/wildcarded
            continue
        op = S.squash(i.op_str)
        m = PCREL.search(op)
        if m:
            coff = i.address + 8 + int(m.group(1), 0)
            tok = "[pc]"
            if 0 <= coff + 4 <= len(raw):
                if relocs.get(addr + coff):
                    tok = "[pc:r]"             # symbol reference, wildcarded
                else:
                    w = int.from_bytes(raw[coff:coff + 4], "little")
                    tok = "[pc:c]"             # literal constant, substitutable
                    vals.append(w)
            op = PCREL.sub(tok, op, count=1)

        def grab(mm):
            vals.append(int(mm.group(1), 0))
            return "#"
        op = IMM.sub(grab, op)
        skel.append(i.mnemonic + "|" + op)
    return tuple(skel), vals


def substitute(src, mvals, uvals):
    """Rewrite source numeric literals per the positional M->U immediate map.
    Returns None if the map is inconsistent or nothing actually changes."""
    cmap = {}
    for mv, uv in zip(mvals, uvals):
        if mv == uv:
            continue
        if cmap.get(mv, uv) != uv:
            return None                        # same M value, two U values: ambiguous
        cmap[mv] = uv
    if not cmap:
        return None                            # identical values: that's clone.py's job

    def rep(m):
        tok = m.group(1)
        try:
            v = int(tok, 0)
        except ValueError:
            return tok
        if v not in cmap:
            return tok
        uv = cmap[v]
        if tok.lower().lstrip("-").startswith("0x"):
            return ("-0x%x" % -uv) if uv < 0 else ("0x%x" % uv)
        return str(uv)
    return NUM.sub(rep, src)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--max", type=lambda x: int(x, 0), default=0x200)
    ap.add_argument("--min", type=lambda x: int(x, 0), default=0x0)
    ap.add_argument("--per-skel", type=int, default=4,
                    help="templates to try per skeleton before giving up")
    ap.add_argument("--apply", action="store_true", help="bank the verified matches")
    ap.add_argument("--dry-run", action="store_true",
                    help="no-op; dry-run is the default (kept for compatibility)")
    args = ap.parse_args()
    do_apply = args.apply and not args.dry_run

    matched_keys = L.matched_set()       # byte-exact matches: template candidates
    done = L.load_done()                 # matched + parked: not re-attempted
    templates = {}     # skeleton -> list of (name, src, vals)
    unmatched = []     # (skeleton, vals, module, addr, size, name, target_bytes)
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        relocs = R.load_relocs_file(mod["relocs"])
        data = mod["bin"].read_bytes()
        for name, addr, size in SW.funcs(mod):
            if not (args.min <= size <= args.max):
                continue
            raw = data[addr - mod["base"]:addr - mod["base"] + size]
            ins = list(S.md.disasm(raw, 0))
            if not ins or S.is_thunk(ins):
                continue
            skel, vals = analyze(addr, ins, raw, relocs)
            # config-recorded reloc destinations, keyed by slot offset
            rd = {o: relocs[addr + o][1] for o in range(0, size, 4)
                  if (addr + o) in relocs}
            if (label, addr) in matched_keys:
                bucket = templates.setdefault(skel, [])
                if len(bucket) < args.per_skel:
                    src = CL.read_src(name)
                    if src:
                        bucket.append((name, src, vals, rd))
            elif (label, addr) not in done:
                unmatched.append((skel, vals, label, addr, size, name, raw, rd))

    print(f"skeleton templates: {len(templates)}   unmatched in range: {len(unmatched)}")

    passed, tried, gated = [], 0, 0
    for skel, uvals, label, addr, size, name, tb, rd in unmatched:
        for old_name, old_src, mvals, mrd in templates.get(skel, []):
            if len(mvals) != len(uvals):
                continue
            if any(mrd[o] != d for o, d in rd.items() if o in mrd):
                gated += 1     # template links a different destination: skip it
                continue
            cand = substitute(old_src, mvals, uvals)
            if cand is None:
                continue
            cand = CL.retarget(cand, old_name, name)
            tried += 1
            try:
                if S.oracle_ok(cand, name, tb):
                    passed.append((name, addr, size, label, cand))
                    break
            except Exception:
                continue

    print(f"parameterized clones VERIFIED {len(passed)} (candidates tried {tried}, "
          f"dest-gated {gated})")
    if not do_apply:
        for name, addr, size, mod, _ in passed[:30]:
            print(f"  would bank {mod} {name} (0x{size:x})")
        print("(dry-run: nothing banked; re-run with --apply)")
        return

    landed = 0
    for name, addr, size, mod, src in passed:
        landed += L.bank({"addr": addr, "name": name, "size": size, "module": mod,
                          "versions": ["paramclone"]}, src) == "banked"
    print(f"banked {landed}/{len(passed)}")


if __name__ == "__main__":
    main()
