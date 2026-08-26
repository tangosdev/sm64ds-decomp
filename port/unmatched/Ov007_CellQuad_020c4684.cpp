/* HOST TRANSCRIPTION of func_ov007_020c4684, THE TITLE SCREEN'S POLYGON
 * SUBMITTER. ov007 0x020c4684, 0x338 bytes (824, 206 ARM instructions).
 *
 * ============================ WHAT THIS IS ================================
 *
 * INTERIM, in exactly the shape Ov007_OamCellBank_020c9688.cpp established for
 * run link60 lane SC1. There is no C for this address anywhere in the tree, it
 * is one of the fifteen ov007 bodies hal/scene_boot.cpp section 3 counts, and
 * until now this address held an L2_UNMATCHED trap that named itself and
 * returned 0. This file fills the hole for the PORT ONLY. It is derived from
 * the ROM's own instructions at that address; it is not a byte-match attempt,
 * is not scored by match.py, is not counted by port/tools/linkage.py, and its
 * stem is deliberately not func_ov007_020c4684 so port/tools/objsrc_check.py
 * cannot read it as the matched TU either. THE CRACK SIDE OWNS THE MATCH, and
 * the CMake block that adds this file is guarded on src/ not having one, so a
 * real decomp retires the interim in the configure that notices it.
 *
 * ======================= WHY IT IS WORTH THE INTERIM ======================
 *
 * THIS IS THE ROUTINE THAT DRAWS THE TITLE SCREEN, and the trap was refusing
 * every call. Measured on the unmodified tree, SM64DS_SCENE=1, 300 frames:
 *
 *     UNMATCHED ov007 body entered: func_ov007_020c4684   16,200 times
 *
 * 54 calls a frame, every one returning 0 without emitting a vertex. What the
 * ROM does in those 824 bytes is not in doubt -- its literal pool is twelve
 * geometry command ports and nothing else:
 *
 *     04000480 COLOR      040004a4 POLYGON_ATTR  04000444 MTX_PUSH
 *     04000470 MTX_TRANS  0400046c MTX_SCALE     04000500 BEGIN_VTXS
 *     04000488 TEXCOORD   0400048c VTX_16        0400049c VTX_YZ
 *     04000498 VTX_XZ     04000504 END_VTXS      04000448 MTX_POP
 *
 * It binds a material, brackets a matrix, places a UNIT QUAD and draws it. The
 * owner's report -- "any other graphics than those have jumbled glitchy
 * textures", and on the fresh captures "its just a big glitched yellow
 * texture" -- is a screen with its background quad, its sparkle sprites and
 * its TOUCH TO START row on it and NOTHING ELSE, because everything else the
 * title draws comes through here.
 *
 * ============================ PROVENANCE ==================================
 *
 * Read straight off extracted/overlays/overlay_0007.bin with capstone, at the
 * CONFIG-ALIGNED base 0x020ad660 -- the same base and the same argument
 * Ov007_OamCellBank_020c9688.cpp records: .text starts there, .bss ends at
 * 0x02103340, the span 0x55ce0 is exactly the file's length, so file offset =
 * address - 0x020ad660 across the whole image. NOT the dsd export, which is
 * short by 1248 bytes for this overlay.
 *
 * THE ALIGNMENT WAS PROVEN BEFORE THE TARGET WAS READ, on a byte-matched
 * neighbour rather than asserted: func_ov007_020c421c (0x020c421c, 0xdc) is
 * this function's own caller and has a matched src TU, and the disassembly at
 * that base reproduces src/func_ov007_020c421c.c instruction for instruction --
 * the MTX_PUSH store, the call here, the `& 4` branch, the three MTX_SCALE
 * stores, the second call here, the MTX_POP. Both disassemblies are banked
 * beside this lane's evidence.
 *
 * RELOCATIONS: FOUR rows originate inside the span and all four are resolved.
 *
 *   from:0x020c472c kind:arm_call to:0x020c1448  src/func_ov007_020c1448.c
 *   from:0x020c4734 kind:arm_call to:0x020c1404  src/func_ov007_020c1404.c
 *   from:0x020c484c kind:arm_call to:0x020c49bc  src/func_ov007_020c49bc.c
 *   from:0x020c4998 kind:load    to:0x0210325c   data_ov007_0210325c
 *
 * All three call targets are MATCHED bodies that are already in the ov007
 * slice, so nothing here invents a callee. The one DATA word is the question
 * port/tools/romdata.py's "is it dereferenced" rule exists for, and the answer
 * is YES: 0x020c4998 loads the ADDRESS and 0x020c47cc dereferences it one
 * instruction later. It is hosted -- port/ov007_syms.txt carries
 * data_ov007_0210325c in the mount -- and the ROM's own image holds 0x00001000
 * at that address, which is 1.0 in 20.12. Nothing in src/ writes it. It is the
 * global quad scale, and it is read here rather than folded to 0x1000 so a
 * writer that appears later is honoured.
 *
 * LITERAL POOL: thirteen words at 0x020c4988..0x020c49b8, twelve of them the
 * command ports listed above and the thirteenth the data address. There is no
 * bare constant to adjudicate.
 *
 * ========================= THE ARGUMENTS, FROM THE CALLER =================
 *
 * src/func_ov007_020c421c.c is matched and calls this twice, which fixes every
 * parameter without guessing:
 *
 *   func_ov007_020c4684(r4, *(u16 *)(r4 + 0x44), (*(int *)(r4 + 0x50) >> 12) & 0xff,
 *                       *(u8 *)(r4 + 0x4e), *(int *)r4, &v);
 *   func_ov007_020c4684(r4, 0, diff & 0xff, *(u8 *)(r4 + 0x7d), 0, r4 + 0x7e);
 *
 * and the body then spends them exactly where a DS material spends them:
 * argument 1 is stored to COLOR, argument 2 lands in POLYGON_ATTR bits 16..20
 * (alpha), argument 3 in bits 24..29 (polygon ID). Argument 4 is a flag word --
 * bit 3 selects a per-corner colour table, bits 0 and 1 select the flipped
 * path -- and argument 5 is the three-short translation the caller built on its
 * own stack. Nine callee-saved registers and a 0x24-byte frame put the fifth
 * and sixth arguments at [sp,#0x48] and [sp,#0x4c], which is what the body
 * reads.
 *
 * ============================ THE BODY ====================================
 *
 * 1. ALPHA ZERO DRAWS NOTHING. 0x020c46d0 compares argument 2 against 0 and
 *    returns before touching a register. That early return is why a trap
 *    returning 0 looked harmless for so long: it is the same shape.
 * 2. THE MATERIAL. self+0x5a indexes the table at self+0x5c for a texture
 *    record; COLOR takes argument 1; func_ov007_020c1448 writes
 *    TEXIMAGE_PARAM and func_ov007_020c1404 writes PLTT_BASE, both matched.
 *    POLYGON_ATTR is (self[0x54] | 0x80) | (id << 24) | (alpha << 16).
 * 3. THE BRACKET. MTX_PUSH, then MTX_TRANS with the caller's vector biased by
 *    half of the negated quad size, then MTX_SCALE with the quad size times
 *    data_ov007_0210325c, then the draw, then MTX_POP. The push and the pop
 *    are the ones this run's lane found were not arriving at all when they are
 *    written from a plain-built translation unit; here they go through
 *    ntr::io_write like every other command in this file.
 * 4. THE DRAW, two arms. With neither flip bit set it delegates to
 *    func_ov007_020c49bc, the matched vertex emitter, with eight arguments.
 *    With either flip bit set it emits the quad itself: BEGIN_VTXS 1, then
 *    four corners as TEXCOORD + VTX_16 / VTX_YZ / VTX_XZ / VTX_YZ, then
 *    END_VTXS. The four vertices are (0,1,0) (0,0,0) (1,0,0) (1,1,0) in 20.12,
 *    a UNIT QUAD -- which is what makes the MTX_SCALE above the whole of its
 *    size. Under self[0] bit 1 each corner also re-stores COLOR from
 *    self+0x46, +0x48, +0x4a, +0x4c, a per-corner gradient.
 *
 * THE TEXTURE COORDINATES ARE 4.4 FIXED, which is what the (x << 12) fields
 * being shifted left 8 and arithmetic-shifted right 16 comes to: a 20.12
 * coordinate through that pair is the same number times 16, and 16ths of a
 * texel is the DS TEXCOORD unit. The transcription keeps the ROM's own shift
 * pair rather than the simplification, because the pair also truncates to 16
 * bits and the simplification does not.
 *
 * WHAT IS NOT CLAIMED. This is not a match and no claim is made that mwccarm
 * would emit these instructions from this C. It is a transcription of what the
 * ROM does, in the order the ROM does it, with every store landing on the
 * command port the ROM's own literal pool names.
 */
