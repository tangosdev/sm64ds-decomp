"""What langmode_audit counts as an optimizer-pragma override."""
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


if __name__ == "__main__":
    unittest.main()
