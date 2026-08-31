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
    def test_duplicate_request_refuses_before_touching_baseline_or_config(self):
        with self.assertRaisesRegex(TP.ProductionTuError, "duplicate"):
            TP.prepare(["ov002/Thing", "ov002/Thing"], "unused", "unused")

    def test_manifest_entry_requires_partitioned_link_verified(self):
        data = {"entries": [{"id": "ov002/Thing",
                              "partitioned_link": {"state": "attempted"}}]}
        with self.assertRaisesRegex(TP.ProductionTuError,
                                    "partitioned-link-verified"):
            TP._entry(data, "ov002/Thing")

    def test_missing_content_bound_baseline_refuses(self):
        with tempfile.TemporaryDirectory() as td, \
                mock.patch.object(TP.TB, "BASELINE_LINK", pathlib.Path(td)):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                        "missing strict stock control"):
                TP._strict_baseline()

    def test_clean_worker_bootstraps_rom_gap_control(self):
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing", "module": "ov047"}}
        baseline = {"symbolErrors": [], "romSha256": "ab" * 32}
        with mock.patch.object(
                TP, "_strict_baseline",
                side_effect=[TP.ProductionTuError("missing"), baseline]) as strict, \
                mock.patch.object(TP.subprocess, "run",
                                  return_value=mock.Mock(returncode=0)) as run:
            self.assertIs(
                TP._current_or_bootstrapped_intact_baseline(entries, 7), baseline)

        self.assertEqual(strict.call_count, 2)
        self.assertEqual(strict.call_args_list,
                         [mock.call(entries), mock.call(entries)])
        command = run.call_args.args[0]
        self.assertIn("--baseline", command)
        self.assertIn("ov047", command)
        self.assertIn("7", command)
        self.assertIs(run.call_args.kwargs["check"], False)

    def test_clean_worker_refuses_failed_control_bootstrap(self):
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing", "module": "ov047"}}
        with mock.patch.object(
                TP, "_strict_baseline",
                side_effect=TP.ProductionTuError("missing")), \
                mock.patch.object(TP.subprocess, "run",
                                  return_value=mock.Mock(returncode=3)):
            with self.assertRaisesRegex(
                    TP.ProductionTuError, "baseline command exited 3"):
                TP._current_or_bootstrapped_intact_baseline(entries, 7)

    def test_rom_gap_control_requires_exact_intact_inventory(self):
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing", "module": "ov047"}}
        with tempfile.TemporaryDirectory() as td:
            root = pathlib.Path(td)
            (root / "final_link.o").write_bytes(b"independent linked control")
            report = {
                "baseline": True,
                "analysis": {
                    "passed": True,
                    "moduleFidelity": {"moduleSetSha256": "ef" * 32},
                },
                "phases": {
                    "checkModules": {"ok": True},
                    "checkSymbols": {"errors": ["[ERROR] old"]},
                },
                "intactTusDemoted": [{
                    "id": "ov047/Thing", "source": "src/actors/Thing.cpp"}],
                "rom": {"sha256": "ab" * 32},
            }
            (root / "linkcheck.json").write_text(
                json.dumps(report), encoding="utf-8")
            with mock.patch.object(TP.TB, "BASELINE_LINK", root), \
                    mock.patch.object(
                        TP.TB, "validate_partition_baseline_evidence",
                        return_value=("cd" * 32, None)):
                baseline = TP._strict_baseline(entries)
                self.assertEqual(baseline["romSha256"], "ab" * 32)
                self.assertEqual(baseline["moduleSetSha256"], "ef" * 32)
                self.assertIsNone(baseline["matchesStockRom"])
                with self.assertRaisesRegex(
                        TP.ProductionTuError, "current intact TU inventory"):
                    TP._strict_baseline({
                        "src/actors/Other.cpp": {
                            "id": "ov047/Other", "module": "ov047"}})