#include <cstdio>
#include <cstdlib>

#include "ntr/mmio.h"

/* SM64DS_CELLQUAD_PROBE=<n>: the first n calls' INPUTS, one line each. DEFAULT
   OFF. A transcription is only as good as the numbers it is fed, and the two
   things that decide where a cell lands -- the record's own quad size at +0x20
   / +0x24 and the texture extents this derives from the bound record -- are
   invisible in a capture. This prints them at the moment they are spent, so a
   wrong reading of a field is a number to check rather than a picture to
   argue about. */
static int cellquad_probe_budget(void)
{
    static int n = -1;
    if (n < 0) {
        const char *e = std::getenv("SM64DS_CELLQUAD_PROBE");
        n = (e && *e) ? std::atoi(e) : 0;
    }
    return n > 0 ? n-- : 0;
}

/* SM64DS_CELLQUAD_OFF=1: THE KILL SWITCH, and it is a RUNTIME one on purpose.
 *
 * The behaviour this file replaces is exactly "return 0 and draw nothing", so
 * the pre-fix arm is one early return rather than a second build. That makes
 * the owner's before and after ONE binary at ONE .dsstate base, which is what
 * notes/port-selftest-bmp-gate.md asks for before two captures may be compared
 * at all -- and it is the arm the whole port shipped with until this file
 * existed, so it is a real control and not a reconstruction.
 *
 * It also carries the attribution: everything that appears on the title screen
 * between the two arms came through here and through nothing else. */
