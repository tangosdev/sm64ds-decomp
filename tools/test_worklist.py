import importlib
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
worklist = importlib.import_module("worklist")
srcpath = importlib.import_module("srcpath")


class TargetIsDoneTest(unittest.TestCase):
    def setUp(self):
        # worklist resolves sources through srcpath now, so the fixture tree is
        # installed there rather than on a module-level path of worklist's own.
        self.temp_dir = tempfile.TemporaryDirectory()
        self.src = pathlib.Path(self.temp_dir.name) / "src"
        self.src.mkdir()
        self.original_repo = srcpath.set_root(self.temp_dir.name)[0]

    def tearDown(self):
        srcpath.set_root(self.original_repo)
        self.temp_dir.cleanup()

    def test_ledger_entry_is_done(self):
        done = {("arm9", 0x02001234)}
        self.assertTrue(
            worklist.target_is_done(done, "arm9", 0x02001234, "func_02001234")
        )

    def test_c_source_is_done_when_ledger_lags(self):
        (self.src / "func_02001234.c").write_text(
            "void func_02001234(void) {}\n", encoding="utf-8"
        )
        self.assertTrue(
            worklist.target_is_done(set(), "arm9", 0x02001234, "func_02001234")
        )

    def test_cpp_source_is_done_when_ledger_lags(self):
        (self.src / "func_02001234.cpp").write_text(
            "//cpp\nvoid func_02001234() {}\n", encoding="utf-8"
        )
        self.assertTrue(
            worklist.target_is_done(set(), "arm9", 0x02001234, "func_02001234")
        )

    def test_source_in_a_subdirectory_is_found(self):
        # The reason worklist resolves through srcpath at all: a function whose file has
        # been filed under a module/class directory is still done, not still available.
        nested = self.src / "unnamed" / "ov063"
        nested.mkdir(parents=True)
        (nested / "func_02001234.c").write_text(
            "void func_02001234(void) {}\n", encoding="utf-8"
        )
        self.assertTrue(
            worklist.target_is_done(set(), "arm9", 0x02001234, "func_02001234")
        )

    def test_missing_source_and_ledger_entry_remains_available(self):
        self.assertFalse(
            worklist.target_is_done(set(), "arm9", 0x02001234, "func_02001234")
        )


if __name__ == "__main__":
    unittest.main()
