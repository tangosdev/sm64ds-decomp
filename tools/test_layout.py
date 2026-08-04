"""What layout_check has to catch.

L1 is the one that matters: a delinks.txt naming a path with no file there makes the
function fall back to retail ROM bytes, so the ROM build still comes out byte-identical
and nothing goes red. If only one of these tests is ever kept, keep that one."""
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import layout_check as LC  # noqa: E402
import srcpath as SP  # noqa: E402


class LayoutCheck(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        (self.repo / "src").mkdir()
        (self.repo / "config" / "m").mkdir(parents=True)
        self._saved = (SP.REPO, SP.SRC, LC.REPO)
        SP.REPO, SP.SRC = self.repo, self.repo / "src"
        LC.REPO = self.repo
        SP.invalidate()

    def tearDown(self):
        SP.REPO, SP.SRC, LC.REPO = self._saved
        SP.invalidate()
        self.tmp.cleanup()

    def write(self, rel):
        p = SP.SRC / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text("int f(void){return 0;}\n")
        SP.invalidate()
        return p

    def delinks(self, *entries):
        (self.repo / "config" / "m" / "delinks.txt").write_text(
            "".join(f"{e}:\n    .text start:0x0 end:0x4\n" for e in entries))
        return self.repo / "config"

    def run_check(self, cfg, known=frozenset()):
        return LC.check(cfg, known=set(known))

    # --- L1: the silent one -------------------------------------------------
    def test_L1_catches_a_delinks_path_with_no_file(self):
        self.write("unnamed/ov006/func_ov006_02100000.c")
        cfg = self.delinks("src/func_ov006_02100000.c")     # stale flat path
        r = self.run_check(cfg)
        self.assertEqual(len(r["L1"]), 1)
        self.assertEqual(r["L1"][0]["actually_at"],
                         "src/unnamed/ov006/func_ov006_02100000.c")

    def test_L1_clean_when_delinks_matches_disk(self):
        self.write("unnamed/ov006/func_ov006_02100000.c")
        cfg = self.delinks("src/unnamed/ov006/func_ov006_02100000.c")
        self.assertEqual(self.run_check(cfg)["L1"], [])

    # --- L2 -----------------------------------------------------------------
    def test_L2_catches_a_symbol_with_two_files(self):
        self.write("dup.c")
        self.write("sub/dup.cpp")
        r = self.run_check(self.delinks())
        self.assertEqual([h["key"] for h in r["L2"]], ["dup"])

    # --- L3 -----------------------------------------------------------------
    def test_L3_catches_a_named_symbol_in_an_unnamed_bucket(self):
        self.write("unnamed/ov006/_ZN3Boo6RenderEv.c")
        r = self.run_check(self.delinks())
        self.assertEqual(len(r["L3"]), 1)
        self.assertIn("not an address", r["L3"][0]["why"])

    def test_L3_catches_the_wrong_module_bucket(self):
        self.write("unnamed/ov006/func_ov002_02100000.c")
        r = self.run_check(self.delinks())
        self.assertEqual(len(r["L3"]), 1)
        self.assertIn("ov002", r["L3"][0]["why"])

    def test_L3_allows_the_right_module_bucket(self):
        self.write("unnamed/ov006/func_ov006_02100000.c")
        self.assertEqual(self.run_check(self.delinks())["L3"], [])

    # --- L4 -----------------------------------------------------------------
    def test_L4_catches_a_class_split_across_directories(self):
        self.write("actors/Boo/Boo_Spawn.cpp")
        self.write("unnamed/ov006/_ZN3Boo6RenderEv.c")
        r = self.run_check(self.delinks())
        self.assertEqual([h["key"] for h in r["L4"]], ["Boo"])
        self.assertEqual(r["L4"][0]["dirs"], ["actors/Boo", "unnamed/ov006"])

    def test_L4_ignores_flat_files(self):
        """The root is the unmigrated default, not a second home."""
        self.write("actors/Boo/Boo_Spawn.cpp")
        self.write("_ZN3Boo6RenderEv.c")
        self.assertEqual(self.run_check(self.delinks())["L4"], [])

    # --- waivers ------------------------------------------------------------
    def test_a_known_issue_is_waived_but_still_counted(self):
        self.write("dup.c")
        self.write("sub/dup.cpp")
        r = self.run_check(self.delinks(), known={("L2", "dup")})
        self.assertEqual(r["L2"], [])
        self.assertEqual([w["check"] for w in r["waived"]], ["L2"])

    def test_a_waiver_does_not_leak_across_checks(self):
        self.write("unnamed/ov006/_ZN3Boo6RenderEv.c")
        r = self.run_check(self.delinks(), known={("L2", "src/unnamed/ov006/_ZN3Boo6RenderEv.c")})
        self.assertEqual(len(r["L3"]), 1)

    def test_known_issues_file_parses_comments_and_blanks(self):
        p = self.repo / "known.txt"
        p.write_text("# a comment\n\nL2 sym_one\nL4 SomeClass   # trailing\n")
        self.assertEqual(LC.known_issues(p), {("L2", "sym_one"), ("L4", "SomeClass")})


if __name__ == "__main__":
    unittest.main()
