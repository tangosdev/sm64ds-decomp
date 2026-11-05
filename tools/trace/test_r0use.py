"""Offline tests for r0use.check: does a function read r0 before writing it?

  python -m unittest tools/trace/test_r0use.py

Hand-encoded ARM words; no ROM, no emulator. Skipped without capstone.
"""
import os
import struct
import sys
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import r0use  # noqa: E402

LDR_R1_R0_8 = 0xe5901008    # ldr r1, [r0, #8]
MOV_R0_1 = 0xe3a00001       # mov r0, #1
MOVEQ_R0_1 = 0x03a00001     # moveq r0, #1
BX_LR = 0xe12fff1e          # bx lr
ADD_R0_SP_8 = 0xe28d0008    # add r0, sp, #8
PUSH_R4_LR = 0xe92d4010     # push {r4, lr}
MOV_R4_R0 = 0xe1a04000      # mov r4, r0
CMP_R1_0 = 0xe3510000       # cmp r1, #0
BL_NEXT = 0xeb000000        # bl <pc + 8>
BNE_PLUS8 = 0x1a000000      # bne <pc + 8>
ADDR = 0x02000000


def code(*words):
    return b''.join(struct.pack('<I', w) for w in words)


@unittest.skipIf(r0use.Cs is None, 'capstone not installed')
class CheckTest(unittest.TestCase):
    def kind(self, *words):
        r = r0use.check(code(*words), ADDR)
        return r['r0_used'], r['r0_first_use'].split('@')[0]

    def test_base(self):
        self.assertEqual(self.kind(LDR_R1_R0_8, BX_LR), (True, 'base'))

    def test_read(self):
        self.assertEqual(self.kind(PUSH_R4_LR, MOV_R4_R0, BX_LR), (True, 'read'))

    def test_overwritten_like_func_020092c4(self):
        r = r0use.check(code(PUSH_R4_LR, CMP_R1_0, ADD_R0_SP_8, BX_LR), ADDR)
        self.assertFalse(r['r0_used'])
        self.assertTrue(r['r0_first_use'].startswith('overwritten@+0x8 add r0, sp'), r)

    def test_call_passes_r0_on(self):
        self.assertEqual(self.kind(PUSH_R4_LR, BL_NEXT, BX_LR), (True, 'call'))

    def test_unused(self):
        self.assertEqual(self.kind(CMP_R1_0, BX_LR), (False, 'unused'))

    def test_conditional_write_leaves_r0_live(self):
        # moveq r0, #1 writes r0 on one path only; the next instruction still reads the entry r0
        self.assertEqual(self.kind(CMP_R1_0, MOVEQ_R0_1, LDR_R1_R0_8, BX_LR), (True, 'base'))

    def test_both_branch_sides_walked(self):
        # bne skips the overwrite; the taken side reads r0 as a base
        self.assertEqual(self.kind(CMP_R1_0, BNE_PLUS8, MOV_R0_1, LDR_R1_R0_8, BX_LR), (True, 'base'))

    def test_no_bytes(self):
        self.assertIsNone(r0use.check(b'', ADDR)['r0_used'])


if __name__ == '__main__':
    unittest.main()
