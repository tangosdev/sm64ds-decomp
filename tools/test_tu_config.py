"""The TU-shape generator must refuse to lose or double-claim a byte.

`config/**/delinks.txt` decides which file owns which bytes. Collapsing 41
per-function entries into 6 per-TU entries is a rewrite of that ownership map,
and both of its failure modes are silent: an OVERLAP hands the same bytes to two
files (dsd picks one, and which one is not something a byte gate would ever
report), a GAP quietly moves bytes from a named file into the module's gap
object, where they still link and still match. Neither shows up downstream. So
they have to be caught here, at the moment the file is written.
"""
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tu_config as TC  # noqa: E402

SECS = {".text": (0x02000000, 0x02000100, "code"),
        ".data": (0x02000100, 0x02000200, "data")}
ORIGINAL = (
    "    .text start:0x02000000 end:0x02000100 kind:code\n"
    "    .data start:0x02000100 end:0x02000200 kind:data\n"
    "src/a.c:\n    complete\n    .text start:0x02000000 end:0x02000080\n\n"
    "src/b.c:\n    complete\n    .text start:0x02000080 end:0x02000100\n")


class Verify(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.path = pathlib.Path(self.tmp.name) / "delinks.txt"
        self.path.write_text(ORIGINAL, encoding="utf-8")

    def tearDown(self):
        self.tmp.cleanup()

    def run_verify(self, entries, fns=(0x02000000, 0x02000080)):
        return TC.verify("modX", self.path, entries, SECS, list(fns))

    def test_the_faithful_rewrite_passes(self):
        stats = self.run_verify([("src_tu/X.cpp", {".text": (0x02000000, 0x02000100)})])
        self.assertEqual(stats["text_bytes"], 0x100)
        self.assertEqual(stats["functions"], 2)

    def test_overlapping_text_entries_are_rejected(self):
        with self.assertRaises(TC.TuConfigError) as e:
            self.run_verify([("src_tu/X.cpp", {".text": (0x02000000, 0x02000090)}),
                             ("src_tu/Y.cpp", {".text": (0x02000080, 0x02000100)})])
        self.assertIn("OVERLAP", str(e.exception))

    def test_a_gap_in_text_is_rejected(self):
        with self.assertRaises(TC.TuConfigError) as e:
            self.run_verify([("src_tu/X.cpp", {".text": (0x02000000, 0x02000040)}),
                             ("src_tu/Y.cpp", {".text": (0x02000080, 0x02000100)})])
        self.assertIn("no longer cover", str(e.exception))

    def test_growing_past_the_old_union_is_allowed_but_measured(self):
        """The per-function config leaves holes wherever a function was never
        enrollable -- a Thumb stub, a 2-byte-aligned address, a NONMATCHING hatch.
        A TU span covering one of those is a correction, not a regression, so it
        is allowed; it is measured so it can never happen unnoticed."""
        self.path.write_text(ORIGINAL.replace("end:0x02000100\n", "end:0x020000c0\n"),
                             encoding="utf-8")
        stats = self.run_verify(
            [("src_tu/X.cpp", {".text": (0x02000000, 0x02000100)})])
        self.assertEqual(stats["textGrewBy"], 0x40)

    def test_growth_is_rejected_under_strict_text(self):
        self.path.write_text(ORIGINAL.replace("end:0x02000100\n", "end:0x020000c0\n"),
                             encoding="utf-8")
        with self.assertRaises(TC.TuConfigError) as e:
            TC.verify("modX", self.path,
                      [("src_tu/X.cpp", {".text": (0x02000000, 0x02000100)})],
                      SECS, [0x02000000, 0x02000080], strict_text=True)
        self.assertIn("coverage changed under --strict-text", str(e.exception))

    def test_a_range_outside_the_declared_section_is_rejected(self):
        with self.assertRaises(TC.TuConfigError) as e:
            self.run_verify([("src_tu/X.cpp", {".text": (0x02000000, 0x02000100),
                                               ".data": (0x02000180, 0x02000280)})])
        self.assertIn("not inside the declared section", str(e.exception))

    def test_a_function_claimed_twice_is_named(self):
        """Overlapping entries keep the right UNION, so the coverage test passes
        them; it is the per-function count that says which function two files now
        both claim, and that is the only line a reader can act on."""
        with self.assertRaises(TC.TuConfigError) as e:
            self.run_verify([("src_tu/X.cpp", {".text": (0x02000000, 0x020000c0)}),
                             ("src_tu/Y.cpp", {".text": (0x02000080, 0x02000100)})])
        self.assertNotIn("coverage changed", str(e.exception))
        self.assertIn("function at 0x02000080 is in 2 .text entries",
                      str(e.exception))


class LinkOrderGate(unittest.TestCase):
    """Attribution is discarded wholesale when it is not in link order.

    Object k's contribution to every section precedes object k+1's. A rule that
    infers ownership from local signals can only be trusted if its answers come
    out in that order; when they do not, the evidence is about some other object
    and there is no safe way to salvage part of it."""

    def make(self, owners):
        att = TC.Attribution("modX", [(0, 1, []), (1, 2, []), (2, 3, [])],
                             {".data": (0x100, 0x200, "data")})
        TC._fill_and_gate(att, ".data", owners, [], att.tus, "test")
        return att

    def test_ordered_owners_are_claimed_and_the_holes_filled(self):
        att = self.make({(0x100, 0x110): 0, (0x140, 0x150): 0, (0x180, 0x190): 2})
        self.assertEqual(att.ranges[0][".data"], (0x100, 0x150))
        self.assertNotIn(".data", att.ranges[1])
        self.assertEqual(att.ranges[2][".data"], (0x180, 0x190))

    def test_out_of_order_owners_are_discarded_entirely(self):
        att = self.make({(0x100, 0x110): 1, (0x180, 0x190): 0})
        self.assertEqual([r.get(".data") for r in att.ranges], [None, None, None])
        self.assertEqual([v for (_s, v, _d) in att.report], ["unattributed"])


if __name__ == "__main__":
    unittest.main()
