"""Regression tests for the load/merge/save layer of tools/nearmiss_db.py.

The shape under test is the 2026-08-25 ov004 0x020ae858 incident: a duplicated
(module, addr) key on disk. save_db sorts ascending by divergences, so the worse
of two duplicate rows is always the LAST one in the file -- a last-wins load_db
made the next save silently discard the better candidate and keep the worse one.
nearmiss/README.md promises one record per (module, addr) keeping the CLOSEST
candidate; these tests hold load_db, merge_batch, and dedupe --check to that.

Nothing here compiles or disassembles. The swarm/match stack imports lazily, so
this suite (and the metadata-only subcommands) runs on a bare interpreter:

    python -m unittest tools.test_nearmiss_db -v
"""
import argparse
import contextlib
import io
import json
import pathlib
import subprocess
import sys
import tempfile
import unittest

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))
import nearmiss_db as NDB  # noqa: E402
import ledger as L  # noqa: E402

KEY = L.make_key("ov004", 0x020AE858)


def row(div, addr="0x020ae858", module="ov004", **kw):
    r = {"module": module, "addr": addr, "name": f"func_{module}", "size": 0x164,
         "target_hex": "00", "lang": "c", "divergences": div,
         "c_source": "void f(void) {}", "source": "test"}
    r.update(kw)
    return r


