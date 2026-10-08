"""src_move: the rewrite that must not miss a reference, and the placement it follows.

The load-bearing property is the same one layout_check L1 guards: a moved source whose
delinks.txt row still names the old path silently builds from ROM bytes. So these tests
pin (a) that the rekey regex rewrites exactly the moved names and never a longer sibling,
(b) that canonical JSON stays canonical after keys change order, and (c) that the plan
sends every symbol to the directory `srcpath` would have put a NEW file in."""
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import layout_check as LC  # noqa: E402
import src_move as SM  # noqa: E402
import srcpath as SP  # noqa: E402


def mv(old, new, kind="unnamed", module="arm9"):
    return SM.Move(old, new, kind, module)


class Rekey(unittest.TestCase):
    def setUp(self):
        self.moves = [mv("src/func_02000000.c", "src/unnamed/arm9/0200/func_02000000.c"),
                      mv("src/func_02000004.cpp", "src/unnamed/arm9/0200/func_02000004.cpp")]
        self.pat = SM.build_pattern(self.moves)
        self.dest = SM._dest_by_name(self.moves)

    def sub(self, text):
        return SM.rekey_text(text, self.pat, self.dest)

    def test_rewrites_a_delinks_entry_and_a_hash_key(self):
        out, n = self.sub("src/func_02000000.c:\n  src/func_02000004.cpp#sym\n")
        self.assertEqual(n, 2)
        self.assertIn("src/unnamed/arm9/0200/func_02000000.c:", out)
        self.assertIn("src/unnamed/arm9/0200/func_02000004.cpp#sym", out)

    def test_a_moved_c_does_not_rewrite_an_unmoved_cpp_sibling(self):
        out, n = self.sub("src/func_02000000.cpp stays")
        self.assertEqual((out, n), ("src/func_02000000.cpp stays", 0))

    def test_does_not_touch_an_already_nested_path(self):
        text = "src/unnamed/arm9/0200/func_02000000.c"
        self.assertEqual(self.sub(text), (text, 0))

    def test_relative_and_trailing_punctuation_forms(self):
        out, n = self.sub("see ../src/func_02000000.c. And `src/func_02000004.cpp`,")
        self.assertEqual(n, 2)
        self.assertIn("../src/unnamed/arm9/0200/func_02000000.c.", out)

    def test_a_name_that_merely_ends_the_same_is_not_rewritten(self):
        text = "src/xfunc_02000000.c and mysrc/func_02000000.c"
        self.assertEqual(self.sub(text), (text, 0))


class CanonicalJson(unittest.TestCase):
    def setUp(self):
        self.moves = [mv("src/a.c", "src/z/a.c"), mv("src/m.c", "src/b/m.c")]
        self.pat = SM.build_pattern(self.moves)
        self.dest = SM._dest_by_name(self.moves)

    def test_sorted_keys_and_lists_stay_sorted(self):
        orig = json.dumps({"src/a.c": 1, "src/m.c": 2, "files": ["src/a.c", "src/m.c"]},
                          indent=2, sort_keys=True) + "\n"
        out, n, canonical = SM.rekey_json(orig, self.pat, self.dest)
        self.assertTrue(canonical)
        self.assertEqual(n, 4)
        want = json.dumps({"src/z/a.c": 1, "src/b/m.c": 2,
                           "files": ["src/b/m.c", "src/z/a.c"]},
                          indent=2, sort_keys=True) + "\n"
        self.assertEqual(out, want)

    def test_unsorted_document_keeps_its_order(self):
        orig = json.dumps({"src/m.c": 1, "src/a.c": 2}, indent=2) + "\n"
        out, n, canonical = SM.rekey_json(orig, self.pat, self.dest)
        self.assertEqual(list(json.loads(out)), ["src/b/m.c", "src/z/a.c"])

    def test_non_canonical_formatting_falls_back_to_plain_replace(self):
        orig = '{"src/a.c":   1}\n'
        out, n, canonical = SM.rekey_json(orig, self.pat, self.dest)
        self.assertFalse(canonical)
        self.assertEqual(out, '{"src/z/a.c":   1}\n')

    def test_sorted_text_file_is_resorted_but_comments_stay_put(self):
        orig = "# header\nsrc/a.c x\nsrc/m.c y\n"
        out, n, _ = SM.rekey_lines(orig, self.pat, self.dest)
        self.assertEqual(out, "# header\nsrc/b/m.c y\nsrc/z/a.c x\n")


