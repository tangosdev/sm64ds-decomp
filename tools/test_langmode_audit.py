"""What langmode_audit counts as an optimizer-pragma override, and as a
hand-spelled mangled definition."""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import langmode_audit as L  # noqa: E402


class Pragmas(unittest.TestCase):
    def test_settings_count_and_resets_are_separable(self):
        text = ("#pragma opt_common_subs off\n"
                "void f(void) {}\n"
                "#pragma opt_common_subs reset\n"
                "#pragma optimize_for_size on\n"
                "#pragma optimization_level 2\n"
                "  #  pragma opt_strength_reduction on\n")
        got = L.find_pragmas(text)
        self.assertEqual(got, [("opt_common_subs", "off"),
                               ("opt_common_subs", "reset"),
                               ("optimize_for_size", "on"),
                               ("optimization_level", "2"),
                               ("opt_strength_reduction", "on")])
        self.assertEqual(sum(1 for _, v in got if v != "reset"), 4)

    def test_comments_and_strings_are_ignored(self):
        text = ("// #pragma opt_common_subs off\n"
                "/* x\n#pragma opt_propagation off\n*/\n"
                'const char *s = "\n#pragma opt_lifetimes off";\n'
                "#pragma opt_loop_invariants off\n")
        self.assertEqual(L.find_pragmas(text), [("opt_loop_invariants", "off")])

    def test_unrelated_pragmas_are_ignored(self):
        text = ("#pragma push\n#pragma pop\n#pragma defer_codegen on\n"
                "#pragma long_calls on\n#pragma thumb on\n"
                "#pragma section code_type begin\n#pragma once\n"
                "#pragma optimizer_other on\n")
        self.assertEqual(L.find_pragmas(text), [])


class HandSpelled(unittest.TestCase):
    """`hand_spells_own_symbol` must score code, never prose."""

    def spells(self, text, stem="_ZN12dMgCardObj_cC1Ev"):
        real = L._source_text
        L._source_text = lambda _p: text
        try:
            return L.hand_spells_own_symbol("x.cpp", stem)
        finally:
            L._source_text = real

    def test_real_definition_counts(self):
        self.assertTrue(self.spells(
            'extern "C" void _ZN12dMgCardObj_cC1Ev(void *self)\n{\n}\n'))

    def test_pointer_return_definition_counts(self):
        self.assertTrue(self.spells(
            "int *_ZN12dMgCardObj_cC1Ev(int *t) {\n}\n"))

    def test_declaration_is_not_a_definition(self):
        self.assertFalse(self.spells(
            "void _ZN12dMgCardObj_cC1Ev(void *self);\n"
            "void other(void)\n{\n}\n"))

    def test_call_is_not_a_definition(self):
        self.assertFalse(self.spells(
            "void other(void) {\n"
            "    if (_ZN12dMgCardObj_cC1Ev(self)) {\n        g();\n    }\n}\n"))

    def test_block_comment_continuation_naming_the_symbol(self):
        """The d_s_mg_card.cpp regression: prose indented without a leading `*`,
        whose `[^;{]*` then bridges `*/` into the next real function's brace."""
        self.assertFalse(self.spells(
            "/* `new` inlines the implicit constructor, as the ROM's\n"
            "   _ZN12dMgCardObj_cC1Ev (0x020dbe30) and _ZN17dMgDilarCardObj_cC1Ev\n"
            "   (0x020dbe14), the last two functions of the run. */\n"
            'extern "C" void *dScMgCard_c_classInit()\n'
            "{\n    return new dScMgCard_c;\n}\n"))

    def test_line_comment_naming_the_symbol(self):
        self.assertFalse(self.spells(
            "// _ZN12dMgCardObj_cC1Ev(self) {  is prose\n"
            "void other(void)\n{\n}\n"))

    def test_string_literal_naming_the_symbol(self):
        self.assertFalse(self.spells(
            'const char *s = "_ZN12dMgCardObj_cC1Ev(x) {";\n'
            "void other(void)\n{\n}\n"))


if __name__ == "__main__":
    unittest.main()
