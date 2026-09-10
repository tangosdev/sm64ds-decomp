"""prepush_linkcheck.py: resolving a src file to the function(s) it verifies.

WHY THIS EXISTS
---------------
`verify(path)` used to derive exactly one function name from `pathlib.Path(path).stem`
and hand back one verdict. That is correct for a legacy one-function source, where the
filename IS the symbol, but a consolidated translation unit -- a merged multi-function
`.cpp`, or a hand-authored actor file whose name is not any symbol it defines -- has no
symbol named after its stem at all. The old code read that as "not found" and printed a
non-blocking NO-SYM, so a consolidated TU verified NOTHING: not one of its functions
ever reached linkcheck.py, so a genuinely wrong relocation inside one could sit on main
forever with the gate reporting only a coverage warning about it.

`verify` now resolves through `srcpath.symbols_for`, the same enrolment table (
config/**/delinks.txt's address range for a file, matched against config/**/symbols.txt)
`dsd` itself reads to link the ROM, and returns one row per function that table
attributes to the file. A legacy one-function source still owns exactly its own stem --
`symbols_for` falls back to the filename for anything not enrolled -- so this is a
superset of the old behaviour and not a replacement for it; that is asserted below by
diffing a real single-function verdict before and after monkeypatching does nothing.

These tests never shell out to linkcheck.py or the compiler: `_load_symbol` and
`_run_linkcheck` are monkeypatched to canned answers, and `srcpath.symbols_for` is
monkeypatched to canned per-file function lists -- the fixture for "a consolidated TU"
the resolver has to handle. No extracted/ ROM, no mwccarm, no elftools import; this
module is stdlib-only and needs none of the toolchain tool-tests.yml cannot wire in.
"""
import pathlib
import sys
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import prepush_linkcheck as PL  # noqa: E402


def _sym_table(table):
    """A `_load_symbol` stand-in: name -> (module, addr, size), None for anything else."""
    def fn(name):
        return table.get(name)
    return fn


def _linkcheck_table(table):
    """A `_run_linkcheck` stand-in: (module, name, addr, size) -> (verdict, blind, diffs)."""
    def fn(module, name, addr, size):
        return table.get(name, ("ERROR", 0, []))
    return fn


