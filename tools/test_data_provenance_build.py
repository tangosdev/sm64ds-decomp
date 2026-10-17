"""Build receipts must bind compilation inputs and reject stale output reuse."""
import json
import contextlib
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import data_provenance_build as PB


class DataProvenanceBuildTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.repo = pathlib.Path(self.temp.name)
        self.build = self.repo / "build"
        self.request_path = self.write("build/requests.json", "[]")
        self.write("src/example.cpp", "int counter;\n")
        self.write("include/example.h", "extern int counter;\n")
        self.write("tools/checker.py", "# tool input\n")
        self.write("config/arm9/symbols.txt", "counter kind:bss addr:0x00001004\n")
        self.delinks = self.write("config/arm9/delinks.txt", """    .data start:0x00001000 end:0x00001004 kind:data align:4
    .bss start:0x00001004 end:0x00001008 kind:bss align:4
src/example.cpp:
    complete
    .bss start:0x00001004 end:0x00001008
""")
        self.write("config/arm9/relocs.txt", "")
        self.write("extracted/dsd/arm9/arm9.bin", "abcd")
        self.write("extracted/dsd/config.yaml", "# extraction inputs\n")
        self.rows = [{"module": "arm9", "symbol": "counter",
                      "source": "src/example.cpp", "size": 4}]

    def write(self, rel, content):
        path = self.repo / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(content, encoding="utf-8")
        return path

    def prepare(self):
        self.request_path.write_text(json.dumps(self.rows), encoding="utf-8")
        with mock.patch.object(PB.DE, "repository_state", return_value={"head": "fixture", "dirty": False}):
            return PB.prepare(self.repo, self.build, self.request_path)

    def test_request_resolves_module_bounds_and_keeps_explicit_size(self):
        self.rows[0]["size"] = "0x4"
        plan = self.prepare()
        self.assertEqual(plan["requests"][0], {**self.rows[0], "size": 4,
                                               "address": 0x1004, "section": ".bss"})
        self.assertEqual(plan["module_images"]["arm9"]["base"], 0x1000)
        PB.require_unchanged(plan)

    def test_rejects_misspelled_or_injected_fields(self):
        self.rows[0]["address"] = 0x2000
        with self.assertRaisesRegex(ValueError, "exactly"):
            self.prepare()

    def test_rejects_duplicate_requests(self):
        self.rows.append(dict(self.rows[0]))
        with self.assertRaisesRegex(ValueError, "duplicate"):
            self.prepare()

    def test_rejects_boolean_zero_and_oversized_extents(self):
        for size in (True, 0, -1, 8):
            with self.subTest(size=size):
                self.rows[0]["size"] = size
                with self.assertRaises(ValueError):
                    self.prepare()

    def test_rejects_source_escape(self):
        for source in ("src/../outside.cpp", "../example.cpp", "C:/example.cpp", "."):
            with self.subTest(source=source):
                self.rows[0]["source"] = source
                with self.assertRaisesRegex(ValueError, "inside src"):
                    self.prepare()

    def test_rejects_incomplete_owner_and_competing_claim(self):
        original = self.delinks.read_text(encoding="utf-8")
        for content in (original.replace("    complete\n", ""), original + """src/other.cpp:
    complete
    .bss start:0x00001006 end:0x00001008
"""):
            self.delinks.write_text(content, encoding="utf-8")
            with self.assertRaisesRegex(ValueError, "one complete source owner"):
                self.prepare()

    def test_changed_header_or_tool_invalidates_compilation_binding(self):
        for rel in ("include/example.h", "tools/checker.py"):
            plan = self.prepare()
            self.write(rel, "changed")
            with self.assertRaisesRegex(ValueError, "inputs changed"):
                PB.require_unchanged(plan)

    def test_generated_config_edits_and_additions_invalidate_binding(self):
        plan = self.prepare()
        config = self.write("build/config/arm9/config.yaml", "# generated\n").parent
        PB.bind_generated_config(plan, config)
        self.write("build/config/arm9/extra.txt", "new input")
        with self.assertRaisesRegex(ValueError, "inputs changed"):
            PB.require_unchanged(plan)

    def test_added_and_removed_inputs_invalidate_binding(self):
        plan = self.prepare()
        added = self.write("include/new.h", "new lookup candidate")
        with self.assertRaisesRegex(ValueError, "inputs changed"):
            PB.require_unchanged(plan)
        added.unlink()
        PB.require_unchanged(plan)
        (self.repo / "src/example.cpp").unlink()
        with self.assertRaisesRegex(ValueError, "inputs changed"):
            PB.require_unchanged(plan)

    def test_changed_request_or_retail_bytes_invalidates_binding(self):
        for rel in ("build/requests.json", "extracted/dsd/arm9/arm9.bin"):
            plan = self.prepare()
            self.write(rel, "changed")
            with self.assertRaisesRegex(ValueError, "inputs changed"):
                PB.require_unchanged(plan)

    def test_outputs_are_removed_before_snapshot_and_other_files_survive(self):
        import data_provenance as DP
        plan = self.prepare()
        outputs = [self.write("build/" + name, "old")
                   for name in ("final_link.o", "final_link.o.xMAP", "build/arm9.bin")]
        keep = self.write("build/keep.txt", "keep")

        def snapshot(*args):
            self.assertTrue(all(not path.exists() for path in outputs))
            self.assertTrue(keep.exists())
            return {"snapshot": True}

        with mock.patch.object(DP, "snapshot_link_inputs", side_effect=snapshot) as capture:
            self.assertEqual(PB.before_link(plan), {"snapshot": True})
        capture.assert_called_once()

    def test_output_escape_rejected_before_any_output_is_deleted(self):
        import data_provenance as DP
        plan = self.prepare()
        output = self.write("build/final_link.o", "old")
        outside = self.write("outside.bin", "valuable")
        plan["module_images"]["arm9"]["built"] = outside
        with mock.patch.object(DP, "snapshot_link_inputs") as capture:
            with self.assertRaisesRegex(ValueError, "escapes build"):
                PB.before_link(plan)
            capture.assert_not_called()
        self.assertEqual(output.read_text(), "old")
        self.assertEqual(outside.read_text(), "valuable")

    def test_source_change_during_audit_cannot_return_pass(self):
        import data_provenance as DP
        plan = self.prepare()

        def audit(snapshot):
            self.write("src/example.cpp", "changed during audit")
            return {"ok": True, "objects": []}

        with mock.patch.object(DP, "audit_link", side_effect=audit):
            with self.assertRaisesRegex(ValueError, "inputs changed"):
                PB.after_link(plan, {})

    def test_receipt_preserves_separate_repository_and_build_bindings(self):
        import data_provenance as DP
        plan = self.prepare()
        with mock.patch.object(DP, "audit_link", return_value={"ok": True, "objects": [{}]}):
            receipt = PB.after_link(plan, {})
        self.assertEqual(receipt["repository"]["head"], "fixture")
        self.assertEqual(receipt["build_input_sha256"], plan["input_sha256"])
        self.assertIn("caller assertion", receipt["requested_size_basis"])

    def run_build_hook(self, proof, link_fails=False):
        """Exercise main's opt-in ordering and failure propagation without a ROM."""
        import rombuild as RB
        tool = self.write("tools/mwccarm/2004/b56/mwccarm.exe", "compiler")
        self.write("tools/mwccarm/1.2/sp2p3/mwldarm.exe", "linker")
        dsd = self.write("tools/bin/dsd.exe", "dsd")
        config = self.repo / "config" / "arm9"
        events = []

        def run(command, phase, **kwargs):
            if phase == "mwldarm":
                events.append("link")
                if link_fails:
                    raise RB.BuildError("mwldarm", 1, "fixture link failure")

        cache = mock.Mock(enabled=False)
        cache.summary.return_value = {}
        report_path = self.build / "report.json"
        argv = ["rombuild.py", "--no-rom", "--no-layout-check", "--no-data-check",
                "--data-provenance-requests", str(self.request_path),
                "--report-json", str(report_path)]
        with contextlib.ExitStack() as stack:
            stack.enter_context(mock.patch.object(sys, "argv", argv))
            for name, value in {"REPO": self.repo, "BUILD": self.build,
                                "MW": tool.parents[2], "DSD": dsd}.items():
                stack.enter_context(mock.patch.object(RB, name, value))
            stack.enter_context(mock.patch.object(RB.DSP, "check", return_value={
                "path": str(dsd), "version": "0.11.0", "sha256": "fixture"}))
            for name, value in {"enrolled": [], "versions": {}, "enrolled_symbols": {},
                                "init_section_sources": set(), "compiler_only_policies": {},
                                "intact_tu_policies": {}, "audit_version_pins": ([], [])}.items():
                stack.enter_context(mock.patch.object(RB, name, return_value=value))
            stack.enter_context(mock.patch.object(RB.RP, "prepare_profile", return_value={
                "configRoot": config, "configYaml": config / "config.yaml",
                "modReplacements": [], "modGapFallbacks": []}))
            stack.enter_context(mock.patch.object(RB.RBK, "ObjectCache", return_value=cache))
            stack.enter_context(mock.patch.object(RB.LI, "patch", return_value=[]))
            stack.enter_context(mock.patch.object(RB, "run", side_effect=run))
            stack.enter_context(mock.patch.object(PB, "prepare", return_value={"requests": self.rows}))
            stack.enter_context(mock.patch.object(PB, "bind_generated_config"))
            stack.enter_context(mock.patch.object(PB, "before_link", side_effect=lambda plan: events.append("snapshot")))
            audit = stack.enter_context(mock.patch.object(PB, "after_link",
                side_effect=lambda plan, snapshot: (events.append("audit") or proof)))
            analyze = stack.enter_context(mock.patch.object(RB.RBC, "analyze", return_value={"passed": True}))
            stack.enter_context(mock.patch.object(RB.RBC, "print_report"))
            stack.enter_context(mock.patch("builtins.print"))
            result = RB.main()
        return result, json.loads(report_path.read_text()), events, audit, analyze

    def test_build_refuses_a_failed_requested_proof_and_records_it(self):
        result, report, events, audit, analyze = self.run_build_hook(
            {"ok": False, "objects": [], "errors": ["ROM fallback"]})
        self.assertEqual(result, 1)
        self.assertEqual(events, ["snapshot", "link", "audit"])
        self.assertFalse(report["dataProvenance"]["ok"])
        analyze.assert_not_called()

    def test_successful_requested_proof_still_runs_full_fidelity_analysis(self):
        result, report, events, audit, analyze = self.run_build_hook({"ok": True, "objects": [{}]})
        self.assertEqual(result, 0)
        self.assertEqual(events, ["snapshot", "link", "audit"])
        self.assertEqual(report["status"], "passed")
        analyze.assert_called_once()

    def test_failed_link_cannot_produce_a_provenance_verdict(self):
        result, report, events, audit, analyze = self.run_build_hook(
            {"ok": True, "objects": [{}]}, link_fails=True)
        self.assertEqual(result, 1)
        self.assertEqual(events, ["snapshot", "link"])
        self.assertEqual(report["dataProvenance"]["status"], "not-run")
        audit.assert_not_called()
        analyze.assert_not_called()


if __name__ == "__main__":
    unittest.main()
