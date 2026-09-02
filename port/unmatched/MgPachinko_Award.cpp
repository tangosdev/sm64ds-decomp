/* PORT_HOST_ABI. func_ov006_020fb7e0, dScMgPachinko_c's per-item scoring tick.
 * Run mg5, lane HISCORE.
 *
 * WHY THIS FILE EXISTS: the owner reported "HIGH SCORE 999999" with "SCORE 0"
 * on scene 368, and this is where the 999999 comes from. It is a dropped
 * argument, the same family as port/unmatched/MgFlower_InitScore.cpp and
 * port/mg_fanout_costs.txt section 6, and the number itself is the ROM's own
 * per-minigame cap rather than anything the port invented.
 *
 * ---- THE CHAIN, DERIVED FROM THE ROM -------------------------------------
 *
 * The HUD is two ov006 calls made every frame out of vtable slot 9:
 *
 *     func_ov006_020fba48   bl func_ov004_020adc1c  -> *(u32*)(scene + 0x4650)
 *                           bl func_ov004_020b19f0  -> label at x=0x20, number
 *                                                      at x=0x48   HIGH SCORE
 *     func_ov006_020fba28   bl func_ov004_020adbc0  -> *(u32*)(scene + 0x464c)
 *                           bl func_ov004_020b1a5c  -> number at x=0xf4  SCORE
 *
 * The high score is STORED at scene+0x4650 and only two things write it:
 * func_ov004_020adc00, which the base constructor calls once with the value
 * out of the save record, and func_ov004_020ada40, which promotes a new best.
 * The stored value at spawn is zero and the port already gets that right --
 * measured, scene 368: +0x4650 = 0 at spawn and at every render sample.
 *
 * SO THE 999999 IS PUT THERE BY THE SETTER, and by the setter's own clamp:
 *
 *     020adb7c  ldr   r0, [r0, #0xc]      ; data_0209b308 + 0x0c
 *     020adb80  cmp   r0, #0
 *     020adb88  cmp   r4, r0
 *     020adb8c  movhi r4, r0              ; score = min(score, that)
 *
 * data_0209b308 is the ov005 minigame record, which hal/scene_mg_sound.cpp
 * seeds verbatim from the ROM. Row 2 is scene 0x170 and its +0x0c word is
 * 0x000f423f -- 999999. So any score handed to func_ov004_020adb1c above the
 * cap is stored as exactly 999999 and then promoted into the high score, where
 * it survives the round reset that puts SCORE back to 0. That is the pair the
 * owner saw. (func_ov004_020b1ea4's `if (val >= 0xf423f) val = 0xf423f` is a
 * second clamp with the same constant, at the DISPLAY. It is not the one that
 * fired: the value is 999999 before it is drawn.)
 *
 * ---- WHAT HANDS THE SETTER A GARBAGE SCORE -------------------------------
 *
 * func_ov006_020fbb2c is the award routine and its whole job is
 *
 *     *(u16*)(c + 0x4cf8 + i*0xc) = val;
 *     func_ov004_020adb1c(that + func_ov004_020adbc0());
 *
 * so `val` IS the points. This function is its only caller, and the ROM hands
 * the points across in r2, produced by the same ldrh that makes the != 0 test
 * (disassembled out of extracted/overlays/overlay_0006.bin at base 0x020bfec0,
 * the shipped image):
 *
 *     020fb8bc  add  r0, r8, #0x5900
 *     020fb8c0  ldrh r2, [r0, #0x62]     ; r2 = *(u16*)(b + 0x5962)
 *     020fb8c4  cmp  r2, #0
 *     020fb8c8  beq  0x20fb8d8           ; src spells ONLY this test
 *     020fb8cc  mov  r0, sl              ; thiz
 *     020fb8d0  mov  r1, sb              ; i
 *     020fb8d4  bl   0x20fbb2c
 *
 * One ldrh does two jobs -- the test and the third argument -- which is free on
 * ARM and unspellable in C. src/func_ov006_020fb7e0.c therefore declares
 *
 *     extern void func_ov006_020fbb2c(char *c, int idx);
 *
 * and calls it with two arguments, while src/func_ov006_020fbb2c.c defines
 *
 *     void func_ov006_020fbb2c(char *c, int idx, unsigned short val)
 *
 * mwccarm accepted the mismatch and the ROM built, so the byte gate has never
 * had an opinion on it -- the same reason section 6's (a), (b) and (c) all
 * survived to be found by the port. port/tools/aritycheck.py finds this split
 * independently and without reading a line of the disassembly:
 *
 *     {"sym":"func_ov006_020fbb2c","def_n":3,
 *      "def_file":"src/func_ov006_020fbb2c.c","def_line":4,"decl_n":2,
 *      "decl_file":"src/func_ov006_020fb7e0.c","decl_line":7,"kind":"DROPS"}
 *
 * WHAT IT COST BEFORE THIS FILE, measured rather than reasoned about. The
 * award-delivery self-check in hal/scene_mg.cpp drives both functions on a
 * scratch object with 0x1234 in the ROM's own +0x5962 slot:
 *
 *     [score:award] ROM hands *(u16*)(b+0x5962) = 4660 in r2;
 *                   the callee received 3104 -> DROPPED (score is garbage)
 *
 * 3104 is a caller stack slot nobody wrote. Every award added a number like
 * that to the score, so the score climbed to the 999999 cap and the high score
 * followed it. SCORE was wrong on the way there too; the high score is only the
 * part that stuck.
 *
 * ---- THE DELTA FROM src, LINE BY LINE ------------------------------------
 *
 * Per the port/unmatched/MgBase_DeclConflict.cpp precedent, the body below is
 * src/func_ov006_020fb7e0.c verbatim except:
 *
 *   1. the declaration of the callee takes the parameters its DEFINITION takes
 *          src:   extern void func_ov006_020fbb2c(char *c, int idx);
 *          here:  extern void func_ov006_020fbb2c(char *c, int idx,
 *                                                 unsigned short val);
 *   2. the call passes the value the ROM has live in r2 at that instruction,
 *      which is the value the line above it has already loaded and tested
 *          src:   func_ov006_020fbb2c(thiz, i);
 *          here:  func_ov006_020fbb2c(thiz, i, *(u16 *)(b + 0x5962));
 *
 * Nothing else. The 0x1e-item loop, the 6-frame counter, the lim of 3 or 5, the
 * sub-object arm and the two clears are src's, unchanged, and every one of them
 * is confirmed against the disassembly of 0x020fb7e0..0x020fb8ec above.
 *
 * NO CLAMP AND NO SPECIAL CASE. This file does not mention 999999 and does not
 * touch the display. It delivers the ROM's own number to the ROM's own setter
 * and lets the ROM's own cap decide what to do with it.
 *
 * THIS FILE REPLACES src/func_ov006_020fb7e0.c rather than joining it, and that
 * TU is out of port/slice_pch.txt for that reason. src/ and include/ are
 * untouched; the byte gate never sees this file.
 *
 * src/func_ov006_020fb7e0.c is NONMATCHING (its own banner says so, div=18), so
 * nothing is lost from the matched count by taking it out of the port's build.
 */
