"""Tests for tools/class_rename.py's rewrite rules.

The mangled rule rewrote `<len><Old>` wherever it appeared inside a `_Z...`
token, without looking at what came before the length. A `<len>` is a whole
digit run, so `5Model` after a digit is the tail of some other source name:
renaming `Model` turned `15ModelComponents` into `117dExtSimpleModel_c...`
(71 mangled tokens in the tree) and the actor names `12WcObj05Model` and
`12Mecha05Model` likewise. The idempotence check could not see it, because the
corrupted text is just as stable under a second pass as the right text.
"""
import os
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import class_rename  # noqa: E402

OLD, NEW = "Model", "dExtSimpleModel_c"


def rename(text, old=OLD, new=NEW, with_derived=False):
    return class_rename.apply_rules(text, class_rename.rules(old, new, with_derived))


class MangledDigitGuard(unittest.TestCase):
    def test_longer_class_with_the_name_as_a_prefix_is_untouched(self):
        for text in ("_ZN15ModelComponentsD1Ev",
                     "_ZTV15ModelComponents",
                     "_ZN15ModelComponents7DestroyEv"):
            self.assertEqual(rename(text), text)

    def test_name_ending_in_a_digit_then_the_class_is_untouched(self):
        for text in ("_ZTV12WcObj05Model",
                     "_ZN12Mecha05ModelD0Ev"):
            self.assertEqual(rename(text), text)

    def test_genuine_mangled_names_are_rewritten(self):
        self.assertEqual(rename("_ZN5Model4DrawEv"),
                         "_ZN17dExtSimpleModel_c4DrawEv")
        self.assertEqual(rename("_ZTV5Model"), "_ZTV17dExtSimpleModel_c")

    def test_nested_name_is_rewritten(self):
        self.assertEqual(rename("_ZN5Model4NodeC1Ev"),
                         "_ZN17dExtSimpleModel_c4NodeC1Ev")
        self.assertEqual(rename("_ZN3Foo3BarERK5Model"),
                         "_ZN3Foo3BarERK17dExtSimpleModel_c")

    def test_only_the_real_token_changes_when_both_appear(self):
        text = "_ZN15ModelComponents4InitEP5Model"
        self.assertEqual(rename(text),
                         "_ZN15ModelComponents4InitEP17dExtSimpleModel_c")

    def test_plain_identifiers_keep_their_word_boundary(self):
        text = "ModelComponents mc; WcObj05Model w; Model m;"
        self.assertEqual(rename(text),
                         "ModelComponents mc; WcObj05Model w; dExtSimpleModel_c m;")

    def test_second_pass_is_a_no_op(self):
        once = rename("_ZN5Model4DrawEv _ZTV15ModelComponents")
        self.assertEqual(rename(once), once)


if __name__ == "__main__":
    unittest.main()
