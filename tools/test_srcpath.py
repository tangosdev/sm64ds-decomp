"""What srcpath has to guarantee, especially the subdirectory case that does not exist
in the tree yet -- that is the whole point of the module, so it is the part most worth
pinning before anything moves."""
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import srcpath as SP  # noqa: E402


class SrcPath(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        repo = pathlib.Path(self.tmp.name)
        (repo / "src").mkdir()
        self._saved = (SP.REPO, SP.SRC)
        SP.REPO, SP.SRC = repo, repo / "src"
        SP.invalidate()

    def tearDown(self):
        SP.REPO, SP.SRC = self._saved
        SP.invalidate()
        self.tmp.cleanup()

    def write(self, rel, text="int f(void) { return 0; }\n"):
        p = SP.SRC / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text)
        SP.invalidate()
        return p

    # --- today's flat tree -------------------------------------------------
    def test_flat_c(self):
        p = self.write("func_0205a61c.c")
        self.assertEqual(SP.path_for("func_0205a61c"), p)

    def test_flat_cpp(self):
        p = self.write("_ZN6Player4InitEv.cpp")
        self.assertEqual(SP.path_for("_ZN6Player4InitEv"), p)

    def test_missing_is_none(self):
        self.assertIsNone(SP.path_for("nope"))

    def test_c_wins_over_cpp(self):
        c = self.write("both.c")
        self.write("both.cpp")
        self.assertEqual(SP.path_for("both"), c)
        self.assertEqual([p.suffix for p in SP.paths_for("both")], [".c", ".cpp"])

    # --- the layout this exists for ----------------------------------------
    def test_finds_symbol_in_subdirectory(self):
        p = self.write("actors/ov063_boo/Boo_Behavior.c")
        self.assertEqual(SP.path_for("Boo_Behavior"), p)

    def test_subdirectory_survives_a_deep_nest(self):
        p = self.write("arm9/nitro/os/OS_SleepThread.c")
        self.assertEqual(SP.path_for("OS_SleepThread"), p)

    def test_direct_hit_wins_over_scan(self):
        """A flat file shadows a nested one of the same name -- no scan, no ambiguity."""
        flat = self.write("dup.c")
        self.write("sub/dup.c")
        self.assertEqual(SP.path_for("dup"), flat)

    # --- writers ------------------------------------------------------------
    def test_new_path_defaults_to_src_root(self):
        self.assertEqual(SP.new_path_for("brand_new", "c"), SP.SRC / "brand_new.c")
        self.assertEqual(SP.new_path_for("brand_new", ".cpp"), SP.SRC / "brand_new.cpp")

    def test_new_path_keeps_an_existing_file_where_it_lives(self):
        self.write("actors/ov063_boo/Boo_Render.c")
        self.assertEqual(SP.new_path_for("Boo_Render", "cpp"),
                         SP.SRC / "actors/ov063_boo/Boo_Render.cpp")

    def test_new_path_rejects_a_non_source_extension(self):
        with self.assertRaises(ValueError):
            SP.new_path_for("x", "h")

    # --- bulk ---------------------------------------------------------------
    def test_index_and_iter_cover_nested_files(self):
        self.write("flat.c")
        self.write("courses/ov022_lethal_lava_land/FallBlockLll.cpp")
        self.assertEqual(
            SP.build_index(),
            {"flat": "src/flat.c",
             "FallBlockLll": "src/courses/ov022_lethal_lava_land/FallBlockLll.cpp"})
        self.assertEqual(len(list(SP.iter_sources())), 2)

    def test_index_agrees_with_path_for_on_a_dual_extension_symbol(self):
        """Directory order must not decide; .c wins in both, or the index lies."""
        self.write("both.cpp")
        self.write("both.c")
        self.assertEqual(SP.build_index()["both"], "src/both.c")
        self.assertEqual(SP.path_for("both"), SP.SRC / "both.c")

    def test_non_source_files_are_ignored(self):
        self.write("notes.txt", "x")
        self.write("header.h", "x")
        self.assertEqual(list(SP.iter_sources()), [])
        self.assertIsNone(SP.path_for("header"))


if __name__ == "__main__":
    unittest.main()
