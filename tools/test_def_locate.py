#!/usr/bin/env python3
"""The member slice has to compile to the symbol it was bound to.

A namespace or extern "C" block is linkage, not decoration. Cutting at the
function line kept the closing brace (it sits before the next member) and
dropped the opening line, so tubuild create handed the compiler a stray
`}` and a function in the wrong scope.
"""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import def_locate as DL  # noqa: E402

ROOT = pathlib.Path(__file__).resolve().parents[1]


def _balanced(frag):
    return frag.count("{") == frag.count("}")


class MemberFragmentScopeTest(unittest.TestCase):
    def test_private_namespace_block_stays_around_the_function(self):
        text = (
            "int before(int a) { return a; }\n"
            "\n"
            "namespace cstd {\n"
            "\n"
            "int fdiv(int numerator, int denominator)\n"
            "{\n"
            "    return numerator;\n"
            "}\n"
            "\n"
            "}\n"
            "\n"
            "namespace cstd {\n"
            "int ldiv(int a, int b) { return a; }\n"
            "}\n")
        owned = ["_Z6beforei", "_ZN4cstd4fdivEii", "_ZN4cstd4ldivEii"]
        bound, _defs = DL.bind("src/slice.cpp", text, owned)
        frag = DL.member_fragment(text, "_ZN4cstd4fdivEii", bound)
        self.assertIn("namespace cstd", frag)
        self.assertIn("fdiv(", frag)
        self.assertNotIn("ldiv(", frag)
        self.assertNotIn("before(", frag)
        self.assertTrue(_balanced(frag), frag)
        before = DL.member_fragment(text, "_Z6beforei", bound)
        self.assertNotIn("namespace", before)
        self.assertTrue(_balanced(before), before)

    def test_extern_c_block_stays_around_a_flat_function(self):
        text = (
            'extern "C" {\n'
            "int func_02053200(int x)\n"
            "{\n"
            "    return x;\n"
            "}\n"
            "}\n"
            "\n"
            "int other(int x) { return x; }\n")
        bound, _defs = DL.bind(
            "src/slice.cpp", text, ["func_02053200", "_Z5otheri"])
        frag = DL.member_fragment(text, "func_02053200", bound)
        self.assertIn('extern "C"', frag)
        self.assertIn("func_02053200", frag)
        self.assertNotIn("other(", frag)
        self.assertTrue(_balanced(frag), frag)

    def test_shared_namespace_is_repeated_around_each_member(self):
        text = (
            "namespace cstd {\n"
            "int fdiv(int a, int b) { return a; }\n"
            "int ldiv(int a, int b) { return b; }\n"
            "}\n")
        owned = ["_ZN4cstd4fdivEii", "_ZN4cstd4ldivEii"]
        bound, _defs = DL.bind("src/slice.cpp", text, owned)
        for sym, keep, drop in (
                ("_ZN4cstd4fdivEii", "fdiv(", "ldiv("),
                ("_ZN4cstd4ldivEii", "ldiv(", "fdiv(")):
            frag = DL.member_fragment(text, sym, bound)
            self.assertIn("namespace cstd", frag)
            self.assertIn(keep, frag)
            self.assertNotIn(drop, frag)
            self.assertTrue(_balanced(frag), frag)

    def test_a_qualified_method_is_not_given_a_namespace(self):
        text = "void A::One() { x(); }\n\nvoid A::Two() { y(); }\n"
        bound, _defs = DL.bind(
            "src/slice.cpp", text, ["_ZN1A3OneEv", "_ZN1A3TwoEv"])
        frag = DL.member_fragment(text, "_ZN1A3OneEv", bound)
        self.assertIn("A::One", frag)
        self.assertNotIn("A::Two", frag)
        self.assertNotIn("namespace", frag)
        self.assertTrue(_balanced(frag), frag)

    def test_real_cstd_slices_keep_their_blocks(self):
        path = ROOT / "src" / "cstd.cpp"
        text = path.read_text(encoding="utf-8")
        bound, _defs = DL.bind(str(path), text)
        fdiv = DL.member_fragment(text, "_ZN4cstd4fdivEii", bound)
        self.assertIsNotNone(fdiv)
        self.assertIn("namespace cstd", fdiv)
        self.assertIn("fdiv(", fdiv)
        self.assertNotIn("ldiv(", fdiv)
        self.assertTrue(_balanced(fdiv), fdiv)
        flat = DL.member_fragment(text, "func_02053200", bound)
        self.assertIsNotNone(flat)
        self.assertIn('extern "C"', flat)
        self.assertIn("func_02053200", flat)
        self.assertNotIn("fdiv(", flat)
        self.assertTrue(_balanced(flat), flat)


if __name__ == "__main__":
    unittest.main()
