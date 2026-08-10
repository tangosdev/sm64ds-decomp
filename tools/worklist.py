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
import asm_policy  # noqa: E402
import pathlib
import random
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import srcpath as SP  # noqa: E402
import swarm as S
import relocs as R
import modules as MOD
import sweep
import knowledge as KB
import demangle as DM
import claims_md as CLM
import ledger as L

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
    # A function can end up with BOTH src/<name>.c and src/<name>.cpp -- typically a
    # stale `// NONMATCHING` near-miss draft left behind when the real match later landed
    # under the other extension. Returning the first-found (old behaviour: .c before .cpp)
    # lets a stale NONMATCHING draft SHADOW a landed match, so every caller that checks
    # "is this matched?" sees the banner and wrongly treats the function as unmatched --
    # resurrecting it as a fan-out/permuter target and a near-miss-DB ghost. Prefer a real
    # match over a NONMATCHING draft when both exist.
    texts = [p.read_text(encoding="utf-8") for p in SP.paths_for(name)]
    for t in texts:
        if not asm_policy.has_draft_banner(t):
            return t
    return texts[0] if texts else None


# A committed `// NONMATCHING` file means one of two very different things, and both
# header families open with "hand-written asm", so the banner alone cannot separate them:
#
#   SETTLED  "// NONMATCHING (ASM-PRIMITIVE): ... no match to chase. Counts as done
#             under the asm-primitive policy"      -> assembly WAS the original source.
#   OWES C   "// NONMATCHING: hand-written asm ... does NOT count as matched. Reverts to
#             a draft until someone reproduces the bytes from real C"  -> still open work.
#
# Only the second is a legitimate target. Schedulers that tested for the banner alone
# treated both as open drafts and kept handing out settled assembly: #822 was a whole
# agent run that re-emitted an asm block already committed under the other extension.
_SETTLED_TAGS = ("NONMATCHING (ASM-PRIMITIVE)", "NONMATCHING (NOT-C-EXPRESSIBLE)")
_SETTLED_PHRASES = ("counts as done", "no match to chase", "nothing here to match")
_OWES_C = "does not count as matched"


def is_policy_done(src):
    """True when a committed NONMATCHING file is SETTLED under notes/asm-policy.md
    rather than an open draft. Never offer these as targets; they are also not valid
    sibling examples, since the body is assembly rather than recovered C."""
    if not src:
        return False
    head = src[:600]
    low = head.lower()
    if _OWES_C in low:                      # explicitly still owes a C decompilation
        return False
    return any(t in head for t in _SETTLED_TAGS) or any(p in low for p in _SETTLED_PHRASES)


def target_is_done(done, label, addr, name):
    """Treat the source tree as authoritative when the generated ledger lags."""
    return ((label, addr) in done
            or bool(SP.paths_for(name)))


