"""Persistent near-miss database: never throw away a close attempt again.

Every fan-out emits "near_misses" -- compiling C that didn't byte-match, usually only a
few instructions off (codegen shape). Those are the most valuable byproduct we have:
logically-correct, already-compiling, often 1-2 fixes from done. This stores them forever,
keyed by (module, addr), keeping the CLOSEST candidate per function (fewest diverging
instructions). When a better model or a new idiom lands, we start from 95%-done, not scratch.

The DB lives at nearmiss/db.jsonl (committed, so the work survives). Each record:
  {module, addr, name, size, target_hex, lang, divergences, c_source, source}
plus, on rows the evaluator has touched:
  cand_size   assembled byte size of c_source's function (size is the TARGET's byte
              size, so cand_size - size is the seed's size gap; retrieval ranks by
              (divergences, |gap|) -- see seed_rank)
  evaluator   fingerprint of the evaluator that produced divergences, "2004/b56|m1"
and, on rows whose stored source NO LONGER scores under the current evaluator:
  status      "noncompile" (the source stopped compiling -- header churn under it)
              or "func-absent" (compiles, but the symbol is missing from the object)
  error       trimmed compiler/extract detail
  stale_divergences  the last score before it went unscorable (divergences itself
              becomes null, so every consumer ranks the row last instead of serving
              poisoned bait at the top of a closest-first worklist)
plus an optional "floor" object on entries whose residual is verified compiler-internal:
  {"class": "ordering", "evidence": "...", "date": "YYYY-MM-DD"}
plus, on a row whose divergence count was set by hand via `set-divergence` rather than
by the live evaluator (see set_divergence / _is_manual below):
  manual_divergences  true
  manual_evidence     what was measured and how (e.g. a pasted match.py run)
  manual_date         "YYYY-MM-DD"
  A manual row is held, not silently overwritten, by both `reeval` and `bank-matches`
  (2026-09-09: before METRIC_REV 2, evaluate_full's own SequenceMatcher-based count
  could UNDERCOUNT a pure instruction reorder relative to tools/match.py's positional
  MISMATCH count -- the number every src/ header, floors.jsonl and worklist.py actually
  reports -- so the live evaluator could be WRONG in a way that silently ratchets a
  correction back down; `set-divergence` exists for exactly that gap). Since
  evaluate_full now calls match.compare() directly (METRIC_REV 2), a manual row's own
  live score is verified every reeval pass, and the hold is RELEASED automatically the
  moment it agrees with the hand-set number on its own -- see _manual_hold_releasable.
  A row whose live score still disagrees stays held, untouched, exactly as before.

Scores are only comparable while the evaluator that produced them still exists:
include/ churn under the stored sources, a canonical-compiler bump, or a metric change
all silently invalidate the ranking (2026-08-30: 5 of 8 sampled rows re-scored
differently under the then-current evaluator; two rows recorded at divergence 13 no
longer compiled at all). nearmiss/eval_pin.json records the evaluator of the last full
`reeval` pass; every ranking consumer warns when the live evaluator no longer matches
it, and tools/test_nearmiss_db.py holds the pin to tools/match.py's CANONICAL and
METRIC_REV below so the drift is a CI failure, not a quiet lie.
Floored entries are excluded from export-close and refine_wl.py by default -- the
permuter provably cannot flip pure instruction-ordering residuals (see
notes/mwccarm-codegen.md), so automated tiers stop burning compute on them. They stay
in the DB as the hand-fix / spelling-hunt backlog; a strictly better ingest replaces
the record and clears the mark (the residual changed, the floor claim is stale).

Usage:
  python tools/nearmiss_db.py ingest --result <fanout-output.json> --worklist progress/wl.jsonl
  python tools/nearmiss_db.py ingest --seeds <seeds.jsonl>
  python tools/nearmiss_db.py stats
  python tools/nearmiss_db.py list --max-div 12
  python tools/nearmiss_db.py export-close --max-div 8 --out progress/close.jsonl   # permuter seeds
  python tools/nearmiss_db.py bank-matches      # re-check every entry; bank any that now score 0
  python tools/nearmiss_db.py reeval            # re-score every row; mark unscorable ones; write the eval pin
  python tools/nearmiss_db.py prune-matched     # drop ghosts already matched in committed src/
  python tools/nearmiss_db.py mark-floor --name <func> --class ordering --evidence "levers tried ..."
  python tools/nearmiss_db.py unmark-floor --name <func>
  python tools/nearmiss_db.py set-divergence --name <func> --divergences <N> --evidence "..."
                                                 # hand-correct one row's count (either
                                                 # direction) when the live evaluator
                                                 # disagrees with the canonical measurement;
                                                 # held out of reeval/bank-matches after
                                                 # (reeval auto-releases the hold once the
                                                 # live evaluator agrees naturally, see
                                                 # _manual_hold_releasable)
  python tools/nearmiss_db.py check-refs --check  # exit 1 if any c_source names a symbol
                                                 # config no longer has
  python tools/nearmiss_db.py fix-refs          # rewrite a dead reference in c_source to
                                                 # its current name, when resolvable by
                                                 # address (see check-refs)
  python tools/nearmiss_db.py dedupe --check    # exit 1 if any (module, addr) holds 2+ rows
"""
import argparse
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import asm_policy  # noqa: E402
import ledger as L
# match/swarm (the compile+disasm stack) need capstone and pyelftools; the functions
# that evaluate candidates import them lazily so the metadata-only subcommands
# (stats, list, dedupe) and tools/test_nearmiss_db.py run on a bare interpreter.

DB = REPO / "nearmiss" / "db.jsonl"
LOCKDIR = REPO / "nearmiss" / ".lock"
PIN = REPO / "nearmiss" / "eval_pin.json"
REG = re.compile(r"\b(r\d+|sb|sl|fp|ip|sp|lr|pc)\b")

# Version of the divergence METRIC itself (the disasm normalization + the differ in
# evaluate_full). Bump it whenever a change here would re-score an unchanged source,
# then run `reeval` on a main-tip checkout and commit db.jsonl + eval_pin.json with
# the bump -- tools/test_nearmiss_db.py fails until the pin agrees.
#
# METRIC_REV 2 (2026-09-09): evaluate_full's divergence count is now ONE call to
# tools/match.py's own compare() -- the exact positional, reloc-wildcarded word
# compare match.py prints -- instead of a second, separate implementation
# (difflib.SequenceMatcher over a re-disassembled instruction list). The two metrics
# disagreed on a pure instruction reorder: a 3-word ROM rotation scored 2 under the
# old SequenceMatcher diff (it finds a cheaper alignment across the reorder) but 3
# under match.py's fixed-position compare -- the number every committed src/ header,
# floors.jsonl and worklist.py actually reports against. Because ingest/merge_batch is
# deliberately strictly-improving (see merge_batch), that disagreement could silently
# freeze a stored row at a WRONG, too-low number forever with no path to correct it --
# the four rows `set-divergence` fixed by hand. A stored row and a fresh
# `tools/match.py` run on that row's own c_source can no longer disagree.
METRIC_REV = 2

_PIN_CACHE = {}


def _fingerprint(canonical, metric):
    return f"{canonical}|m{metric}"


def pin_fingerprint():
    """Evaluator fingerprint recorded by the last full `reeval` pass
    (nearmiss/eval_pin.json), or None if no pass was ever recorded. Deliberately
    light -- no compile stack -- so dedupe on a bare interpreter (the union-merge
    cleanup in CI) can still rank stamped rows against stale ones."""
    key = str(PIN)
    if key not in _PIN_CACHE:
        fp = None
        try:
            pin = json.loads(PIN.read_text(encoding="utf-8"))
            fp = _fingerprint(pin["canonical"], pin["metric"])
        except (OSError, ValueError, KeyError, TypeError):
            pass
        _PIN_CACHE[key] = fp
    return _PIN_CACHE[key]


def current_fingerprint():
    """Fingerprint of the LIVE evaluator, or None where the compile stack is absent
    (bare interpreters running the metadata-only subcommands)."""
    try:
        import match as M
    except ImportError:
        return None
    return _fingerprint(M.CANONICAL, METRIC_REV)