class Verify(unittest.TestCase):
    """`verify(path)` -- the resolver, isolated from linkcheck.py and the compiler."""

    def test_legacy_one_function_source_is_still_one_row(self):
        with mock.patch.object(PL.srcpath, "symbols_for", return_value=["func_0205a61c"]), \
             mock.patch.object(PL, "_load_symbol",
                               _sym_table({"func_0205a61c": ("arm9", 0x0205a61c, 0x40)})), \
             mock.patch.object(PL, "_run_linkcheck",
                               _linkcheck_table({"func_0205a61c": ("VERIFIED", 0, [])})):
            rows = PL.verify("src/func_0205a61c.c")
        self.assertEqual(rows, [{"file": "src/func_0205a61c.c", "name": "func_0205a61c",
                                  "module": "arm9", "addr": "0x205a61c",
                                  "verdict": "VERIFIED", "blind": 0, "diffs": []}])

    def test_consolidated_tu_reports_one_row_per_owned_function(self):
        """The case this whole change exists for: a merged TU's stem names nothing, but
        the enrolment table lists every function it owns, in ROM order."""
        owned = ["_ZN18dScMgTrampoline2_cD1Ev", "_ZN18dScMgTrampoline2_cD0Ev",
                 "func_ov006_021227c8"]
        syms = {"_ZN18dScMgTrampoline2_cD1Ev": ("ov006", 0x021225ac, 0x104),
                "_ZN18dScMgTrampoline2_cD0Ev": ("ov006", 0x021226b0, 0x118),
                "func_ov006_021227c8": ("ov006", 0x021227c8, 0x4c)}
        verdicts = {"_ZN18dScMgTrampoline2_cD1Ev": ("VERIFIED", 0, []),
                    "_ZN18dScMgTrampoline2_cD0Ev": ("BLIND-1", 1, []),
                    "func_ov006_021227c8": ("WRONG", 0, [{"off": "+0x10", "sym": "x"}])}
        with mock.patch.object(PL.srcpath, "symbols_for", return_value=owned), \
             mock.patch.object(PL, "_load_symbol", _sym_table(syms)), \
             mock.patch.object(PL, "_run_linkcheck", _linkcheck_table(verdicts)):
            rows = PL.verify("src/minigames/d_s_mg_trampoline2.cpp")
        self.assertEqual(len(rows), 3)
        self.assertEqual([r["name"] for r in rows], owned)
        self.assertTrue(all(r["file"] == "src/minigames/d_s_mg_trampoline2.cpp" for r in rows))
        self.assertEqual([r["verdict"] for r in rows], ["VERIFIED", "BLIND-1", "WRONG"])
        # the WRONG row is the whole point: it used to be invisible inside a file-level
        # NO-SYM and must now carry its diff through untouched.
        self.assertEqual(rows[2]["diffs"], [{"off": "+0x10", "sym": "x"}])

    def test_one_owned_function_with_no_symbol_entry_stays_no_sym_the_rest_do_not(self):
        """A compiler-emitted passenger (a this-adjusting thunk, a weak dtor copy) can be
        in the enrolment table's function list without a symbols.txt entry of its own.
        That one row is NO-SYM; it must not drag its siblings down with it."""
        owned = ["_ZN7Some_cD1Ev", "_ZThn8_N7Some_c4CallEv"]
        syms = {"_ZN7Some_cD1Ev": ("arm9", 0x02100000, 0x40)}  # thunk absent on purpose
        verdicts = {"_ZN7Some_cD1Ev": ("VERIFIED", 0, [])}
        with mock.patch.object(PL.srcpath, "symbols_for", return_value=owned), \
             mock.patch.object(PL, "_load_symbol", _sym_table(syms)), \
             mock.patch.object(PL, "_run_linkcheck", _linkcheck_table(verdicts)):
            rows = PL.verify("src/_ZN7Some_cD1Ev.cpp")
        by_name = {r["name"]: r for r in rows}
        self.assertEqual(by_name["_ZN7Some_cD1Ev"]["verdict"], "VERIFIED")
        self.assertEqual(by_name["_ZThn8_N7Some_c4CallEv"]["verdict"], "NO-SYM")

    def test_unenrolled_source_still_falls_back_to_the_filename(self):
        """`symbols_for` itself owns the enrolled/unenrolled distinction (test_srcpath.py
        pins that); this only has to prove `verify` does not add a second one function
        list on top of it. A single-element fallback list behaves exactly like the old
        stem lookup."""
        with mock.patch.object(PL.srcpath, "symbols_for", return_value=["func_0209aaaa"]), \
             mock.patch.object(PL, "_load_symbol", _sym_table({})):
            rows = PL.verify("src/func_0209aaaa.c")
        self.assertEqual(rows, [{"file": "src/func_0209aaaa.c", "name": "func_0209aaaa",
                                  "verdict": "NO-SYM", "note": "no symbol entry"}])

    def test_itcm_housed_symbol_resolves_through_the_real_load_symbol(self):
        """`_load_symbol` is intentionally left UNMOCKED here -- every other test in this
        class stands in for it, which proves the row-per-function plumbing but never
        exercises the real resolver (stamp_provenance.load_symbol -> relocs.module_universe)
        this gate actually ships. That resolver used to walk config/ with a regex that only
        recognized a directory named arm9, arm7, or arm9/overlays/ovNNN, so every ITCM-
        housed symbol -- OSReadROMArea real one among them -- read NO-SYM here regardless
        of `symbols_for`, because the module lookup itself, not the file-to-function
        mapping, was blind to config/arm9/itcm/symbols.txt. Only `_run_linkcheck` (the
        compiler/linker subprocess) is mocked; symbol resolution reads the real, committed
        config/arm9/itcm/symbols.txt, so this is stdlib-only and needs no toolchain."""
        with mock.patch.object(PL.srcpath, "symbols_for", return_value=["OSReadROMArea"]), \
             mock.patch.object(PL, "_run_linkcheck",
                               _linkcheck_table({"OSReadROMArea": ("VERIFIED", 0, [])})):
            rows = PL.verify("src/OSReadROMArea.c")
        self.assertEqual(rows, [{"file": "src/OSReadROMArea.c", "name": "OSReadROMArea",
                                  "module": "itcm", "addr": "0x1ffdbd8",
                                  "verdict": "VERIFIED", "blind": 0, "diffs": []}])

    def test_error_is_retried_once_per_function_not_once_per_file(self):
        calls = {"n": 0}

        def flaky(module, name, addr, size):
            calls["n"] += 1
            return ("ERROR", 0, []) if calls["n"] == 1 else ("VERIFIED", 0, [])

        with mock.patch.object(PL.srcpath, "symbols_for", return_value=["func_0205a61c"]), \
             mock.patch.object(PL, "_load_symbol",
                               _sym_table({"func_0205a61c": ("arm9", 0x0205a61c, 0x40)})), \
             mock.patch.object(PL, "_run_linkcheck", side_effect=flaky):
            rows = PL.verify("src/func_0205a61c.c")
        self.assertEqual(calls["n"], 2)
        self.assertEqual(rows[0]["verdict"], "VERIFIED")


