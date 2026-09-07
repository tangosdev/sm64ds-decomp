"""A banner is the only thing that excuses an asm body; a dcd blob without one demotes."""
import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import asm_policy  # noqa: E402

DCD_DUMP = ("asm void func_ov002_020cfea4(void) {\n"
            "    dcd 0xe92d43f0\n"
            "    dcd 0xe12fff1e\n"
            "}\n")
MNEMONIC_ASM = ("asm void Copy32Bytes(void)\n"
                "{\n"
                "    ldmia r0!, {r2, r3, ip}\n"
                "    stmia r1!, {r2, r3, ip}\n"
                "    bx lr\n"
                "}\n")
PLAIN_C = "int Example(void) {\n    return 0;\n}\n"


class Classify(unittest.TestCase):
    def test_unbannered_dcd_dump_is_transcribed(self):
        self.assertEqual(asm_policy.classify(DCD_DUMP), "transcribed")

    def test_nonmatching_banner_makes_a_dcd_dump_an_honest_draft(self):
        self.assertIsNone(asm_policy.classify("// NONMATCHING\n" + DCD_DUMP))

    def test_hand_asm_banner_excuses_an_asm_body(self):
        self.assertIsNone(asm_policy.classify(
            "// HAND-ASM PRIMITIVE: byte-faithful asm-block match.\n" + MNEMONIC_ASM))

    def test_mnemonic_asm_without_banner_is_the_warn_tier(self):
        self.assertEqual(asm_policy.classify(MNEMONIC_ASM), "unbannered-asm")

    def test_plain_c_is_none(self):
        self.assertIsNone(asm_policy.classify(PLAIN_C))

    def test_embedded_asm_hatch_in_real_c_is_warned_not_demoted(self):
        # The gray zone: a C function with an inline asm hatch (CP15 intrinsics,
        # Player::InitResources) must never be swept into "transcribed".
        hatch = ("unsigned int GetControl(void){\n"
                 "    unsigned int v;\n"
                 "    asm { mrc p15,0,v,c1,c0,0 }\n"
                 "    return v;\n"
                 "}\n")
        self.assertEqual(asm_policy.classify(hatch), "unbannered-asm")

    def test_banner_anywhere_in_the_file_is_exculpatory(self):
        # The banner search is whole-file, not head-200: a banner can only ever
        # excuse a file, so a deep mention must count.
        deep = DCD_DUMP + "\n" * 20 + "// NONMATCHING: see the wall analysis above\n"
        self.assertIsNone(asm_policy.classify(deep))

    # -- precision regressions: the 38-file warn-list audit found 32 false ------
    # -- positives from register-named locals and disassembly-trace comments ----

    def test_register_named_locals_opening_blocks_are_not_a_register_list(self):
        # Largest false-positive class (26 files): real C whose locals are named
        # r0..r8 opening blocks. `{ r5 = ...` is an assignment, not an STM list.
        c = ("int Setup(void) {\n"
             "    int r0; int r4; int r5;\n"
             "    struct Obj* r2;\n"
             "    { r5 = 0x10000; }\n"
             "    { r4 = LoadFile(0x30); }\n"
             "    { r0 = r4 + r5; }\n"
             "    { r0->a = 0; }\n"
             "    return r0;\n"
             "}\n")
        self.assertIsNone(asm_policy.classify(c))

    def test_r2c_style_prefix_names_are_not_registers(self):
        # `r2c` starts with `r2`; the word boundary after the register name must
        # keep the r\d prefix from matching inside a longer identifier.
        self.assertIsNone(asm_policy.classify(
            "void f(void) {\n"
            "    { r2c = 0; }\n"
            "    { r2c = 0; }\n"
            "    { r2c = 0; }\n"
            "}\n"))

    def test_disassembly_trace_comments_do_not_fire_the_backstop(self):
        # Second false-positive class (6 files): disassembly traces / call maps
        # kept in comments are documentation ABOUT asm, not an asm body.
        traced = ("// call map:\n"
                  "// mov r0,r2; bl 0x0205a61c\n"
                  "// bl 0x02037764 = RaycastLine::~RaycastLine\n"
                  "/* ldr r1,[sp,#4]; bx lr */\n"
                  "int Example(void) {\n"
                  "    return Helper(2);\n"
                  "}\n")
        self.assertIsNone(asm_policy.classify(traced))

    def test_register_list_tell_matches_lists_not_c_blocks(self):
        # The tightened tell must keep every real STM/PUSH list form and reject
        # every audited C-block shape.
        for s in ("{r4,r5,lr}", "{ r0 - r3 }", "{r4-r11,lr}", "{r2, r3, ip}", "{lr}"):
            self.assertTrue(asm_policy._ARM_TELLS.search(s), s)
        for s in ("{ r5 = 0x10000;", "{ r4 = LoadFile(0x30);",
                  "{ r0->a = 0; }", "{ r2c = 0;"):
            self.assertFalse(asm_policy._ARM_TELLS.search(s), s)

    def test_real_register_lists_in_an_asm_body_are_still_detected(self):
        # A genuine unbannered transcription must still be caught after the
        # tightening -- lists and ranges alike, without the `asm` keyword.
        self.assertEqual(asm_policy.classify(
            "void func(void) {\n"
            "    stmdb sp!, {r4 - r6, lr}\n"
            "    ldmia r0!, {r2, r3, ip}\n"
            "    ldmia sp!, {r4-r6, pc}\n"
            "}\n"), "unbannered-asm")

    def test_dcd_dump_with_trace_comments_is_still_transcribed(self):
        # Comment stripping guards the asm detection; a real dcd blob next to a
        # commentary line must still demote.
        self.assertEqual(
            asm_policy.classify("// raw words follow\n" + DCD_DUMP), "transcribed")

    def test_banner_is_read_from_raw_text_before_comment_stripping(self):
        # Banners ARE comments and are only exculpatory: the stripping that
        # guards detection must never run before the banner search.
        self.assertIsNone(asm_policy.classify(
            "/* HAND-ASM PRIMITIVE: byte-faithful asm-block match. */\n" + MNEMONIC_ASM))


if __name__ == "__main__":
    unittest.main()
