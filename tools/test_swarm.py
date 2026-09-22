"""Regression tests for swarm's native deleting-destructor preference."""

import pathlib
import sys
import tempfile
import types
import unittest
from unittest import mock

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

import swarm as S  # noqa: E402


class NativeDeletingDestructorTests(unittest.TestCase):
    def setUp(self):
        self._old_repo = S.REPO
        self._tmp = tempfile.TemporaryDirectory()
        S.REPO = pathlib.Path(self._tmp.name)
        (S.REPO / "include").mkdir()

    def tearDown(self):
        S.REPO = self._old_repo
        self._tmp.cleanup()

    def write_header(self, name, text):
        (S.REPO / "include" / f"{name}.h").write_text(text, encoding="utf-8")

    @staticmethod
    def deleting_dtor_insns():
        mnemonics = ["push", "ldr", "mov", "str", "bl", "ldr", "mov", "ldr",
                     "bl", "mov", "pop", "bx"]
        operands = ["{r4,lr}", "r1,[pc,#0]", "r4,r0", "r1,[r4]", "", "r1,[pc,#0]",
                    "r0,r4", "r1,[r1]", "", "r0,r4", "{r4,lr}", "lr"]
        return [types.SimpleNamespace(mnemonic=mn, op_str=op, address=i * 4)
                for i, (mn, op) in enumerate(zip(mnemonics, operands))]

    def run_rule(self):
        ins = self.deleting_dtor_insns()
        relocs = {0x1010: "member-dtor", 0x1020: "deallocator"}
        with mock.patch.object(S.R, "name_for_reloc", side_effect=lambda entry, _syms: entry):
            return S.rule_deleting_dtor(
                "_ZN4FishD0Ev", ins, b"", 0x1000, relocs, {}
            )

    def test_real_derived_header_emits_native_cpp(self):
        self.write_header("Fish", "struct Fish : dActor_c { virtual ~Fish(); };\n")
        source = S.native_deleting_dtor("_ZN4FishD0Ev")
        self.assertIsNotNone(source)
        self.assertTrue(source.startswith("//cpp\n// @symbol _ZN4FishD0Ev\n"))
        self.assertIn('#include "Fish.h"', source)
        self.assertIn("Fish::~Fish()", source)
        self.assertNotIn('extern "C"', source)
        self.assertNotIn("operator delete", source)

    def test_rule_prefers_native_cpp_when_real_header_exists(self):
        self.write_header("Fish", "struct Fish : dActor_c { virtual ~Fish(); };\n")
        source, label = self.run_rule()
        self.assertEqual(label, "deleting_dtor_native")
        self.assertIn('#include "Fish.h"', source)
        self.assertNotIn("deallocator", source)

    def test_public_base_spelling_is_accepted(self):
        self.write_header("Widget", "class Widget : public Base { virtual ~Widget(); };\n")
        self.assertIsNotNone(S.native_deleting_dtor("_ZN6WidgetD0Ev"))

    def test_flat_header_keeps_the_transcription_fallback_available(self):
        self.write_header("Fish", "struct Fish { virtual ~Fish(); };\n")
        self.assertIsNone(S.native_deleting_dtor("_ZN4FishD0Ev"))

    def test_rule_uses_transcription_only_for_flat_header(self):
        self.write_header("Fish", "struct Fish { virtual ~Fish(); };\n")
        source, label = self.run_rule()
        self.assertEqual(label, "deleting_dtor")
        self.assertIn("deallocator", source)
        self.assertFalse(source.startswith("//cpp"))

    def test_header_without_virtual_destructor_is_not_treated_as_owner(self):
        self.write_header("Fish", "struct Fish : dActor_c { ~Fish(); };\n")
        self.assertIsNone(S.native_deleting_dtor("_ZN4FishD0Ev"))

    def test_mangled_length_must_name_the_whole_class(self):
        self.write_header("Fish", "struct Fish : dActor_c { virtual ~Fish(); };\n")
        self.assertIsNone(S.native_deleting_dtor("_ZN5FishD0Ev"))


if __name__ == "__main__":
    unittest.main()
