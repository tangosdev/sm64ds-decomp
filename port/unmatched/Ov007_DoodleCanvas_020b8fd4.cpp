/* HOST TRANSCRIPTION of func_ov007_020b8fd4, THE TITLE-SCREEN DOODLE CANVAS.
 * ov007 0x020b8fd4, 0x1e0 bytes (480, 120 ARM instructions).
 *
 * ============================ WHAT THIS IS ================================
 *
 * INTERIM, and it is the last body standing between the port's title screen and
 * a working DOODLE (the DS lets you draw on the bottom screen with the stylus at
 * the title). This function is the DOODLE CANVAS/PAPER renderer: its one caller
 * is the title's render dispatcher src/func_ov007_020bcf90.c, which calls it in
 * outer states 1..4 (the draw screen) right before it calls the seated stroke
 * hub func_ov007_020b91b4. It sizes the paper (writes 0x80000/0x60000 into the
 * canvas object's +4/+8 fields), runs the two-case slide-in animation on the
 * doodle mode word data_ov007_02104ba0[8], and positions the two paper billboards
 * (data_ov007_02104b9c[3] and [4]) from the doodle object's coordinate pair and
 * the p342c[0x4a] view object, lerping by q1 = MAX(y/0x1f4, x/0xc8) -- max, not
 * min; see the caught-bug paragraph below.
 *
 * WHY IT NEEDED A BODY. hal/scene_boot.cpp stood an L2_UNMATCHED trap here that
 * named itself and returned 0. Measured on this tree, scene 1 driven into the
 * doodle screen (SM64DS_SCENE=1 with a tap chain that reaches title element
 * state 2 and screen mode data_ov007_0210342c+0x20 == 1): the trap was entered
 * 402 times over 1200 frames and returned 0 every time, so the canvas fields
 * were never sized and the two paper billboards were never positioned -- the
 * drawing surface never came up and the strokes the seated hub draws had no
 * paper to sit on. The trap was the right answer while nobody had read the ROM
 * at that address. It is not the right answer once somebody has.
 *
 * THE CRACK SIDE OWNS THE MATCH. This body is NOT a byte-match: the residual
 * against mwccarm 2004/b56 is SIX instructions, all one r0/r1 tie-break in the
 * second block's setup (the ROM colours `flag` into r1 and the p342c transient
 * into r0; sixty-five source-level variants all land the reverse) plus the
 * branch offset that shifts with it -- the pure register-permutation residue
 * class of notes/mwccarm-codegen.md 6aa, recorded as floor(coloring) in
 * nearmiss/db.jsonl. EVERY MEMORY STORE AND EVERY CALL TARGET IN THIS BODY
 * MATCHES THE ROM BYTE FOR BYTE -- verified with tools/match.py: of the 6
 * diffs, 0 are a str/stm/bl/b, so what is written and what is called is the
 * ROM's exactly and only two intermediate register choices differ.
 *
 * ONE REAL BUG WAS CAUGHT ON THE WAY HERE, and it is why this body is the
 * crack's and not the first draft's: the near-miss draft computed the paper
 * ratio as min(q1, q2) where the ROM's `movle r8, r0` is a MAX. A conditional
 * move is neither a store nor a call, so a store/call fidelity check alone
 * cannot see that class of divergence; the byte-level crack could and did.
 *
 * When src/ gains a byte-matched body for 0x020b8fd4, THIS FILE RETIRES
 * AUTOMATICALLY: the CMake block that adds it is guarded on the src TU not
 * existing (port/CMakeLists.txt, DOODLE_INTERIM_SOURCES), so the matched body
 * wins the day it lands and the interim leaves the binary in the same
 * configure. Its stem is deliberately NOT func_ov007_020b8fd4 so
 * port/tools/linkage.py does not count it as the matched TU and
 * port/tools/objsrc_check.py cannot read it as one either.
 *
 * ============================ PROVENANCE ==================================
 *
 * The C shape here is the push0829 crack of the fanout-opus near-miss draft
 * from nearmiss/db.jsonl (addr 0x020b8fd4), 110 candidates deep, verified
 * against the ROM at extracted/overlays/overlay_0007.bin, config-aligned base
 * 0x020ad660 (file offset = address - 0x020ad660, which holds across the whole
 * ov007 image; NOT the dsd export, which is short for this overlay).
 * Byte-identical to the ROM except the six-instruction r0/r1 tie-break
 * described above: the head through +0xb0, the whole x/y/a/flag block, both
 * div calls, the max, and the entire billboard-store tail all match.
 *
 * RELOCATIONS: SIX arm_call plus THREE data loads, from config/arm9/overlays/
 * ov007/relocs.txt (from:0x020b9088..0x020b91b0), all to bodies already linked:
 *
 *   from:0x020b9088 -> 0x020c4388  func_ov007_020c4388  (billboard commit)
 *   from:0x020b9098 -> 0x020c421c  func_ov007_020c421c  (billboard submit)
 *   from:0x020b90a8 -> 0x020c421c  func_ov007_020c421c
 *   from:0x020b9110 -> 0x02052f4c  _ZN4cstd3divEii      (the ratio div)
 *   from:0x020b9120 -> 0x02052f4c  _ZN4cstd3divEii
 *   from:0x020b917c -> 0x020c4388  func_ov007_020c4388
 *   from:0x020b9188 -> 0x020c4388  func_ov007_020c4388
 *   from:0x020b9190 -> 0x020c421c  func_ov007_020c421c
 *   from:0x020b9198 -> 0x020c421c  func_ov007_020c421c
 *   load 0x020b91a8 -> 0x02104b9c  data_ov007_02104b9c  (paper-object handle)
 *   load 0x020b91ac -> 0x02104ba0  data_ov007_02104ba0  (doodle-object handle)
 *   load 0x020b91b0 -> 0x0210342c  data_ov007_0210342c  (the title scene global)
 *
 * There is not one relocated DATA word inside the instruction stream: the three
 * loads above are the literal pool at the function tail, so the romdata pointer
 * question has no instances here.
 *
 * THE CALLER SPELLS IT UNTAGGED. src/func_ov007_020bcf90.c externs it as
 * func_020b8fd4 (no ov007 tag) and reaches it through the
 * /alternatename:_func_020b8fd4=_func_ov007_020b8fd4 already in
 * hal/scene_boot.cpp, so this file defines the tagged name and the alias carries
 * the caller to it. The L2_UNMATCHED(func_ov007_020b8fd4) line in
 * hal/scene_boot.cpp is removed in the same change; leaving it would be an
 * LNK2005 against this body.
 */

typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" {

extern int* data_ov007_02104b9c;
extern int* data_ov007_02104ba0;
extern int* data_ov007_0210342c;

void func_ov007_020c4388(char* r0, int r1);
void func_ov007_020c421c(char* r0);
int _ZN4cstd3divEii(int a, int b);

void func_ov007_020b8fd4(char* sb)
{
    int* g = data_ov007_02104b9c;
    int r4 = g[0];

    if (r4 <= 0x1000) {
        *(int*)(((int**)g)[2] + 1) = 0x80000;
        *(int*)(((int**)data_ov007_02104b9c)[2] + 2) = 0x60000;

        switch (((int*)data_ov007_02104ba0)[8]) {
        case 0: {
            int t = 0x1000 - r4;
            int* p = (int*)((((int)((int**)data_ov007_02104b9c)[2]) + 4) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + (t << 8);
            break;
        }
        case 3: {
            int t = 0x1000 - r4;
            int* p = (int*)((((int)((int**)data_ov007_02104b9c)[2]) + 8) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + ((-t) << 8);
            break;
        }
        }

        func_ov007_020c4388((char*)((int**)data_ov007_02104b9c)[2], data_ov007_0210342c[0xd]);
        func_ov007_020c421c((char*)((int**)data_ov007_02104b9c)[1]);
        func_ov007_020c421c((char*)((int**)data_ov007_02104b9c)[2]);
    }

    {
        int q1, q2;
        char* r7;
        char* r6;
        int* r5;
        int x;
        int* pba0 = data_ov007_02104ba0;
        int** pb9c = (int**)data_ov007_02104b9c;
        int* p342c = data_ov007_0210342c;
        int flag = pba0[0xc];
        int* r2;
        int y;
        s16 a;

        r2 = (int*)pba0[0];
        r5 = (int*)p342c[0x4a];
        r7 = (char*)pb9c[3];
        r6 = (char*)pb9c[4];
        x = r2[2];
        y = r2[5];
        if (flag != 0) { x = 0; y = 0; }
        a = *(s16*)(((int**)(r5[0]))[1]);
        if (a == 1) return;

        q1 = _ZN4cstd3divEii(y << 0xc, 0x1f4);
        q2 = _ZN4cstd3divEii(x << 0xc, 0xc8);
        if (q1 <= q2) q1 = q2;   /* the ROM's movle: MAX, not min */

        {
            int m = *(u16*)(r7 + 0xc) * q1;
            *(int*)(r7 + 4) = ((int*)r5[0])[2] + m / 2;
            *(int*)(r7 + 8) = ((int*)r5[0])[3];
            *(int*)(r7 + 0x3c) = 0x1000 - q1;
            *(int*)(r6 + 4) = ((int*)r5[0])[2];
            *(int*)(r6 + 8) = ((int*)r5[0])[3];
        }

        func_ov007_020c4388(r7, (int)sb);
        func_ov007_020c4388(r6, (int)sb);
        func_ov007_020c421c(r7);
        func_ov007_020c421c(r6);
    }
}

}  /* extern "C" */
