"""pgate.py: resolving a src file to the function(s) a parallel sweep verifies.

WHY THIS EXISTS
---------------
pgate.py is prepush_linkcheck.py's threaded twin -- same verdicts, run across a thread
pool instead of one file at a time for a 100-200 file readability sweep. It carried the
identical stem bug independently: `name = pathlib.Path(f).stem` then one `load_symbol`
lookup, so a consolidated multi-function TU or a convention-named file (its own stem
naming none of its functions) resolved to a single NO-SYM row and none of its functions
ever reached linkcheck.py -- the same blind spot LINKCOV closed in prepush_linkcheck.py,
independently present here because the two tools never shared the resolution step.

pgate.py now resolves through `srcpath.symbols_for`, imported rather than reimplemented
-- the same enrolment table (config/**/delinks.txt's address range for a file, matched
against config/**/symbols.txt) prepush_linkcheck.py's own `verify` uses. A legacy
one-function source still owns exactly its own stem, since `symbols_for` falls back to
the filename for anything not enrolled, so this is a superset of the old behaviour and
not a replacement for it -- proven below by a single-function fixture behaving exactly
as the old stem lookup did. The BLOCKING split (WRONG / NO-REPRO) is unchanged; only the
population `main()` walks per file widens from one job to one job per owned function.

These tests never shell out to linkcheck.py or the compiler: `load_symbol_fn` and
`run_linkcheck` are monkeypatched to canned answers, and `srcpath.symbols_for` is
monkeypatched to canned per-file function lists -- the consolidated-TU fixture is the
same one test_prepush_linkcheck.py uses, so the two gates are proven to resolve the
identical population from the identical table. No extracted/ ROM, no mwccarm, no
elftools import; this module is stdlib-only and needs none of the toolchain
tool-tests.yml cannot wire in.
"""
import json
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import pgate as PG  # noqa: E402


def _sym_table(table):
    """A `load_symbol_fn()` stand-in: name -> (module, addr, size), None for anything else."""
    def fn(name):
        return table.get(name)
    return fn


def _linkcheck_table(table):
    """A `run_linkcheck` stand-in: (module, name, addr, size) -> (verdict, blind)."""
    def fn(module, name, addr, size):
        return table.get(name, ("ERROR", 0))
    return fn