class NearMissDbTests(unittest.TestCase):
    def setUp(self):
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self._saved_db = NDB.DB
        self.addCleanup(setattr, NDB, "DB", self._saved_db)
        NDB.DB = pathlib.Path(tmp.name) / "db.jsonl"
        # Redirect the eval pin too (absent by default, so tests that predate it see
        # the pre-pin ranking), and keep its cache from leaking across tests.
        self.addCleanup(setattr, NDB, "PIN", NDB.PIN)
        NDB.PIN = pathlib.Path(tmp.name) / "eval_pin.json"
        NDB._PIN_CACHE.clear()
        self.addCleanup(NDB._PIN_CACHE.clear)

    def write_rows(self, *rows):
        NDB.DB.write_text("".join(json.dumps(r) + "\n" for r in rows), encoding="utf-8")

    def write_pin(self, canonical="2004/b56", metric=NDB.METRIC_REV):
        NDB.PIN.write_text(json.dumps({"canonical": canonical, "metric": metric}),
                           encoding="utf-8")
        NDB._PIN_CACHE.clear()

    def load_quiet(self):
        with contextlib.redirect_stderr(io.StringIO()) as err:
            db = NDB.load_db()
        return db, err.getvalue()

    # ------------------------------------------------------------------ load_db
    def test_load_db_keeps_closest_row_and_warns(self):
        # Incident file shape: ascending save order puts the worse duplicate LAST,
        # exactly where a last-wins reader would keep it.
        self.write_rows(row(10), row(50))
        db, err = self.load_quiet()
        self.assertEqual(len(db), 1)
        self.assertEqual(db[KEY]["divergences"], 10)
        self.assertIn("duplicate (module, addr)", err)
        # The other order must give the same answer.
        self.write_rows(row(50), row(10))
        db, _ = self.load_quiet()
        self.assertEqual(db[KEY]["divergences"], 10)

    def test_load_db_is_quiet_without_duplicates(self):
        self.write_rows(row(10), row(42, addr="0x020ee994", module="ov006"))
        db, err = self.load_quiet()
        self.assertEqual(len(db), 2)
        self.assertEqual(err, "")

    def test_save_after_load_keeps_the_better_duplicate(self):
        # The data-loss step itself: load a duplicated file, save it back, and the
        # surviving row on disk must be the closer one (the old last-wins load wrote
        # back div=50 and dropped div=10 here).
        self.write_rows(row(10), row(50))
        db, _ = self.load_quiet()
        NDB.save_db(db)
        lines = [json.loads(l) for l in NDB.DB.read_text().splitlines()]
        self.assertEqual([r["divergences"] for r in lines], [10])

    def test_load_db_floor_mark_breaks_a_divergence_tie(self):
        floored = row(10, floor={"class": "ordering", "evidence": "e", "date": "2026-08-25"})
        self.write_rows(row(10), floored)
        db, _ = self.load_quiet()
        self.assertTrue(db[KEY].get("floor"))

    def test_load_db_collides_hex_string_and_int_addr_forms(self):
        self.write_rows(row(50, addr="0x020ae858"), row(10, addr=0x020AE858))
        db, _ = self.load_quiet()
        self.assertEqual(list(db), [KEY])
        self.assertEqual(db[KEY]["divergences"], 10)

    # ----------------------------------------------------- eval-pin stamped ranking
    def test_load_db_prefers_a_row_stamped_by_the_current_pin(self):
        # Union merges resurrect copies scored under an older evaluator; their stale
        # LOWER divergences must not undo a reeval correction (230 -> 354 shape).
        self.write_pin()
        stale = row(13)
        corrected = row(354, evaluator="2004/b56|m%d" % NDB.METRIC_REV)
        self.write_rows(corrected, stale)
        db, _ = self.load_quiet()
        self.assertEqual(db[KEY]["divergences"], 354)

    def test_load_db_prefers_a_stamped_unscorable_row_over_stale_bait(self):
        # The worst resurrection: a row whose source no longer compiles, recorded at
        # div 13 -- poisoned bait atop every closest-first worklist.
        self.write_pin()
        bait = row(13)
        marked = row(None, evaluator="2004/b56|m%d" % NDB.METRIC_REV,
                     status="noncompile", error="undefined identifier 'pos'",
                     stale_divergences=13)
        self.write_rows(bait, marked)
        db, _ = self.load_quiet()
        self.assertIsNone(db[KEY]["divergences"])
        self.assertEqual(db[KEY]["status"], "noncompile")

    def test_load_db_without_a_pin_keeps_the_pre_pin_order(self):
        # No eval_pin.json (older checkouts): the stamp must not influence ranking.
        self.write_rows(row(13), row(354, evaluator="2004/b56|m%d" % NDB.METRIC_REV))
        db, _ = self.load_quiet()
        self.assertEqual(db[KEY]["divergences"], 13)

    def test_save_db_ranks_unscorable_rows_last(self):
        other = L.make_key("ov006", 0x020EE994)
        db = {KEY: row(None, status="noncompile", error="x", stale_divergences=13),
              other: row(42, addr="0x020ee994", module="ov006")}
        NDB.save_db(db)
        lines = [json.loads(l) for l in NDB.DB.read_text().splitlines()]
        self.assertEqual([r["divergences"] for r in lines], [42, None])

    # -------------------------------------------------------------- merge_batch
    def test_merge_counts_a_better_candidate_as_improved_not_new(self):
        db = {KEY: row(50)}
        added, improved = NDB.merge_batch(db, [], {L.make_key("ov004", "0x020ae858"): row(10)})
        self.assertEqual((added, improved), (0, 1))
        self.assertEqual(db[KEY]["divergences"], 10)

    def test_merge_ignores_a_worse_candidate(self):
        db = {KEY: row(10)}
        added, improved = NDB.merge_batch(db, [], {KEY: row(50)})
        self.assertEqual((added, improved), (0, 0))
        self.assertEqual(db[KEY]["divergences"], 10)

    def test_merge_counts_a_fresh_key_as_new(self):
        db = {}
        added, improved = NDB.merge_batch(db, [], {KEY: row(10)})
        self.assertEqual((added, improved), (1, 0))

    def test_merge_same_divergence_closer_size_wins(self):
        # Edit distance is non-monotonic in candidate size and the permuter cannot add
        # or remove instructions: of two equally-close drafts, the one assembling
        # nearer the target size is the better seed and must win the upsert.
        db = {KEY: row(10, cand_size=0x150)}            # 0x14 short of the 0x164 target
        added, improved = NDB.merge_batch(db, [], {KEY: row(10, cand_size=0x164)})
        self.assertEqual((added, improved), (0, 1))
        self.assertEqual(db[KEY]["cand_size"], 0x164)

    def test_merge_same_divergence_farther_size_does_not_replace(self):
        db = {KEY: row(10, cand_size=0x164)}
        added, improved = NDB.merge_batch(db, [], {KEY: row(10, cand_size=0x150)})
        self.assertEqual((added, improved), (0, 0))
        self.assertEqual(db[KEY]["cand_size"], 0x164)

    def test_merge_never_regresses_divergences_for_a_closer_size(self):
        # Divergences still lead the key: size proximity only breaks exact ties.
        db = {KEY: row(10, cand_size=0x150)}
        added, improved = NDB.merge_batch(db, [], {KEY: row(11, cand_size=0x164)})
        self.assertEqual((added, improved), (0, 0))
        self.assertEqual(db[KEY]["divergences"], 10)

    def test_merge_replaces_an_unscorable_row_with_any_compiling_candidate(self):
        db = {KEY: row(None, status="noncompile", error="x", stale_divergences=13)}
        added, improved = NDB.merge_batch(db, [], {KEY: row(200)})
        self.assertEqual((added, improved), (0, 1))
        self.assertEqual(db[KEY]["divergences"], 200)
        self.assertNotIn("status", db[KEY])

    def test_merge_drop_beats_a_same_run_update(self):
        # One seeds file can carry two names for one key; if one of them proves the
        # function matched, the update from the other must not resurrect the row.
        db = {KEY: row(50)}
        added, improved = NDB.merge_batch(db, [KEY], {KEY: row(10)})
        self.assertEqual((added, improved), (0, 0))
        self.assertNotIn(KEY, db)

    # ------------------------------------------------------------ dedupe --check
    def test_dedupe_check_fails_on_a_duplicated_key(self):
        self.write_rows(row(10), row(50))
        with contextlib.redirect_stdout(io.StringIO()):
            with self.assertRaises(SystemExit) as ctx:
                NDB.dedupe(argparse.Namespace(check=True, dry_run=False))
        self.assertEqual(ctx.exception.code, 1)

    def test_dedupe_check_passes_a_clean_db(self):
        self.write_rows(row(10), row(42, addr="0x020ee994", module="ov006"))
        with contextlib.redirect_stdout(io.StringIO()) as out:
            NDB.dedupe(argparse.Namespace(check=True, dry_run=False))
        self.assertIn("check ok", out.getvalue())

    # ------------------------------------------------------------- dead callees
    # A renamed callee is INVISIBLE to the byte gate: relocation slots are wildcarded,
    # so a draft calling a symbol that no longer exists scores exactly as well as one
    # calling the real thing and only fails when someone tries to link it. 13 of 59 rows
    # carried one on 2026-09-07, all of them renames that landed on main after the row
    # was banked.
    def _fixture_config(self, *names):
        root = pathlib.Path(tempfile.mkdtemp())
        d = root / "config"
        d.mkdir(parents=True)
        (d / "symbols.txt").write_text(
            "".join("%s kind:function(arm,size=0x10) addr:0x%08x\n" % (n, 0x02000000 + i * 4)
                    for i, n in enumerate(names)), encoding="utf-8")
        return root

    def test_check_refs_passes_when_every_callee_resolves(self):
        self.write_rows(row(10, c_source="void f(void){ Sound_PlayIfNotActive(); }"))
        NDB.CONFIG_GLOB_ROOT = self._fixture_config("Sound_PlayIfNotActive")
        try:
            with contextlib.redirect_stdout(io.StringIO()) as out:
                NDB.check_refs(argparse.Namespace(check=True))
            self.assertIn("every reference", out.getvalue())
        finally:
            NDB.CONFIG_GLOB_ROOT = NDB.REPO

    def test_check_refs_fails_on_a_callee_config_does_not_have(self):
        self.write_rows(row(10, c_source="void f(void){ func_020124c4(); }"))
        NDB.CONFIG_GLOB_ROOT = self._fixture_config("Sound_PlayIfNotActive")
        try:
            with contextlib.redirect_stdout(io.StringIO()) as out:
                with self.assertRaises(SystemExit) as ctx:
                    NDB.check_refs(argparse.Namespace(check=True))
            self.assertEqual(ctx.exception.code, 1)
            self.assertIn("func_020124c4", out.getvalue())
        finally:
            NDB.CONFIG_GLOB_ROOT = NDB.REPO

    # -------------------------------------------------------------- fix-refs
    # check-refs only REPORTS a dead reference; fix-refs is the tool-native repair --
    # a whole-word rewrite of the stale name to its current, address-resolved one,
    # touching only c_source. Mirrors the real pre-existing row: arm9 __rethrow's
    # c_source still spells its own function `func_020717c0` after config renamed the
    # symbol at that address.
    def _fixture_config_addr(self, name, addr):
        root = pathlib.Path(tempfile.mkdtemp())
        d = root / "config"
        d.mkdir(parents=True)
        (d / "symbols.txt").write_text(
            "%s kind:function(arm,size=0x10) addr:0x%08x\n" % (name, addr),
            encoding="utf-8")
        return root

    def test_fix_refs_rewrites_a_reference_resolvable_by_address(self):
        self.write_rows(row(1, name="__rethrow",
                             c_source="void func_020717c0(void) { __rethrow(); }"))
        NDB.CONFIG_GLOB_ROOT = self._fixture_config_addr("__rethrow", 0x020717C0)
        try:
            with contextlib.redirect_stdout(io.StringIO()) as out:
                NDB.fix_refs(argparse.Namespace(dry_run=False))
            self.assertIn("rewrote 1 dead reference(s) across 1 row(s)", out.getvalue())
            db, _ = self.load_quiet()
            self.assertNotIn("func_020717c0", db[KEY]["c_source"])
            self.assertEqual(db[KEY]["c_source"], "void __rethrow(void) { __rethrow(); }")
            # check-refs is clean afterward -- the fix actually closes the finding.
            with contextlib.redirect_stdout(io.StringIO()) as out2:
                NDB.check_refs(argparse.Namespace(check=True))
            self.assertIn("every reference", out2.getvalue())
        finally:
            NDB.CONFIG_GLOB_ROOT = NDB.REPO

    def test_fix_refs_dry_run_changes_nothing(self):
        self.write_rows(row(1, name="__rethrow",
                             c_source="void func_020717c0(void) { __rethrow(); }"))
        NDB.CONFIG_GLOB_ROOT = self._fixture_config_addr("__rethrow", 0x020717C0)
        try:
            with contextlib.redirect_stdout(io.StringIO()) as out:
                NDB.fix_refs(argparse.Namespace(dry_run=True))
            self.assertIn("func_020717c0 -> __rethrow", out.getvalue())
            self.assertIn("(dry run)", out.getvalue())
            db, _ = self.load_quiet()
            self.assertIn("func_020717c0", db[KEY]["c_source"])
        finally:
            NDB.CONFIG_GLOB_ROOT = NDB.REPO

    def test_fix_refs_leaves_an_unresolved_reference_alone(self):
        # No address in config for func_020124c4 -- nothing fix-refs can do with it;
        # it must not touch c_source or crash.
        self.write_rows(row(10, c_source="void f(void){ func_020124c4(); }"))
        NDB.CONFIG_GLOB_ROOT = self._fixture_config("Sound_PlayIfNotActive")
        try:
            with contextlib.redirect_stdout(io.StringIO()) as out:
                NDB.fix_refs(argparse.Namespace(dry_run=False))
            self.assertIn("rewrote 0 dead reference(s)", out.getvalue())
            db, _ = self.load_quiet()
            self.assertIn("func_020124c4", db[KEY]["c_source"])
        finally:
            NDB.CONFIG_GLOB_ROOT = NDB.REPO

    def test_fix_refs_is_a_no_op_when_every_reference_resolves(self):
        self.write_rows(row(10, c_source="void f(void){ Sound_PlayIfNotActive(); }"))
        NDB.CONFIG_GLOB_ROOT = self._fixture_config("Sound_PlayIfNotActive")
        try:
            with contextlib.redirect_stdout(io.StringIO()) as out:
                NDB.fix_refs(argparse.Namespace(dry_run=False))
            self.assertIn("rewrote 0 dead reference(s)", out.getvalue())
        finally:
            NDB.CONFIG_GLOB_ROOT = NDB.REPO

    # ---------------------------------------------------------- set-divergence
    # nearmiss/db.jsonl's own metric can UNDERCOUNT a stored row (2026-09-09: a pure
    # 3-instruction ROM reorder on _ZN3MrI13InitResourcesEv scored 2 under
    # evaluate_full's SequenceMatcher diff but 3 under tools/match.py's positional
    # MISMATCH count, the number the committed src/ header and floors.jsonl agreed on),
    # and merge_batch/ingest is deliberately strictly-improving, so nothing else in
    # this file can ever RAISE a stored divergence. set_divergence is the escape hatch;
    # these tests hold it to both directions and to surviving reeval/bank-matches.
    def test_set_divergence_can_raise_a_stale_row(self):
        self.write_rows(row(2))
        with contextlib.redirect_stdout(io.StringIO()) as out:
            NDB.set_divergence(argparse.Namespace(
                name="func_ov004", divergences=3,
                evidence="match.py --strict-relocs 2004/b56: 3 MISMATCH at +0x20/+0x24/+0x28"))
        db, _ = self.load_quiet()
        self.assertEqual(db[KEY]["divergences"], 3)
        self.assertTrue(db[KEY]["manual_divergences"])
        self.assertIn("MISMATCH", db[KEY]["manual_evidence"])
        self.assertIn("2 -> 3", out.getvalue())

    def test_set_divergence_can_also_lower_a_row(self):
        # Not one-directional: the same verb corrects an over-count too.
        self.write_rows(row(50))
        NDB.set_divergence(argparse.Namespace(
            name="func_ov004", divergences=10, evidence="match.py: 10 MISMATCH"))
        db, _ = self.load_quiet()
        self.assertEqual(db[KEY]["divergences"], 10)

    def test_set_divergence_unknown_name_exits_without_writing(self):
        self.write_rows(row(2))
        with contextlib.redirect_stdout(io.StringIO()):
            with self.assertRaises(SystemExit):
                NDB.set_divergence(argparse.Namespace(
                    name="nope", divergences=3, evidence="x"))
        db, _ = self.load_quiet()
        self.assertEqual(db[KEY]["divergences"], 2)      # untouched

    def test_set_divergence_drops_cand_size_not_backed_by_a_fresh_compile(self):
        self.write_rows(row(2, cand_size=0x164))
        NDB.set_divergence(argparse.Namespace(
            name="func_ov004", divergences=3, evidence="x"))
        db, _ = self.load_quiet()
        self.assertNotIn("cand_size", db[KEY])

    def test_is_manual_false_by_default(self):
        self.assertFalse(NDB._is_manual(row(10)))

    def test_is_manual_true_after_a_correction(self):
        self.write_rows(row(2))
        NDB.set_divergence(argparse.Namespace(
            name="func_ov004", divergences=3, evidence="x"))
        db, _ = self.load_quiet()
        self.assertTrue(NDB._is_manual(db[KEY]))

    def test_reeval_order_excludes_manual_rows(self):
        # reeval must never hand a manual row to evaluate_full -- that recompile is
        # exactly what would silently undo the correction (see _is_manual). This checks
        # the filter reeval builds `order` from, without invoking reeval itself (which
        # unconditionally imports the compiler-backed match module).
        manual = row(3, addr="0x020ae858", manual_divergences=True, evidence="x")
        other = row(42, addr="0x020ee994", module="ov006")
        self.write_rows(manual, other)
        db, _ = self.load_quiet()
        order = [r for r in db.values() if not NDB._is_manual(r)]
        self.assertEqual([r["divergences"] for r in order], [42])

    # ---------------------------------------------- manual hold auto-release (reeval)
    # METRIC_REV 2 makes evaluate_full agree with match.py, so a manual correction's
    # OWN stored c_source should now reproduce the hand-set number under the live
    # evaluator on its own -- reeval verifies that every pass and retires the hold
    # instead of leaving a row manual forever (_manual_hold_releasable is the pure
    # decision function reeval's compiler-backed loop calls; these test it directly).
    def test_manual_hold_releasable_when_live_score_agrees(self):
        r = row(3, manual_divergences=True, manual_evidence="x", manual_date="2026-09-09")
        full = {"status": None, "error": None, "divergences": 3, "cand_size": 20, "ok": False}
        self.assertTrue(NDB._manual_hold_releasable(r, full))

    def test_manual_hold_not_releasable_when_live_score_disagrees(self):
        r = row(3, manual_divergences=True)
        full = {"status": None, "error": None, "divergences": 2, "cand_size": 20, "ok": False}
        self.assertFalse(NDB._manual_hold_releasable(r, full))

    def test_manual_hold_not_releasable_when_unscorable(self):
        r = row(3, manual_divergences=True)
        full = {"status": "noncompile", "error": "x", "divergences": None, "cand_size": None,
                "ok": False}
        self.assertFalse(NDB._manual_hold_releasable(r, full))

    # ------------------------------------------------- apply_eval: not strictly-improving
    # ingest/merge_batch is deliberately strictly-improving (never regresses a stored
    # divergence between two competing drafts); apply_eval is the OTHER half of the
    # rule -- re-measuring the SAME draft under today's live scorer moves the number
    # whichever way the scorer says, including up, when a compiler/metric bump makes
    # yesterday's count wrong (see merge_batch's and apply_eval's docstrings).
    def test_apply_eval_can_raise_a_rows_divergences(self):
        r = row(2, cand_size=0x150)
        NDB.apply_eval(r, {"status": None, "error": None, "divergences": 5,
                            "cand_size": 0x164})
        self.assertEqual(r["divergences"], 5)
        self.assertEqual(r["cand_size"], 0x164)

    def test_apply_eval_can_lower_a_rows_divergences(self):
        r = row(50)
        NDB.apply_eval(r, {"status": None, "error": None, "divergences": 3,
                            "cand_size": 0x164})
        self.assertEqual(r["divergences"], 3)

    # ------------------------------------------------------------------ imports
    def test_module_imports_without_the_compile_stack(self):
        # stats/list/dedupe and this suite must run where capstone/pyelftools are
        # not installed; the compile+disasm stack has to stay a lazy import.
        code = ("import sys; sys.path.insert(0, sys.argv[1]); import nearmiss_db; "
                "heavy = [m for m in sys.modules if m == 'capstone' or m.startswith('elftools')]; "
                "print(','.join(heavy) or 'clean')")
        out = subprocess.run([sys.executable, "-c", code, str(TOOLS)],
                             capture_output=True, text=True)
        self.assertEqual(out.returncode, 0, out.stderr)
        self.assertEqual(out.stdout.strip(), "clean")

    # --------------------------------------------- match.py scorer parity (METRIC_REV 2)
    # The bug this change fixes: a pure instruction-position ROTATE reads differently
    # under an edit-distance diff (which finds a cheaper alignment across the reorder)
    # than under match.py's own fixed-position word compare -- 2 vs 3 on a real
    # 3-instruction ROM rotation (_ZN3MrI13InitResourcesEv, 2026-09-09). These need
    # capstone/pyelftools to import tools/match.py (compare(verbose=False) never
    # disassembles, so no capstone CALL happens, only the import) but never mwccarm --
    # skip cleanly wherever those two packages are not installed, run for real
    # wherever tool-tests.yml's `pip install -r tools/requirements.txt` step ran.
    def _require_compile_stack(self):
        try:
            import match
            import swarm
        except ImportError as e:
            self.skipTest(f"capstone/pyelftools not installed: {e}")
        return match, swarm

    def test_match_compare_counts_a_pure_reorder_positionally(self):
        M, _ = self._require_compile_stack()
        a, b, c = b"\x01\x00\x00\x00", b"\x02\x00\x00\x00", b"\x03\x00\x00\x00"
        target = a + b + c                  # the ROM's order
        candidate = b + c + a               # a full rotation of the same three words
        ok, ndiff = M.compare(target, candidate, set(), verbose=False)
        self.assertFalse(ok)
        self.assertEqual(ndiff, 3)           # match.py --brief's own number for this shape

    def test_evaluate_full_divergences_is_match_compares_own_number(self):
        # Wire-level parity, not just equal-by-coincidence: stub only the compile/
        # extract steps (oracle_check, extract_func) so evaluate_full's divergence
        # count runs through the REAL match.compare on these bytes, not a
        # reimplementation. No mwccarm involved.
        M, swarm = self._require_compile_stack()
        from unittest import mock
        a, b, c = b"\x01\x00\x00\x00", b"\x02\x00\x00\x00", b"\x03\x00\x00\x00"
        target = a + b + c
        candidate = b + c + a
        with mock.patch.object(swarm, "oracle_check", return_value=(False, b"fake-obj")), \
             mock.patch.object(M, "extract_func", return_value=(candidate, set())):
            full = NDB.evaluate_full("void f(void){}", "f", target)
        direct_ok, direct_ndiff = M.compare(target, candidate, set(), verbose=False)
        self.assertEqual(full["divergences"], direct_ndiff)
        self.assertEqual(full["divergences"], 3)
        self.assertEqual(full["ok"], direct_ok)


