/* HOST TRANSCRIPTION of func_ov080_021261f4 -- the PAINTING's RIPPLE RENDER.
 * ov080 0x021261f4, 0x2f8 bytes (760; 167 ARM instructions plus a 22-word
 * literal pool). Run link100, wave 17, lane PAINT5.
 *
 * ============================ WHAT THIS IS ================================
 *
 * PLAINLY: this is the picture on a castle painting being drawn as a rippling
 * sheet. The painting keeps a grid of points that its Behavior wobbles every
 * frame; this walks that grid a row at a time and hands each row to the DS
 * geometry engine as one triangle strip, with a texture matrix set up so the
 * texture slides over the surface as the sheet moves. It is the frame the
 * player sees while Mario dives into a course and while he pops back out of
 * it, which is why the entrance fix on this branch's base is what made the
 * port reach it at all.
 *
 * WHY IT EXISTS. It is the ONE hole in the Painting's twelve-cell state table
 * seated in port/hal/actor_overlays.cpp's g_painting_states. Cell 1
 * (data_ov080_0212821c) holds 0x021261f4, and until this file the port seated
 * a trap there that printed
 *     FATAL: Painting state 5's Render (ov080 0x021261f4) is UNMATCHED --
 *     no host body exists
 * and aborted. That trap is what fixer ENTRANCE's level-37 row hit
 * (out/HEALTH1/bugs.md section 2): level 37 exits to level 4 entrance 9,
 * param 0x0409 -> slot 8 -> entry mode 0x08, the COME-OUT-OF-A-PAINTING
 * entry, and the painting's state machine then reaches this cell. Before the
 * entrance fix every level change on the port entered at record 0, so no gate
 * in this run had ever entered a level through a painting and the hole had
 * never been reached.
 *
 * IT IS A HOLE, NOT A LOOKUP MISS. config/arm9/overlays/ov080/delinks.txt runs
 *     src/func_ov080_02126124.c   .text start:0x02126124 end:0x021261f4
 *     src/func_ov080_021264ec.c   .text start:0x021264ec end:0x021265ec
 * and the 0x2f8 bytes between them are covered by nothing. There is no C in
 * src/ and no draft in nearmiss/db.jsonl. config/arm9/overlays/ov080/
 * symbols.txt:65 does name it --
 *     func_ov080_021261f4 kind:function(arm,size=0x2f8) addr:0x021261f4
 * -- and a named symbol is not a decompiled body.
 *
 * IT IS A REAL STANDALONE ENTRY, checked the way the phantom-seat rule asks:
 * 0x021261f4 opens with a real ARM prologue,
 *     021261f4  push {r4,r5,r6,r7,r8,sb,sl,fp,lr}
 *     021261f8  sub  sp, sp, #0x34
 *     021261fc  mov  r4, r0            <- the receiver out of r0
 * it is the address immediately after the previous delink block ends, and the
 * ROM's own pointer-to-member table parks it at data_ov080_0212821c with
 * delta 0. It is not an interior address of a tail-shared body.
 *
 * PROVENANCE. Read off a capstone listing of
 *     extracted/overlays/overlay_0080.bin   at base 0x02123740
 * (the ov080 .text start in config/arm9/overlays/ov080/delinks.txt line 1).
 * The dsd export extracted/dsd/arm9_overlays/ov080.bin was NOT used for
 * anything: those files are not load-image aligned.
 *
 * THE OVERLAY IS DRIVEN FROM THE DATA, NOT FROM RECALL, because ov077 loads at
 * the same addresses: port/ov077_syms.txt calls 0x021261f4 daTgz_c_classInit.
 * The address came out of hal/actor_overlays.cpp's g_painting_states row
 * {data_ov080_0212821c, 0x021261f4}, whose mount is checked against the ROM's
 * own word before it is replaced, so the overlay is ov080 by construction.
 *
 * THE BASE IS PROVED BY A FUNCTION WHOSE C IS ALREADY TRUSTED. At base
 * 0x02123740, 0x02125fd0 disassembles to exactly what src/func_ov080_02125fd0.c
 * says -- MTX_PUSH 0, MulMat4x3Mat4x3(self+0xd4, &data_0209b3ec, tmp),
 * MTX_MODE 2, func_020553a4(tmp), LIGHT_VECTOR 0xe0000000, LIGHT_COLOR
 * 0xc0007fff, func_ov080_02125460(self), three MTX_SCALE 0x20000, BEGIN_VTXS 1,
 * four vertices, END_VTXS 0, MTX_POP 1 -- instruction for instruction and in
 * that order. That function is one of this one's eleven siblings in the same
 * state table.
 *
 * ================== CALLEES: THE HOLE DOES NOT CASCADE ====================
 *
 * Four distinct callees, four call sites, and ALL FOUR ALREADY HAVE
 * DECOMPILED BODIES that this port already links. Nothing had to be invented
 * and no second hole opens behind this one.
 *
 *   0x02052914  MulMat4x3Mat4x3   src/MulMat4x3Mat4x3.c
 *   0x02055998  func_02055998     src/func_02055998.c   MTX_MODE 3 then
 *                                 MTX_LOAD_4x4, first twelve words << 4
 *   0x02055388  func_02055388     src/func_02055388.c   GXFIFO 0x19,
 *                                 MTX_MULT_4x3, twelve words
 *   0x020553a4  func_020553a4     src/func_020553a4.c   GXFIFO 0x17,
 *                                 MTX_LOAD_4x3, twelve words
 *
 * ===================== THE LITERAL POOL, RESOLVED =========================
 *
 * 0x02126494..0x021264eb, twenty-two words. Two carry relocation rows and are
 * therefore addresses; the other twenty carry none and are therefore plain
 * immediates -- which is the evidence that every 0x040004xx below is a DS
 * command port and not a pointer.
 *
 *   02126494  0x0209b3ec  data_0209b3ec   relocs.txt kind:load module:main
 *   0212649c  0x0212771c  data_ov080_0212771c
 *                                         relocs.txt kind:load module:overlay(80)
 *   02126498  0x04000444  MTX_PUSH          021264a0  0x04000440  MTX_MODE
 *   021264a4  0x04000400  (a TEXCOORD VALUE, see below, not the FIFO)
 *   021264a8  0x04000488  TEXCOORD          021264ac  0x040004c8  LIGHT_VECTOR
 *   021264b0  0xc0007fff  (light colour)    021264b4  0x040004cc  LIGHT_COLOR
 *   021264b8  0x7fff0000  (dif/amb or)      021264bc  0x040004c0  DIF_AMB
 *   021264c0  0x040004c4  SPE_EMI           021264c4  0x040004a8  TEXIMAGE_PARAM
 *   021264c8  0x040004ac  PLTT_BASE         021264cc  0x040004a4  POLYGON_ATTR
 *   021264d0  0x01000088  (poly attr base)  021264d4  0x0400046c  MTX_SCALE
 *   021264d8  0x04000484  NORMAL            021264dc  0x0400048c  VTX_16
 *   021264e0  0x04000500  BEGIN_VTXS        021264e4  0x04000504  END_VTXS
 *   021264e8  0x04000448  MTX_POP
 *
 * 0x04000400 AT 0x021264a4 IS A VALUE, NOT A PORT. It is loaded into r1 at
 * 0x02126240 and STORED THROUGH r0 = 0x04000488 at 0x0212624c, so it is the
 * word written to TEXCOORD: s = 0x0400, t = 0x0400 in the DS's 1:11:4 texel
 * units. mwccarm pooled one word and this function happens to use that bit
 * pattern for both jobs one instruction apart. Transcribed as the value it is.
 *
 * ================== THE OBJECT'S FIELDS, AND HOW EACH IS PINNED ===========
 *
 *   +0x008  u32   the actor's spawn/param word. Only bits 12..8 are read here,
 *                 and only to pick the polygon alpha. dActor_c's own field;
 *                 src/func_ov080_02126124.c reads the same word at +8 for its
 *                 two nibbles and func_ov080_02125460 reads bits 14..13 of it.
 *   +0x0d4  Mat4x3 the actor's world matrix, the first argument of
 *                 MulMat4x3Mat4x3. IDENTICAL to src/func_ov080_02125fd0.c's
 *                 `(void*)(self + 0xd4)`, which is a matched sibling.
 *   +0x1a0  ptr   the vertex array, stride 0x18. The port already names this
 *                 field: hal/actor_classes_painting.cpp's pt_init prints it as
 *                 `buf` and pt_behavior refuses a tick when it holds a small
 *                 integer.
 *   +0x1a8  ptr   the GX material record. Its five fields are NOT invented
 *                 here: include/private/Ov080Mat.h already carries the struct,
 *                 written for the matched src/func_ov080_02125460.c, which
 *                 reaches the same five through the same pointer.
 *   +0x1ba  u8    the STRIP COUNT. Strips drawn = this minus one.
 *   +0x1bb  u8    the VERTEX-PAIR COUNT per strip, and also the ROW STRIDE of
 *                 the vertex array. Both jobs are the SAME BYTE in the ROM:
 *                 0x0212637c loads it as the inner bound and 0x0212638c
 *                 multiplies the inner counter by the value in that same
 *                 register. Kept as one read, because splitting it into two
 *                 fields would be inventing a second field.
 *
 *   The class is 0x1bc bytes (include/Painting.h), so +0x1bb is its last byte.
 *
 * =================== THE VERTEX RECORD, 0x18 BYTES =========================
 *
 *   +0x00  s32  component A, Fix12         +0x04  s32  component B, Fix12
 *   +0x08  s32  component C, Fix12         +0x10  u32  packed NORMAL word
 *   +0x0c, +0x14   not read by this function
 *
 * A, B AND C ARE NOT NAMED X, Y AND Z HERE, deliberately. What is established
 * is the ORDER they are fed to the geometry engine in -- VTX_16's first word
 * is (A | B << 16) and its second is C -- and that is exactly what this file
 * reproduces. Which world axis each is would be a guess, and the matched
 * sibling func_ov080_02125fd0.c does not name them either (it calls the same
 * three v0, v1, v2).
 *
 * ============ WHAT THE ROM DOES THAT LOOKS ODD, KEPT ANYWAY ===============
 *
 * 1. MTX_PUSH IS PUSHED IN THE CALLER'S MODE AND POPPED IN MODE 2. The push at
 *    0x0212621c happens before func_02055998 sets MTX_MODE to 3, and the pop
 *    at 0x02126484 happens with MTX_MODE left at 2. The matched sibling
 *    func_ov080_02125fd0 brackets itself the same way. Transcribed as found.
 *
 * 2. THE TEXTURE MATRIX IS MULTIPLIED BY THE MODELVIEW. func_02055998 loads
 *    data_ov080_0212771c (scale 0x40000 on S, -0x40000 on T) into the TEXTURE
 *    matrix, and func_02055388 then MULTIPLIES that by the same tmp matrix
 *    that is about to become the position matrix -- while MTX_MODE is still 3.
 *    With TEXIMAGE_PARAM's texGen set to 2 (from the normal), that is the DS's
 *    environment-map setup, which is the shimmer on a painting.
 *
 * 3. THE MATERIAL POINTER IS RE-READ FIVE TIMES, once per register it feeds
 *    (0x02126264, 0x0212627c, 0x02126290, 0x021262d8 and the pair inside the
 *    TEXIMAGE_PARAM block). The matched func_ov080_02125460 re-reads it the
 *    same way. Kept, because collapsing it to one read would change what the
 *    function does if a callee could move it.
 *
 * 4. THE DIFFUSE/AMBIENT OR IS UNCONDITIONAL HERE. func_ov080_02125460 ORs
 *    0x7fff0000 into difAmb only when a mode field is >= 2 or a u16 at +0x1b6
 *    is zero; this function ORs it every time, with no test. The two bodies
 *    are not the same function and this one has no branch there.
 *
 * 5. THE 16-BIT COORDINATE FORM IS KEPT AS THE ROM SPELLS IT: shift left 9,
 *    arithmetic shift right 16, truncate to 16 bits. That is the same value as
 *    a plain `(s16)(v >> 7)` and it is written the long way on purpose, so a
 *    later matcher sees the shape the ROM emitted. The matched sibling uses a
 *    DIFFERENT pair (>> 8, cast s16) and its own src file carries a note
 *    saying so, which is the evidence the two really do differ.
 *
 * 6. NOTHING IS BOUNDS-CHECKED. The strip loop indexes the vertex array at
 *    i + j * stride and i + 1 + j * stride with no test against any count, and
 *    the ROM has no test either. The port's own guard against a painting
 *    ticked before it is initialised lives one level up, in
 *    hal/actor_classes_painting.cpp's pt_behavior. No guard is added here,
 *    because a guard the ROM does not have is a behaviour change.
 *
 * SEMANTICS, NOT BYTES. This is a host copy shaped for MSVC, not a byte match.
 * No claim is made that mwccarm would emit the ROM's instructions from this C.
 * When a byte match lands in src/ this file retires per the port rule.
 *
 * NO GAME LOGIC WAS INVENTED. Every constant, branch, field offset, register
 * port and call below is carried over from the disassembly, and every
 * statement group carries the ROM address range it came from.
 */
