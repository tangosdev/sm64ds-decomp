"""The source-text overrides pr_linkcheck applies to a link verdict.

Regression test for a NameError that sat in this branch from #1367 until a PR finally
produced a NO-REPRO. The rule reads the module as `asm_policy` while the import binds
it as `AP`, so the moment the gate had a failing file to grade it raised instead --
and the validator reported "worker error: pr_linkcheck exited 1 without a report"
rather than the verdict. Nothing caught it because the branch is only reachable when a
file fails, which no green run does; these cases drive it directly.
"""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import pr_linkcheck as PL  # noqa: E402

DRAFT = "// NONMATCHING\nvoid f(void) { }\n"
PLAIN = "void f(void) { }\n"
DCD = ("asm void func_ov002_020cfea4(void) {\n"
       "    dcd 0xe92d43f0\n"
       "    dcd 0xe12fff1e\n"
       "}\n")


class SourcePolicy(unittest.TestCase):
    def test_draft_banner_downgrades_no_repro(self):
        """The case that raised: a declared draft that does not reproduce."""
        self.assertEqual(PL.source_policy("NO-REPRO", DRAFT), "DRAFT")

    def test_unbannered_no_repro_still_fails(self):
        """A file claiming to be a match must not be waved through."""
        self.assertEqual(PL.source_policy("NO-REPRO", PLAIN), "NO-REPRO")

    def test_draft_banner_does_not_excuse_wrong_dest(self):
        """A draft's call graph must be honest even when its bytes differ."""
        self.assertEqual(PL.source_policy("WRONG", DRAFT), "WRONG")

    def test_verified_is_untouched(self):
        self.assertEqual(PL.source_policy("VERIFIED", PLAIN), "VERIFIED")

    def test_transcription_is_raw_asm(self):
        self.assertEqual(PL.source_policy("VERIFIED", DCD), "RAW-ASM")

    def test_bannered_transcription_is_a_draft_not_raw_asm(self):
        """The collision the inline comment claims cannot happen -- pinned here."""
        self.assertEqual(PL.source_policy("NO-REPRO", "// NONMATCHING\n" + DCD), "DRAFT")

    def test_empty_text_is_inert(self):
        """check_file passes "" when the path cannot be read; it must not reclassify."""
        self.assertEqual(PL.source_policy("NO-REPRO", ""), "NO-REPRO")


if __name__ == "__main__":
    unittest.main()