def callee_set(addr, ins, relocs, syms):
    """Frozenset of resolved bl/blx callee names for a function."""
    out = set()
    for i in ins:
        if i.mnemonic in ("bl", "blx"):
            e = relocs.get(addr + i.address)
            if e:
                out.add(R.name_for_reloc(e, syms))
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
                who = R.name_for_reloc(e, syms)
                note = f"  ; -> {who}"
                callees.append(who)
        else:
            m = PCREL.search(S.squash(i.op_str))
            if m:
                coff = i.address + 8 + int(m.group(1), 0)
                if 0 <= coff + 4 <= len(tgt):
                    e = relocs.get(addr + coff)
                    if e:
                        sym = R.name_for_reloc(e, syms)
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
    ap.add_argument("--max", type=lambda x: int(x, 0), default=None)
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
    ap.add_argument("--ignore-claims", action="store_true",
                    help="schedule targets even if CLAIMS.md marks them active/partial "
                         "(default: skip them, so a batch never duplicates held work)")
    ap.add_argument("--examples", type=int, default=2,
                    help="attach up to N verified sibling sources (same mnemonic "
                         "sequence) as few-shot examples per record; 0 disables")
    ap.add_argument("--similar", action="store_true",
                    help="similarity scheduling: order functions by how many already-"
                         "matched lookalikes they have (most references first), so the "
                         "fan-out gets the best in-context examples. Collects the whole "
                         "pile then takes the top --limit.")
    ap.add_argument("--random", action="store_true",
                    help="pull unmatched functions uniformly at random (any size), reshuffled "
                         "every run -- the Random role; an infinite loop re-rolls each batch")
    args = ap.parse_args()
    # --random spans any size unless the caller set --max; other modes keep the 0x200 default.
    if args.max is None:
        args.max = 0x40000 if args.random else 0x200

    matched = L.matched_set()            # example pool: byte-exact matches only
    # matched + locally parked + verified floors. NOT load_done(): that reads only the
    # gitignored nonmatching.jsonl, so on a fresh clone this handed floor-marked
    # functions straight to an agent that built its worklist here instead of via coddog.
    done = L.schedule_skip_set()
    gsyms = R.load_all_syms()
    kb = KB.build_kb()
    ex_mnem, ex_callee = (build_example_index(matched, gsyms, args.max)
                          if (args.examples > 0 or args.similar) else ({}, {}))

    if args.list_classes:
        import collections
        counts = collections.Counter()
        for mod in MOD.modules():
            if args.module and mod["name"] != args.module:
                continue
            label = "arm9" if mod["name"] == "main" else mod["name"]
            for name, addr, size in sweep.funcs(mod):
                if (target_is_done(done, label, addr, name)
                        or not (args.min <= size <= args.max)):
                    continue
                d = DM.demangle(name)
                if d and d["class"]:
                    counts[d["class"]] += 1
        for cls, n in counts.most_common(40):
            print(f"  {n:4}  {cls}")
        return

    # Someone else's active claim (API lock or CLAIMS.md row) means that function is taken.
    # Scheduling it anyway is how a batch ends up duplicating work another contributor
    # already finished.
    held = CLM.held_targets() if not args.ignore_claims else {"names": set(), "addrs": set(), "ranges": [], "rows": 0}
    try:
        import claims as _CL
        _msg = _CL.key_reminder()
        if _msg:
            print(f"[claims] {_msg}", file=sys.stderr)
    except Exception:
        pass
    skipped_claimed = [0]

    def emit(rec):
        if CLM.is_held(held, rec.get("name"), rec.get("addr"), module=rec.get("module")):
            skipped_claimed[0] += 1
            return
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
            rec.pop("_sim", None)
            print(json.dumps(rec))

    # Build per-module candidate lists (sweep.funcs yields in address order, so the
    # head of each list is that module's freshest top-of-pile). In default mode we
    # stream module-by-module and stop at --limit; in --spread mode we round-robin
    # across modules so a single batch skims the easy head of many modules at once.
    def build_rec(label, name, addr, size, tgt, relocs):
        """Disassemble, filter (thunk/easy/class/template), and annotate one function into a
        worklist record with callee signatures + few-shot examples; None if it's filtered out."""
        ins = list(S.md.disasm(tgt, 0))
        if not ins or S.is_thunk(ins):
            return None
        if args.easy and not is_easy(ins):
            return None
        if args.klass:
            d = DM.demangle(name)
            if not d or d["class"] != args.klass:
                return None
        if args.no_template and has_template(name, ins, tgt, addr, relocs, gsyms):
            return None
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
        if args.similar:
            # similarity = count of already-matched lookalikes (same mnemonic
            # shape or same callee set). More references -> schedule earlier.
            rec["_sim"] = (len(ex_mnem.get(mnem_key(ins), []))
                           + len(ex_callee.get(frozenset(callees), [])))
        return rec

    # --random (the Random role): pull unmatched functions uniformly at random. Collect all
    # candidates cheaply (name/addr/size), shuffle, then annotate just up to --limit of them.
    # random reseeds per process, so the console's infinite loop -- which spawns a fresh worklist
    # run each cycle -- re-rolls a different set every batch.
    if args.random:
        cands = []
        for mod in MOD.modules():
            if args.module and mod["name"] != ("main" if args.module == "arm9" else args.module):
                continue
            label = "arm9" if mod["name"] == "main" else mod["name"]
            data = mod["bin"].read_bytes()
            for name, addr, size in sweep.funcs(mod):
                if (target_is_done(done, label, addr, name)
                        or not (args.min <= size <= args.max)):
                    continue
                cands.append((label, name, addr, size, mod, data))
        random.shuffle(cands)
        reloc_cache = {}
        emitted = 0
        for label, name, addr, size, mod, data in cands:
            if args.limit and emitted >= args.limit:
                break
            relocs = reloc_cache.get(mod["name"])
            if relocs is None:
                relocs = reloc_cache[mod["name"]] = R.load_relocs_file(mod["relocs"])
            tgt = data[addr - mod["base"]:addr - mod["base"] + size]
            rec = build_rec(label, name, addr, size, tgt, relocs)
            if rec is not None:
                emit(rec)
                emitted += 1
        return

    buckets = {}
    order = []
    for mod in MOD.modules():
        label = "arm9" if mod["name"] == "main" else mod["name"]
        # accept either vocabulary: internal "main" or its public label "arm9"
        if args.module and mod["name"] != ("main" if args.module == "arm9" else args.module):
            continue
        relocs = R.load_relocs_file(mod["relocs"])
        data = mod["bin"].read_bytes()
        for name, addr, size in sweep.funcs(mod):
            if args.addr is not None and addr != args.addr:
                continue
            # An exact --addr means the caller wants THAT function - a hand-picked drive target -
            # even if it's already matched or parked as nonmatching. Only apply the completed-
            # target exclusion in list/scheduling mode, so batch generation still skips them.
            if ((args.addr is None and target_is_done(done, label, addr, name))
                    or not (args.min <= size <= args.max)):
                continue
            tgt = data[addr - mod["base"]:addr - mod["base"] + size]
            rec = build_rec(label, name, addr, size, tgt, relocs)
            if rec is None:
                continue
            if label not in buckets:
                buckets[label] = []
                order.append(label)
            buckets[label].append(rec)
            if not args.spread and not args.similar and args.limit and \
                    sum(len(v) for v in buckets.values()) >= args.limit:
                break
        else:
            continue
        if not args.spread:
            break

    if args.similar:
        for label in order:
            buckets[label].sort(key=lambda r: -r.get("_sim", 0))

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