def warn_stale_pin():
    """One loud stderr line when the live evaluator no longer matches the recorded
    pin: every stored divergence is then suspect until `reeval` runs. Quiet on bare
    interpreters (they cannot know the live evaluator; the CI pin test still gates
    canonical/metric bumps there)."""
    cur = current_fingerprint()
    if cur is None:
        return
    rec = pin_fingerprint()
    if rec is None:
        print("nearmiss_db: WARNING no nearmiss/eval_pin.json -- stored divergences have "
              "no recorded evaluator; run `python tools/nearmiss_db.py reeval` on a "
              "main-tip checkout", file=sys.stderr)
        return
    if rec != cur:
        print(f"nearmiss_db: WARNING stored divergences were scored under {rec} but the "
              f"live evaluator is {cur}; scores and rankings are stale -- run "
              f"`python tools/nearmiss_db.py reeval` on a main-tip checkout",
              file=sys.stderr)
        return
    try:
        pin = json.loads(PIN.read_text(encoding="utf-8"))
        import match as M
        if pin.get("flags") != M.DEFAULT_FLAGS:
            print("nearmiss_db: WARNING build flags changed since the last reeval pass "
                  "(nearmiss/eval_pin.json disagrees with match.DEFAULT_FLAGS); re-run "
                  "`python tools/nearmiss_db.py reeval`", file=sys.stderr)
    except (OSError, ValueError):
        pass


def locked():
    """Cross-process mutex (atomic mkdir on nearmiss/.lock) so multiple cruncher
    instances can safely read-modify-write the DB. Hold it only for the brief write,
    never while permuting. Ledger writes have their own lock inside ledger.py."""
    return L.locked(LOCKDIR)


def _disasm(code, relocs):
    import swarm as S
    out = []
    for ins in S.code_insns(list(S.md.disasm(code, 0))):
        if ins.address in relocs:
            out.append("reloc")
        else:
            op = re.sub(r"\[pc, #-?0x[0-9a-fA-F]+\]", "[pc,#pool]", ins.op_str)
            out.append(f"{ins.mnemonic} {op}".rstrip())
    return out


def _trim_error(text, limit=240):
    """Compiler chatter -> one storable line. compile_c prints its detail with a
    '! compile failed (<version>):' prefix; strip it, drop warning lines when a real
    error line exists (every run leads with an inert MWCIncludes warning), collapse."""
    m = re.search(r"! compile failed \([^)]*\):\s*(.*)", text, re.S)
    if m:
        text = m.group(1)
    lines = [l.strip() for l in text.splitlines() if l.strip()]
    keep = [l for l in lines if "warning:" not in l] or lines
    text = " | ".join(keep)
    # The temp compile dir is machine-local noise (and carries a home-dir path into a
    # committed file); keep only the candidate file name and line.
    text = re.sub(r"\S*[\\/](cand\.c(?:pp)?)", r"\1", text)
    return text[:limit] if text else "compile failed (no diagnostic captured)"


_NAME_INDEX_CACHE = None


def _name_index():
    """Lazy, process-wide cache of reloc_audit.build_name_index() (a scan of every
    committed symbols.txt) -- expensive enough that evaluate_full must not rebuild
    it per row when reeval/bank-matches call it hundreds of times in one pass."""
    global _NAME_INDEX_CACHE
    if _NAME_INDEX_CACHE is None:
        import reloc_audit as RA
        _NAME_INDEX_CACHE = RA.build_name_index()
    return _NAME_INDEX_CACHE


def evaluate_full(src, name, target, module=None, addr=None, size=None, name_index=None):
    """Evaluate a candidate against the target bytes with the live evaluator. Returns
      {divergences, ok, cand_size, status, error}
    divergences  count of differing WORDS (reloc-wildcarded) -- tools/match.py's own
                 compare(), the exact number a pasted `match.py --brief` run prints for
                 this c_source. None if unscorable. A size mismatch scores 999
                 (compare()'s own sentinel: the permuter cannot add or remove
                 instructions, so a differently-sized candidate is not "close" on any
                 axis a word compare can express; cand_size / _size_gap rank those).
    ok           true RELOC-AWARE oracle match (reloc slots wildcarded, NOT raw equality)
    cand_size    assembled byte size of the candidate function, None if unscorable
    status/error None on a scorable row; "noncompile"/"func-absent" + detail otherwise
    resolved   the symbol actually scored, when it is not the row's stored `name`
    ONE compile, ONE compare: match.compare(target, cand, relocs) is called exactly
    once and its (ok, ndiff) is BOTH the match verdict and the divergence count, so a
    stored row and a fresh `tools/match.py` run on the same c_source can never
    disagree with each other -- and never with the oracle, since there is only one
    scorer, not two (a second, separate difflib.SequenceMatcher-based diff used to
    compute the count here; see METRIC_REV).

    A name miss falls back to the object's SOLE sized STT_FUNC before it is called an
    absence: the stored `name` goes stale against its own `c_source` (a symbol import
    renames the row to the C++ symbol while the source still spells func_<addr>), and
    the repo's substitution-compressed manglings are not the spelling mwccarm emits.
    Both look identical to an exact-string lookup and both demote a perfectly good,
    often size-exact seed to last place in every worklist. See match.sole_func_symbol.

    module/addr/size/name_index (all optional; every existing caller that omits them
    keeps the exact behavior above unchanged) let evaluate_full resolve the candidate
    the same way tools/linkcheck.py's byte gate does, for the same two shapes
    LINKCHK2 (#2535) closed there -- shared, not re-derived, so a stored row can never
    disagree with a fresh linkcheck run on its own c_source:
      * zero-size EABI alias (size == 0, e.g. _dadd at 0x01ff8000): the row's own
        declared size is a second name for a differently-named, correctly-sized
        primary at the SAME address (func_01ff8000, size 0x59c) -- nothing compiles
        to 0 bytes, so a 0-byte target could never be byte-compared.
        bytegate.alias_target_size finds the sized twin's real length; `target` and
        `size` are both replaced with it (reverify_corpus.rom_bytes re-reads the ROM
        at the corrected length) before extraction or comparison.
      * nested entry point: `name` has no symbol of its own in the compiled object at
        all -- a hand-asm block packs several ROM functions into ONE compiled symbol
        (see func_01ff97d8.c: 0xb6c bytes, five more ROM addresses inside). When the
        exact-name lookup and the sole-symbol fallback both miss, every function the
        object DOES define is checked for one that CONTAINS name's ROM range,
        resolved by address (reloc_audit.resolve_nested_slice) -- never by scanning
        source text for a label."""
    import contextlib
    import io
    import match as M
    import swarm as S
    unscorable = {"divergences": None, "ok": False, "cand_size": None}
    chat = io.StringIO()
    resolved = None
    if size == 0 and module is not None and addr is not None:
        import bytegate as BG
        alt = BG.alias_target_size(module, addr)
        if alt:
            import reverify_corpus as RV
            rebased = RV.rom_bytes(module, addr, alt)
            if rebased is not None:
                target, size = rebased, alt
    try:
        with contextlib.redirect_stdout(chat):    # compile_c prints; capture the detail
            _, obj = S.oracle_check(src, name, target)
        if obj is None:
            return dict(unscorable, status="noncompile", error=_trim_error(chat.getvalue()))
        cand, crel = M.extract_func(obj, name)
        if cand is None:
            # oracle_check looked the same name up and found nothing either, so the
            # only case worth resolving is the object defining exactly one function.
            resolved = M.sole_func_symbol(obj)
            if resolved is not None:
                cand, crel = M.extract_func(obj, resolved)
                if size and cand is not None and len(cand) != size:
                    # The object's one function is a CONTAINER (a nested entry
                    # point -- func_01ff97d8.c's whole 0xb6c-byte body is exactly
                    # this shape), not a renamed/mangled copy of `name` itself: a
                    # genuine rename compiles to `name`'s own length by
                    # construction, since it IS that function under another
                    # spelling. Undo the guess and let the nested-container search
                    # below resolve it by address instead of reporting a false
                    # size mismatch under the wrong resolved name.
                    cand, crel, resolved = None, None, None
        if cand is None and addr is not None and size:
            # Neither the exact name nor the sole-symbol fallback found it: try every
            # function the object DOES define as a possible CONTAINER (see
            # reloc_audit.resolve_nested_slice).
            import probe_versions as PV
            import reloc_audit as RA
            idx = name_index if name_index is not None else _name_index()
            for sym in PV.funcs_in(obj):
                full_code, full_relocs = M.extract_func(obj, sym)
                if full_code is None:
                    continue
                sliced = RA.resolve_nested_slice(sym, full_code, full_relocs, addr, size, idx)
                if sliced is not None:
                    cand, crel, _off = sliced
                    resolved = sym
                    break
    except Exception as e:                        # malformed object, ELF parse crash, ...
        return dict(unscorable, status="noncompile",
                    error=_trim_error(f"{type(e).__name__}: {e}"))
    if cand is None:
        return dict(unscorable, status="func-absent",
                    error=f"function {name!r} absent from the compiled object")
    ok, div = M.compare(target, cand, crel or set(), verbose=False)
    return {"divergences": 0 if ok else div, "ok": ok, "cand_size": len(cand),
            "status": None, "error": None, "resolved": resolved}


