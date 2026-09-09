"""Regression tests for tools/check_literal_load.py.

The bug shape: a matched function's ROM body loads a pool word relocs.txt names
as `kind:load to:<addr>`, and the C source spells that destination address as a
bare integer literal (decimal or hex) instead of naming the symbol relocs.txt
already records there. func_02065b94 and func_02065b88 (SIGFIX3) are the two
real hits this tool was written to generalize; these tests exercise the parser
and scanner in isolation so the census stays trustworthy on the rest of the tree.
"""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import check_literal_load as CL


SYMBOLS_TEXT = """\
data_020a94d4 kind:bss addr:0x020a94d4
func_02065b94 kind:function(arm,size=0xc) addr:0x02065b94
func_02065b98 kind:function(arm,size=0x0) addr:0x02065b94
func_02065ba0 kind:function(arm,size=0x18) addr:0x02065ba0
"""


class TestParseFunctions(unittest.TestCase):
    def test_reads_name_addr_size(self):
        fns = CL.parse_functions(SYMBOLS_TEXT)
        self.assertIn(("func_02065b94", 0x02065b94, 0xc), fns)

    def test_ignores_bss_and_data_lines(self):
        fns = CL.parse_functions(SYMBOLS_TEXT)
        self.assertTrue(all(name != "data_020a94d4" for name, _a, _s in fns))

    def test_zero_size_alias_is_kept(self):
        """An mwccarm-emitted alias at the same address (size=0x0) owns no
        relocations of its own -- it must survive parsing so load_relocs_in_range
        can correctly find nothing for it, rather than the row silently vanishing
        one step earlier and looking like it was never considered."""
        fns = CL.parse_functions(SYMBOLS_TEXT)
        self.assertIn(("func_02065b98", 0x02065b94, 0x0), fns)


RELOCS_BY_FROM = {
    0x02065b9c: ("load", 0x020a94d4, "arm9"),
    0x02065ba4: ("arm_call", 0x02059d30, "arm9"),
    0x02065bb0: ("load", 0x020a9570, "arm9"),
}


class TestLoadRelocsInRange(unittest.TestCase):
    def test_finds_a_load_inside_the_function(self):
        hits = CL.load_relocs_in_range(RELOCS_BY_FROM, 0x02065b94, 0xc)
        self.assertEqual(hits, [(0x02065b9c, 0x020a94d4, "arm9")])

    def test_ignores_non_load_kinds(self):
        """An arm_call relocation is a callee address, not a pool load -- the
        wrong-callee shape is check_call_arity's/the earlier SIGFIX sweeps'
        territory, not this one's."""
        hits = CL.load_relocs_in_range(RELOCS_BY_FROM, 0x02065ba0, 0x8)
        self.assertEqual(hits, [])

    def test_ignores_relocations_outside_the_range(self):
        hits = CL.load_relocs_in_range(RELOCS_BY_FROM, 0x02065bb0, 0x4)
        self.assertEqual(hits, [(0x02065bb0, 0x020a9570, "arm9")])
        hits_before = CL.load_relocs_in_range(RELOCS_BY_FROM, 0x02065b94, 0x4)
        self.assertEqual(hits_before, [])  # 0x9c is one word past this range


class TestLiteralHits(unittest.TestCase):
    def test_decimal_literal_matches(self):
        hits = CL.literal_hits("return 34247892;", 0x020a94d4)
        self.assertEqual(hits, ["34247892"])

    def test_hex_literal_matches(self):
        hits = CL.literal_hits("return 0x020a94d4;", 0x020a94d4)
        self.assertEqual(hits, ["0x020a94d4"])

    def test_no_hit_for_a_different_value(self):
        self.assertEqual(CL.literal_hits("return 34247890;", 0x020a94d4), [])

    def test_no_hit_when_the_number_is_part_of_a_longer_token(self):
        """0x020a94d40 (one extra digit) must not read as a match for 0x020a94d4."""
        self.assertEqual(CL.literal_hits("return 0x020a94d40;", 0x020a94d4), [])


MATCHED_LITERAL_SOURCE = "int func_02065b94(void)\n{\n    return 34247892;\n}\n"
MATCHED_NAMED_SOURCE = ("extern unsigned char data_020a94d4[];\n"
                        "void *func_02065b94(void)\n{\n    return data_020a94d4;\n}\n")
DRAFT_LITERAL_SOURCE = ("// NONMATCHING: unresolved (div=1). Logic verified correct vs ROM;\n"
                        "// not byte-exact yet.\n" + MATCHED_LITERAL_SOURCE)
COMMENTED_LITERAL_SOURCE = "int func_02065b94(void)\n{\n    /* was 34247892 */\n    return 0;\n}\n"

SYM_INDEX = {("arm9", 0x020a94d4): "data_020a94d4"}
FUNCTIONS = [("func_02065b94", 0x02065b94, 0xc)]
RELOCS_ONE = {0x02065b9c: ("load", 0x020a94d4, "arm9")}