class EvaluateFullResolverParityTests(unittest.TestCase):
    """evaluate_full resolves the candidate for the same two shapes LINKCHK2 (#2535)
    closed in tools/linkcheck.py's byte gate -- the nine ITCM rows NMDB2's whole-DB
    reeval marked func-absent (extract by exact name found nothing, because that is
    what a zero-size alias record and a nested entry point both look like to a plain
    symbol lookup). Shared, not re-derived: bytegate.alias_target_size and
    reloc_audit.resolve_nested_slice are the exact functions tools/linkcheck.py
    calls, imported here and there, so a stored divergence can never disagree with a
    fresh linkcheck run on the same c_source.

    The Unit tests below mock oracle_check/extract_func/alias_target_size/
    resolve_nested_slice directly and need no compiler at all -- same convention
    test_linkcheck.py's own AliasSizeSubstitutionUnit / NestedOffsetRebaseUnit use,
    for the same reason: the WIRING is what these prove, and resolve_nested_slice's
    own offset arithmetic already has dedicated coverage there (shared code, not
    re-tested here). RealCompileFixtures below compiles the actual committed
    src/_dmul.c and src/func_01ff97d8.c and skips if the canonical (2004/b56)
    compiler is absent, same as tools/test_linkcheck.py's RealCompileFixtures."""

    def _require_compile_stack(self):
        try:
            import match
            import swarm
        except ImportError as e:
            self.skipTest(f"capstone/pyelftools not installed: {e}")
        return match, swarm

    def test_alias_zero_size_row_is_scored_against_its_sized_twin(self):
        """A zero-size EABI alias row (e.g. _dadd, config size 0 at the same address
        as the sized primary func_01ff8000): the caller's own target/size are 0
        bytes -- exactly the shape that used to read func-absent regardless of how
        correct the source was, because a 0-byte target can never be byte-compared
        against a nonzero compiled candidate. evaluate_full must resolve the real
        size the same way linkcheck() does (bytegate.alias_target_size) and re-read
        the ROM at that length (reverify_corpus.rom_bytes) before comparing -- proven
        through the REAL match.compare on the resolved bytes, not a stand-in."""
        M, swarm = self._require_compile_stack()
        import bytegate
        import reverify_corpus
        from unittest import mock
        real = b"\x01\x00\x00\x00\x02\x00\x00\x00"        # the sized twin's ROM bytes
        candidate = b"\x01\x00\x00\x00\x03\x00\x00\x00"   # one word off
        with mock.patch.object(swarm, "oracle_check", return_value=(False, b"fake-obj")), \
             mock.patch.object(M, "extract_func", return_value=(candidate, set())), \
             mock.patch.object(bytegate, "alias_target_size", return_value=len(real)) as alt, \
             mock.patch.object(reverify_corpus, "rom_bytes", return_value=real) as rb:
            full = NDB.evaluate_full("asm double _dadd(...) { ... }", "_dadd", b"",
                                     module="itcm", addr=0x01ff8000, size=0)
        alt.assert_called_once_with("itcm", 0x01ff8000)
        rb.assert_called_once_with("itcm", 0x01ff8000, len(real))
        direct_ok, direct_ndiff = M.compare(real, candidate, set(), verbose=False)
        self.assertEqual(full["divergences"], direct_ndiff)
        self.assertEqual(full["ok"], direct_ok)
        self.assertEqual(full["cand_size"], len(candidate))

    def test_alias_row_that_byte_matches_its_sized_twin_reads_zero_and_ok(self):
        """The MATCH case: once the size is resolved, a byte-exact candidate must
        read divergences 0 / ok True -- the number reeval treats as bankable."""
        M, swarm = self._require_compile_stack()
        import bytegate
        import reverify_corpus
        from unittest import mock
        real = b"\x01\x00\x00\x00\x02\x00\x00\x00"
        with mock.patch.object(swarm, "oracle_check", return_value=(False, b"fake-obj")), \
             mock.patch.object(M, "extract_func", return_value=(real, set())), \
             mock.patch.object(bytegate, "alias_target_size", return_value=len(real)), \
             mock.patch.object(reverify_corpus, "rom_bytes", return_value=real):
            full = NDB.evaluate_full("asm double _dadd(...) { ... }", "_dadd", b"",
                                     module="itcm", addr=0x01ff8000, size=0)
        self.assertEqual(full["divergences"], 0)
        self.assertTrue(full["ok"])

    def test_nested_entry_point_is_found_inside_its_containing_symbol(self):
        """A nested entry point (e.g. func_01ff98f4 inside func_01ff97d8.c's single
        compiled symbol): `name` has no symbol of its own in the object at all --
        extract_func(obj, name) and the sole-symbol fallback both miss. evaluate_full
        must search every symbol the object DOES define for one that CONTAINS
        name's ROM range, the exact address-anchored resolution
        reloc_audit.resolve_nested_slice gives tools/linkcheck.py -- shared, not
        re-derived, so the two can never disagree on which symbol a nested name
        resolves against."""
        M, swarm = self._require_compile_stack()
        import probe_versions
        import reloc_audit
        from unittest import mock
        container_code = b"\x11" * 0x10 + b"\x01\x00\x00\x00"   # nested func at +0x10
        sliced = container_code[0x10:0x14]
        target = b"\x02\x00\x00\x00"                             # one word off

        def fake_extract(obj, sym):
            if sym == "func_01ff98f4":
                return None, None                # absent from the object by that name
            return container_code, set()          # the containing symbol's full body

        with mock.patch.object(swarm, "oracle_check", return_value=(False, b"fake-obj")), \
             mock.patch.object(M, "extract_func", side_effect=fake_extract), \
             mock.patch.object(M, "sole_func_symbol", return_value=None), \
             mock.patch.object(probe_versions, "funcs_in",
                               return_value={"func_01ff97d8": b""}), \
             mock.patch.object(reloc_audit, "resolve_nested_slice",
                               return_value=(sliced, set(), 0x10)) as rns:
            full = NDB.evaluate_full("//asm container", "func_01ff98f4", target,
                                     module="itcm", addr=0x01ff98f4, size=4, name_index={})
        rns.assert_called_once_with("func_01ff97d8", container_code, set(),
                                    0x01ff98f4, 4, {})
        direct_ok, direct_ndiff = M.compare(target, sliced, set(), verbose=False)
        self.assertEqual(full["divergences"], direct_ndiff)
        self.assertEqual(full["ok"], direct_ok)
        self.assertEqual(full["resolved"], "func_01ff97d8")

    def test_no_container_found_still_reads_func_absent(self):
        """No symbol in the object contains name's range either (resolve_nested_slice
        returns None for every candidate): the row stays func-absent, same as before
        this change -- the search must not fabricate a container that is not there."""
        M, swarm = self._require_compile_stack()
        import probe_versions
        import reloc_audit
        from unittest import mock
        with mock.patch.object(swarm, "oracle_check", return_value=(False, b"fake-obj")), \
             mock.patch.object(M, "extract_func", return_value=(None, None)), \
             mock.patch.object(M, "sole_func_symbol", return_value=None), \
             mock.patch.object(probe_versions, "funcs_in", return_value={}), \
             mock.patch.object(reloc_audit, "resolve_nested_slice", return_value=None):
            full = NDB.evaluate_full("", "func_01ff8df8", b"\x00" * 24,
                                     module="itcm", addr=0x01ff8df8, size=24)
        self.assertIsNone(full["divergences"])
        self.assertEqual(full["status"], "func-absent")