#include <cstdio>
#include <cstdlib>

#include "ntr/gx.h"
#include "ntr/mmio.h"

extern "C" {

/* The four matched callees, spelled as their own src TUs define them. */
void MulMat4x3Mat4x3(const void *m1, const void *m0, void *mF);
void func_02055998(int *m);
void func_02055388(int *m);
void func_020553a4(int *m);

/* data_0209b3ec is the arm9 matrix MulMat4x3Mat4x3's second argument names;
   src/func_ov080_02125fd0.c declares it `extern int data_0209b3ec` and takes
   its address, and this file repeats that declaration rather than inventing a
   type for it. data_ov080_0212771c is the texture matrix in ov080's rodata;
   SPELLED AS THE MOUNT SPELLS IT -- build/port/host-src/ov080_syms.c defines
   it `u8 data_ov080_0212771c[64]` with align(4) -- because an `extern int[]`
   of the same name would link fine and disagree with the definition. */
extern int data_0209b3ec;
extern unsigned char data_ov080_0212771c[64];

void func_ov080_021261f4(char *self);

}  /* extern "C" */

namespace {

/* The material record at *(self + 0x1a8). The same five fields, in the same
   order, that include/private/Ov080Mat.h already carries for the matched
   func_ov080_02125460; repeated here rather than included because port/ TUs do
   not take include/private/ on their include path. */
struct Ov080Mat {
    unsigned int texAddr; /* 0x00 */
    unsigned int pltAddr; /* 0x04 */
    unsigned int difAmb;  /* 0x08 */
    unsigned int speEmi;  /* 0x0c */
    unsigned int param;   /* 0x10 */
};

inline Ov080Mat *mat_of(char *self)
{
    return *(Ov080Mat **)(self + 0x1a8);
}

/* The ROM's 20.12 -> 16-bit vertex component, in the exact shift pair it
   emits: lsl #9, asr #16, then lsl #16 / lsr #16 to truncate. Written through
   `unsigned` for the left shift because a bare left shift of a negative int is
   UB even though MSVC emits the SHL the ROM wants -- the same reason CT1's
   rom_lsl12 exists in MgCurling_Collide_020e1dc8.cpp. */
inline unsigned short rom_v16(int v)
{
    return (unsigned short)(((int)((unsigned)v << 9)) >> 16);
}

}  /* namespace */

