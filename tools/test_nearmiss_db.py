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
