// NONMATCHING: arg-build scheduling (div=5, was 21). Logic verified correct vs ROM.
// #pragma opt_propagation off + `int result = 0` reproduces the ROM's single
// unconditional mov r0,#0 shared by both predicated guard epilogues (second guard
// sets no r0 at all). Residual: final call's arg schedule - ROM emits
// strb|lsl|strc|asr|ldrb|mov r1|mov r0, mwccarm emits strb|mov r1|lsl|asr|strc|mov r0|ldrb
// (mask's mov r1,r8 hoisted early, ldrb sunk last). Swept: named/inline arg temps,
// fake-dep ternaries on p2/mask/e[2], nested-call args, K&R prototype, u64-launder
// on mask (adds insn), volatile e[2], opt_dead_assignments off, optimize_for_size on.
// Counts as decompiled, not matched.
int func_02065970(void);
int func_02065bc0(void);
int func_02065bd0(void);
int func_02064eac(int p0, unsigned short mask, unsigned char e2, short a, int b, int c);

#pragma opt_propagation off
int func_02063e08(char* obj, int idx, int p2)
{
    unsigned char* e = (unsigned char*)(obj + 0x1d4 + idx * 0x68);
    unsigned short mask = (unsigned short)(1 << idx);
    int result = 0;
    if (e[0] != 2) return result;
    if (e[1] != 2) return result;
    result = func_02064eac(p2, mask, e[2], (short)func_02065970(), func_02065bc0(), func_02065bd0());
    e[0] = 1;
    return result;
}