class PlanFollowsPlacement(unittest.TestCase):
    """A scratch tree with a few root files and a symbols.txt saying who lives where."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        (self.repo / "src").mkdir()
        mod = self.repo / "config" / "arm9" / "overlays" / "ov006"
        mod.mkdir(parents=True)
        (self.repo / "config" / "arm9" / "symbols.txt").write_text(
            "FreeFn kind:function(arm,size=0x4) addr:0x02001000\n")
        (mod / "symbols.txt").write_text(
            "_ZN7daTrs_c6RenderEv kind:function(arm,size=0x4) addr:0x02100000\n"
            "_ZN7daTrs_c4ThinkEv kind:function(arm,size=0x4) addr:0x02100010\n")
        self._saved = (SP.REPO, SP.SRC)
        SP.set_root(self.repo)
        for name in ("func_02045678.c", "func_ov006_02100020.c", "FreeFn.c",
                     "_ZN7daTrs_c6RenderEv.cpp", "_ZN7daTrs_c4ThinkEv.cpp"):
            (SP.SRC / name).write_text("int f(void){return 0;}\n")
        SP.invalidate()

    def tearDown(self):
        SP.set_root(self._saved[0])
        self.tmp.cleanup()

    def destinations(self, *selects):
        moves, skipped = SM.plan(list(selects), files=[])
        return {pathlib.PurePosixPath(m.old).name: m.new for m in moves}, skipped

    def test_arm9_address_file_is_sharded_on_the_high_half(self):
        d, _ = self.destinations("unnamed:arm9")
        self.assertEqual(d, {"func_02045678.c": "src/unnamed/arm9/0204/func_02045678.c"})

    def test_overlay_address_file_goes_to_its_module(self):
        d, _ = self.destinations("unnamed:overlays")
        self.assertEqual(d, {"func_ov006_02100020.c": "src/unnamed/ov006/func_ov006_02100020.c"})

    def test_named_goes_to_named_module_and_a_class_stays_together(self):
        d, _ = self.destinations("named")
        self.assertEqual(d["FreeFn.c"], "src/named/arm9/FreeFn.c")
        self.assertEqual(d["_ZN7daTrs_c6RenderEv.cpp"], "src/named/ov006/_ZN7daTrs_c6RenderEv.cpp")
        self.assertEqual(d["_ZN7daTrs_c4ThinkEv.cpp"], "src/named/ov006/_ZN7daTrs_c4ThinkEv.cpp")

    def test_a_file_port_names_by_literal_path_is_never_moved(self):
        moves, skipped = SM.plan(["all"], files=["port/hal/x.cpp"])
        # `files` only names what port_pinned scans; no port file exists on disk here.
        self.assertTrue(moves)
        (self.repo / "port" / "hal").mkdir(parents=True)
        (self.repo / "port" / "hal" / "x.cpp").write_text('#include "../../src/FreeFn.c"\n')
        moves, skipped = SM.plan(["all"], files=["port/hal/x.cpp"])
        self.assertNotIn("src/FreeFn.c", [m.old for m in moves])
        self.assertIn("src/FreeFn.c", skipped)

    def test_apply_then_layout_check_is_clean_and_delinks_follow(self):
        delinks = self.repo / "config" / "arm9" / "delinks.txt"
        delinks.write_text("src/func_02045678.c:\n    .text start:0x02045678 end:0x0204567c\n")
        moves, _ = SM.plan(["unnamed:arm9"], files=[])
        # apply() stages through git; a scratch dir is not a repo, so exercise the two
        # halves it is made of: the rename and the rekey.
        for m in moves:
            (SP.REPO / m.new).parent.mkdir(parents=True, exist_ok=True)
            (SP.REPO / m.old).rename(SP.REPO / m.new)
        n, _, changed = SM.rekey_file("config/arm9/delinks.txt", SM.build_pattern(moves),
                                      SM._dest_by_name(moves))
        self.assertEqual((n, changed), (1, True))
        self.assertTrue(delinks.read_text().startswith("src/unnamed/arm9/0204/func_02045678.c:"))
        SP.invalidate()
        found = LC.check(self.repo / "config", known=set())
        self.assertEqual(found["L1"], [])
        self.assertEqual(found["L3"], [])


class LayoutRules(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.repo = pathlib.Path(self.tmp.name)
        (self.repo / "src").mkdir()
        (self.repo / "config" / "arm9").mkdir(parents=True)
        self._saved = (SP.REPO, SP.SRC, LC.REPO)
        SP.set_root(self.repo)
        LC.REPO = self.repo

    def tearDown(self):
        SP.set_root(self._saved[0])
        LC.REPO = self._saved[2]
        self.tmp.cleanup()

    def write(self, rel):
        p = SP.SRC / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text("int f(void){return 0;}\n")
        SP.invalidate()

    def test_L3_flags_the_wrong_arm9_shard(self):
        self.write("unnamed/arm9/0205/func_02045678.c")
        r = LC.check(self.repo / "config", known=set())
        self.assertEqual(len(r["L3"]), 1)
        self.assertIn("unnamed/arm9/0204/", r["L3"][0]["why"])

    def test_L3_accepts_the_right_arm9_shard(self):
        self.write("unnamed/arm9/0204/func_02045678.c")
        self.assertEqual(LC.check(self.repo / "config", known=set())["L3"], [])

    def test_L6_flags_an_address_name_under_named(self):
        self.write("named/arm9/func_02045678.c")
        r = LC.check(self.repo / "config", known=set())
        self.assertEqual(len(r["L6"]), 1)

    def test_placement_uses_the_shard_once_the_bucket_exists(self):
        self.assertIsNone(SP.placement_for("func_02045678"))
        (SP.SRC / "unnamed" / "arm9").mkdir(parents=True)
        self.assertEqual(SP.placement_for("func_02045678"), SP.SRC / "unnamed/arm9/0204")
        self.assertEqual(SP.new_path_for("func_02045678", "c"),
                         SP.SRC / "unnamed/arm9/0204/func_02045678.c")

    def test_rename_out_of_unnamed_leaves_the_shard(self):
        self.write("unnamed/arm9/0204/func_02045678.c")
        (self.repo / "config" / "arm9" / "symbols.txt").write_text(
            "FreeFn kind:function(arm,size=0x4) addr:0x02045678\n")
        SP.invalidate()
        old = SP.SRC / "unnamed/arm9/0204/func_02045678.c"
        # No named/ bucket yet: the shard directory is not a legal home for a named
        # symbol, so the file falls back to the root.
        self.assertEqual(SP.rename_target(old, "FreeFn"), SP.SRC / "FreeFn.c")
        (SP.SRC / "named" / "arm9").mkdir(parents=True)
        self.assertEqual(SP.rename_target(old, "FreeFn"), SP.SRC / "named/arm9/FreeFn.c")


if __name__ == "__main__":
    unittest.main()