/* 0x021261f4..0x021261fc   push {r4-r8,sb,sl,fp,lr}; sub sp,#0x34; mov r4,r0.
   tmp is the 0x30 bytes at sp+4; sp+0 is the inner counter's spill slot, which
   is why this frame is four bytes wider than the sibling's. */
void func_ov080_021261f4(char *self)
{
    int tmp[12];
    int i;

    /* 0x02126200..0x0212620c
       MulMat4x3Mat4x3(self + 0xd4, &data_0209b3ec, tmp) */
    MulMat4x3Mat4x3((void *)(self + 0xd4), (void *)&data_0209b3ec, tmp);

    /* 0x02126210..0x0212621c   MTX_PUSH, in whatever mode the caller left */
    NTR_MMIO(int, 0x04000444) = 0;

    /* 0x02126218,0x02126220   MTX_MODE 3 and MTX_LOAD_4x4 of the texture
       matrix, both inside func_02055998 */
    func_02055998((int *)data_ov080_0212771c);

    /* 0x02126224..0x02126228   MTX_MULT_4x3 by tmp, STILL IN TEXTURE MODE */
    func_02055388(tmp);

    /* 0x0212622c..0x02126238   MTX_MODE 2, position and vector together */
    NTR_MMIO(int, 0x04000440) = 2;

    /* 0x02126234,0x0212623c   MTX_LOAD_4x3 of tmp as the position matrix */
    func_020553a4(tmp);

    /* 0x02126240..0x0212624c   TEXCOORD, s = 0x0400, t = 0x0400 */
    NTR_MMIO(unsigned, 0x04000488) = 0x04000400u;

    /* 0x02126250,0x0212625c   LIGHT_VECTOR */
    NTR_MMIO(unsigned, 0x040004c8) = 0xe0000000u;

    /* 0x02126254,0x02126258,0x02126260   LIGHT_COLOR */
    NTR_MMIO(unsigned, 0x040004cc) = 0xc0007fffu;

    /* 0x02126264..0x02126278   DIF_AMB, ORed unconditionally */
    NTR_MMIO(unsigned, 0x040004c0) = mat_of(self)->difAmb | 0x7fff0000u;

    /* 0x0212627c..0x0212628c   SPE_EMI */
    NTR_MMIO(unsigned, 0x040004c4) = mat_of(self)->speEmi;

    /* 0x02126290..0x021262d4   TEXIMAGE_PARAM. The bit assembly is the ROM's,
       field for field, and it is the same one G3_TexImageParam performs in the
       matched src/func_ov080_02125460.c: (addr >> 3) | (fmt << 26) |
       (sizeS << 20) | (sizeT << 23) | (texGen << 30) | (pltt0 << 29), with
       repeat and flip zero. texGen is 2 here (a bare `orr #0x80000000`), where
       02125460 passes 0. */
    {
        Ov080Mat *m = mat_of(self);
        unsigned param = m->param;
        unsigned fmt   = (param >> 0x1a) & 7u;
        unsigned sizeS = (param >> 0x14) & 7u;
        unsigned sizeT = (param >> 0x17) & 7u;
        unsigned pltt0 = (param >> 0x1d) & 1u;
        NTR_MMIO(unsigned, 0x040004a8) =
            (m->texAddr >> 3) | (fmt << 26) | 0x80000000u
            | (sizeS << 20) | (sizeT << 23) | (pltt0 << 29);
    }

    /* 0x021262d8..0x02126308   PLTT_BASE, the same shift G3_TexPlttBase does:
       >> 4 normally, >> 3 when the texture format is 2 (4-colour palette). */
    {
        Ov080Mat *m = mat_of(self);
        unsigned fmt = (m->param >> 0x1a) & 7u;
        NTR_MMIO(unsigned, 0x040004ac) = m->pltAddr >> (4 - (fmt == 2u ? 1u : 0u));
    }

    /* 0x0212630c..0x02126334   POLYGON_ATTR. 0x01000088 is polygon id 1, light
       3 enabled, cull mode 2; the alpha is 0x14 when bits 12..8 of the spawn
       word read 7 and 0x1f otherwise. 02125460's constant 0x011f0088 is this
       same word with the 0x1f arm folded in. */
    {
        unsigned n = (unsigned)(unsigned char)((*(unsigned *)(self + 8) >> 8) & 0x1fu);
        unsigned alpha = (n == 7u) ? 0x14u : 0x1fu;
        NTR_MMIO(unsigned, 0x040004a4) = 0x01000088u | (alpha << 16);
    }

    /* 0x02126338..0x02126348   MTX_SCALE 0x10000 on all three axes */
    NTR_MMIO(int, 0x0400046c) = 0x10000;
    NTR_MMIO(int, 0x0400046c) = 0x10000;
    NTR_MMIO(int, 0x0400046c) = 0x10000;

    /* 0x0212634c..0x02126478   the strips. The outer bound is re-read at
       0x0212646c and the inner bound at 0x02126454 on every pass, so both are
       read fresh here too. */
    for (i = 0; i < (int)*(unsigned char *)(self + 0x1ba) - 1; i++) {
        int j;

        /* 0x0212636c..0x02126378   BEGIN_VTXS 2, a triangle strip */
        NTR_MMIO(int, 0x04000500) = 2;

        for (j = 0; j < (int)*(unsigned char *)(self + 0x1bb); j++) {
            /* 0x0212638c..0x021263ac   the pair of vertices this step feeds
               the strip: i + j * stride and i + 1 + j * stride, stride being
               the SAME byte as the inner bound. */
            int stride = (int)*(unsigned char *)(self + 0x1bb);
            char *v = *(char **)(self + 0x1a0);
            char *a = v + (i + j * stride) * 0x18;
            char *b = v + (i + 1 + j * stride) * 0x18;

            /* 0x021263b0..0x02126400   vertex a: NORMAL, then VTX_16 as
               (A | B << 16) and then C. */
            NTR_MMIO(unsigned, 0x04000484) = *(unsigned *)(a + 0x10);
            NTR_MMIO(unsigned, 0x0400048c) =
                (unsigned)rom_v16(*(int *)(a + 0))
                | ((unsigned)rom_v16(*(int *)(a + 4)) << 16);
            NTR_MMIO(unsigned, 0x0400048c) = (unsigned)rom_v16(*(int *)(a + 8));

            /* 0x02126404..0x02126450   vertex b, the same three stores */
            NTR_MMIO(unsigned, 0x04000484) = *(unsigned *)(b + 0x10);
            NTR_MMIO(unsigned, 0x0400048c) =
                (unsigned)rom_v16(*(int *)(b + 0))
                | ((unsigned)rom_v16(*(int *)(b + 4)) << 16);
            NTR_MMIO(unsigned, 0x0400048c) = (unsigned)rom_v16(*(int *)(b + 8));
        }

        /* 0x02126460..0x02126468   END_VTXS 0 */
        NTR_MMIO(int, 0x04000504) = 0;
    }

    /* 0x0212647c..0x02126490   MTX_POP 1, then the epilogue */
    NTR_MMIO(int, 0x04000448) = 1;
}