def evaluate(src, name, target):
    """Back-compat shape: (divergences, ok). (None, False) if the source does not
    compile or the function is absent. Callers that also want the candidate size or
    the failure detail use evaluate_full."""
    r = evaluate_full(src, name, target)
    return r["divergences"], r["ok"]


def _is_manual(r):
    """True for a row whose divergences were last set by `set-divergence`, not by the
    live evaluator. reeval/bank-matches must hold these rather than silently overwrite
    them (before METRIC_REV 2, evaluate_full's SequenceMatcher-based diff could
    UNDERCOUNT a pure instruction reorder relative to tools/match.py's positional
    MISMATCH count -- measured 2026-09-09 on _ZN3MrI13InitResourcesEv: a 3-instruction
    ROM rotation scored 2 under the old evaluate_full but 3 under match.py, the number
    the committed src/ header, floors.jsonl and worklist.py all report -- so
    re-running the auto-scorer over a hand-corrected row could quietly ratchet the
    correction back to the wrong number). reeval still checks a manual row's live
    score every pass; see _manual_hold_releasable for when the hold comes back off."""
    return bool(r.get("manual_divergences"))


def _manual_hold_releasable(r, full):
    """True once a manual_divergences row's OWN stored c_source scores the SAME
    divergence count under the live evaluator that `set-divergence` recorded by hand.

    The hold exists only to stop the live evaluator from silently overwriting a
    correction it disagrees with (see _is_manual). Once the live evaluator -- now
    tools/match.py's own compare(), METRIC_REV 2 -- reproduces the hand-set number on
    its own, the correction is no longer doing anything the evaluator can't do for
    itself, so `reeval` retires it instead of leaving the row stuck manual forever. A
    row that is unscorable (status set) or whose live score still disagrees is left
    held, untouched, exactly as before."""
    return full["status"] is None and full["divergences"] == r.get("divergences")


def apply_eval(r, full):
    """Fold an evaluate_full result into a row, in place: re-score + stamp, or mark
    it unscorable (status/error, divergences=None) keeping the last good score as
    stale_divergences. Never deletes the row -- a broken source is still the record
    of an attempt, it just must not sit in a closest-first worklist as bait.

    Deliberately NOT strictly-improving, unlike merge_batch: this is re-measuring the
    SAME row's own c_source under today's live scorer, not choosing between two
    competing drafts, so divergences moves whichever way the scorer says -- up when a
    compiler/metric bump makes yesterday's count wrong, not just down. See
    merge_batch's docstring for the boundary between the two."""
    if full["status"]:
        if r.get("divergences") is not None:
            r["stale_divergences"] = r["divergences"]
        r["divergences"] = None
        r.pop("cand_size", None)
        r["status"] = full["status"]
        r["error"] = full["error"]
    else:
        r["divergences"] = full["divergences"]
        r["cand_size"] = full["cand_size"]
        for k in ("status", "error", "stale_divergences"):
            r.pop(k, None)
    fp = current_fingerprint()
    if fp:
        r["evaluator"] = fp


def _size_gap(r):
    """|cand_size - size| in bytes; huge when either side is unknown so unmeasured
    rows never outrank measured ones on the size axis."""
    cs, ts = r.get("cand_size"), r.get("size")
    if isinstance(ts, str):
        try:
            ts = int(ts, 0)
        except ValueError:
            ts = None
    if not isinstance(cs, int) or not isinstance(ts, int):
        return 1 << 30
    return abs(cs - ts)


def closeness(r):
    """The strictly-improving upsert key: (divergences, size gap). Edit distance still
    leads -- an upsert never regresses divergences -- but of two equally-close drafts
    the one assembling nearer the target size wins. Edit distance alone is
    NON-MONOTONIC in candidate size (measured 2026-08-30 on one ov065 family: a
    ten-instruction-SHORT draft scored 225 while the one-instruction-off draft scored
    267 and four-over drafts scored 241), and the permuter cannot add or remove
    instructions, so a size-blind keep-best can hold materially worse permuter fuel."""
    div = r.get("divergences")
    return (div if isinstance(div, int) else 1 << 30, _size_gap(r))


def seed_rank(r):
    """Total retrieval order for worklists and exports: closeness, then (module, addr)
    so equal-closeness rows land in a deterministic, diff-stable order."""
    return closeness(r) + (str(r.get("module")), str(r.get("addr")))


def _rank(r):
    """Collapse order for duplicate rows of one (module, addr). A row stamped by the
    CURRENT eval pin outranks any other row FIRST: a LOCAL union merge (db.jsonl is
    merge=union; GitHub does not honour the driver, so on the server this shows as a
    real conflict instead) resurrects copies scored under an older evaluator, or never
    re-scored at all, and a stale score that happens to be LOWER would otherwise win
    the collapse and silently undo a reeval correction.

    Stale is not a synonym for lower -- drift goes both ways, measured 25 up / 22 down
    across the 2026-08-30 full pass (230->354 and 205->217 up; ov015 78->18 down). The
    stamp is what makes the collapse safe, not an assumed direction: it prefers the
    row scored by the evaluator we can still reproduce, whichever number is smaller.

    Then lower divergences wins; a floor mark breaks a divergence tie (the more-informed
    verdict); a smaller size gap breaks what remains. Unscored rows rank last within
    their stamp class. With no pin recorded this reduces to the old order."""
    pin = pin_fingerprint()
    stamped = 0 if (pin is not None and r.get("evaluator") == pin) else 1
    div = r.get("divergences")
    return (stamped, div if isinstance(div, int) else 1 << 30,
            0 if r.get("floor") else 1, _size_gap(r))


def load_db():
    db, dups = {}, 0
    for r in L.read_records(DB):        # corrupt lines are reported, not swallowed
        # A row that parses as JSON but lacks addr/module is unkeyable. That used to raise
        # out of key_of and take down every caller -- one bad ingest silently disabled the
        # whole permuter crunch pipeline. Skip it loudly instead; the row keeps its place
        # in the file so it can be repaired rather than lost.
        if r.get("addr") is None or r.get("module") is None:
            print(f"nearmiss_db: skipping unkeyable row (no addr/module): "
                  f"{r.get('name') or '<unnamed>'}", file=sys.stderr)
            continue
        key = L.key_of(r)               # normalized: addr is stored as both hex str and int
        cur = db.get(key)
        # merge=union (and any pre-#1676 raw-key ingest still running in a stale lane
        # checkout -- ov004 0x020ae858 got duplicated by one on 2026-08-25) can leave two
        # rows for one key. Keep the BEST, never the last read: last-wins meant the next
        # save_db() silently discarded whichever row happened to sort earlier in the file
        # -- ov006 0x020d7c4c lost its lower-divergence attempt exactly that way. Collapse
        # loudly: a duplicate on disk survives until someone runs dedupe, and a silent
        # collapse reads as a healthy DB while the file carries a dead row (219 lines
        # reporting as 218 entries in the 2026-08-25 incident).
        if cur is not None:
            dups += 1
        if cur is None or _rank(r) < _rank(cur):
            db[key] = r
    if dups:
        print(f"nearmiss_db: {dups} duplicate (module, addr) row(s) in {DB.name}; "
              f"kept the closest per key (run `python tools/nearmiss_db.py dedupe`)",
              file=sys.stderr)
    return db


