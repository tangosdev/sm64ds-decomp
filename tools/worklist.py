"""Emit a worklist of unmatched functions with fully-resolved context, so an LLM
(or a fan-out of subagents) can crack them without running any exploration tools.

Per function it emits: module, name, addr, size, the target bytes (hex), and an
annotated disassembly where every bl/blx is resolved to its callee name and every
pc-relative load is resolved to the pool value -- a named symbol if that slot is a
relocation, else the literal constant. That resolved context is the difference
between a cheap agent ("here is the function, write the C") and an expensive one
("go figure out what this calls"), so it is all pre-computed here in the parent.

Usage:
    python tools/worklist.py --module ov002 --max 0x60 --limit 50        # JSONL
    python tools/worklist.py --module ov002 --addr 0x020b7f2c --pretty   # one, readable
    python tools/worklist.py --no-template --max 0x80 --limit 200        # only the LLM pile

A worklist line is self-contained JSON. To verify a candidate, an agent compiles it
and calls swarm.oracle_ok(c_source, name, bytes.fromhex(target_hex)); sources whose
first line is //cpp compile as C++ (swarm.CPP_FLAGS). See tools/triage.py for the
exact oracle / regperm-oracle code.
"""
import argparse
import json
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import swarm as S
import relocs as R
import modules as MOD
import sweep
import knowledge as KB
import demangle as DM

REPO_SRC = pathlib.Path(__file__).resolve().parent.parent / "src"
PCREL = re.compile(r"\[pc,#(0x[0-9a-fA-F]+|\d+)\]")
BRANCH = re.compile(r"b(eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le|al)?$")   # b<cond>, not bl/blx/bx


def is_easy(ins):
    """Heuristic for a high-LLM-hit-rate function: no loop, little control flow.
    The fan-out hit rate tracks this closely -- straight-line field/call code solves
    fast; loops and multi-branch logic are where agents miss."""
    branches = 0
    for i in ins:
        if not BRANCH.fullmatch(i.mnemonic):
            continue
        branches += 1
        m = re.fullmatch(r"#(0x[0-9a-fA-F]+|\d+)", S.squash(i.op_str))
        if m and int(m.group(1), 0) <= i.address:          # backward branch == loop
            return False
    return branches <= 2


def mnem_key(ins):
    """A function's instruction-mnemonic sequence, ignoring operands. Functions
    that share this are almost always the same idiom, so an already-matched one
    is a strong few-shot example for cracking the rest."""
    return tuple(i.mnemonic for i in ins)


def read_src_text(name):
    for ext in ("c", "cpp"):
        p = REPO_SRC / f"{name}.{ext}"
        if p.exists():
            return p.read_text(encoding="utf-8")
    return None


def callee_set(addr, ins, relocs, syms):
    """Frozenset of resolved bl/blx callee names for a function."""
    out = set()
    for i in ins:
        if i.mnemonic in ("bl", "blx"):
            e = relocs.get(addr + i.address)
            if e:
                out.add(R.name_of(e[1], syms))
    return frozenset(out)


def build_example_index(done, gsyms, size_cap, per_key=4):
    """Two indexes of matched functions for few-shot lookup: by mnemonic
    sequence (precise, same idiom) and by resolved callee set (catches the same
    library-call idiom across overlays even when offsets differ). Built once."""
    mnem_idx, callee_idx = {}, {}
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        relocs = R.load_relocs_file(mod["relocs"])
        data = mod["bin"].read_bytes()
        for name, addr, size in sweep.funcs(mod):
            if size > size_cap or (label, addr) not in done:
                continue
            ins = list(S.md.disasm(data[addr - mod["base"]:addr - mod["base"] + size], 0))
            if not ins:
                continue
            src = None
            mk = mnem_key(ins)
            mb = mnem_idx.setdefault(mk, [])
            if len(mb) < per_key:
                src = read_src_text(name)
                if src:
                    mb.append((name, src))
            ck = callee_set(addr, ins, relocs, gsyms)
            if ck:
                cb = callee_idx.setdefault(ck, [])
                if len(cb) < per_key:
                    if src is None:
                        src = read_src_text(name)
                    if src:
                        cb.append((name, src))
    return mnem_idx, callee_idx


def has_template(name, ins, tgt, addr, relocs, syms):
    """True if any rule even produces a candidate (shape recognized)."""
    for rule in S.RULES:
        if rule(name, ins, tgt):
            return True
    for rule in S.RELOC_RULES:
        if rule(name, ins, tgt, addr, relocs, syms):
            return True
    return False