static int cellquad_off(void)
{
    static int off = -1;
    if (off < 0) {
        const char *e = std::getenv("SM64DS_CELLQUAD_OFF");
        off = (e && *e && *e != '0') ? 1 : 0;
        if (off)
            std::fprintf(stderr, "  [cellquad] SM64DS_CELLQUAD_OFF=1: the "
                         "title's polygon submitter draws nothing, which is "
                         "what the L2_UNMATCHED trap did\n");
    }
    return off;
}

extern "C" {

/* The two matched helpers this calls, spelled as their own src TUs define
   them. func_ov007_020c1448 takes a texture record and writes TEXIMAGE_PARAM;
   func_ov007_020c1404 takes the same record and writes PLTT_BASE. */
void func_ov007_020c1448(void *self, unsigned texGen, unsigned repeat,
                         unsigned flip, unsigned color0);
void func_ov007_020c1404(int *c);
void func_ov007_020c49bc(int arg0, int arg1, int arg2, int arg3, int arg4,
                         int arg5, unsigned short *arg6, int arg7);

/* The global quad scale, 0x00001000 in the ROM image. Dereferenced one
   instruction after its address is loaded; see PROVENANCE. */
extern int data_ov007_0210325c;

void func_ov007_020c4684(char *self, unsigned color, unsigned alpha,
                         unsigned polyid, unsigned flags, const short *vec);

void func_ov007_020c4684(char *self, unsigned color, unsigned alpha,
                         unsigned polyid, unsigned flags, const short *vec)
{
    /* 0x020c468c .. 0x020c46d8: the frame's live values, in the ROM's order.
       sizeS/sizeT come from the texture record when there is one and are
       overridden from self+0x0c / +0x0e when the corresponding count is 1;
       corner_s is the PRE-OVERRIDE value, captured at 0x020c4748 before the
       two overrides at 0x020c4768 and 0x020c4770 can reach it. */
    unsigned sizeS = 0, sizeT = 0, corner_s = 0;
    const int x12 = (int)(*(short *)(self + 0x70)) << 12;
    const int y12 = (int)(*(short *)(self + 0x72)) << 12;
    const unsigned short cols = *(unsigned short *)(self + 0x60);
    const unsigned short rows = *(unsigned short *)(self + 0x62);
    unsigned short *corner_tab =
        (flags & 8) ? *(unsigned short **)(self + 0x84) : (unsigned short *)0;
    int *tex;

    /* 0x020c46d0: alpha 0 draws nothing. */
    if (alpha == 0)
        return;

    /* The kill switch sits HERE rather than at the top so the two arms differ
       only in what reaches the geometry engine: the field reads above have no
       side effects, and the early return above is the ROM's own. */
    if (cellquad_off())
        return;

    /* 0x020c46e8 .. 0x020c4734: the material. */
    tex = (*(int ***)(self + 0x5c))[*(unsigned short *)(self + 0x5a)];
    NTR_MMIO(unsigned, 0x04000480) = color;                     /* COLOR */
    if (tex) {
        sizeS = 8u << (unsigned)tex[0x14 / 4];
        sizeT = 8u << (unsigned)tex[0x18 / 4];
    }
    func_ov007_020c1448(tex, 1u, *(unsigned *)(self + 0x64),
                        *(unsigned *)(self + 0x68),
                        *(unsigned *)(self + 0x6c));
    func_ov007_020c1404(tex);

    /* 0x020c4738 .. 0x020c475c: POLYGON_ATTR. */
    corner_s = sizeS;
    NTR_MMIO(unsigned, 0x040004a4) =
        ((*(unsigned *)(self + 0x54) | 0x80u) | (polyid << 24)) | (alpha << 16);

    /* 0x020c4760 .. 0x020c4770: a single row or column takes its extent from
       the record instead of from the texture. */
    if (tex) {
        if (cols == 1) sizeS = *(unsigned short *)(self + 0x0c);
        if (rows == 1) sizeT = *(unsigned short *)(self + 0x0e);
    }

    /* 0x020c4774 .. 0x020c4810: the bracket, the translation and the scale. */
    NTR_MMIO(unsigned, 0x04000444) = 0;                         /* MTX_PUSH */
    {
        const int w = *(int *)(self + 0x20);
        const int h = *(int *)(self + 0x24);
        const int nw = -w, nh = -h;
        const int scale = data_ov007_0210325c;
        NTR_MMIO(int, 0x04000470) =                             /* MTX_TRANS */
            (int)vec[0] + ((nw + (int)((unsigned)nw >> 31)) >> 1);
        NTR_MMIO(int, 0x04000470) =
            (int)vec[1] + ((nh + (int)((unsigned)nh >> 31)) >> 1);
        NTR_MMIO(int, 0x04000470) = (int)vec[2];
        NTR_MMIO(int, 0x0400046c) =                             /* MTX_SCALE */
            (int)((((long long)w * (long long)scale) + 0x800) >> 12);
        NTR_MMIO(int, 0x0400046c) =
            (int)((((long long)h * (long long)scale) + 0x800) >> 12);
        NTR_MMIO(int, 0x0400046c) = 0x1000;
    }

    if (cellquad_probe_budget())
        std::fprintf(stderr,
                     "[cellquad] quad(%d,%d) vec(%d,%d,%d) tex=%p sizeS=%u "
                     "sizeT=%u cols=%u rows=%u xy12=(%d,%d) alpha=%u id=%u "
                     "flags=%08x arm=%s\n",
                     *(int *)(self + 0x20), *(int *)(self + 0x24),
                     (int)vec[0], (int)vec[1], (int)vec[2], (void *)tex,
                     sizeS, sizeT, (unsigned)cols, (unsigned)rows, x12, y12,
                     alpha, polyid, flags,
                     (!(flags & 1) && !(flags & 2)) ? "020c49bc" : "inline");

    /* 0x020c4814 .. 0x020c496c: the two draw arms. */
    if (!(flags & 1) && !(flags & 2)) {
        func_ov007_020c49bc((int)sizeS, (int)sizeT, x12, y12, (int)cols,
                            (int)rows, corner_tab, (int)corner_s);
    } else {
        const int mode = *(int *)self;
        /* The ROM's own truncation: a 20.12 coordinate shifted left 8 and
           arithmetic-shifted right 16 is that coordinate in 16ths of a texel,
           kept to sixteen bits. */
        const unsigned t = (unsigned short)(((int)((unsigned)y12 << 8)) >> 16);
        const unsigned s = (unsigned short)(((int)((unsigned)x12 << 8)) >> 16);
        unsigned s2, t2;

        NTR_MMIO(unsigned, 0x04000500) = 1;                     /* BEGIN_VTXS */
        if (mode & 2)
            NTR_MMIO(unsigned, 0x04000480) = *(unsigned short *)(self + 0x46);
        NTR_MMIO(unsigned, 0x04000488) = s | (t << 16);          /* TEXCOORD */
        NTR_MMIO(unsigned, 0x0400048c) = 0x10000000;             /* VTX_16 */
        NTR_MMIO(unsigned, 0x0400048c) = 0;

        if (mode & 2)
            NTR_MMIO(unsigned, 0x04000480) = *(unsigned short *)(self + 0x48);
        t2 = (unsigned short)(((int)((unsigned)(((int)(sizeT << 12) *
                    (int)rows) + y12) << 8)) >> 16);
        NTR_MMIO(unsigned, 0x04000488) = s | (t2 << 16);
        NTR_MMIO(unsigned, 0x0400049c) = 0;                      /* VTX_YZ */

        if (mode & 2)
            NTR_MMIO(unsigned, 0x04000480) = *(unsigned short *)(self + 0x4a);
        s2 = (unsigned short)(((int)((unsigned)(((int)(sizeS << 12) *
                    (int)cols) + x12) << 8)) >> 16);
        NTR_MMIO(unsigned, 0x04000488) = s2 | (t2 << 16);
        NTR_MMIO(unsigned, 0x04000498) = 0x1000;                 /* VTX_XZ */

        if (mode & 2)
            NTR_MMIO(unsigned, 0x04000480) = *(unsigned short *)(self + 0x4c);
        NTR_MMIO(unsigned, 0x04000488) = s2 | (t << 16);
        NTR_MMIO(unsigned, 0x0400049c) = 0x1000;                 /* VTX_YZ */
        NTR_MMIO(unsigned, 0x04000504) = 0;                      /* END_VTXS */
    }

    NTR_MMIO(unsigned, 0x04000448) = 1;                          /* MTX_POP */
}

}  /* extern "C" */