@unittest.skipUnless(
    (TOOLS / "mwccarm" / "2004" / "b56" / "mwccarm.exe").is_file(),
    "canonical (2004/b56) mwccarm not present")
class RealCompileResolverFixtures(unittest.TestCase):
    """Real compiles of the two committed sources tools/test_linkcheck.py's own
    RealCompileFixtures were written for, run through evaluate_full instead of
    linkcheck() -- proving the near-miss evaluator and the byte gate now agree on
    both shapes using the SAME committed source, not just on mocked wiring."""

    def test_dmul_scores_zero_when_evaluated_under_its_alias_row_shape(self):
        """src/_dmul.c, fed to evaluate_full as if it were a near-miss row's own
        c_source with the row's zero-declared size (the alias shape), must resolve
        to the sized twin's real length and read a byte-exact MATCH -- divergences 0,
        ok True -- the number reeval treats as bankable."""
        import match as M
        src = (TOOLS.parent / "src" / "_dmul.c").read_text(encoding="utf-8")
        full = NDB.evaluate_full(src, "_dmul", b"", module="itcm", addr=0x01ff8708, size=0)
        self.assertEqual(full["divergences"], 0)
        self.assertTrue(full["ok"])

    def test_nested_entry_point_scores_zero_from_the_containing_source(self):
        """src/func_01ff97d8.c compiles to ONE ELF symbol, "func_01ff97d8", 0xb6c
        bytes; func_01ff98f4 (config size 0xb0) has no symbol of its own anywhere in
        that object. Fed to evaluate_full as func_01ff98f4's own c_source (the
        nested shape: a stored near-miss draft that is really the whole containing
        block), it must slice the containing symbol's compiled body at the right
        address-resolved offset and read a byte-exact MATCH."""
        import match as M
        import reverify_corpus as RV
        src = (TOOLS.parent / "src" / "func_01ff97d8.c").read_text(encoding="utf-8")
        target = RV.rom_bytes("itcm", 0x01ff98f4, 0xb0)
        full = NDB.evaluate_full(src, "func_01ff98f4", target,
                                 module="itcm", addr=0x01ff98f4, size=0xb0)
        self.assertEqual(full["divergences"], 0)
        self.assertTrue(full["ok"])
        self.assertEqual(full["resolved"], "func_01ff97d8")


