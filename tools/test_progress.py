"""The clean-clone progress scan uses the published MATCHED policy."""
import pathlib
import tempfile
import unittest
import sys
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import progress as P  # noqa: E402
import bytegate as BG  # noqa: E402
import relocs as RL  # noqa: E402
import srcpath as SP  # noqa: E402


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

    def test_zero_size_alias_is_not_in_the_denominator_either(self):
        """The published fraction, both halves, over a fake config tree.

        source_counts_as_matched has always refused an aliased zero-size record, but
        synced_from_src counted it in `n` regardless, so ten symbols sat in the
        denominator that nothing could ever move into the numerator. `alias` is a second
        name for `sized` at the same address and must not appear on either side;
        `lonely` is size 0 at an address nothing shares, which is real outstanding work
        and must still be counted, or the fix would raise the rate by hiding functions
        rather than by counting honestly."""
        temp = tempfile.TemporaryDirectory()
        self.addCleanup(temp.cleanup)
        repo = pathlib.Path(temp.name)
        (repo / "src").mkdir()
        sym = repo / "symbols.txt"
        sym.write_text(
            "good kind:function(arm,size=0x10) addr:0x02000000\n"
            "sized kind:function(arm,size=0x100) addr:0x02000030\n"
            "alias kind:function(arm,size=0x0) addr:0x02000030\n"
            "lonely kind:function(arm,size=0x0) addr:0x02000200\n", encoding="utf-8")
        for name in ("good", "sized", "alias", "lonely"):
            (repo / "src" / f"{name}.c").write_text(
                f"int {name}(void) {{ return 0; }}\n", encoding="utf-8")

        with mock.patch.object(P, "REPO", repo), \
                mock.patch.object(RL, "module_universe", lambda: [(sym, "arm9")]), \
                mock.patch.object(BG, "excluded_paths", lambda *a, **k: set()), \
                mock.patch.object(
                    SP, "path_for",
                    lambda n: (repo / "src" / f"{n}.c"
                               if (repo / "src" / f"{n}.c").is_file() else None)):
            done_n, done_b, n, total_bytes = P.synced_from_src()

        self.assertEqual(n, 3, "four symbol lines, three functions")
        self.assertEqual(done_n, 3, "and every one of them has a source")
        self.assertEqual(total_bytes, 0x10 + 0x100)
        self.assertEqual(done_b, 0x10 + 0x100)

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