class MainAggregation(unittest.TestCase):
    """`main()`'s blocking/warning split and reporting, once `verify` returns several
    rows for one file. `verify` itself is mocked here -- these tests are about what
    `main` does with a multi-row result, not about the resolver again."""

    def _run(self, file_rows, files):
        """file_rows: {path: [row, ...]}. Runs main() with --files=files, --json to a
        temp path, monkeypatching verify/is_draft so nothing touches disk or a
        subprocess. Returns (exit_code, stdout_text, json_report)."""
        import argparse
        import io
        import json
        import tempfile

        def fake_verify(path):
            return file_rows[path]

        out_path = tempfile.mktemp(suffix=".json")
        argv = ["prepush_linkcheck.py", "--files", *files, "--json", out_path]
        buf = io.StringIO()
        with mock.patch.object(PL, "verify", side_effect=fake_verify), \
             mock.patch.object(PL, "is_draft", return_value=False), \
             mock.patch.object(sys, "argv", argv), \
             mock.patch("sys.stdout", buf), \
             mock.patch("sys.stderr", buf):
            code = PL.main()
        report = json.loads(pathlib.Path(out_path).read_text())
        pathlib.Path(out_path).unlink(missing_ok=True)
        return code, buf.getvalue(), report

    def test_all_functions_verified_is_clean(self):
        rows = {"src/_fixture_tu.cpp": [
            {"file": "src/_fixture_tu.cpp", "name": "a", "verdict": "VERIFIED", "blind": 0, "diffs": []},
            {"file": "src/_fixture_tu.cpp", "name": "b", "verdict": "VERIFIED", "blind": 0, "diffs": []},
        ]}
        code, out, report = self._run(rows, ["src/_fixture_tu.cpp"])
        self.assertEqual(code, 0)
        self.assertEqual(len(report), 2)
        self.assertIn("2 functions", out)

    def test_one_wrong_function_inside_a_consolidated_file_blocks_the_push(self):
        """The exact failure mode the old file-level NO-SYM hid: a false match sitting
        inside a TU whose own filename never resolved to any symbol at all."""
        rows = {"src/_fixture_tu.cpp": [
            {"file": "src/_fixture_tu.cpp", "name": "a", "verdict": "VERIFIED", "blind": 0, "diffs": []},
            {"file": "src/_fixture_tu.cpp", "name": "b", "verdict": "WRONG", "blind": 0,
             "diffs": [{"off": "+0x4", "sym": "bad_callee"}]},
            {"file": "src/_fixture_tu.cpp", "name": "c", "verdict": "BLIND-2", "blind": 2, "diffs": []},
        ]}
        code, out, report = self._run(rows, ["src/_fixture_tu.cpp"])
        self.assertEqual(code, 1)
        self.assertEqual(len(report), 3)
        self.assertIn("PUSH BLOCKED", out)
        self.assertIn("b (WRONG)", out)
        # the clean and the coverage-warned siblings must not be swept into the block
        self.assertNotIn("a (", out)
        self.assertNotIn("c (", out)

    def test_a_single_row_file_gets_no_function_count_header(self):
        rows = {"src/func_0205a61c.c": [
            {"file": "src/func_0205a61c.c", "name": "func_0205a61c", "verdict": "VERIFIED",
             "blind": 0, "diffs": []},
        ]}
        code, out, _report = self._run(rows, ["src/func_0205a61c.c"])
        self.assertEqual(code, 0)
        self.assertNotIn("functions)", out)

    def test_json_report_is_flat_one_entry_per_function_not_per_file(self):
        rows = {
            "src/_fixture_tu.cpp": [
                {"file": "src/_fixture_tu.cpp", "name": "a", "verdict": "VERIFIED", "blind": 0, "diffs": []},
                {"file": "src/_fixture_tu.cpp", "name": "b", "verdict": "VERIFIED", "blind": 0, "diffs": []},
            ],
            "src/func_0205a61c.c": [
                {"file": "src/func_0205a61c.c", "name": "func_0205a61c", "verdict": "VERIFIED",
                 "blind": 0, "diffs": []},
            ],
        }
        code, _out, report = self._run(rows, list(rows.keys()))
        self.assertEqual(code, 0)
        self.assertEqual(len(report), 3)
        self.assertEqual({r["name"] for r in report}, {"a", "b", "func_0205a61c"})


if __name__ == "__main__":
    unittest.main()
