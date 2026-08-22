// NONMATCHING: one loop-induction-variable difference and its fallout. Logic
// verified instruction-for-instruction against
// extracted/overlays/overlay_0006.bin at base 0x020bfec0. Counts as
// decompiled, not matched.
//
// dScMgPachinko2_c (actor id 0x171, scene 369). func_ov006_0210246c, the
// fifteen-state dispatcher, calls this with (self, entry) immediately after
// every state it dispatches -- unconditionally, inside the guarded arm, at
// 0x021024c4 -- and it had no body at all. It is the Lakitu-versus-shell
// catch test, so the port could launch shells and never catch one.
//
// It early-outs unless the carrier's state index at +0x5296 is in 1..11, then
// walks all 0x30 shells. A shell counts only if it is live (+0x4698), at least
// state 2 (+0x4699) and rising faster than 0x2000 (+0x466c). Inside a 0x30 by
// 0..8 box around the carrier the horizontal distance picks one of three
// outcomes: within 0x10 the shell is caught -- the carrier's catch counter at
// +0x5299 goes up, the shell is retired and func_ov006_02100408,
// func_02012718(0x19a) and func_ov006_02101148 run; within 0x20 it is
// deflected with the carrier's own velocity plus or minus 0x1800 and bounced
// at -0x2000; beyond that it is batted straight back at plus or minus 0x1800
// and -0x2800. Both deflections play sound 0x19b.
//
// COMPILER: mwccarm 2004/b56. Measured: -O4,p at 2004/b56 is the ONLY
// combination in a sweep of 25 compiler builds times seven -O settings times
// four -proc values that reproduces the ROM's size of 0x24c at all.
//
// 147 words = 137 instructions (0x0210076c..0x02100990) + 10 pool words
// (0x02100990..0x021009b4) = 0x24c/4. The candidate is the same 0x24c and the
// same 137 instructions.
//
// THE ONE CAUSE. mwcc strength-reduces the shell index into an induction
// variable and the ROM does not:
//
//     ROM         add r5, sb, r8, lsl #6      ; r8 is j, shifted in the
//                 add r8, r8, #1              ; addressing mode
//                 cmp r8, #0x30
//
//     candidate   add r5, sb, r8              ; r8 is j * 0x40, kept
//                 add r8, r8, #0x40          ; incremented by the stride
//                 ldr r0, [sp, #4]           ; and j itself spilled, which
//                 add r0, r0, #1             ; is the extra stack slot
//                 str r0, [sp, #4]           ; (0x24 of frame vs the ROM's
//                 cmp r0, #0x30              ;  0x1c)
//
// Everything else in the diff is that difference propagating: one more live
// value shifts the stack slot numbering by four, and the tighter register
// budget makes mwcc if-convert the +-0x1800 deflection arm that the ROM
// branches around. Nothing computes a different value and no instruction is
// missing or added.
//
// LEVERS ALREADY SPENT, all recorded in nearmiss/db.jsonl: both
// access-expression spellings for both indices in all four combinations,
// opt_strength_reduction / opt_loop_invariants / opt_common_subs /
// opt_lifetimes / opt_markcounterloops / opt_movepostops on and off, a Row
// struct of 0x40 bytes indexed by the counter (the idiom
// unmatched/MgPachinko2_StateDispatch.cpp records for this class's own
// dispatcher), a loop-local base pointer, j * 0x40 instead of j << 6,
// unsigned j, -inline off and every -proc. The best of all of them is the
// spelling below.
//
// The full diff, ROM on the left and candidate on the right:
//
//   02100770 | sub      sp, sp, #0x1c                   | sub      sp, sp, #0x24
//   0210078C | addeq    sp, sp, #0x1c                   | addeq    sp, sp, #0x24
//   0210079C | addhs    sp, sp, #0x1c                   | addhs    sp, sp, #0x24
//   021007B4 | str      r0, [sp, #4]                    | str      r0, [sp, #8]
//   021007BC | ldr      r2, [pc, #0x1d4]                | str      r0, [sp, #0xc]
//   021007C0 | str      r0, [sp, #8]                    | mov      r0, #0x1800
//   021007C4 | mov      r0, #0x1800                     | str      r0, [sp, #0x14]
//   021007C8 | str      r0, [sp, #0x10]                 | mov      r0, #0x2800
//   021007D0 | ldr      r3, [pc, #0x1c4]                | ldr      r2, [pc, #0x1c0]
//   021007D4 | add      r7, r4, r2                      | ldr      r3, [pc, #0x1c0]
//   021007D8 | mov      r1, #0x2800                     | mov      r8, #0
//   021007DC | str      r0, [sp, #0x14]                 | mov      r1, #0x2000
//   021007E0 | rsb      r0, r1, #0                      | str      r0, [sp, #0x1c]
//   021007E4 | mov      r2, #0x2000                     | rsb      r0, r1, #0
//   021007E8 | str      r0, [sp, #0x18]                 | str      r0, [sp, #0x10]
//   021007EC | rsb      r0, r2, #0                      | ldr      r0, [sp, #0x14]
//   021007F0 | mov      r8, #0                          | str      r8, [sp, #4]
//   021007F4 | add      r6, r4, r3                      | rsb      r0, r0, #0
//   021007F8 | mov      r4, r8                          | add      r7, r4, r2
//   021007FC | str      r0, [sp, #0xc]                  | add      r6, r4, r3
//   02100800 | ldr      r0, [pc, #0x198]                | mov      r4, r8
//   02100804 | add      r5, sb, r8, lsl #6              | str      r0, [sp, #0x18]
//   02100808 | add      fp, r5, r0                      | ldr      r0, [pc, #0x190]
//   0210080C | ldrb     r0, [fp]                        | add      r5, sb, r8
//   02100810 | cmp      r0, #0                          | add      fp, r5, r0
//   02100814 | beq      #0x2100978                      | ldrb     r0, [fp]
//   02100818 | add      r1, r5, #0x4000                 | cmp      r0, #0
//   0210081C | ldrb     r0, [r1, #0x699]                | beq      #0x210096c
//   02100820 | cmp      r0, #2                          | add      r1, r5, #0x4000
//   02100824 | blo      #0x2100978                      | ldrb     r0, [r1, #0x699]
//   02100828 | ldr      r0, [pc, #0x174]                | cmp      r0, #2
//   0210082C | add      lr, r5, r0                      | blo      #0x210096c
//   02100830 | ldr      r0, [lr]                        | ldr      r0, [pc, #0x16c]
//   02100834 | cmp      r0, #0x2000                     | add      lr, r5, r0
//   02100838 | ble      #0x2100978                      | ldr      r0, [lr]
//   0210083C | ldr      r0, [sp, #4]                    | cmp      r0, #0x2000
//   02100840 | ldr      r2, [r1, #0x660]                | ble      #0x210096c
//   02100844 | ldr      r3, [r0]                        | ldr      r0, [sp, #8]
//   02100848 | ldr      r0, [pc, #0x158]                | ldr      r2, [r1, #0x660]
//   0210084C | add      ip, r5, r0                      | ldr      r3, [r0]
//   02100850 | sub      r0, r3, r2                      | ldr      r0, [pc, #0x150]
//   02100854 | asr      sl, r0, #0xc                    | add      ip, r5, r0
//   02100858 | ldr      r0, [sp, #8]                    | sub      r0, r3, r2
//   0210085C | ldr      r2, [ip]                        | asr      sl, r0, #0xc
//   02100860 | ldr      r3, [r0]                        | ldr      r0, [sp, #0xc]
//   02100864 | mvn      r0, #0x2f                       | ldr      r2, [ip]
//   02100868 | sub      r2, r3, r2                      | ldr      r3, [r0]
//   0210086C | cmp      sl, r0                          | mvn      r0, #0x2f
//   02100870 | asr      r0, r2, #0xc                    | sub      r2, r3, r2
//   02100874 | blt      #0x2100978                      | cmp      sl, r0
//   02100878 | cmp      sl, #0x30                       | asr      r0, r2, #0xc
//   0210087C | bgt      #0x2100978                      | blt      #0x210096c
//   02100880 | cmp      r0, #0                          | cmp      sl, #0x30
//   02100884 | blt      #0x2100978                      | bgt      #0x210096c
//   02100888 | cmp      r0, #8                          | cmp      r0, #0
//   0210088C | bgt      #0x2100978                      | blt      #0x210096c
//   02100890 | mvn      r0, #0xf                        | cmp      r0, #8
//   02100894 | cmp      sl, r0                          | bgt      #0x210096c
//   02100898 | blt      #0x21008ec                      | mvn      r0, #0xf
//   0210089C | cmp      sl, #0x10                       | cmp      sl, r0
//   021008A0 | bgt      #0x21008ec                      | blt      #0x21008f4
//   021008A4 | ldrb     r2, [r7]                        | cmp      sl, #0x10
//   021008A8 | ldr      r0, [pc, #0xfc]                 | bgt      #0x21008f4
//   021008AC | add      r2, r2, #1                      | ldrb     r2, [r7]
//   021008B0 | strb     r2, [r7]                        | ldr      r0, [pc, #0xf4]
//   021008B4 | strb     r4, [r1, #0x69a]                | add      r2, r2, #1
//   021008B8 | add      r5, r5, r0                      | strb     r2, [r7]
//   021008BC | strb     r4, [fp]                        | strb     r4, [r1, #0x69a]
//   021008C0 | ldr      r2, [ip]                        | add      r5, r5, r0
//   021008C4 | ldr      r1, [r5]                        | strb     r4, [fp]
//   021008C8 | mov      r0, sb                          | ldr      r2, [ip]
//   021008D0 | ldr      r1, [r5]                        | mov      r0, sb
//   021008DC | ldr      r1, [sp]                        | ldr      r0, [pc, #0xcc]
//   021008E8 | b        #0x2100978                      | mov      r0, sb
//   021008F0 | cmp      sl, r0                          | b        #0x210096c
//   021008F4 | blt      #0x2100944                      | mvn      r0, #0x1f
//   021008F8 | cmp      sl, #0x20                       | cmp      sl, r0
//   021008FC | bgt      #0x2100944                      | blt      #0x2100940
//   02100900 | cmp      sl, #0                          | cmp      sl, #0x20
//   02100904 | ldrge    r1, [r6]                        | bgt      #0x2100940
//   02100908 | addge    r0, sb, r8, lsl #6              | cmp      sl, #0
//   0210090C | addge    r1, r1, #0x1800                 | ldrge    r1, [r6]
//   02100910 | addge    r0, r0, #0x4000                 | addge    r0, r5, #0x4000
//   02100914 | strge    r1, [r0, #0x668]                | addge    r1, r1, #0x1800
//   02100918 | bge      #0x2100930                      | strge    r1, [r0, #0x668]
//   0210091C | ldr      r1, [r6]                        | ldrlt    r1, [r6]
//   02100920 | add      r0, sb, r8, lsl #6              | addlt    r0, r5, #0x4000
//   02100924 | sub      r1, r1, #0x1800                 | sublt    r1, r1, #0x1800
//   02100928 | add      r0, r0, #0x4000                 | strlt    r1, [r0, #0x668]
//   0210092C | str      r1, [r0, #0x668]                | ldr      r0, [sp, #0x10]
//   02100930 | ldr      r0, [sp, #0xc]                  | str      r0, [lr]
//   02100934 | str      r0, [lr]                        | ldr      r0, [pc, #0x78]
//   02100940 | b        #0x2100978                      | cmp      sl, #0
//   02100944 | cmp      sl, #0                          | ldrlt    r0, [sp, #0x14]
//   02100948 | addlt    r0, sb, r8, lsl #6              | addlt    r1, r5, #0x4000
//   0210094C | addlt    r1, r0, #0x4000                 | strlt    r0, [r1, #0x668]
//   02100950 | ldrlt    r0, [sp, #0x10]                 | ldrge    r0, [sp, #0x18]
//   02100954 | strlt    r0, [r1, #0x668]                | addge    r1, r5, #0x4000
//   02100958 | addge    r0, sb, r8, lsl #6              | strge    r0, [r1, #0x668]
//   0210095C | addge    r1, r0, #0x4000                 | ldr      r0, [sp, #0x1c]
//   02100960 | ldrge    r0, [sp, #0x14]                 | str      r0, [lr]
//   02100964 | strge    r0, [r1, #0x668]                | ldr      r0, [pc, #0x48]
//   0210096C | str      r0, [lr]                        | ldr      r0, [sp, #4]
//   02100970 | ldr      r0, [pc, #0x3c]                 | add      r8, r8, #0x40
//   02100978 | add      r8, r8, #1                      | str      r0, [sp, #4]
//   0210097C | cmp      r8, #0x30                       | cmp      r0, #0x30
//   02100980 | blt      #0x2100800                      | blt      #0x2100808
//   02100984 | add      sp, sp, #0x1c                   | add      sp, sp, #0x24

