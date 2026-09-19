#!/usr/bin/env python3
"""Two `tubuild.py create` defects that each turned a generated TU into one a human
had to repair by hand before it would compile.

Both were found by a fold hitting them, not by review, so each test here carries its
own POSITIVE CONTROL: the thing the generator used to do, asserted to still be broken.
A regression test that cannot fail is not evidence.
"""
import sys
import pathlib
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import tubuild  # noqa: E402

MARK = "// @" + "symbol "   # spelled in two halves on purpose: a literal marker in a
                            # file the tiers and decl-agreement gates both scan reads
                            # as a real one, and this file defines no such symbol.


def closes_early(block):
    """True when a `/* ... */` block ends before its last line.

    This is the defect stated the way the compiler sees it: the first `*/` after the
    opener closes the comment, and everything after it is code again.
    """
    assert block.startswith("/*")
    return block.index("*/", 2) != len(block) - 2


class ConflictBodyTest(unittest.TestCase):
    """Defect A: carried text containing `*/` closed its own conflict marker."""

    # A shadow struct as legacy files really carry them -- with a comment inside.
    LEGACY = "struct Obj {  /* the ROM's own field order */\n    int x;\n};"

    def block(self, text):
        return ("/* TUBUILD CONFLICT -- alternate body of struct 'Obj', from the "
                "legacy file for func_0212af74, NOT applied:\n" + text + "\n*/")

    def test_positive_control_raw_text_closes_the_marker_early(self):
        """Unfixed, the block really does end at the inner comment's terminator."""
        self.assertTrue(closes_early(self.block(self.LEGACY)))

    def test_carried_text_no_longer_closes_the_marker(self):
        self.assertFalse(closes_early(self.block(tubuild._conflict_body(self.LEGACY))))

    def test_the_text_is_still_readable(self):
        """Neutralised, not dropped: a human must still see what was not applied."""
        out = tubuild._conflict_body(self.LEGACY)
        self.assertIn("the ROM's own field order", out)
        self.assertIn("int x;", out)
        self.assertEqual(out.count("*" + chr(92) + "/"), 1)

    def test_text_without_a_terminator_is_untouched(self):
        plain = "struct Obj { int x; };"
        self.assertEqual(tubuild._conflict_body(plain), plain)

    def test_every_terminator_is_neutralised_not_just_the_first(self):
        two = "/* a */ int x; /* b */"
        self.assertFalse(closes_early(self.block(tubuild._conflict_body(two))))


class PromotedMemberFragmentTest(unittest.TestCase):
    """Defect B: a member already folded into a promoted TU carried the WHOLE TU.

    `srcpath.path_for` answers with the file the linker reads. Once a symbol has been
    folded, that file holds every member of the promoted TU, while
    `split_legacy_source` documents that it assumes a one-function file -- so `create`
    carried all of them, once per ROM ordinal.
    """

    TU = (MARK + "_ZN1A3OneEv\n"
          "void A::One() { x(); }\n\n"
          + MARK + "_ZN1A3TwoEv\n"
          "void A::Two() { y(); }\n")

    def frag(self, owned, text=None, name="_ZN1A3OneEv"):
        with mock.patch.object(tubuild.SP, "symbols_for", return_value=owned):
            return tubuild._promoted_member_fragment(
                pathlib.Path("src/actors/A.cpp"),
                self.TU if text is None else text, name)

    def test_one_function_legacy_file_is_left_entirely_alone(self):
        """The common case must behave exactly as it always did."""
        self.assertIsNone(self.frag(["_ZN1A3OneEv"]))
        self.assertIsNone(self.frag([]))

    def test_multi_symbol_tu_yields_only_the_asked_for_member(self):
        fragment, owned = self.frag(["_ZN1A3OneEv", "_ZN1A3TwoEv"])
        self.assertEqual(owned, 2)
        self.assertIn("A::One", fragment)
        self.assertNotIn("A::Two", fragment)

    def test_positive_control_the_whole_file_holds_both(self):
        """What create used to carry, so the test above can actually fail."""
        self.assertIn("A::One", self.TU)
        self.assertIn("A::Two", self.TU)

    def test_an_unsliceable_member_refuses_rather_than_guessing(self):
        fragment, owned = self.frag(["_ZN1A3OneEv", "_ZN1A3TwoEv"],
                                    text="void A::One() {}\nvoid A::Two() {}\n")
        self.assertIsNone(fragment)
        self.assertEqual(owned, 2)

    def test_a_member_named_twice_is_also_refused(self):
        doubled = self.TU + MARK + "_ZN1A3OneEv\nvoid A::One() {}\n"
        fragment, _owned = self.frag(["_ZN1A3OneEv", "_ZN1A3TwoEv"], text=doubled)
        self.assertIsNone(fragment)


if __name__ == "__main__":
    unittest.main()
