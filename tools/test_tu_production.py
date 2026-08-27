import json
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rombuild as RB  # noqa: E402
import tu_production as TP  # noqa: E402


class ProductionTuAdmission(unittest.TestCase):
    def test_configured_ids_are_ordered_and_fail_closed(self):
        with tempfile.TemporaryDirectory() as td:
            path = pathlib.Path(td) / "production-tus.json"
            path.write_text(json.dumps({
                "schema_version": 1,
                "partitioned_tus": ["ov002/A", "ov047/B"],
                "derived_text_tus": ["ov006/C"],
            }), encoding="utf-8")
            self.assertEqual(TP.configured_ids(path),
                             ["ov002/A", "ov047/B", "ov006/C"])
            path.write_text(json.dumps({
                "schema_version": 1,
                "partitioned_tus": ["ov002/A", "ov002/A"],
            }), encoding="utf-8")
            with self.assertRaisesRegex(TP.ProductionTuError, "duplicate"):
                TP.configured_ids(path)

    def test_duplicate_request_refuses_before_touching_baseline_or_config(self):
        with self.assertRaisesRegex(TP.ProductionTuError, "duplicate"):
            TP.prepare(["ov002/Thing", "ov002/Thing"], "unused", "unused")

    def test_manifest_entry_requires_partitioned_link_verified(self):
        data = {"entries": [{"id": "ov002/Thing",
                              "partitioned_link": {"state": "attempted"}}]}
        with self.assertRaisesRegex(TP.ProductionTuError,
                                    "partitioned-link-verified"):
            TP._entry(data, "ov002/Thing")

    def test_derived_text_entry_requires_partial_link_verified(self):
        data = {"entries": [{"id": "ov006/Thing",
                              "production_mode": "derived-text",
                              "partial_isolation": {"state": "attempted"}}]}
        with self.assertRaisesRegex(TP.ProductionTuError,
                                    "partial-link-verified"):
            TP._entry(data, "ov006/Thing")

    def test_missing_content_bound_baseline_refuses(self):
        with tempfile.TemporaryDirectory() as td, \
                mock.patch.object(TP.TB, "BASELINE_LINK", pathlib.Path(td)):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                        "missing strict stock control"):
                TP._strict_baseline()


class ProductionTuObjects(unittest.TestCase):
    def test_derived_text_discarded_output_is_exactly_inventory_bound(self):
        entry = {
            "functions": [{"symbol": "Owned"}],
            "unlicensed_output_observed": {
                "text": [{"symbol": "CompilerOnly", "size": "0x4",
                          "binding": "STB_GLOBAL"}],
                "data": [{"symbol": "_ZTI4Thing", "size": "0xc",
                          "binding": "STB_LOPROC"}],
            },
        }
        inventory = {
            "sections": [{"index": 7, "name": ".data"}],
            "symbols": [],
        }
        unlicensed = (
            [{"name": "CompilerOnly", "size": 4, "bind": "STB_GLOBAL",
              "shndx": 3}],
            [{"name": "_ZTI4Thing", "size": 12, "bind": "STB_LOPROC",
              "shndx": 7}],
            [],
        )
        with mock.patch.object(TP.TB, "elf_inventory", return_value=inventory), \
                mock.patch.object(TP.TB, "unlicensed_inventory",
                                  return_value=unlicensed):
            result = TP._derived_text_output_audit(entry, b"object")
        self.assertTrue(result["ok"])

    def test_derived_text_discarded_output_refuses_drift(self):
        inventory = {"sections": [], "symbols": []}
        unlicensed = ([{"name": "NewOutput", "size": 4,
                        "bind": "STB_GLOBAL", "shndx": 3}], [], [])
        with mock.patch.object(TP.TB, "elf_inventory", return_value=inventory), \
                mock.patch.object(TP.TB, "unlicensed_inventory",
                                  return_value=unlicensed):
            result = TP._derived_text_output_audit(
                {"functions": [], "unlicensed_output_observed": {}}, b"object")
        self.assertFalse(result["ok"])
        self.assertIn("new discarded output", result["errors"][0])

    def test_compile_one_installs_prepared_object_without_compiler(self):
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            prepared = root / "prepared.o"
            prepared.write_bytes(b"derived object")
            rel, error, outcome = RB.compile_one(
                "src/Legacy.cpp", build_root=root / "build", prebuilt=prepared)
            self.assertEqual((rel, error, outcome),
                             ("src/Legacy.cpp", None, "tu-derived"))
            self.assertEqual((root / "build/src/Legacy.o").read_bytes(),
                             b"derived object")

    def test_final_gate_allows_only_baseline_symbol_errors(self):
        prepared = {
            "baseline": {"symbolErrors": ["[ERROR] old"]},
            "entries": [{"biases": {"_ZTV1A": {"bias": 8}}}],
        }
        calls = [
            (True, "modules ok", 0.1),
            (False, "[ERROR] old\nError: Some symbol(s) did not match.", 0.1),
        ]
        with mock.patch.object(TP.TB, "_run_dsd", side_effect=calls), \
                mock.patch.object(TP.TB, "verify_linked_storage_aliases",
                                  return_value={"ok": True, "rows": [{"exact": True}],
                                                "errors": []}):
            result = TP.verify_link("config.yaml", "final_link.o", prepared)
        self.assertTrue(result["ok"])
        self.assertEqual(result["newSymbolErrors"], [])

    def test_final_gate_rejects_new_symbol_error(self):
        prepared = {"baseline": {"symbolErrors": ["[ERROR] old"]}, "entries": []}
        calls = [
            (True, "modules ok", 0.1),
            (False, "[ERROR] old\n[ERROR] new", 0.1),
        ]
        with mock.patch.object(TP.TB, "_run_dsd", side_effect=calls):
            result = TP.verify_link("config.yaml", "final_link.o", prepared)
        self.assertFalse(result["ok"])
        self.assertEqual(result["newSymbolErrors"], ["[ERROR] new"])


if __name__ == "__main__":
    unittest.main()