#include "types.h"
extern void func_ov006_02100408(char* p, int x, int y);
extern void func_02012718(int a, int b);
extern void func_ov006_02101148(char* p, int i);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int n);

void func_ov006_0210076c(char* p, int i)
{
    u32 s;
    int j, dx, dy;

    s = *(u8*)(p + (i << 6) + 0x5296);
    if (s == 0) return;
    if (s >= 0xc) return;

    for (j = 0; j < 0x30; j++) {
        if (*(u8*)(p + (j << 6) + 0x4698) == 0) continue;
        if (*(u8*)(p + (j << 6) + 0x4699) < 2) continue;
        if (*(int*)(p + (j << 6) + 0x466c) <= 0x2000) continue;

        dx = (*(int*)(p + (i << 6) + 0x5260) -
              *(int*)(p + (j << 6) + 0x4660)) >> 12;
        dy = (*(int*)(p + (i << 6) + 0x5264) -
              *(int*)(p + (j << 6) + 0x4664)) >> 12;

        if (dx < -0x30) continue;
        if (dx > 0x30) continue;
        if (dy < 0) continue;
        if (dy > 8) continue;

        if (dx >= -0x10 && dx <= 0x10) {
            *(u8*)(p + (i << 6) + 0x5299) += 1;
            *(u8*)(p + (j << 6) + 0x469a) = 0;
            *(u8*)(p + (j << 6) + 0x4698) = 0;
            func_ov006_02100408(p, *(int*)(p + (j << 6) + 0x4660),
                                   *(int*)(p + (j << 6) + 0x4664));
            func_02012718(0x19a, *(int*)(p + (j << 6) + 0x4660));
            func_ov006_02101148(p, i);
        } else if (dx >= -0x20 && dx <= 0x20) {
            if (dx >= 0)
                *(int*)(p + (j << 6) + 0x4668) =
                    *(int*)(p + (i << 6) + 0x5268) + 0x1800;
            else
                *(int*)(p + (j << 6) + 0x4668) =
                    *(int*)(p + (i << 6) + 0x5268) - 0x1800;
            *(int*)(p + (j << 6) + 0x466c) = -0x2000;
            _ZN5Sound12PlayBank2_2DEj(0x19b);
        } else {
            if (dx < 0)
                *(int*)(p + (j << 6) + 0x4668) = 0x1800;
            else
                *(int*)(p + (j << 6) + 0x4668) = -0x1800;
            *(int*)(p + (j << 6) + 0x466c) = -0x2800;
            _ZN5Sound12PlayBank2_2DEj(0x19b);
        }
    }
}