def save_db(db):
    """Atomic rewrite: db.jsonl is a committed file; a crash mid-write must not
    truncate it. Write a sibling temp then os.replace."""
    import os
    DB.parent.mkdir(parents=True, exist_ok=True)
    items = sorted(db.values(), key=seed_rank)   # deterministic: closeness, then key
    tmp = DB.with_name(DB.name + ".tmp")
    tmp.write_text("".join(json.dumps(r) + "\n" for r in items), encoding="utf-8")
    os.replace(tmp, DB)


def load_meta(worklist):
    meta = {}
    if worklist and pathlib.Path(worklist).exists():
        for l in pathlib.Path(worklist).read_text().splitlines():
            if l.strip():
                r = json.loads(l)
                meta[r["name"]] = (r["addr"], int(r["size"], 16), r["module"], r["target_hex"])
    return meta


_NAME_IDX = None


def resolve_name(name):
    """Find a function's (addr_hex, size, module, target_hex) by name across all modules.
    Lets us backfill near-misses whose original worklist is gone."""
    global _NAME_IDX
    if _NAME_IDX is None:
        import modules as MOD
        import sweep
        _NAME_IDX = {}
        for mod in MOD.modules():
            label = "arm9" if mod["name"] == "main" else mod["name"]
            data = mod["bin"].read_bytes()
            for fn, addr, size in sweep.funcs(mod):
                _NAME_IDX[fn] = (f"0x{addr:08x}", size, label,
                                 data[addr - mod["base"]:addr - mod["base"] + size].hex())
    return _NAME_IDX.get(name)


def merge_batch(db, drops, updates):
    """Apply one ingest batch to a loaded db dict, in place. Returns (added, improved).

    Strictly-improving in the closeness() key: an upsert never regresses divergences,
    and on a divergence tie a candidate replaces the incumbent only when its assembled
    size is strictly closer to the target (see closeness for why size-blind keep-best
    holds worse permuter fuel). A row marked unscorable (status set, divergences null)
    ranks worst, so any compiling candidate replaces it.

    SCOPE, decided 2026-09-09 alongside the match.py-parity scorer fix (METRIC_REV 2):
    this rule picks the better of two DIFFERENT candidate drafts competing for the
    same (module, addr) -- an ingested draft must never evict a strictly closer one
    already banked. It is not, and must never become, a rule about whether a row's
    stored number is allowed to change when nothing about the draft did. That is
    re-measurement, not competition, and it belongs entirely to `reeval` /
    `bank-matches` (apply_eval), which overwrite a row's divergences with whatever the
    live pinned scorer says today, UP OR DOWN, unconditionally -- "the stored number is
    what the pinned scorer says today", not "never up". Conflating the two is exactly
    how four rows went stale for weeks before `set-divergence` existed: nothing else in
    this file could ever raise a stored divergence, ingest because it is deliberately
    strictly-improving (correctly) and reeval because its own metric was wrong (the
    real bug, now fixed). See apply_eval and its test coverage for the "not never up"
    half of that split.

    Drops win over updates. A dropped key means the function is matched (the ledger or
    committed src/ says so), and one seeds file can carry two names for one (module,
    addr) -- a stale func_ADDR placeholder next to the real symbol -- where the
    src-file check catches one name but not the other. Popping the key and then
    letting the update land re-created the ghost row the drop existed to remove, and
    counted the survivor as "+1 new" because the pop had emptied the slot the update
    compared against."""
    dropped = set(drops)
    for key in dropped:
        db.pop(key, None)
    added = improved = 0
    for key, rec in updates.items():
        if key in dropped:
            continue
        cur = db.get(key)
        if cur is None or closeness(rec) < closeness(cur):
            db[key] = rec
            added += cur is None
            improved += cur is not None
    return added, improved


def ingest(args):
    import worklist as WL
    import names as NM
    warn_stale_pin()
    db = load_db()
    meta = load_meta(args.worklist)
    # MATCHED only, not load_done(): parked functions keep their pending drafts
    # in this DB (the permuter/hand-fix backlog would be wiped otherwise).
    done = L.matched_set()
    items = []
    if args.result:
        o = json.loads(pathlib.Path(args.result).read_text())
        o = o.get("result", o)
        for nm in (o.get("nearMisses") or o.get("near_misses") or []):
            items.append((nm.get("name"), nm.get("c_source")))
    if args.seeds:
        for l in pathlib.Path(args.seeds).read_text().splitlines():
            if l.strip():
                d = json.loads(l)
                items.append((d.get("name"), d.get("c_source")))
    updates, drops = {}, []
    for name, src in items:
        if not name or not src:
            continue
        m = meta.get(name) or resolve_name(name)
        if m:
            addr, size, mod, thex = m
        else:
            ex = next((r for r in db.values() if r["name"] == name), None)
            if not ex:
                continue
            addr, size, mod, thex = ex["addr"], ex["size"], ex["module"], ex["target_hex"]
        # Key with the SAME normalization load_db uses (ledger.make_key): addr arrives
        # here as a hex string but existing rows may store it as an int, and a raw
        # (mod, addr) tuple misses the loaded row, so the merge below APPENDED a
        # duplicate instead of replacing (two rows each for ov006 0x02102fe8 /
        # 0x0210076c on 2026-08-22). Same for drops: an unnormalized key never popped.
        key = L.make_key(mod, addr)
        if key in done:                     # already matched -- not a pending near-miss
            drops.append(key)
            continue
        # Resolve the CURRENT symbol name at (mod, addr): the seed's `name` may be a
        # stale func_ADDR placeholder while the committed src/ file is under the real
        # symbol (see tools/names.py). Keying the src-file check by the stored name
        # missed renamed matches, resurrecting ghosts prune-matched had dropped.
        stext = WL.read_src_text(NM.name_at(mod, addr) or name)
        if stext is not None and not asm_policy.has_draft_banner(stext):
            # matched in committed src/ -- the local matched ledger is stale on
            # multi-contributor checkouts, so without this check a seeds file
            # generated before someone else's match RESURRECTS a ghost entry
            # that prune-matched already dropped (67 came back that way on
            # 2026-07-12). Skip before the expensive evaluate().
            drops.append(key)
            continue
        full = evaluate_full(src, name, bytes.fromhex(thex),
                             module=mod, addr=L.norm_addr(addr), size=size)
        if full["divergences"] is None or full["ok"]:   # broken seed, or already a match; skip
            continue
        rec = {"module": mod, "addr": addr, "name": name, "size": size,
               "target_hex": thex, "lang": "cpp" if src.startswith("//cpp") else "c",
               "divergences": full["divergences"], "cand_size": full["cand_size"],
               "c_source": src, "source": args.label or "fanout"}
        fp = current_fingerprint()
        if fp:
            rec["evaluator"] = fp
        best = updates.get(key)
        if best is None or closeness(rec) < closeness(best):
            updates[key] = rec
    # merge under the lock: evaluate() above is slow, so the read-modify-write
    # happens against a FRESH db snapshot to not clobber concurrent crunchers
    with locked():
        db = load_db()
        added, improved = merge_batch(db, drops, updates)
        save_db(db)
        total = len(db)
    print(f"ingested: +{added} new, {improved} improved. DB now {total} entries.")