class Main(unittest.TestCase):
    """`main()` end to end, with `srcpath.symbols_for` / `load_symbol_fn` / `run_linkcheck`
    mocked so nothing touches disk, the compiler, or a real subprocess. Every case here
    has a direct counterpart in test_prepush_linkcheck.py's `Verify` class -- same
    fixtures, same resolver, the two gates proven to agree."""

    def _run(self, files, symbols_for, syms, verdicts, jobs=4):
        out_path = tempfile.mktemp(suffix=".json")
        argv = ["pgate.py", "--out", out_path, "--jobs", str(jobs), *files]
        with mock.patch.object(PG.srcpath, "symbols_for", side_effect=symbols_for), \
             mock.patch.object(PG, "load_symbol_fn", return_value=_sym_table(syms)), \
             mock.patch.object(PG, "run_linkcheck", side_effect=_linkcheck_table(verdicts)), \
             mock.patch.object(sys, "argv", argv):
            code = PG.main()
        rows = json.loads(pathlib.Path(out_path).read_text())
        pathlib.Path(out_path).unlink(missing_ok=True)
        return code, rows

    def test_legacy_one_function_source_resolves_to_its_own_stem(self):
        code, rows = self._run(
            ["src/func_0205a61c.c"],
            symbols_for=lambda f: ["func_0205a61c"],
            syms={"func_0205a61c": ("arm9", 0x0205a61c, 0x40)},
            verdicts={"func_0205a61c": ("VERIFIED", 0)},
        )
        self.assertEqual(code, 0)
        self.assertEqual(rows, [{"file": "src/func_0205a61c.c", "name": "func_0205a61c",
                                  "module": "arm9", "addr": "0x205a61c",
                                  "verdict": "VERIFIED", "blind": 0}])

    def test_itcm_housed_symbol_resolves_through_the_real_load_symbol(self):
        """`load_symbol_fn()` is intentionally left UNMOCKED here -- every other test in
        this class stands in for it, which proves the per-function job dispatch but never
        exercises the real resolver (stamp_provenance.load_symbol -> relocs.module_universe)
        this gate actually ships. That resolver used to walk config/ with a regex that only
        recognized a directory named arm9, arm7, or arm9/overlays/ovNNN, so every
        ITCM-housed symbol -- OSReadROMArea, a real one -- read NO-SYM here regardless of
        `symbols_for`, because the module lookup itself, not the file-to-function mapping,
        was blind to config/arm9/itcm/symbols.txt. Only `run_linkcheck` (the compiler/
        linker subprocess) is mocked; symbol resolution reads the real, committed
        config/arm9/itcm/symbols.txt, so this is stdlib-only and needs no toolchain --
        same fixture as prepush_linkcheck.py's twin of this test, proving the two gates
        resolve the real ITCM symbol identically, not just the mocked ones."""
        out_path = tempfile.mktemp(suffix=".json")
        argv = ["pgate.py", "--out", out_path, "--jobs", "1", "src/OSReadROMArea.c"]
        with mock.patch.object(PG.srcpath, "symbols_for", return_value=["OSReadROMArea"]), \
             mock.patch.object(PG, "run_linkcheck",
                               _linkcheck_table({"OSReadROMArea": ("VERIFIED", 0)})), \
             mock.patch.object(sys, "argv", argv):
            code = PG.main()
        rows = json.loads(pathlib.Path(out_path).read_text())
        pathlib.Path(out_path).unlink(missing_ok=True)
        self.assertEqual(rows, [{"file": "src/OSReadROMArea.c", "name": "OSReadROMArea",
                                  "module": "itcm", "addr": "0x1ffdbd8",
                                  "verdict": "VERIFIED", "blind": 0}])
        self.assertEqual(code, 0)

    def test_consolidated_tu_gets_one_job_per_owned_function(self):
        """The case this whole change exists for: a merged TU's stem names nothing, but
        the enrolment table lists every function it owns. Same fixture as
        test_prepush_linkcheck.py's consolidated-TU test, proving pgate resolves the
        identical population."""
        owned = ["_ZN18dScMgTrampoline2_cD1Ev", "_ZN18dScMgTrampoline2_cD0Ev",
                 "func_ov006_021227c8"]
        syms = {"_ZN18dScMgTrampoline2_cD1Ev": ("ov006", 0x021225ac, 0x104),
                "_ZN18dScMgTrampoline2_cD0Ev": ("ov006", 0x021226b0, 0x118),
                "func_ov006_021227c8": ("ov006", 0x021227c8, 0x4c)}
        verdicts = {"_ZN18dScMgTrampoline2_cD1Ev": ("VERIFIED", 0),
                    "_ZN18dScMgTrampoline2_cD0Ev": ("BLIND-1", 1),
                    "func_ov006_021227c8": ("WRONG", 0)}
        code, rows = self._run(
            ["src/minigames/d_s_mg_trampoline2.cpp"],
            symbols_for=lambda f: owned,
            syms=syms, verdicts=verdicts,
        )
        self.assertEqual(len(rows), 3)
        self.assertEqual({r["name"] for r in rows}, set(owned))
        self.assertTrue(all(r["file"] == "src/minigames/d_s_mg_trampoline2.cpp" for r in rows))
        by_name = {r["name"]: r for r in rows}
        self.assertEqual(by_name["_ZN18dScMgTrampoline2_cD1Ev"]["verdict"], "VERIFIED")
        self.assertEqual(by_name["_ZN18dScMgTrampoline2_cD0Ev"]["verdict"], "BLIND-1")
        self.assertEqual(by_name["func_ov006_021227c8"]["verdict"], "WRONG")
        # the WRONG row is the whole point: it used to be invisible inside a file-level
        # NO-SYM and must now block the exit code.
        self.assertEqual(code, 1)

    def test_one_owned_function_with_no_symbol_entry_stays_no_sym_the_rest_do_not(self):
        """A compiler-emitted passenger can be in the enrolment table's function list
        without a symbols.txt entry of its own. That one row is NO-SYM; it must not drag
        its siblings down with it, and must not disappear the way a file-level NO-SYM
        used to swallow the whole file."""
        owned = ["_ZN7Some_cD1Ev", "_ZThn8_N7Some_c4CallEv"]
        syms = {"_ZN7Some_cD1Ev": ("arm9", 0x02100000, 0x40)}  # thunk absent on purpose
        verdicts = {"_ZN7Some_cD1Ev": ("VERIFIED", 0)}
        code, rows = self._run(
            ["src/_ZN7Some_cD1Ev.cpp"],
            symbols_for=lambda f: owned,
            syms=syms, verdicts=verdicts,
        )
        by_name = {r["name"]: r for r in rows}
        self.assertEqual(by_name["_ZN7Some_cD1Ev"]["verdict"], "VERIFIED")
        self.assertEqual(by_name["_ZThn8_N7Some_c4CallEv"]["verdict"], "NO-SYM")
        self.assertEqual(code, 0)

    def test_unenrolled_source_still_falls_back_to_the_filename(self):
        """`symbols_for` itself owns the enrolled/unenrolled distinction; this only has
        to prove `main` does not add a second one-function list on top of it. A
        single-element fallback list behaves exactly like the old stem lookup."""
        code, rows = self._run(
            ["src/func_0209aaaa.c"],
            symbols_for=lambda f: ["func_0209aaaa"],
            syms={},
            verdicts={},
        )
        self.assertEqual(rows, [{"file": "src/func_0209aaaa.c", "name": "func_0209aaaa",
                                  "verdict": "NO-SYM"}])
        self.assertEqual(code, 0)

    def test_error_is_retried_once_per_function(self):
        calls = {"n": 0}

        def flaky(module, name, addr, size):
            calls["n"] += 1
            return ("ERROR", 0) if calls["n"] == 1 else ("VERIFIED", 0)

        out_path = tempfile.mktemp(suffix=".json")
        argv = ["pgate.py", "--out", out_path, "--jobs", "1", "src/func_0205a61c.c"]
        with mock.patch.object(PG.srcpath, "symbols_for", return_value=["func_0205a61c"]), \
             mock.patch.object(PG, "load_symbol_fn",
                                return_value=_sym_table({"func_0205a61c": ("arm9", 0x0205a61c, 0x40)})), \
             mock.patch.object(PG, "run_linkcheck", side_effect=flaky), \
             mock.patch.object(sys, "argv", argv):
            code = PG.main()
        rows = json.loads(pathlib.Path(out_path).read_text())
        pathlib.Path(out_path).unlink(missing_ok=True)
        self.assertEqual(calls["n"], 2)
        self.assertEqual(rows[0]["verdict"], "VERIFIED")
        self.assertEqual(code, 0)

    def test_multiple_files_each_resolve_through_the_shared_table(self):
        """Two files, one legacy and one consolidated, in the same sweep -- proves the
        per-file loop in main() asks `symbols_for` for each file independently rather
        than resolving the whole batch off one name."""
        owned_map = {
            "src/func_0205a61c.c": ["func_0205a61c"],
            "src/minigames/d_s_mg_trampoline2.cpp": ["_ZN18dScMgTrampoline2_cD1Ev",
                                                       "func_ov006_021227c8"],
        }
        syms = {"func_0205a61c": ("arm9", 0x0205a61c, 0x40),
                "_ZN18dScMgTrampoline2_cD1Ev": ("ov006", 0x021225ac, 0x104),
                "func_ov006_021227c8": ("ov006", 0x021227c8, 0x4c)}
        verdicts = {"func_0205a61c": ("VERIFIED", 0),
                    "_ZN18dScMgTrampoline2_cD1Ev": ("VERIFIED", 0),
                    "func_ov006_021227c8": ("VERIFIED", 0)}
        code, rows = self._run(
            list(owned_map.keys()),
            symbols_for=lambda f: owned_map[f],
            syms=syms, verdicts=verdicts,
        )
        self.assertEqual(len(rows), 3)
        self.assertEqual({r["name"] for r in rows},
                          {"func_0205a61c", "_ZN18dScMgTrampoline2_cD1Ev", "func_ov006_021227c8"})
        self.assertEqual(code, 0)


if __name__ == "__main__":
    unittest.main()