class TestScanModule(unittest.TestCase):
    def test_flags_a_literal_spelling_the_load_destination(self):
        rows, scanned, no_source, drafts = CL.scan_module(
            "arm9", FUNCTIONS, RELOCS_ONE, SYM_INDEX,
            lambda name: MATCHED_LITERAL_SOURCE)
        self.assertEqual(len(rows), 1)
        row = rows[0]
        self.assertEqual(row["name"], "func_02065b94")
        self.assertEqual(row["to"], "0x020a94d4")
        self.assertEqual(row["sym"], "data_020a94d4")
        self.assertEqual(row["literal"], "34247892")
        self.assertEqual((scanned, no_source, drafts), (1, 0, 0))

    def test_stays_silent_once_the_symbol_is_named(self):
        rows, *_ = CL.scan_module("arm9", FUNCTIONS, RELOCS_ONE, SYM_INDEX,
                                   lambda name: MATCHED_NAMED_SOURCE)
        self.assertEqual(rows, [])

    def test_skips_a_nonmatching_draft(self):
        """A NONMATCHING file has not reproduced the ROM at all -- flagging its
        literal would recommend fixing a signature nobody has proven matches."""
        rows, scanned, no_source, drafts = CL.scan_module(
            "arm9", FUNCTIONS, RELOCS_ONE, SYM_INDEX,
            lambda name: DRAFT_LITERAL_SOURCE)
        self.assertEqual(rows, [])
        self.assertEqual(drafts, 1)

    def test_a_literal_inside_a_comment_does_not_count(self):
        rows, *_ = CL.scan_module("arm9", FUNCTIONS, RELOCS_ONE, SYM_INDEX,
                                   lambda name: COMMENTED_LITERAL_SOURCE)
        self.assertEqual(rows, [])

    def test_no_source_file_is_counted_not_flagged(self):
        rows, scanned, no_source, drafts = CL.scan_module(
            "arm9", FUNCTIONS, RELOCS_ONE, SYM_INDEX, lambda name: None)
        self.assertEqual(rows, [])
        self.assertEqual((scanned, no_source), (0, 1))

    def test_a_function_with_no_load_in_range_is_never_visited(self):
        """No candidate means text_getter is not even called -- the scan does not
        pay to read a source file it has no reason to suspect."""
        calls = []

        def getter(name):
            calls.append(name)
            return MATCHED_LITERAL_SOURCE

        rows, scanned, no_source, drafts = CL.scan_module(
            "arm9", FUNCTIONS, {}, SYM_INDEX, getter)
        self.assertEqual(rows, [])
        self.assertEqual(calls, [])

    def test_unrelated_literal_in_a_matched_function_is_silent(self):
        source = "int func_02065b94(void)\n{\n    return 12;\n}\n"
        rows, *_ = CL.scan_module("arm9", FUNCTIONS, RELOCS_ONE, SYM_INDEX,
                                   lambda name: source)
        self.assertEqual(rows, [])

    def test_unnamed_destination_reports_sym_none(self):
        rows, *_ = CL.scan_module("arm9", FUNCTIONS, RELOCS_ONE, {},
                                   lambda name: MATCHED_LITERAL_SOURCE)
        self.assertEqual(len(rows), 1)
        self.assertIsNone(rows[0]["sym"])

    def test_a_load_can_cross_modules(self):
        """func_02058df4 (arm9) loads data_023c0000, which is defined in the dtcm
        symbols.txt, not arm9's -- looking the destination up under the CALLER's
        module (a bug this test pins) silently reports no symbol for something
        that has one. The reloc's own recorded module wins the lookup instead."""
        relocs_cross = {0x02065b9c: ("load", 0x020a94d4, "dtcm")}
        sym_index_dtcm = {("dtcm", 0x020a94d4): "data_020a94d4"}
        rows, *_ = CL.scan_module("arm9", FUNCTIONS, relocs_cross, sym_index_dtcm,
                                   lambda name: MATCHED_LITERAL_SOURCE)
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0]["sym"], "data_020a94d4")
        self.assertEqual(rows[0]["to_module"], "dtcm")

    def test_hand_asm_primitive_is_not_flagged(self):
        """Entry's CRT0 boot trampoline spells every pool load as a raw
        `ldr r0, =0x...` operand inside an asm block -- that is how asm is
        written, not the C-hides-a-symbol bug this census hunts, and it has no
        C form to rewrite it into."""
        asm_source = ("// HAND-ASM PRIMITIVE: byte-faithful asm-block match.\n"
                      "asm void func_02065b94(void) {\n"
                      "    ldr     r0, =0x020a94d4\n"
                      "    bx      lr\n"
                      "}\n")
        rows, *_ = CL.scan_module("arm9", FUNCTIONS, RELOCS_ONE, SYM_INDEX,
                                   lambda name: asm_source)
        self.assertEqual(rows, [])


if __name__ == "__main__":
    unittest.main()