def stats(args):
    warn_stale_pin()
    db = load_db()
    ds = [r["divergences"] for r in db.values() if r.get("divergences") is not None]
    ds.sort()
    if not ds:
        print(f"DB: {len(db)} entries (none scored)")
        return
    b = {"0 (bankable!)": sum(d == 0 for d in ds), "1-4": sum(1 <= d <= 4 for d in ds),
         "5-12": sum(5 <= d <= 12 for d in ds), "13-30": sum(13 <= d <= 30 for d in ds),
         ">30": sum(d > 30 for d in ds)}
    print(f"DB: {len(db)} entries. median divergences={ds[len(ds)//2]}, min={ds[0]}")
    for k, v in b.items():
        print(f"  {k:14} {v}")
    floored = [r for r in db.values() if r.get("floor")]
    if floored:
        print(f"  floored        {len(floored)} (verified compiler-internal residual; hand-fix backlog)")
    bad = [r for r in db.values() if r.get("status")]
    if bad:
        print(f"  unscorable     {len(bad)} (status set: stored source no longer evaluates; "
              f"excluded from every worklist until a better ingest replaces it)")


def _list(args):
    warn_stale_pin()
    db = load_db()
    rows = sorted(db.values(), key=seed_rank)
    for r in rows:
        div = r.get("divergences")
        # isinstance, not truthiness: `or 1e9` treated a bankable div=0 row as
        # unscored and hid it from every --max-div listing.
        if args.max_div is not None and (div if isinstance(div, int) else 1 << 30) > args.max_div:
            continue
        if getattr(args, "floor_only", False) and not r.get("floor"):
            continue
        tag = f"  FLOOR({r['floor'].get('class', '?')})" if r.get("floor") else ""
        if r.get("status"):
            tag += f"  {r['status'].upper()}({r.get('error', '')[:60]})"
        print(f"  div={str(div):<4} {r['module']:6} {r['name'][:46]:46} {r['lang']}{tag}")


def mark_floor(args):
    """Persistently mark entries whose residual is verified compiler-internal (e.g. a
    pure instruction-ordering swap the scheduler owns). Marked entries drop out of
    export-close and refine_wl.py; they remain in the DB as the hand-fix backlog."""
    import datetime
    names = [n.strip() for n in args.name.split(",") if n.strip()]
    with locked():
        db = load_db()
        hit = 0
        for r in db.values():
            if r["name"] in names:
                r["floor"] = {"class": args.floor_class, "evidence": args.evidence,
                              "date": str(datetime.date.today())}
                hit += 1
        save_db(db)
    missing = set(names) - {r["name"] for r in db.values() if r.get("floor")}
    print(f"marked {hit}/{len(names)} as floor({args.floor_class})"
          + (f"; NOT FOUND: {', '.join(sorted(missing))}" if hit < len(names) else ""))


def unmark_floor(args):
    names = [n.strip() for n in args.name.split(",") if n.strip()]
    with locked():
        db = load_db()
        hit = 0
        for r in db.values():
            if r["name"] in names and r.pop("floor", None) is not None:
                hit += 1
        save_db(db)
    print(f"unmarked {hit}/{len(names)}")


def set_divergence(args):
    """Hand-correct ONE row's stored divergence count, in either direction, when a
    measurement against the canonical proof tool (tools/match.py, the pinned compiler)
    disagrees with what the live evaluator's SequenceMatcher-based diff reports for the
    row's OWN stored c_source.

    This exists because `ingest`/merge_batch is deliberately strictly-improving (an
    upsert never regresses divergences -- see closeness()) and reeval's own evaluator
    can UNDERCOUNT a pure instruction reorder relative to match.py's positional count
    (see _is_manual), so neither existing path can lower confidence in a stored row: a
    DB that only ever ratchets divergences down, even when a fresh authoritative
    measurement says the true residue is worse, is lying about the floor. This is the
    escape hatch, and it is intentionally narrow: one named row, evidence required,
    never touches c_source.

    The corrected row is stamped manual_divergences so reeval and bank-matches HOLD it
    (see _is_manual) instead of silently re-scoring it back with the disagreeing
    metric; evaluator is still stamped to the live pin because the correction WAS
    measured under the live pinned compiler, just via match.py's diff, not
    evaluate_full's."""
    import datetime
    name = args.name
    with locked():
        db = load_db()
        r = next((r for r in db.values() if r["name"] == name), None)
        if r is None:
            sys.exit(f"set-divergence: no row named {name!r} in {DB.name}")
        old = r.get("divergences")
        r["divergences"] = args.divergences
        r["manual_divergences"] = True
        r["manual_evidence"] = args.evidence
        r["manual_date"] = str(datetime.date.today())
        r.pop("cand_size", None)     # the manual count is not backed by a fresh compile
        fp = current_fingerprint()
        if fp:
            r["evaluator"] = fp
        save_db(db)
    print(f"{name}: divergences {old} -> {args.divergences} (manual, held from reeval/bank-matches)")


def export_close(args):
    warn_stale_pin()
    db = load_db()
    out = [r for r in db.values()
           if r.get("divergences") is not None and 0 < r["divergences"] <= args.max_div]
    if not getattr(args, "include_floor", False):
        floored = [r for r in out if r.get("floor")]
        if floored:
            print(f"skipping {len(floored)} floored entries (--include-floor to keep)")
        out = [r for r in out if not r.get("floor")]
    if args.category:
        # category-routed export (permuter wants "register allocation" / "instruction
        # reorder"); uses the classification cache refine_wl.py maintains, classifying
        # on demand for uncached entries.
        import categorize_misses as CAT
        cachep = REPO / "progress" / "nm_categories.json"
        cache = json.loads(cachep.read_text()) if cachep.exists() else {}
        want = {c.strip().lower() for c in args.category.split(",")}
        kept = []
        for r in out:
            a = r["addr"]
            key = f"{r['module']}:{int(a, 0) if isinstance(a, str) else a}:{r['divergences']}"
            cat = cache.get(key)
            if cat is None:
                try:
                    cat = CAT.classify_entry(r["c_source"], r["name"],
                                             bytes.fromhex(r["target_hex"]))
                except Exception:
                    cat = "error"
                cache[key] = cat
            if any(w in cat.lower() for w in want):
                kept.append(r)
        cachep.write_text(json.dumps(cache))
        out = kept
    out.sort(key=seed_rank)                       # closest first, size gap breaks ties
    pathlib.Path(args.out).write_text(
        "".join(json.dumps({"module": r["module"], "addr": r["addr"], "name": r["name"],
                            "divergences": r["divergences"], "size": r.get("size"),
                            "cand_size": r.get("cand_size"),
                            "c_source": r["c_source"]}) + "\n" for r in out))
    print(f"exported {len(out)} close seeds (div<={args.max_div}) -> {args.out}")


def _current_source(module, addr, fallback_name):
    """(path, text) for whatever src/ file currently answers to (module, addr), or
    (None, None) if nothing does.

    Tries the CURRENT symbol name first -- tools/names.py resolves a rename,
    tools/srcpath.py's enrolment table then resolves an ordinary match OR a
    symbol promoted whole into a consolidated TU -- and falls back to raw
    delinks-range CONTAINMENT by address (srcpath.owner_at) for the one shape
    name resolution cannot see: a consolidated TU that folded this address into
    a NEIGHBOUR's compiled body with no symbols.txt row of its own left there
    (the nested-entry-point shape reloc_audit.resolve_nested_slice resolves at
    the compiled-object level for linkcheck.py/pr_linkcheck.py -- func_01ff97d8.c
    is the canonical example). owner_at shares srcpath's existing delinks.txt
    parse; this is not a second resolver, just the by-address query on it."""
    import names as NM
    import srcpath as SP
    name = NM.name_at(module, addr) or fallback_name
    path = SP.path_for(name) or SP.owner_at(module, addr)
    if path is None:
        return None, None
    return path, path.read_text(encoding="utf-8", errors="ignore")


