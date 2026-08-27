import importlib.util
import json
import pathlib
import subprocess
import tempfile
import unittest


HERE = pathlib.Path(__file__).resolve().parent
SPEC = importlib.util.spec_from_file_location("cpp_tu_state", HERE / "cpp_tu_state.py")
STATE = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(STATE)


class CppTuStateTests(unittest.TestCase):
    @staticmethod
    def readers():
        return {
            "progress_reader": lambda: (3, 0x30, 4, 0x40),
            "converted_reader": lambda: {
                "converted": 2,
                "functions": 4,
                "source_files": 3,
                "pct": 50.0,
                "criteria": {"real_name": 3},
            },
            "compatibility_reader": lambda: {
                "productionCompatible": False,
                "allSurfacesReady": False,
                "blockers": ["rombuild"],
                "policyAndMetricGaps": ["tiers"],
                "surfaces": [
                    {"surface": "srcpath", "status": "ready"},
                    {"surface": "rombuild", "status": "blocked"},
                    {"surface": "tiers", "status": "gap"},
                ],
            },
            "require_clean_inputs": False,
        }

    def fixture(self):
        temp = tempfile.TemporaryDirectory()
        repo = pathlib.Path(temp.name)
        (repo / "src").mkdir()
        (repo / "src_tu").mkdir()
        cfg = repo / "config" / "arm9"
        cfg.mkdir(parents=True)
        (repo / "src" / "Together.cpp").write_text(
            "//cpp\nint One() { return 1; }\nint Two() { return 2; }\n",
            encoding="utf-8",
        )
        (repo / "src" / "Solo.c").write_text("int Solo(void) { return 3; }\n", encoding="utf-8")
        (repo / "src_tu" / "Future.cpp").write_text("//cpp\n", encoding="utf-8")
        (repo / "src" / "_ZN3Foo3OldEv.c").write_text(
            "int _ZN3Foo3OldEv(void *self) { return self != 0; }\n", encoding="utf-8"
        )
        (repo / "src" / "_ZN3Bar3NewEv.cpp").write_text(
            "//cpp\nstruct Bar { void New(); };\nvoid Bar::New() {}\n", encoding="utf-8"
        )
        (cfg / "symbols.txt").write_text(
            "One kind:function(arm,size=0x10) addr:0x00001000\n"
            "Two kind:function(arm,size=0x10) addr:0x00001010\n"
            "Solo kind:function(arm,size=0x10) addr:0x00001020\n",
            encoding="utf-8",
        )
        (cfg / "delinks.txt").write_text(
            ".text start:0x00001000 end:0x00001030 kind:code align:4\n\n"
            "src/Together.cpp:\n"
            "    complete\n"
            "    .text start:0x00001000 end:0x00001020\n\n"
            "src/Solo.c:\n"
            "    complete\n"
            "    .text start:0x00001020 end:0x00001030\n",
            encoding="utf-8",
        )
        manifest_d = repo / "config" / "tu_manifest.d"
        manifest_d.mkdir(parents=True, exist_ok=True)
        (manifest_d / "_meta.json").write_text(
            json.dumps({"schema_version": 1}), encoding="utf-8")
        (manifest_d / "arm9-Together.json").write_text(
            json.dumps({
                "id": "arm9/Together",
                "module": "arm9",
                "source": "src_tu/Future.cpp",
                "promoted_source": "src/Together.cpp",
                "status": "promoted",
                "sections": [{"name": ".text", "start": "0x1000", "end": "0x1020"}],
                "functions": [{"symbol": "One"}, {"symbol": "Two"}],
            }),
            encoding="utf-8",
        )
        subprocess.run(["git", "init", "-q", str(repo)], check=True)
        subprocess.run(["git", "-C", str(repo), "add", "src", "src_tu", "config"], check=True)
        return temp, repo

    def test_collect_uses_delinks_for_two_function_source(self):
        temp, repo = self.fixture()
        self.addCleanup(temp.cleanup)
        report = STATE.collect(repo, **self.readers())

        self.assertEqual(report["source_files"]["production"], {
            "total": 4, "c": 2, "cpp": 2,
            "cpp_marker_missing": 0, "cpp_marker_missing_paths": [],
        })
        self.assertEqual(report["production_enrollment"]["functions"], 3)
        self.assertEqual(report["production_enrollment"]["source_files"], 2)
        self.assertEqual(report["production_enrollment"]["multi_function_source_files"], 1)
        self.assertEqual(report["production_enrollment"]["functions_in_multi_function_sources"], 2)
        self.assertEqual(report["production_enrollment"]["largest_function_count"], 2)
        self.assertEqual(
            report["production_enrollment"]["multi_function_sources"],
            {"src/Together.cpp": ["One", "Two"]},
        )
        self.assertEqual(report["tu_reconstruction"]["production_promoted_entries"], 1)
        self.assertEqual(report["tu_reconstruction"]["production_partitioned_entries"], 0)
        self.assertEqual(report["semantic_language_mode"]["mangled_symbol_files"], 2)
        self.assertEqual(report["semantic_language_mode"]["unmigrated"], 1)
        self.assertEqual(report["semantic_language_mode"]["genuinely_migrated"], 1)
        self.assertEqual(report["matching_progress"]["matched_functions"], 3)
        self.assertEqual(report["matching_progress"]["function_percent"], 75.0)
        self.assertEqual(report["converted_tier"]["strict_converted_functions"], 2)
        self.assertEqual(report["converted_tier"]["source_functions"], 4)
        self.assertEqual(report["converted_tier"]["source_files"], 3)
        self.assertEqual(report["converted_tier"]["percent"], 50.0)
        self.assertEqual(report["production_tu_compatibility"]["blockers"], ["rombuild"])

    def test_collect_reports_configured_canonical_partitioned_tu(self):
        temp, repo = self.fixture()
        self.addCleanup(temp.cleanup)
        source = repo / "src" / "Partitioned.cpp"
        source.write_text("//cpp\nint Partitioned() { return 1; }\n", encoding="utf-8")
        manifest = repo / "config" / "tu_manifest.d" / "ov002-Partitioned.json"
        manifest.write_text(json.dumps({
            "id": "ov002/Partitioned",
            "module": "ov002",
            "source": "src/Partitioned.cpp",
            "promoted_source": "src/Partitioned.cpp",
            "status": "text-verified",
            "production_mode": "partitioned",
            "partitioned_link": {"state": "partitioned-link-verified"},
            "sections": [
                {"name": ".text", "start": "0x2000", "end": "0x2010"},
                {"name": ".data", "start": "0x3000", "end": "0x3010"},
            ],
            "functions": [{"symbol": "Partitioned"}],
        }), encoding="utf-8")
        registry = repo / "config" / "production-tus.json"
        registry.write_text(json.dumps({
            "schema_version": 1,
            "partitioned_tus": ["ov002/Partitioned"],
        }), encoding="utf-8")
        subprocess.run(
            ["git", "-C", str(repo), "add", str(source), str(manifest), str(registry)],
            check=True)

        report = STATE.collect(repo, **self.readers())
        tu = report["tu_reconstruction"]
        self.assertEqual(tu["production_partitioned_entries"], 1)
        self.assertEqual(tu["production_partitioned_functions"], 1)
        self.assertEqual(tu["partitioned_mismatch_entries"], [])

    def test_dirty_authority_inputs_are_refused_by_default(self):
        temp, repo = self.fixture()
        self.addCleanup(temp.cleanup)
        (repo / "src" / "Solo.c").write_text(
            "int Solo(void) { return 4; }\n", encoding="utf-8")
        readers = self.readers()
        readers.pop("require_clean_inputs")
        with self.assertRaisesRegex(STATE.StateError, "unstaged/untracked"):
            STATE.collect(repo, **readers)

    def test_untracked_authority_inputs_are_refused_by_default(self):
        temp, repo = self.fixture()
        self.addCleanup(temp.cleanup)
        (repo / "src" / "Untracked.c").write_text(
            "int Untracked(void) { return 5; }\n", encoding="utf-8")
        readers = self.readers()
        readers.pop("require_clean_inputs")
        with self.assertRaisesRegex(STATE.StateError, "unstaged/untracked"):
            STATE.collect(repo, **readers)

    def test_rombytes_path_is_not_counted_as_source_built(self):
        temp, repo = self.fixture()
        self.addCleanup(temp.cleanup)
        delinks = repo / "config" / "arm9" / "delinks.txt"
        delinks.write_text(
            delinks.read_text(encoding="utf-8").replace(
                "src/Solo.c:\n    complete\n", "src/Solo.c:\n"
            ),
            encoding="utf-8",
        )

        report = STATE.collect(repo, **self.readers())
        self.assertEqual(report["production_path_ownership"]["functions"], 3)
        self.assertEqual(report["production_path_ownership"]["rombytes_only_functions"], 1)
        self.assertEqual(report["production_enrollment"]["functions"], 2)
        self.assertEqual(report["production_enrollment"]["source_files"], 1)

    def test_markdown_explains_evidence_is_not_enrollment(self):
        temp, repo = self.fixture()
        self.addCleanup(temp.cleanup)
        text = STATE.render_markdown(STATE.collect(repo, **self.readers()))

        self.assertIn("Complete sources owning more than one function | 1", text)
        self.assertIn("MATCHED functions | 3 / 4 (75.00%)", text)
        self.assertIn("Strict CONVERTED source functions | 2 / 4 (50.00%)", text)
        self.assertIn("Physical production source files | 3", text)
        self.assertIn("no ambient or gitignored chaos database", text)
        self.assertIn("not enroll a TU", text)
        self.assertIn("filenames are not treated as the authority", text)
        self.assertIn("All surfaces ready: **NO**", text)
        self.assertIn("Current ready surfaces: `srcpath`", text)
        self.assertIn("Current hard production blockers: `rombuild`", text)
        self.assertIn("Current policy/metric gaps: `tiers`", text)


if __name__ == "__main__":
    unittest.main()
