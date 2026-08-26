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
        policy = {"src/Pair.cpp": ["_ZN4PairC2Ev"]}
        with mock.patch.object(
                RB.OI, "derive_deadstrip",
                return_value=(b"reduced object", {"error": None})) as deadstrip, \
                mock.patch.object(RB.OI, "isolate_many",
                                  return_value={"error": None}) as many:
            self.assertIsNone(RB._isolate(
                obj, "src/Pair.cpp", {"src/Pair.cpp": ["First", "Second"]},
                compiler_only=policy))
        deadstrip.assert_called_once_with(b"raw object", ["_ZN4PairC2Ev"])
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
            {"src/Pair.cpp": ["_ZN4PairC2Ev"]})

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