def prune_matched(args):
    """Drop entries whose function already has a committed, CI-validated match in
    src/. The local matched ledger is often stale on multi-contributor checkouts,
    so ingest's matched_set() drop misses these; they linger as ghosts and pollute
    stats and export-close.

    The ghost test is asm_policy.counts_as_matched, the one function every counting
    tool publishes from, not a second spelling of it. It used to read "no
    NONMATCHING banner", which was the same answer until the hand-asm ruling of
    2026-09-09 gave a HAND-ASM PRIMITIVE file that also says NONMATCHING back its
    place in the count. Eight rows then sat in this DB describing functions the
    progress bar already counted -- __rethrow, func_02052ec8, func_02057014,
    func_02057078, func_02058568, func_02059468, func_02059824, func_0205a588 --
    and every close-first worklist put them at the top of the queue, because a
    divergence of 1 against an assembly primitive with no C to recover is the
    lowest number in the file.

    Resolution is _current_source's: the CURRENT symbol name at (module, addr)
    via tools/names.py, then tools/srcpath.py -- by name for an ordinary match, a
    rename, or a symbol promoted whole into a consolidated TU, and by raw
    delinks-range address containment (srcpath.owner_at) for a TU that absorbed
    this address into a neighbour's body with no symbols.txt row of its own left.
    Surviving entries also have their display name resynced so the label never
    drifts from the key. Every drop is printed with the file that matched it --
    never a silent count alone -- both in --dry-run and on the real run."""
    import names as NM
    db = load_db()
    ghosts = []
    for key, r in db.items():
        path, text = _current_source(r["module"], L.norm_addr(r["addr"]), r["name"])
        if text is not None and asm_policy.counts_as_matched(text):
            ghosts.append((key, path))
    if args.dry_run:
        for key, path in ghosts:
            r = db[key]
            print(f"  would drop div={r.get('divergences'):<4} {r['module']:6} {r['name']:<40} "
                  f"-> {path.relative_to(REPO).as_posix()}")
        print(f"{len(ghosts)} ghost entries (matched in committed src/)")
        return
    with locked():
        db = load_db()
        dropped = renamed = 0
        for key, path in ghosts:
            r = db.pop(key, None)
            if r is not None:
                dropped += 1
                print(f"  dropped {r['module']:6} {r['name']:<40} "
                      f"-> {path.relative_to(REPO).as_posix()}")
        for r in db.values():                       # resync survivors' labels to the key
            cur = NM.name_at(r["module"], r["addr"])
            if cur and cur != r["name"]:
                r["name"] = cur
                renamed += 1
        save_db(db)
        remaining = len(db)
    print(f"dropped {dropped} ghost entries (matched in committed src/); "
          f"resynced {renamed} stale names; DB now {remaining}.")


def dedupe(args):
    """Collapse duplicate rows for the same (module, addr), keeping the BEST tip.

    nearmiss/db.jsonl is `merge=union` in .gitattributes: two sides that both bank a tip for
    the same function land both rows instead of a merge conflict. That is the point - a LOCAL
    merge of concurrent match work resolves itself - but it leaves duplicate rows for whatever
    functions both sides touched. (Only local: GitHub's own merge does not honour the union
    driver and shows a real conflict, see .gitattributes.) This is the automatic cleanup, and
    the update-chaos-data workflow's rewrite of this file on main goes through it: for each
    (module, addr) keep
    the row with the LOWEST divergence (a floored entry outranks a same-div non-floored one,
    since the floor mark is the more-informed verdict), drop the rest. A merge never regresses
    a tip anyone improved. Idempotent; run in CI after every push to main.

    --check is the regression-gate form: writes nothing, lists each duplicated key,
    and exits 1 if any normalized (module, addr) holds more than one row."""
    seen, best, order, unkeyable, dup_keys = {}, {}, [], [], {}
    for r in L.read_records(DB):                    # RAW rows, incl. union duplicates
        try:
            key = L.key_of(r)                       # int/hex-str addr forms must collide
        except (KeyError, TypeError, ValueError):
            # A row with no resolvable (module, addr) cannot be deduped or matched. Skip
            # it with a warning rather than aborting: one malformed salvage row must never
            # crash the whole progress refresh (it did -- a row saved with name+label but
            # no addr froze the public percent until repaired).
            sys.stderr.write(f"nearmiss dedupe: skipping unkeyable row {r.get('name') or r.get('label') or '?'}\n")
            unkeyable.append(r)
            continue
        if key not in seen:
            seen[key] = _rank(r)                    # lower div wins; floor breaks a div tie
            best[key] = r
            order.append(key)
        else:
            dup_keys[key] = dup_keys.get(key, 0) + 1
            if _rank(r) < seen[key]:
                seen[key] = _rank(r)
                best[key] = r
    dups = sum(dup_keys.values())
    unique = len(order) + len(unkeyable)
    if getattr(args, "check", False):
        for key, extra in sorted(dup_keys.items()):
            print(f"  DUP {key[0]} 0x{key[1]:08x}: {extra + 1} rows ({best[key].get('name')})")
        if dups:
            print(f"CHECK FAILED: {dups} extra row(s) across {len(dup_keys)} duplicated "
                  f"(module, addr) key(s) in {DB}")
            sys.exit(1)
        print(f"check ok: no duplicate (module, addr) rows; DB has {unique} entries")
        return
    if dups == 0:
        print(f"no duplicate (module, addr) rows; DB has {unique} entries")
        return
    if args.dry_run:
        print(f"would collapse {dups} duplicate row(s) -> {unique} unique entries")
        return
    out = {k: best[k] for k in order}
    # unkeyable rows survive the rewrite under synthetic keys save_db never serializes
    out.update({("__unkeyable__", i): r for i, r in enumerate(unkeyable)})
    with locked():
        save_db(out)
    print(f"collapsed {dups} duplicate row(s); DB now {unique} unique entries")


def resync_names(args):
    """Rewrite each entry's display name to the current symbol at its (module, addr).
    The DB is keyed by (module, addr); `name` is only a label and goes stale when a
    symbol import renames a func_ADDR placeholder. Idempotent; run after symbol imports
    so stats and any name-keyed cross-reference stop lying (tools/names.py)."""
    import names as NM
    db = load_db()
    changes = [(r["module"], r["addr"], r["name"], cur)
               for r in db.values()
               for cur in [NM.name_at(r["module"], r["addr"])]
               if cur and cur != r["name"]]
    if args.dry_run:
        for mod, addr, old, cur in changes[:40]:
            print(f"  {mod:7} {addr}  {old[:34]:34} -> {cur}")
        print(f"{len(changes)} stale names (dry run)")
        return
    with locked():
        db = load_db()
        for r in db.values():
            cur = NM.name_at(r["module"], r["addr"])
            if cur and cur != r["name"]:
                r["name"] = cur
        save_db(db)
    print(f"resynced {len(changes)} stale names.")


def bank_matches(args):
    """Re-evaluate every entry; bank any that now byte-match (score 0). Everything
    else is re-scored and stamped in passing (same shape as reeval, minus the pin).
    Rows marked manual_divergences (see set_divergence) are held, not re-scored: their
    own stored c_source is known-non-matching either way, so there is nothing to bank,
    and re-evaluating them would silently overwrite a hand correction (see _is_manual)."""
    import swarm as S
    warn_stale_pin()
    db = load_db()
    held = sum(1 for r in db.values() if _is_manual(r))
    banked, banked_keys, rescored = 0, [], {}
    for key, r in list(db.items()):
        if _is_manual(r):
            continue
        full = evaluate_full(r["c_source"], r["name"], bytes.fromhex(r["target_hex"]),
                             module=r["module"], addr=L.norm_addr(r["addr"]), size=r["size"])
        div, ok = full["divergences"], full["ok"]
        if ok and not getattr(args, "no_strict", False):
            # the byte oracle wildcards reloc slots; refuse a draft whose relocations
            # point somewhere other than the config/**/relocs.txt records
            import reloc_audit as RA
            _, obj = S.oracle_check(r["c_source"], r["name"], bytes.fromhex(r["target_hex"]))
            # No object, or gate_wrong_dests returning None (unknown module ID / symbol
            # absent), both mean the relocation check did not run. None is falsy, so
            # this used to fall through `if bad:` and bank the draft as strict-verified
            # having verified nothing. Refuse instead.
            bad = RA.gate_wrong_dests(obj, r["name"], L.norm_addr(r["addr"]),
                                      r["size"], r["module"]) if obj else None
            if bad is None:
                print(f"  SKIP {r['name']}: bytes match but the reloc-destination check "
                      f"could not run (no object, module {r['module']!r} unknown, "
                      f"or symbol absent)")
                continue
            if bad:
                print(f"  SKIP {r['name']}: bytes match but {len(bad)} reloc "
                      f"destination(s) WRONG (e.g. {bad[0]['cand']} != {bad[0]['cfg']})")
                continue
        if ok:
            st = L.bank({"addr": r["addr"], "name": r["name"], "size": r["size"],
                         "module": r["module"], "versions": ["nearmiss-db"]},
                        r["c_source"])
            if st != "refused":         # dup = matched meanwhile; drop either way
                banked_keys.append(key)
                banked += st == "banked"
        else:
            rescored[key] = (r["c_source"], full)
    # merge under the lock so a concurrent cruncher's improvements survive; a rescore
    # only lands if the row still holds the SOURCE it was computed from (a cruncher
    # may have replaced c_source mid-pass, and this score is not that source's score)
    with locked():
        cur = load_db()
        for key in banked_keys:
            cur.pop(key, None)
        for key, (src, full) in rescored.items():
            tgt = cur.get(key)
            if tgt is not None and tgt.get("c_source") == src:
                apply_eval(tgt, full)
        save_db(cur)
        remaining = len(cur)
    print(f"banked {banked} now-matching entries; DB now {remaining}."
          + (f" {held} manual row(s) held, not re-scored." if held else ""))