def annotate(name, addr, size, tgt, relocs, syms):
    """Return (lines, callees, pool) where lines is the annotated disasm."""
    lines, callees, pool = [], [], []
    for i in S.md.disasm(tgt, 0):
        note = ""
        if i.mnemonic in ("bl", "blx", "b"):
            e = relocs.get(addr + i.address)
            if e:
                who = R.name_of(e[1], syms)
                note = f"  ; -> {who}"
                callees.append(who)
        else:
            m = PCREL.search(S.squash(i.op_str))
            if m:
                coff = i.address + 8 + int(m.group(1), 0)
                if 0 <= coff + 4 <= len(tgt):
                    e = relocs.get(addr + coff)
                    if e:
                        sym = R.name_of(e[1], syms)
                        note = f"  ; = &{sym}"
                        pool.append({"off": f"0x{coff:x}", "sym": sym})
                    else:
                        w = int.from_bytes(tgt[coff:coff + 4], "little")
                        note = f"  ; = 0x{w:x}"
                        pool.append({"off": f"0x{coff:x}", "const": f"0x{w:x}"})
        lines.append(f"  {i.mnemonic:<7} {i.op_str}{note}")
    return lines, list(dict.fromkeys(callees)), pool


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--module", default=None)
    ap.add_argument("--addr", type=lambda x: int(x, 0), default=None)
    ap.add_argument("--min", type=lambda x: int(x, 0), default=0x0)
    ap.add_argument("--max", type=lambda x: int(x, 0), default=0x200)
    ap.add_argument("--limit", type=int, default=0, help="0 = no limit")
    ap.add_argument("--no-template", action="store_true",
                    help="only functions no rule shape touches (the LLM pile)")
    ap.add_argument("--easy", action="store_true",
                    help="only straight-line / single-guard funcs (high LLM hit rate)")
    ap.add_argument("--spread", action="store_true",
                    help="round-robin across all modules, taking each module's freshest "
                         "(top-of-pile) funcs first -- keeps a batch in the high-hit-rate "
                         "regime instead of draining one module into its depleted tail")
    ap.add_argument("--class", dest="klass", default=None,
                    help="only methods of this C++ class (subsystem batching)")
    ap.add_argument("--list-classes", action="store_true",
                    help="print unmatched-function counts per C++ class, then exit")
    ap.add_argument("--pretty", action="store_true")
    ap.add_argument("--examples", type=int, default=2,
                    help="attach up to N verified sibling sources (same mnemonic "
                         "sequence) as few-shot examples per record; 0 disables")
    args = ap.parse_args()

    done = sweep.load_done()
    gsyms = R.load_all_syms()
    kb = KB.build_kb()
    ex_mnem, ex_callee = (build_example_index(done, gsyms, args.max)
                          if args.examples > 0 else ({}, {}))

    if args.list_classes:
        import collections
        counts = collections.Counter()
        for mod in MOD.modules():
            if args.module and mod["name"] != args.module:
                continue
            label = "arm9" if mod["name"] == "main" else mod["name"]
            for name, addr, size in sweep.funcs(mod):
                if (label, addr) in done or not (args.min <= size <= args.max):
                    continue
                d = DM.demangle(name)
                if d and d["class"]:
                    counts[d["class"]] += 1
        for cls, n in counts.most_common(40):
            print(f"  {n:4}  {cls}")
        return

    def emit(rec):
        if args.pretty:
            print(f"=== {rec['module']} {rec['name']} @ {rec['addr']} ({rec['size']}) ===")
            if rec["self"]:
                print(f"  signature: {rec['self']}")
            for ln in rec["disasm"]:
                print(ln)
            for c in rec["callees"]:
                print(f"  callee {c}: {rec['signatures'].get(c, '(unknown sig)')}")
            for ex in rec.get("examples", []):
                print(f"  example (same shape) {ex['name']}:")
                for ln in ex["c_source"].splitlines():
                    print(f"    {ln}")
            print()
        else:
            print(json.dumps(rec))

    # Build per-module candidate lists (sweep.funcs yields in address order, so the
    # head of each list is that module's freshest top-of-pile). In default mode we
    # stream module-by-module and stop at --limit; in --spread mode we round-robin
    # across modules so a single batch skims the easy head of many modules at once.
    buckets = {}
    order = []
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        if args.module and mod["name"] != args.module:
            continue
        relocs = R.load_relocs_file(mod["relocs"])
        data = mod["bin"].read_bytes()
        for name, addr, size in sweep.funcs(mod):
            if args.addr is not None and addr != args.addr:
                continue
            if (label, addr) in done or not (args.min <= size <= args.max):
                continue
            tgt = data[addr - mod["base"]:addr - mod["base"] + size]
            ins = list(S.md.disasm(tgt, 0))
            if not ins or S.is_thunk(ins):
                continue
            if args.easy and not is_easy(ins):
                continue
            if args.klass:
                d = DM.demangle(name)
                if not d or d["class"] != args.klass:
                    continue
            if args.no_template and has_template(name, ins, tgt, addr, relocs, gsyms):
                continue
            lines, callees, pool = annotate(name, addr, size, tgt, relocs, gsyms)
            # the payoff: each callee's known signature, plus this function's own
            sigs = {c: KB.sig_for(c, kb) for c in callees}
            sigs = {c: v for c, v in sigs.items() if v}
            rec = {"module": label, "name": name, "addr": f"0x{addr:08x}",
                   "size": f"0x{size:x}", "target_hex": tgt.hex(), "self": KB.sig_for(name, kb),
                   "callees": callees, "signatures": sigs, "pool": pool, "disasm": lines}
            if args.examples > 0:
                # prefer same mnemonic sequence (precise), then same callee set
                cands = list(ex_mnem.get(mnem_key(ins), []))
                if callees:
                    cands += ex_callee.get(frozenset(callees), [])
                seen, ex = set(), []
                for en, es in cands:
                    if en == name or en in seen:
                        continue
                    seen.add(en)
                    ex.append({"name": en, "c_source": es})
                    if len(ex) >= args.examples:
                        break
                if ex:
                    rec["examples"] = ex
            if label not in buckets:
                buckets[label] = []
                order.append(label)
            buckets[label].append(rec)
            if not args.spread and args.limit and \
                    sum(len(v) for v in buckets.values()) >= args.limit:
                break
        else:
            continue
        if not args.spread:
            break

    emitted = 0
    if args.spread:
        # round-robin: one from each module per pass, repeat until limit/exhausted
        idx = 0
        while True:
            progressed = False
            for label in order:
                if idx < len(buckets[label]):
                    emit(buckets[label][idx])
                    emitted += 1
                    progressed = True
                    if args.limit and emitted >= args.limit:
                        return
            if not progressed:
                return
            idx += 1
    else:
        for label in order:
            for rec in buckets[label]:
                emit(rec)
                emitted += 1
                if args.limit and emitted >= args.limit:
                    return


if __name__ == "__main__":
    main()
