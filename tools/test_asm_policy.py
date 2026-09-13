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


HAND_ASM = ("// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no\n"
            "// original C to recover and no match to chase.\n"
            "// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Per asm policy.\n"
            + MNEMONIC_ASM)


class CountsAsMatched(unittest.TestCase):
    """Tango's ruling, 2026-09-09: byte-exact hand-written assembly counts."""

    def test_plain_c_counts(self):
        self.assertTrue(asm_policy.counts_as_matched(PLAIN_C))

    def test_a_draft_banner_alone_does_not_count(self):
        self.assertFalse(asm_policy.counts_as_matched("// NONMATCHING\n" + PLAIN_C))

    def test_hand_asm_banner_alone_counts(self):
        self.assertTrue(asm_policy.counts_as_matched(
            "// HAND-ASM PRIMITIVE: byte-faithful asm-block match.\n" + MNEMONIC_ASM))

    def test_both_banners_count_because_hand_asm_wins(self):
        # The twenty files this ruling is about. They say in their own headers that
        # there is no C to chase, and then print the word NONMATCHING; the count read
        # only the second half and left them out.
        self.assertTrue(asm_policy.counts_as_matched(HAND_ASM))

    def test_unbannered_dcd_dump_never_counts(self):
        self.assertFalse(asm_policy.counts_as_matched(DCD_DUMP))

    def test_a_dcd_dump_cannot_be_laundered_by_the_hand_asm_banner(self):
        # The clause that keeps the ruling from reopening the vacuous match: a raw
        # word dump under both banners is still a transcription, whatever it claims.
        self.assertFalse(asm_policy.counts_as_matched(
            "// NONMATCHING\n"
            "// HAND-ASM PRIMITIVE: byte-faithful asm-block match.\n" + DCD_DUMP))

    def test_a_deep_hand_asm_mention_does_not_rescue_a_draft(self):
        # has_draft_banner searches the header region and is exculpatory, so a loose
        # search is safe there. This banner puts a file INTO the count, so it has to
        # be a header claim, not the phrase turning up in a wall analysis.
        deep = ("// NONMATCHING: register allocation wall.\n"
                + MNEMONIC_ASM
                + "\n// why not a HAND-ASM PRIMITIVE: the body is ordinary ARM.\n")
        self.assertFalse(asm_policy.counts_as_matched(deep))

    def test_has_hand_banner_reads_the_header_region_only(self):
        self.assertTrue(asm_policy.has_hand_banner(HAND_ASM))
        self.assertFalse(asm_policy.has_hand_banner(
            PLAIN_C + "\n// HAND-ASM PRIMITIVE mentioned in a trailing note\n"))

    def test_the_ordinary_arm_draft_still_does_not_count(self):
        # notes/asm-policy.md's second row, untouched: ordinary ARM the compiler
        # cannot yet reproduce is an unsolved matching problem, not a primitive.
        self.assertFalse(asm_policy.counts_as_matched(
            "// NONMATCHING: scheduling wall, see the analysis below.\n" + PLAIN_C))

    def test_classify_is_unchanged_by_the_ruling(self):
        # The acceptance criteria and the transcription gate are untouched: only the
        # yes/no the counting tools ask for is new.
        self.assertEqual(asm_policy.classify(DCD_DUMP), "transcribed")
        self.assertEqual(asm_policy.classify(MNEMONIC_ASM), "unbannered-asm")
        self.assertIsNone(asm_policy.classify(HAND_ASM))


if __name__ == "__main__":
    unittest.main()
