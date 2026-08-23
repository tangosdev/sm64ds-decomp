"""The clean-clone progress scan uses the published MATCHED policy."""
import pathlib
import tempfile
import unittest
import sys
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import progress as P  # noqa: E402


class ProgressPolicy(unittest.TestCase):
    def source(self, text):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        path = pathlib.Path(temp.name) / "Example.c"
        path.write_text(text, encoding="utf-8")
        return path

    def counted(self, text="int Example(void) { return 0; }", *, size=4,
                aliases=frozenset(), excluded=frozenset()):
        return P.source_counts_as_matched(
            self.source(text), "src/Example.c", "arm9", 0x02000000, size,
            aliases, excluded)

    def test_ordinary_source_counts(self):
        self.assertTrue(self.counted())

    def test_nonmatching_and_transcribed_sources_do_not_count(self):
        self.assertFalse(self.counted("// NONMATCHING\nint Example(void) { return 0; }"))
        self.assertFalse(self.counted("asm void Example(void) { dcd 0xe12fff1e }"))

    def test_zero_size_alias_and_manifest_exclusion_do_not_count(self):
        self.assertFalse(self.counted(
            size=0, aliases=frozenset({("arm9", 0x02000000)})))
        self.assertFalse(self.counted(excluded=frozenset({"src/Example.c"})))

    def test_from_src_ignores_an_ambient_database(self):
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        repo = pathlib.Path(temp.name)
        (repo / "chaos-db.json").write_text("{}", encoding="utf-8")
        with mock.patch.object(P.sys, "argv", ["progress.py", "--bar", "--from-src"]), \
                mock.patch.object(P, "REPO", repo):
            self.assertIsNone(P._db_path())


if __name__ == "__main__":
    unittest.main()
