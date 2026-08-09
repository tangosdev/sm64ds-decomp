"""ROM enrollment accepts only real C/C++ sources inside src/ or mods/."""
import pathlib
import sys
import tempfile
import unittest

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


if __name__ == "__main__":
    unittest.main()