def reeval(args):
    """Re-score EVERY row with the live evaluator, stamp the results, and record the
    evaluator in nearmiss/eval_pin.json.

    This is the corrective pass for score drift: stored divergences go stale when
    include/ churns under the stored sources, when the canonical compiler moves, or
    when the metric changes. Rows whose stored source no longer scores are KEPT but
    marked (status/error, divergences=None, prior score in stale_divergences) so they
    rank last everywhere instead of topping a closest-first worklist; a later
    strictly-improving ingest replaces them and clears the mark.

    Only a MAIN-TIP pass is authoritative (the stale-lane rule): run on a checkout of
    current origin/main and commit db.jsonl together with eval_pin.json.

    Rows marked manual_divergences (see set_divergence) are never silently overwritten
    -- reeval must not undo a hand correction the live evaluator still disagrees with
    (see _is_manual). But their own stored c_source IS compiled and scored every pass
    (see _manual_hold_releasable): once the live evaluator reproduces the hand-set
    number on its own, the hold is retired (manual_divergences/manual_evidence/
    manual_date cleared, the row goes back to ordinary auto-scored status) instead of
    sitting manual forever. A manual row whose live score still disagrees is left
    completely untouched, exactly as the old HELD-entirely behavior did."""
    import datetime
    import match as M
    exe = M.MW / M.CANONICAL / "mwccarm.exe"
    if not exe.is_file():
        sys.exit(f"reeval: canonical compiler {M.CANONICAL} is not installed at {exe}; "
                 f"a pass without it would mark every row noncompiling")
    db = load_db()
    name_idx = _name_index()   # built once for the whole pass, not once per row
    manual_items = [(k, r) for k, r in db.items() if _is_manual(r)]
    held = len(manual_items)
    order = sorted((kv for kv in db.items() if not _is_manual(kv[1])),
                   key=lambda kv: seed_rank(kv[1]))
    results, n = {}, len(order)
    same = drifted = broke = recovered = bankable = 0
    for i, (key, r) in enumerate(order, 1):
        full = evaluate_full(r["c_source"], r["name"], bytes.fromhex(r["target_hex"]),
                             module=r["module"], addr=L.norm_addr(r["addr"]), size=r["size"],
                             name_index=name_idx)
        results[key] = (r["c_source"], full)
        old, new = r.get("divergences"), full["divergences"]
        label = f"[{i}/{n}] {r['module']:6} {r['name'][:46]:46}"
        if full.get("resolved"):
            # the stored name did not resolve but the object holds exactly one function;
            # say so, because the row is now scored against a symbol it does not spell
            print(f"{label} NAME: scored against {full['resolved']!r} "
                  f"(stored name absent from the object)", flush=True)
        if full["status"]:
            broke += 1
            print(f"{label} div {old} -> UNSCORABLE {full['status']}: {full['error']}",
                  flush=True)
        elif old is None:
            recovered += 1
            print(f"{label} recovered: div None -> {new}", flush=True)
        elif new != old:
            drifted += 1
            note = "  (floor claim may be stale)" if r.get("floor") else ""
            bank = "  BANKABLE -- run bank-matches" if full["ok"] else ""
            print(f"{label} div {old} -> {new} DRIFT{note}{bank}", flush=True)
        else:
            same += 1
        bankable += full["ok"]
    # Manual rows: verify against the live evaluator (so a corrected pin can retire a
    # hold on its own) without ever silently overwriting one that still disagrees.
    manual_results, released = {}, 0
    for i, (key, r) in enumerate(manual_items, 1):
        full = evaluate_full(r["c_source"], r["name"], bytes.fromhex(r["target_hex"]),
                             module=r["module"], addr=L.norm_addr(r["addr"]), size=r["size"],
                             name_index=name_idx)
        manual_results[key] = (r["c_source"], full)
        label = f"[manual {i}/{held}] {r['module']:6} {r['name'][:46]:46}"
        if _manual_hold_releasable(r, full):
            released += 1
            print(f"{label} div {r.get('divergences')} agrees naturally now -- "
                  f"RELEASING the manual hold", flush=True)
        else:
            live = full["divergences"] if full["status"] is None else f"UNSCORABLE {full['status']}"
            print(f"{label} manual={r.get('divergences')} live={live} -- hold kept",
                  flush=True)
    still_held = held - released
    print(f"\nreeval: {n} rows -- {same} unchanged, {drifted} drifted, {broke} unscorable, "
          f"{recovered} recovered, {bankable} bankable (run bank-matches)"
          + (f", {still_held} held (manual correction)" if still_held else "")
          + (f", {released} manual hold(s) released (now agrees naturally)" if released else ""))
    if args.dry_run:
        print("dry run: nothing written")
        return
    # merge under the lock against a FRESH snapshot; a result only lands on a row that
    # still holds the source it was computed from (concurrent crunchers improve rows)
    with locked():
        cur = load_db()
        applied = skipped = 0
        for key, (src, full) in results.items():
            tgt = cur.get(key)
            if tgt is None or tgt.get("c_source") != src:
                skipped += 1
                continue
            apply_eval(tgt, full)
            applied += 1
        for key, (src, full) in manual_results.items():
            tgt = cur.get(key)
            if tgt is None or tgt.get("c_source") != src or not _is_manual(tgt):
                continue
            if _manual_hold_releasable(tgt, full):
                tgt.pop("manual_divergences", None)
                tgt.pop("manual_evidence", None)
                tgt.pop("manual_date", None)
                apply_eval(tgt, full)          # stamps evaluator/cand_size again
                applied += 1
        save_db(cur)
    pin = {"canonical": M.CANONICAL, "flags": M.DEFAULT_FLAGS,
           "cpp_flags": M.DEFAULT_FLAGS.replace("-lang c99", "-lang c++"),
           "metric": METRIC_REV,
           "reevaluated": str(datetime.date.today()), "rows": len(db), "held": still_held}
    PIN.write_text(json.dumps(pin, indent=2) + "\n", encoding="utf-8")
    _PIN_CACHE.clear()
    print(f"applied {applied} rows ({skipped} changed under the pass and were left "
          f"alone); pinned evaluator {_fingerprint(M.CANONICAL, METRIC_REV)} in {PIN.name}")


CONFIG_SYM = re.compile(r"^(\S+)\s+kind:(\w+)(?:\([^)]*\))?\s+addr:0x([0-9a-fA-F]+)")
SRC_IDENT = re.compile(r"\b(_Z[A-Za-z0-9_]{3,}|func_[A-Za-z0-9_]+|data_[A-Za-z0-9_]+)\b")
NAME_ADDR = re.compile(r"_(0[0-9a-f]{7})$")
# tests point this at a fixture tree; production always reads the repo's config/
CONFIG_GLOB_ROOT = REPO


