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


if __name__ == "__main__":
    unittest.main()
