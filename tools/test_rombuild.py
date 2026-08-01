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


if __name__ == "__main__":
    unittest.main()