def _config_symbols():
    """(names, {addr: first name}) from every committed symbol table.

    kind: comes in two shapes -- `kind:function(arm,size=0x10)` and a bare `kind:bss`
    with no parentheses. A pattern that requires the parentheses silently drops all 787
    bss symbols and reports live data references as dead, which is how the first pass of
    this audit produced 137 false positives against 27 real ones.
    """
    names, by_addr = set(), {}
    for f in sorted(CONFIG_GLOB_ROOT.glob("config/**/symbols.txt")):
        for line in f.read_text(encoding="utf-8", errors="replace").splitlines():
            m = CONFIG_SYM.match(line)
            if not m:
                continue
            names.add(m.group(1))
            by_addr.setdefault(int(m.group(3), 16), m.group(1))
    return names, by_addr


def check_refs(args):
    """Report near-miss sources naming a symbol that no longer exists in config.

    A dead callee is invisible to the byte gate: relocation slots are wildcarded, so a
    draft that calls a renamed or invented function scores exactly as well as one that
    calls the real thing, and only fails when someone tries to LINK it. 13 of 59 rows
    carried one on 2026-09-07, every one of them a rename that landed on main after the
    row was banked (Actor became dActor_c, Scene became dScene_c, and a handful of
    func_ADDR placeholders got real names).

    Renames are recoverable because the ADDRESS is still in the name: func_020124c4 is
    Sound_PlayIfNotActive now, and resolving by address rather than by name is the same
    rule the port uses when main renames something under it.
    """
    names, by_addr = _config_symbols()
    db = load_db()
    dead = 0
    for key in sorted(db):
        r = db[key]
        src = r.get("c_source") or ""
        bad = sorted({n for n in SRC_IDENT.findall(src)} - {r["name"]} - names)
        if not bad:
            continue
        print(f"{r.get('module')} {r['name']} (div={r.get('divergences')})")
        for n in bad:
            dead += 1
            m = NAME_ADDR.search(n)
            hit = by_addr.get(int(m.group(1), 16)) if m else None
            if hit:
                print(f"    {n}  ->  {hit}   (resolved by address)")
            else:
                print(f"    {n}  ->  UNRESOLVED, no address in the name")
    if dead:
        print()
        print(f"check-refs: {dead} dead reference(s) across the database")
        if args.check:
            sys.exit(1)
    else:
        print(f"check-refs: every reference in {len(db)} row(s) resolves in config")


def _resolvable_dead_refs(db, names, by_addr):
    """(row, old_name, new_name) for every check_refs dead reference that resolves by
    address -- the same lookup check_refs prints as "-> NAME (resolved by address)".
    An unresolved one (no address in the name) is not returned: there is nothing a
    text substitution could do with it."""
    out = []
    for r in db.values():
        src = r.get("c_source") or ""
        bad = sorted({n for n in SRC_IDENT.findall(src)} - {r["name"]} - names)
        for n in bad:
            m = NAME_ADDR.search(n)
            hit = by_addr.get(int(m.group(1), 16)) if m else None
            if hit and hit != n:
                out.append((r, n, hit))
    return out


def fix_refs(args):
    """Rewrite a check-refs dead reference inside c_source to its current, resolved
    name -- e.g. a stored candidate that still spells a callee `func_020717c0` after
    config renamed the symbol at that address to `__rethrow`.

    A dead callee is invisible to the byte gate (relocation slots are wildcarded), so
    it never demands attention the way a compile error would; check-refs is the only
    thing that notices, and until now correcting one meant hand-editing db.jsonl. This
    is the tool-native fix: a whole-word text substitution in c_source, address-scoped
    exactly like check-refs's own resolution, so it can never touch an unrelated
    identifier that happens to share a substring. Only c_source changes -- the row's
    own `name`/key (resync-names' job) and its divergence count are untouched; a
    substitution of a dead callee name cannot change what the candidate compiles to,
    since it's a rename of an already-nonexistent symbol, not a code edit that
    survives compilation as-is only because the compiler treats the callee as an
    unresolved extern either way."""
    names, by_addr = _config_symbols()
    db = load_db()
    pending = _resolvable_dead_refs(db, names, by_addr)
    if args.dry_run:
        for r, old, new in pending:
            print(f"  {r['module']:6} {r['name'][:34]:34} {old} -> {new}")
        print(f"{len(pending)} dead reference(s) resolvable by address (dry run)")
        return
    with locked():
        db = load_db()
        pending = _resolvable_dead_refs(db, names, by_addr)
        rows = set()
        for r, old, new in pending:
            r["c_source"] = re.sub(r"\b%s\b" % re.escape(old), new, r["c_source"])
            rows.add(r["name"])
        save_db(db)
    print(f"fix-refs: rewrote {len(pending)} dead reference(s) across {len(rows)} row(s)")


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    p = sub.add_parser("ingest"); p.add_argument("--result"); p.add_argument("--seeds")
    p.add_argument("--worklist"); p.add_argument("--label"); p.set_defaults(fn=ingest)
    p = sub.add_parser("stats"); p.set_defaults(fn=stats)
    p = sub.add_parser("list"); p.add_argument("--max-div", type=int)
    p.add_argument("--floor-only", action="store_true",
                   help="only entries marked as compiler-internal floor")
    p.set_defaults(fn=_list)
    p = sub.add_parser("export-close"); p.add_argument("--max-div", type=int, default=8)
    p.add_argument("--out", default="progress/close.jsonl")
    p.add_argument("--category", default=None,
                   help="comma list of category substrings to keep (e.g. "
                        "'register allocation,instruction reorder' for permuter seeds)")
    p.add_argument("--include-floor", action="store_true",
                   help="keep entries marked as compiler-internal floor (skipped by default)")
    p.set_defaults(fn=export_close)
    p = sub.add_parser("mark-floor")
    p.add_argument("--name", required=True, help="function name, or comma list")
    p.add_argument("--class", dest="floor_class", default="ordering",
                   help="floor class (ordering, materialization, ...)")
    p.add_argument("--evidence", required=True,
                   help="what was tried and why the residual is compiler-internal")
    p.set_defaults(fn=mark_floor)
    p = sub.add_parser("unmark-floor")
    p.add_argument("--name", required=True, help="function name, or comma list")
    p.set_defaults(fn=unmark_floor)
    p = sub.add_parser("set-divergence")
    p.add_argument("--name", required=True, help="function name (one row)")
    p.add_argument("--divergences", required=True, type=int,
                   help="the corrected count, either direction")
    p.add_argument("--evidence", required=True,
                   help="what was measured and how (e.g. a pasted match.py run)")
    p.set_defaults(fn=set_divergence)
    p = sub.add_parser("bank-matches")
    p.add_argument("--no-strict", action="store_true",
                   help="skip the reloc-destination gate (bytes-only banking)")
    p.set_defaults(fn=bank_matches)
    p = sub.add_parser("reeval")
    p.add_argument("--dry-run", action="store_true",
                   help="print the drift report without writing db.jsonl or the pin")
    p.set_defaults(fn=reeval)
    p = sub.add_parser("prune-matched")
    p.add_argument("--dry-run", action="store_true",
                   help="list the ghost entries without dropping them")
    p.set_defaults(fn=prune_matched)
    p = sub.add_parser("resync-names")
    p.add_argument("--dry-run", action="store_true",
                   help="list stale names without rewriting them")
    p.set_defaults(fn=resync_names)
    p = sub.add_parser("check-refs")
    p.add_argument("--check", action="store_true",
                   help="regression gate: exit 1 if any row names a symbol config "
                        "does not have")
    p.set_defaults(fn=check_refs)
    p = sub.add_parser("fix-refs")
    p.add_argument("--dry-run", action="store_true",
                   help="list what would be rewritten without changing db.jsonl")
    p.set_defaults(fn=fix_refs)
    p = sub.add_parser("dedupe")
    p.add_argument("--dry-run", action="store_true",
                   help="report duplicate rows without collapsing them")
    p.add_argument("--check", action="store_true",
                   help="regression gate: write nothing, exit 1 if any normalized "
                        "(module, addr) holds more than one row")
    p.set_defaults(fn=dedupe)
    args = ap.parse_args()
    args.fn(args)


if __name__ == "__main__":
    main()
