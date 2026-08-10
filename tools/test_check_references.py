"""Regression tests for address-keyed unresolved-reference tracking."""
import json
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import check_references as C  # noqa: E402


class AddressIdentityTests(unittest.TestCase):
    def _current(self, rows):
        with tempfile.TemporaryDirectory() as td:
            report = pathlib.Path(td) / "report.json"
            report.write_text(json.dumps(rows), encoding="utf-8")
            return C.current(report, require_fresh=False, symbols={})

    def test_unresolved_symbol_rename_to_eligible_is_a_fix(self):
        identity = "ov006:0x020e3578"
        base = C.normalise_baseline(
            {"eligible": 10, "unresolved": {"func_ov006_020e3578": ["old_ref"]}},
            {"func_ov006_020e3578": identity},
        )
        cur, eligible, reasons, _names = self._current([{
            "name": "_ZN14dScMgCurling_c13InitResourcesEv",
            "module": "ov006", "addr": "0x020e3578",
            "reason": None, "missing": [],
        }])

        bad, disguised, fixed = C.changes(cur, reasons, base)
        self.assertEqual((bad, disguised, fixed), ([], [], [identity]))
        self.assertEqual(eligible, 1)

    def test_same_address_compile_failure_is_still_disguised_regression(self):
        identity = "ov006:0x020e3578"
        base = C.normalise_baseline(
            {"eligible": 10, "unresolved": {"old": ["old_ref"]}},
            {"old": identity},
        )
        cur, _eligible, reasons, _names = self._current([{
            "name": "new", "module": "ov006", "addr": "0x020e3578",
            "reason": "compile failed", "missing": [],
        }])

        bad, disguised, fixed = C.changes(cur, reasons, base)
        self.assertEqual((bad, disguised, fixed), ([], [identity], []))

    def test_renamed_function_cannot_add_an_unresolved_reference(self):
        identity = "ov006:0x020e3578"
        base = C.normalise_baseline(
            {"eligible": 10, "unresolved": {"old": ["old_ref"]}},
            {"old": identity},
        )
        cur, _eligible, reasons, _names = self._current([{
            "name": "new", "module": "ov006", "addr": "0x020e3578",
            "reason": "unresolvable: old_ref,new_ref",
            "missing": ["old_ref", "new_ref"],
        }])

        bad, disguised, fixed = C.changes(cur, reasons, base)
        self.assertEqual(bad, [(identity, ["new_ref"], "new unresolved reference")])
        self.assertEqual((disguised, fixed), ([], []))

    def test_overlays_with_the_same_ram_address_are_distinct(self):
        old_identity = "ov006:0x020e3578"
        base = C.normalise_baseline(
            {"eligible": 10, "unresolved": {"old": ["old_ref"]}},
            {"old": old_identity},
        )
        cur, _eligible, reasons, _names = self._current([{
            "name": "new", "module": "ov007", "addr": "0x020e3578",
            "reason": None, "missing": [],
        }])

        _bad, disguised, fixed = C.changes(cur, reasons, base)
        self.assertEqual(disguised, [old_identity])
        self.assertEqual(fixed, [])

    def test_new_baseline_schema_keeps_name_as_display_metadata(self):
        identity = "ov006:0x020e3578"
        base = C.normalise_baseline({
            "eligible": 10,
            "unresolved": {
                identity: {"name": "old", "missing": ["b", "a", "a"]},
            },
        }, {})
        self.assertEqual(base["unresolved"], {identity: ["a", "b"]})
        self.assertEqual(base["names"], {identity: "old"})

    def test_symbol_identity_can_be_resolved_from_the_baseline_revision(self):
        old_repo = C.REPO
        try:
            with tempfile.TemporaryDirectory() as td:
                repo = pathlib.Path(td)
                symbols = repo / "config/arm9/overlays/ov006/symbols.txt"
                symbols.parent.mkdir(parents=True)
                subprocess.run(["git", "init", "-q"], cwd=repo, check=True)
                subprocess.run(["git", "config", "user.email", "test@example.com"],
                               cwd=repo, check=True)
                subprocess.run(["git", "config", "user.name", "Test"],
                               cwd=repo, check=True)
                symbols.write_text(
                    "old kind:function(arm,size=0x4) addr:0x020e3578\n",
                    encoding="utf-8")
                subprocess.run(["git", "add", "config"], cwd=repo, check=True)
                subprocess.run(["git", "commit", "-qm", "old name"],
                               cwd=repo, check=True)
                symbols.write_text(
                    "new kind:function(arm,size=0x4) addr:0x020e3578\n",
                    encoding="utf-8")
                subprocess.run(["git", "commit", "-qam", "new name"],
                               cwd=repo, check=True)

                C.REPO = repo
                identity = "ov006:0x020e3578"
                self.assertEqual(C.symbol_identities("HEAD~1")["old"], identity)
                self.assertEqual(C.symbol_identities()["new"], identity)
        finally:
            C.REPO = old_repo


if __name__ == "__main__":
    unittest.main()