class EvalPinGuardTests(unittest.TestCase):
    """The committed evaluator pin must agree with the live evaluator.

    Moving match.CANONICAL or nearmiss_db.METRIC_REV re-scores every stored source,
    so all the recorded divergences (and the closest-first ranking built on them) go
    stale the moment either moves -- silently, since nothing else reads both sides.
    This test is the tripwire: a canonical/metric bump fails CI until someone runs
    `python tools/nearmiss_db.py reeval` on a main-tip checkout and commits the
    re-scored db.jsonl together with the refreshed nearmiss/eval_pin.json.

    CANONICAL and DEFAULT_FLAGS are read out of tools/match.py statically because this
    suite runs on a bare interpreter (tool-tests.yml: no capstone/pyelftools), where
    match.py cannot be imported. Statically via the AST, and asserting EXACTLY ONE
    module-level assignment per name: a regex `search` returns the FIRST match, so a
    second `CANONICAL = ...` further down the file -- the shape a careless edit or a
    bad merge actually produces -- left this guard green while the live evaluator was
    something else entirely."""

    @staticmethod
    def _module_constants(path, names):
        """{name: literal value} for module-level `NAME = <literal>` in a source file.
        Fails if a name is assigned zero or 2+ times at module level, or is not a
        plain literal: the guard must never silently read a value that is not the one
        the tools actually use."""
        import ast
        path = pathlib.Path(path)
        tree = ast.parse(path.read_text(encoding="utf-8"))
        out = {}
        for name in names:
            hits = [n.value for n in tree.body
                    if isinstance(n, ast.Assign)
                    and any(isinstance(t, ast.Name) and t.id == name for t in n.targets)]
            if len(hits) != 1:
                raise AssertionError(
                    f"{path.name} has {len(hits)} module-level assignments to {name}; "
                    f"expected exactly 1. Fix the source -- a shadow re-assignment "
                    f"makes every reader of {name} a coin flip -- or fix this parser "
                    f"AND re-check the eval pin.")
            try:
                out[name] = ast.literal_eval(hits[0])
            except ValueError as e:
                raise AssertionError(
                    f"{name} in {path.name} is no longer a plain literal ({e}); fix "
                    f"this test's parser AND re-check the eval pin.")
        return out

    def test_eval_pin_matches_the_live_evaluator(self):
        pin_path = TOOLS.parent / "nearmiss" / "eval_pin.json"
        self.assertTrue(pin_path.is_file(),
                        "nearmiss/eval_pin.json is missing: the DB has no recorded "
                        "evaluator. Run `python tools/nearmiss_db.py reeval` on a "
                        "main-tip checkout and commit db.jsonl + eval_pin.json.")
        pin = json.loads(pin_path.read_text(encoding="utf-8"))
        live = self._module_constants(TOOLS / "match.py", ("CANONICAL", "DEFAULT_FLAGS"))
        # Flags are gated too, not just the compiler and the metric. -O4,p -> -O2
        # re-scores every stored source far harder than a compiler bump does, and the
        # pin already records `flags`, so leaving it out of the assertion made the
        # pin's own field decorative. cpp_flags is derived from DEFAULT_FLAGS
        # (swarm.CPP_FLAGS), so gating flags covers the C++ lane as well.
        self.assertEqual(
            (pin.get("canonical"), pin.get("metric"), pin.get("flags")),
            (live["CANONICAL"], NDB.METRIC_REV, live["DEFAULT_FLAGS"]),
            "the near-miss DB was last re-scored under a different evaluator "
            "(compiler, metric revision, or build flags), so every stored divergence "
            "is stale relative to the live one. Run "
            "`python tools/nearmiss_db.py reeval` on a main-tip checkout and commit "
            "db.jsonl + eval_pin.json together with this change.")


if __name__ == "__main__":
    unittest.main()
