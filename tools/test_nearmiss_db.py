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

    def write_rows(self, *rows):
        NDB.DB.write_text("".join(json.dumps(r) + "\n" for r in rows), encoding="utf-8")

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


if __name__ == "__main__":
    unittest.main()
