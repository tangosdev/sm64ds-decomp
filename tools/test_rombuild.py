"""ROM enrollment accepts only real C/C++ sources inside src/ or mods/."""
import pathlib
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import rombuild as RB  # noqa: E402


class RomBuildEnrollment(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        self.config = self.repo / "config" / "arm9"
        self.config.mkdir(parents=True)
        (self.repo / "src").mkdir()
        (self.repo / "src" / "Example.c").write_text(
            "int Example(void) { return 0; }\n", encoding="utf-8")
        self.old_repo = RB.REPO
        RB.REPO = self.repo

    def tearDown(self):
        RB.REPO = self.old_repo
        self.tmp.cleanup()

    def write_entry(self, rel):
        (self.config / "delinks.txt").write_text(
            f"{rel}:\n"
            "    complete\n"
            "    .text start:0x02000000 end:0x02000004\n",
            encoding="utf-8")

    def test_enrolled_accepts_a_repo_source(self):
        self.write_entry("src/Example.c")
        self.assertEqual(RB.enrolled(self.config), ["src/Example.c"])

    def test_enrolled_rejects_a_path_escape(self):
        self.write_entry("../outside.c")
        with self.assertRaises(RB.BuildError) as raised:
            RB.enrolled(self.config)
        self.assertIn("unsafe complete", raised.exception.output)

    def test_enrolled_rejects_a_missing_source(self):
        self.write_entry("src/Missing.c")
        with self.assertRaises(RB.BuildError) as raised:
            RB.enrolled(self.config)
        self.assertIn("missing or symlinked", raised.exception.output)

    def test_enrolled_rejects_non_source_paths(self):
        self.write_entry("tools/Example.py")
        with self.assertRaises(RB.BuildError) as raised:
            RB.enrolled(self.config)
        self.assertIn("unsafe complete", raised.exception.output)

    def test_enrolled_rejects_duplicate_complete_source_paths(self):
        (self.config / "delinks.txt").write_text(
            "src/Example.c:\n    complete\n"
            "    .text start:0x02000000 end:0x02000004\n\n"
            "src/Example.c:\n    complete\n"
            "    .data start:0x02001000 end:0x02001004\n",
            encoding="utf-8")
        with self.assertRaises(RB.BuildError) as raised:
            RB.enrolled(self.config)
        self.assertIn("enrolled more than once", raised.exception.output)

    def test_enrolled_symbols_group_one_source_in_rom_order(self):
        candidates = [
            (self.config, "Second", "src/Pair.cpp", 0x02000004, 4, ".text"),
            (self.config, "Only", "src/Only.cpp", 0x02000020, 4, ".text"),
            (self.config, "First", "src/Pair.cpp", 0x02000000, 4, ".text"),
        ]
        with mock.patch("enroll.candidates", return_value=(candidates, {})):
            self.assertEqual(RB.enrolled_symbols(), {
                "src/Pair.cpp": ["First", "Second"],
                "src/Only.cpp": ["Only"],
            })

    def test_nested_function_records_are_aliases_not_required_definitions(self):
        rel = "src/func_01ff97d8.c"
        candidates = [
            (self.config, "func_01ff98f4", rel, 0x01ff98f4, 0x0b0, ".text"),
            (self.config, "func_01ff99a4", rel, 0x01ff99a4, 0x39c, ".text"),
            (self.config, "func_01ff97d8", rel, 0x01ff97d8, 0xb6c, ".text"),
            (self.config, "func_01ff9e2c", rel, 0x01ff9e2c, 0x518, ".text"),
            (self.config, "func_01ff9d40", rel, 0x01ff9d40, 0x0ec, ".text"),
        ]
        with mock.patch("enroll.candidates", return_value=(candidates, {})):
            mapping = RB.enrolled_symbols()
        self.assertEqual(mapping, {rel: ["func_01ff97d8"]})

        obj = self.repo / "Outer.o"
        obj.write_bytes(b"object")
        with mock.patch.object(RB.OI, "isolate", return_value={"error": None}) as one, \
                mock.patch.object(RB.OI, "isolate_many",
                                  return_value={"error": None}) as many:
            self.assertIsNone(RB._isolate(obj, rel, mapping))
        one.assert_called_once_with(obj, "func_01ff97d8")
        many.assert_not_called()

    def test_non_symbol_tu_path_never_guesses_among_overlapping_records(self):
        rows = [
            (0x02000000, 0x100, "Outer"),
            (0x02000020, 0x020, "Nested"),
        ]
        self.assertEqual(RB._definition_symbols("src/actors/Pair.cpp", rows),
                         ["Outer", "Nested"])

    def test_stem_owner_with_a_disjoint_member_never_collapses(self):
        rows = [
            (0x02000000, 0x100, "Outer"),
            (0x02000020, 0x020, "Nested"),
            (0x02000100, 0x010, "Disjoint"),
        ]
        self.assertEqual(RB._definition_symbols("src/Outer.c", rows),
                         ["Outer", "Nested", "Disjoint"])

    def test_isolate_dispatches_multi_and_singleton_ownership_separately(self):
        obj = self.repo / "Example.o"
        obj.write_bytes(b"object")
        with mock.patch.object(RB.OI, "isolate", return_value={"error": None}) as one, \
                mock.patch.object(RB.OI, "isolate_many",
                                  return_value={"error": None}) as many:
            self.assertIsNone(RB._isolate(
                obj, "src/Pair.cpp", {"src/Pair.cpp": ["First", "Second"]}))
            many.assert_called_once_with(obj, ["First", "Second"])
            one.assert_not_called()

            many.reset_mock()
            self.assertIsNone(RB._isolate(
                obj, "src/Only.cpp", {"src/Only.cpp": ["Only"]}))
            one.assert_called_once_with(obj, "Only")
            many.assert_not_called()

    def test_multi_source_applies_exact_compiler_only_deadstrip_first(self):
        obj = self.repo / "Pair.o"
        obj.write_bytes(b"raw object")
        policy = {"src/Pair.cpp": {"deadstrip": ["_ZN4PairC2Ev"], "expect": {}}}
        with mock.patch.object(
                RB.OI, "derive_deadstrip",
                return_value=(b"reduced object", {"error": None})) as deadstrip, \
                mock.patch.object(RB.OI, "isolate_many",
                                  return_value={"error": None}) as many:
            self.assertIsNone(RB._isolate(
                obj, "src/Pair.cpp", {"src/Pair.cpp": ["First", "Second"]},
                compiler_only=policy))
        deadstrip.assert_called_once_with(b"raw object", ["_ZN4PairC2Ev"], {})
        many.assert_called_once_with(obj, ["First", "Second"])
        self.assertEqual(obj.read_bytes(), b"reduced object")

    def test_compiler_only_policy_refuses_a_symbol_with_a_rom_home(self):
        manifest = {"entries": [{
            "id": "arm9/Pair",
            "source": "src/Pair.cpp",
            "functions": [{"symbol": "First"}, {"symbol": "Second"}],
            "compiler_only_output": [{
                "symbol": "_ZN4PairC2Ev", "disposition": "deadstrip",
                "reason": "compiler-generated constructor variant"
            }]
        }]}
        with self.assertRaises(RB.BuildError) as raised:
            RB.compiler_only_policies(
                manifest=manifest, homes={"_ZN4PairC2Ev": [("arm9", 0x02000008)]})
        self.assertIn("configured ROM home", raised.exception.output)

    def test_compiler_only_policy_accepts_an_exact_homeless_variant(self):
        manifest = {"entries": [{
            "id": "arm9/Pair",
            "source": "src/Pair.cpp",
            "functions": [{"symbol": "First"}, {"symbol": "Second"}],
            "compiler_only_output": [{
                "symbol": "_ZN4PairC2Ev", "disposition": "deadstrip",
                "reason": "compiler-generated constructor variant"
            }]
        }]}
        self.assertEqual(
            RB.compiler_only_policies(manifest=manifest, homes={}),
            {"src/Pair.cpp": {"deadstrip": ["_ZN4PairC2Ev"], "expect": {}, "data": [], "homes": {}}})

    def test_duplicate_disposition_requires_a_rom_home(self):
        """The two dispositions have opposite preconditions, deliberately."""
        manifest = {"entries": [{
            "id": "arm9/Pair",
            "source": "src/Pair.cpp",
            "functions": [{"symbol": "First"}, {"symbol": "Second"}],
            "compiler_only_output": [{
                "symbol": "_ZN7Vector3D1Ev", "disposition": "deadstrip-duplicate",
                "reason": "vague-linkage copy of types.h's empty ~Vector3"
            }]
        }]}
        with self.assertRaises(RB.BuildError) as raised:
            RB.compiler_only_policies(manifest=manifest, homes={})
        self.assertIn("no configured ROM home", raised.exception.output)

    def test_duplicate_disposition_refuses_an_ambiguous_home(self):
        """Two homes means the name is ambiguous; the body proof would be a guess."""
        manifest = {"entries": [{
            "id": "arm9/Pair",
            "source": "src/Pair.cpp",
            "functions": [{"symbol": "First"}],
            "compiler_only_output": [{
                "symbol": "_ZN7Vector3D1Ev", "disposition": "deadstrip-duplicate",
                "reason": "vague-linkage copy"
            }]
        }]}
        with self.assertRaises(RB.BuildError) as raised:
            RB.compiler_only_policies(manifest=manifest, homes={
                "_ZN7Vector3D1Ev": [("arm9", 0x02000008), ("ov002", 0x020f0000)]})
        self.assertIn("2 configured homes", raised.exception.output)

    def test_an_unknown_disposition_is_refused(self):
        manifest = {"entries": [{
            "id": "arm9/Pair", "source": "src/Pair.cpp",
            "functions": [{"symbol": "First"}],
            "compiler_only_output": [{"symbol": "X", "disposition": "keep",
                                      "reason": "why not"}]}]}
        with self.assertRaises(RB.BuildError) as raised:
            RB.compiler_only_policies(manifest=manifest, homes={})
        self.assertIn("disposition must be one of", raised.exception.output)

    def _data_manifest(self, **row):
        base = {"symbol": "_ZTV4Pair", "disposition": "deadstrip-data",
                "reason": "the cartridge owns this vtable"}
        return {"entries": [{
            "id": "arm9/Pair",
            "module": "arm9",
            "source": "src/Pair.cpp",
            "sections": [{"name": ".text", "start": "0x02000000", "end": "0x02000100"}],
            "functions": [{"symbol": "First"}],
            "compiler_only_output": [base | row]}]}

    def test_data_disposition_requires_a_rom_home(self):
        """A homeless data object is a plain deadstrip -- there is nothing to prove."""
        with self.assertRaises(RB.BuildError) as raised:
            RB.compiler_only_policies(manifest=self._data_manifest(), homes={})
        self.assertIn("a homeless object is a plain deadstrip", raised.exception.output)

    def test_data_disposition_refuses_an_entry_with_no_module(self):
        """Without a module every claimed range reads as somebody else's, so the
        address argument would clear itself vacuously for the whole ROM."""
        manifest = self._data_manifest()
        del manifest["entries"][0]["module"]
        with self.assertRaises(RB.BuildError) as raised:
            RB.compiler_only_policies(manifest=manifest, homes={
                "_ZTV4Pair": [("arm9", 0x02000040)]})
        self.assertIn("this entry declares no module", raised.exception.output)

    def test_data_disposition_refuses_a_home_this_entry_claims(self):
        """Inside a claimed range, dsd does NOT delink it: the source must build it."""
        with self.assertRaises(RB.BuildError) as raised:
            RB.compiler_only_policies(manifest=self._data_manifest(), homes={
                "_ZTV4Pair": [("arm9", 0x02000040)]})
        self.assertIn("must be built, not discarded", raised.exception.output)

    def test_data_disposition_accepts_a_home_outside_every_claimed_range(self):
        """The licence is an address argument: dsd delinks it from the cartridge
        regardless, so discarding this object's copy cannot cost the image a byte."""
        self.assertEqual(
            RB.compiler_only_policies(manifest=self._data_manifest(), homes={
                "_ZTV4Pair": [("arm9", 0x02008000)]}),
            {"src/Pair.cpp": {"deadstrip": ["_ZTV4Pair"], "expect": {}, "homes": {},
                              "data": ["_ZTV4Pair"]}})

    def test_data_disposition_rejects_a_canonical_home_that_is_not_configured(self):
        with self.assertRaises(RB.BuildError) as raised:
            RB.compiler_only_policies(
                manifest=self._data_manifest(canonical_module="arm9",
                                             canonical_address="0x02009999"),
                homes={"_ZTV4Pair": [("arm9", 0x02008000)]})
        self.assertIn("which is not one of its configured ROM home(s)",
                      raised.exception.output)

    def test_policy_keys_on_the_promoted_path_not_the_src_tu_path(self):
        """A promoted entry is enrolled under promoted_source.

        Keying on `source` would let the enrolled-set filter drop the policy and the
        build would then refuse the very object it was written for.
        """
        manifest = {"entries": [{
            "id": "ov100/TU",
            "source": "src_tu/actors/TU.cpp",
            "promoted_source": "src/actors/TU.cpp",
            "functions": [{"symbol": "First"}],
            "compiler_only_output": [{"symbol": "_ZN1PD2Ev", "disposition": "deadstrip",
                                      "reason": "homeless variant"}]}]}
        self.assertEqual(
            RB.compiler_only_policies({"src/actors/TU.cpp"}, manifest=manifest, homes={}),
            {"src/actors/TU.cpp": {"deadstrip": ["_ZN1PD2Ev"], "expect": {}, "data": [], "homes": {}}})

    def test_compiler_only_policy_ignores_unenrolled_shadow_manifests(self):
        manifest = {"entries": [{
            "id": "arm9/Shadow",
            "source": "src_tu/Shadow.cpp",
            "functions": [{"symbol": "First"}],
            "compiler_only_output": [{
                "symbol": "ConfiguredElsewhere", "disposition": "deadstrip",
                "reason": "research-only observation"
            }]
        }]}
        self.assertEqual(RB.compiler_only_policies(
            enrolled=["src/Pair.cpp"], manifest=manifest,
            homes={"ConfiguredElsewhere": [("arm9", 0x02000008)]}), {})

    def test_intact_policy_requires_promoted_full_ordinary_link_proof(self):
        entry = {
            "id": "ov047/TU", "status": "promoted",
            "production_mode": "intact-object",
            "source": "src/actors/TU.cpp", "promoted_source": "src/actors/TU.cpp",
            "sections": [
                {"name": ".text", "start": "0x1000", "end": "0x1010"},
                {"name": ".data", "start": "0x2000", "end": "0x2010"}],
            "verification": {"linkcheck": {
                "result": "scratch-data-verified",
                "phases": {name: True for name in
                           ("delink", "lcf", "compile", "link", "checkModules", "rom")},
                "symbolCheckNewVsBaseline": [],
                "symbolCheckErrors": ["[ERROR] old"],
                "symbolCheckBaselineErrors": ["[ERROR] old"],
                "tuRanges": [
                    {"section": ".text", "start": "0x1000", "end": "0x1010",
                     "differingBytes": 0},
                    {"section": ".data", "start": "0x2000", "end": "0x2010",
                     "differingBytes": 0}],
                "rom": {"sha256": "a" * 64, "matchesStockRom": True},
            }},
        }
        manifest = {"entries": [entry]}
        self.assertEqual(RB.intact_tu_policies(
            {"src/actors/TU.cpp"}, manifest=manifest),
            {"src/actors/TU.cpp": entry})
        duplicate = dict(entry)
        duplicate["id"] = "ov047/Duplicate"
        with self.assertRaises(RB.BuildError) as raised:
            RB.intact_tu_policies({"src/actors/TU.cpp"},
                                   manifest={"entries": [entry, duplicate]})
        self.assertIn("declared by multiple entries", raised.exception.output)

        entry["verification"]["linkcheck"]["tuRanges"][1]["differingBytes"] = 1
        with self.assertRaises(RB.BuildError) as raised:
            RB.intact_tu_policies({"src/actors/TU.cpp"}, manifest=manifest)
        self.assertIn("every current manifest range exact", raised.exception.output)

        entry["verification"]["linkcheck"]["tuRanges"][1]["differingBytes"] = 0
        entry["verification"]["linkcheck"]["tuRanges"].pop()
        with self.assertRaises(RB.BuildError) as raised:
            RB.intact_tu_policies({"src/actors/TU.cpp"}, manifest=manifest)
        self.assertIn("every current manifest range exact", raised.exception.output)

        entry["verification"]["linkcheck"]["tuRanges"].append(
            {"section": ".data", "start": "0x2000", "end": "0x2010",
             "differingBytes": 0})
        entry["verification"]["linkcheck"]["rom"]["matchesStockRom"] = False
        with self.assertRaises(RB.BuildError) as raised:
            RB.intact_tu_policies({"src/actors/TU.cpp"}, manifest=manifest)
        self.assertIn("identical to stock", raised.exception.output)

        entry["verification"]["linkcheck"]["rom"]["matchesStockRom"] = True
        entry["sections"][1]["name"] = ".rodata"
        entry["sections"][1]["module_section"] = ".data"
        entry["verification"]["linkcheck"]["tuRanges"][1]["section"] = ".rodata"
        with self.assertRaises(RB.BuildError) as raised:
            RB.intact_tu_policies({"src/actors/TU.cpp"}, manifest=manifest)
        self.assertIn("input-section retargeting", raised.exception.output)

        entry["sections"][1] = {"name": ".data", "start": "0x2000", "end": "0x2010"}
        entry["verification"]["linkcheck"]["tuRanges"][1]["section"] = ".data"
        # Every licensed non-text field follows the same fail-closed rule, not just
        # `.data`; vtables can be represented under `.rodata` in a manifest.
        entry["rodata"] = [{"symbol": "_ZTV1T", "storage_alias": {
            "symbol": "data_00002000", "address": "0x2000", "size": "0x8"}}]
        with self.assertRaises(RB.BuildError) as raised:
            RB.intact_tu_policies({"src/actors/TU.cpp"}, manifest=manifest)
        self.assertIn("baseline bootstrapping is non-circular", raised.exception.output)

    def test_intact_rom_comparison_uses_current_same_worker_control(self):
        verification = {
            "baseline": {
                "romSha256": "b" * 64,
                "moduleSetSha256": "c" * 64,
            },
            "admittedRomSha256": ["a" * 64],
        }
        result = RB.intact_rom_comparison("b" * 64, verification)
        self.assertEqual(result, {
            "expectedSha256": "b" * 64,
            "admittedBootstrapSha256": ["a" * 64],
            "actualSha256": "b" * 64,
            "moduleSetSha256": "c" * 64,
            "identical": True,
        })
        self.assertFalse(
            RB.intact_rom_comparison("d" * 64, verification)["identical"])

    def test_intact_policy_ignores_unenrolled_shadow(self):
        manifest = {"entries": [{
            "id": "ov047/Shadow", "status": "text-verified",
            "production_mode": "intact-object",
            "promoted_source": "src/actors/Shadow.cpp",
        }]}
        self.assertEqual(RB.intact_tu_policies(
            {"src/actors/Elsewhere.cpp"}, manifest=manifest), {})

    def test_intact_policy_refuses_promoted_source_missing_from_enrollment(self):
        manifest = {"entries": [{
            "id": "ov047/Missing", "status": "promoted",
            "production_mode": "intact-object",
            "promoted_source": "src/actors/Missing.cpp",
        }]}
        with self.assertRaises(RB.BuildError) as raised:
            RB.intact_tu_policies(
                ["src/actors/Elsewhere.cpp"], manifest=manifest)
        self.assertIn("enrolled 0 time(s), expected exactly 1", raised.exception.output)


def _compiler():
    exe = RB.MW / RB.VERSION / "mwccarm.exe"
    return exe if exe.is_file() else None


@unittest.skipUnless(_compiler(), "mwccarm not present")
class Retarget(unittest.TestCase):
    """An object `.init` retargeting cannot handle must fail one file, not the run.

    Real mwcc output rather than a fixture, for the reason test_objisolate gives: a
    C++ destructor genuinely compiles to three `.text` sections, and renaming an
    arbitrary one would have dsd's `File.o(.init)` selector place whichever it
    happened to be.
    """

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        d = pathlib.Path(self.tmp.name)
        self.obj = d / "t.o"
        src = d / "t.cpp"
        src.write_text("//cpp\nstruct V { virtual ~V(); int x; };\nV::~V() { x = 0; }\n",
                       encoding="utf-8")
        import os
        import subprocess
        r = subprocess.run(
            [*os.environ.get("MWCCARM_LAUNCHER", "").split(), str(_compiler()),
             *RB.CFLAGS.replace("-lang c99", "-lang c++").split(),
             "-i", str(RB.INCLUDE), "-c", str(src), "-o", str(self.obj)],
            capture_output=True, text=True, cwd=RB.REPO,
            env=dict(os.environ, LM_LICENSE_FILE=str(RB.MW / "license.dat")))
        self.assertEqual(r.returncode, 0, r.stdout + r.stderr)

    def tearDown(self):
        self.tmp.cleanup()

    def test_multi_text_object_is_refused(self):
        with self.assertRaises(RuntimeError) as raised:
            RB.retarget_text_section(self.obj)
        self.assertIn(".text sections", str(raised.exception))

    def test_refusal_becomes_a_per_file_verdict(self):
        # The point of the wrapper: these calls run under ex.map, where a raise ends
        # the whole build with a traceback instead of failing the one file.
        err = RB._retarget(self.obj, "src/t.cpp", {"src/t.cpp"})
        self.assertIsNotNone(err)
        self.assertIn(".text sections", err)

    def test_a_file_not_marked_init_is_untouched(self):
        before = self.obj.read_bytes()
        self.assertIsNone(RB._retarget(self.obj, "src/t.cpp", set()))
        self.assertEqual(self.obj.read_bytes(), before)


class VersionPinAudit(unittest.TestCase):
    """A pin that stops applying is the most expensive failure this build can emit.

    It produces wrong bytes for a function whose source is correct, and every
    per-file gate keeps calling that function exact, because they all compile it
    WITH the pin (build_pin reads the same table). Only the whole-module compare
    disagrees. So the pin table is checked against the tree before the first
    compile rather than discovered afterwards.
    """

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        (self.repo / "src").mkdir()
        (self.repo / "mods").mkdir()
        for stem in ("Pinned", "Waiting"):
            (self.repo / "src" / f"{stem}.c").write_text("int x;\n", encoding="utf-8")
        self.mw = self.repo / "tools" / "mwccarm"
        (self.mw / "1.2" / "base").mkdir(parents=True)
        (self.mw / "1.2" / "base" / "mwccarm.exe").write_bytes(b"stub")
        self.old_repo, self.old_mw = RB.REPO, RB.MW
        RB.REPO, RB.MW = self.repo, self.mw

    def tearDown(self):
        RB.REPO, RB.MW = self.old_repo, self.old_mw
        self.tmp.cleanup()

    def test_enrolled_pin_is_reported_as_applied(self):
        applied, inert = RB.audit_version_pins({"Pinned": "1.2/base"}, ["src/Pinned.c"])
        self.assertEqual(applied, ["Pinned"])
        self.assertEqual(inert, [])

    def test_pin_on_a_file_that_exists_but_is_not_enrolled_is_inert_not_fatal(self):
        # Configured and waiting for a `complete` marker. Not a defect.
        applied, inert = RB.audit_version_pins({"Waiting": "1.2/base"}, ["src/Pinned.c"])
        self.assertEqual(applied, [])
        self.assertEqual(inert, ["Waiting"])

    def test_pin_naming_no_file_at_all_is_fatal(self):
        # The shape of a rename whose pin was not re-keyed in the same commit.
        with self.assertRaises(RB.BuildError) as caught:
            RB.audit_version_pins({"func_ov013_021112a8": "1.2/base"}, ["src/Pinned.c"])
        self.assertIn("func_ov013_021112a8", caught.exception.output)
        self.assertIn("re-key", caught.exception.output)

    def test_pin_naming_an_uninstalled_compiler_is_fatal(self):
        # The default-only preflight cannot catch this: it never looks at the pins.
        with self.assertRaises(RB.BuildError) as caught:
            RB.audit_version_pins({"Pinned": "1.2/sp4"}, ["src/Pinned.c"])
        self.assertIn("1.2/sp4", caught.exception.output)

    def test_a_mods_file_satisfies_a_pin(self):
        (self.repo / "mods" / "Replaced.cpp").write_text("int y;\n", encoding="utf-8")
        applied, inert = RB.audit_version_pins({"Replaced": "1.2/base"},
                                               ["mods/Replaced.cpp"])
        self.assertEqual(applied, ["Replaced"])


if __name__ == "__main__":
    unittest.main()