typedef unsigned char u8;
typedef unsigned short u16;

extern "C" {

extern void func_ov006_020fbb2c(char *c, int idx, unsigned short val);

// PORT_HOST_ABI: src drops the third argument to func_ov006_020fbb2c that the ROM rides through r2 (one ldrh doing test and arg); host copy passes it so the score is not stack garbage
void func_ov006_020fb7e0(char *thiz)
{
    int i;
    char *b = thiz;
    for (i = 0; i < 0x1e; i++) {
        if (*(u8 *)(b + 0x5964) != 0) {
            u16 *cnt = (u16 *)(b + 0x5960);
            int lim;
            u8 sub;
            (*cnt)++;
            if (*cnt >= 6) {
                *(u16 *)(b + 0x5960) = 0;
                (*(u8 *)(b + 0x5965))++;
                lim = 3;
                if (*(u8 *)(b + 0x5967) == 2) lim = 5;
                sub = *(u8 *)(b + 0x5968);
                if (sub != 0) {
                    if (lim - 2 == *(u8 *)(b + 0x5965)) {
                        char *q = thiz + (sub - 1) * 0xc + 0x5000;
                        *(u8 *)(q + 0xbd4) = 0;
                        *(u8 *)(q + 0xbd6) = 1;
                    }
                }
                if (*(u8 *)(b + 0x5965) >= lim) {
                    *(u8 *)(b + 0x5966) = 0;
                    *(u8 *)(b + 0x5964) = 0;
                    if (*(u16 *)(b + 0x5962) != 0) {
                        func_ov006_020fbb2c(thiz, i, *(u16 *)(b + 0x5962));
                    }
                }
            }
        }
        b = b + 0x14;
    }
}

}  /* extern "C" */