class ProductionTuObjects(unittest.TestCase):
    def test_automatic_intact_link_plan_uses_current_control_and_vtable_biases(self):
        claims = [{"name": ".text", "start": 0x1000, "end": 0x1010},
                  {"name": ".data", "start": 0x2000, "end": 0x2010}]
        baseline = {
            "symbolErrors": ["[ERROR] old"],
            "romSha256": "ab" * 32,
            "matchesStockRom": True,
            "moduleSetSha256": "ef" * 32,
        }
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": ["[ERROR] old"],
                "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline), \
                mock.patch.object(TP.TB, "manifest_section_claims",
                                  return_value=(claims, [])), \
                mock.patch.object(TP.TB, "partition_vtable_rebiases",
                                  return_value=({"_ZTV1T": {"bias": 8}}, [])):
            prepared = TP.prepare_intact_link_verification(entries)
        self.assertIs(prepared["baseline"], baseline)
        self.assertEqual(prepared["admittedRomSha256"], ["ab" * 32])
        self.assertEqual(prepared["admittedModuleSetSha256"], "ef" * 32)
        self.assertEqual(prepared["entries"], [{
            "id": "ov047/Thing", "source": "src/actors/Thing.cpp",
            "biases": {"_ZTV1T": {"bias": 8}},
        }])

    def test_automatic_intact_link_plan_rejects_laundered_control_error(self):
        baseline = {"symbolErrors": ["[ERROR] old", "[ERROR] new"],
                    "romSha256": "ab" * 32, "matchesStockRom": True,
                    "moduleSetSha256": "ef" * 32}
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": ["[ERROR] old"],
                "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                         "pre-promotion inventory"):
                TP.prepare_intact_link_verification(entries)

    def test_same_worker_stock_control_allows_environment_specific_outer_rom(self):
        baseline = {
            "symbolErrors": [], "romSha256": "cd" * 32,
            "matchesStockRom": True, "moduleSetSha256": "ef" * 32,
        }
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": [], "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline), \
                mock.patch.object(TP.TB, "manifest_section_claims",
                                  return_value=([], [])), \
                mock.patch.object(TP.TB, "partition_vtable_rebiases",
                                  return_value=({}, [])):
            prepared = TP.prepare_intact_link_verification(entries)
        self.assertEqual(prepared["baseline"]["romSha256"], "cd" * 32)
        self.assertEqual(prepared["admittedRomSha256"], ["ab" * 32])

    def test_environment_specific_control_without_stock_comparison_refuses(self):
        baseline = {
            "symbolErrors": [], "romSha256": "cd" * 32,
            "matchesStockRom": None, "moduleSetSha256": "ef" * 32,
        }
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": [], "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                        "no same-worker stock-ROM comparison"):
                TP.prepare_intact_link_verification(entries)

    def test_same_worker_stock_control_still_requires_admitted_module_set(self):
        baseline = {
            "symbolErrors": [], "romSha256": "cd" * 32,
            "matchesStockRom": True, "moduleSetSha256": "12" * 32,
        }
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": [], "moduleSetSha256": "ef" * 32,
                "rom": {"sha256": "ab" * 32}}},
        }}
        with mock.patch.object(TP, "_strict_baseline", return_value=baseline):
            with self.assertRaisesRegex(TP.ProductionTuError,
                                        "executable-module fingerprint"):
                TP.prepare_intact_link_verification(entries)

    def test_admitted_intact_proof_requires_module_set_fingerprint(self):
        entries = {"src/actors/Thing.cpp": {
            "id": "ov047/Thing",
            "verification": {"linkcheck": {
                "symbolCheckErrors": [],
                "rom": {"sha256": "ab" * 32}}},
        }}
        with self.assertRaisesRegex(TP.ProductionTuError,
                                    "executable-module fingerprint"):
            TP.prepare_intact_link_verification(entries)

    def test_intact_object_runs_all_fail_closed_policy_gates(self):
        entry = {"id": "ov047/Thing"}
        claims = [{"name": ".text", "start": 0x1000, "end": 0x1010},
                  {"name": ".data", "start": 0x2000, "end": 0x2010}]
        owned = {"ok": True, "rows": [], "errors": []}
        with mock.patch.object(TP.TB, "manifest_section_claims",
                               return_value=(claims, [])), \
                mock.patch.object(TP.TB, "apply_compiler_only_policy",
                                  return_value=(b"compiler", {"deadstripped": []}, [])), \
                mock.patch.object(TP.TB, "apply_externalized_output_policy",
                                  return_value=(b"external", {"externalized": []}, [])), \
                mock.patch.object(TP.TB, "verify_owned_sections",
                                  side_effect=[owned, owned]) as verify, \
                mock.patch.object(TP.TB, "partition_vtable_rebiases",
                                  return_value=({"_ZTV1T": {"bias": 8}}, [])), \
                mock.patch.object(TP.TB.OI, "rebias_object_symbols",
                                  return_value=(b"linked", {"error": None})) as rebias, \
                mock.patch.object(TP.TB, "complete_ranges", return_value={}), \
                mock.patch.object(TP.TB, "audit_tu_object",
                                  return_value=([], [], [], True)), \
                mock.patch.object(TP.TB, "object_audit_refusals", return_value=[]):
            output, evidence = TP.prepare_intact_object(b"raw", entry)
        self.assertEqual(output, b"linked")
        rebias.assert_called_once_with(
            b"external", {"_ZTV1T": {"bias": 8}}, normalize_undefined=True)
        self.assertEqual(verify.call_args_list, [
            mock.call(b"external", entry, claims),
            mock.call(b"linked", entry, claims, public_address_points=True,
                      normalized_undefined_vtables=True),
        ])
        self.assertEqual(evidence["sha256"],
                         __import__("hashlib").sha256(b"linked").hexdigest())

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
            (True, "[ERROR] old\nError: Some symbol(s) did not match.", 0.1),
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

    def test_final_gate_rejects_symbol_check_operational_failure(self):
        prepared = {"baseline": {"symbolErrors": []}, "entries": []}
        calls = [(True, "modules ok", 0.1),
                 (False, "tool crashed before producing an inventory", 0.1)]
        with mock.patch.object(TP.TB, "_run_dsd", side_effect=calls):
            result = TP.verify_link("config.yaml", "final_link.o", prepared)
        self.assertFalse(result["ok"])
        self.assertFalse(result["symbolsCommandOk"])


if __name__ == "__main__":
    unittest.main()
