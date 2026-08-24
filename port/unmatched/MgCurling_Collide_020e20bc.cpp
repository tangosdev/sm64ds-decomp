// HOST TRANSCRIPTION of func_ov006_020e20bc -- dScMgCurling_c's PER-FRAME
// SHELL-AGAINST-SHELL COLLISION. Run link60, lane CUR2.
//
// WHAT IT IS, PLAINLY. This is the bit of curling where one sliding shell hits
// another and they bounce off each other. Every frame, for the shell that just
// moved, it looks for another shell close enough to be touching; if it finds
// one it works out the line between them, swaps the part of each shell's
// motion that runs along that line (which is what makes a head-on hit stop the
// striker dead and send the target off), rotates the result back, writes both
// shells a new direction and speed, un-stacks them, keeps them inside the
// rink, and plays the clack.
//
// It is the SECOND half of the owner's third defect. The first half, the
// snap-apart done once at the moment of the shot, is func_ov006_020e1dc8 in
// MgCurling_Collide_020e1dc8.cpp. Both were return-0 faces; the owner's
// 2026-08-15 session walked into both:
//     [scene] UNMATCHED ov004/ov006 body entered: func_ov006_020e20bc
// banked at
//     the link60 run curling review log (private run dir)
// against the report "no collision against each other on the shells". This is
// the one that carries the physics; see port/curling_round2.txt.
//
// ITS CALLER IS MATCHED AND SETTLES THE ARGUMENTS AND THE CADENCE.
// src/func_ov006_020e2868.c is dScMgCurling_c's per-shell physics tick: it
// advances shell `idx` along its heading, bounces it off the four rink walls,
// applies the ice friction, and THEN calls func_ov006_020e20bc(c, idx) --
// every frame, for every shell it ticks, immediately before
// func_ov006_020e269c. So `idx` is the shell that just moved.
//
// A SECOND CALLER EXISTS, IT IS A VENEER, AND SEATING THIS BODY BROKE IT.
// src/func_ov006_020e285c.c is a twelve-byte ARM tail-call forwarder
// (relocs.txt:3425, from:0x020e2864 kind:load to:0x020e20bc) and its src
// declares itself AND its callee `(void)`. On the DS that is correct: r0 and
// r1 already hold self and the index across the bx, and the veneer never
// names them. On the host it was correct only for as long as 020e20bc was a
// face that read nothing -- unmatched/MgCurling_StateDispatch.cpp says as much
// where it dispatches 0x020e285c, and it was right when it was written.
//
// It stops being correct the moment this file exists, and that is this lane's
// doing rather than a defect it found: a (void) frame now stands between the
// one-argument dispatch slot and a body that wants both arguments. THE ANSWER
// IS THE TAIL JUMP, NOT A HOST COPY -- the matched src TU stays in the link
// and port/tools/tailjump_guard.py holds it to the jmp that carries the two
// arguments through. The note at the bottom of this file has the whole
// argument, including what the first attempt got wrong.
//
// PROVENANCE, BASE AND EXTENT.
// Read straight off a capstone listing of
//   extracted/overlays/overlay_0006.bin   at base 0x020BFEC0.
// The base is proved by this function's own relocation rows: relocs.txt:3430
//   from:0x020e2b44 kind:arm_call to:0x020e20bc module:overlay(6)
// and at that base the word there is 0xebfffd5c, a BL resolving to exactly
// 0x020e20bc; and relocs.txt:3425's kind:load word at 0x020e2864 literally
// READS 0x020e20bc. The dsd export was not used for anything.
//
// config/arm9/overlays/ov006/symbols.txt:609
//     func_ov006_020e20bc kind:function(arm,size=0x5e0) addr:0x020e20bc
//   0x020e20bc..0x020e2687   code, 371 ARM instructions
//   0x020e2688..0x020e269b   literal pool, 5 words (resolved below)
//   0x020e269c               next symbol begins. The extent is tight.
//
// ---- THE LITERAL POOL, RESOLVED ------------------------------------------
//
//   0x020e2688  0x00004660   field offset (plain immediate, no reloc)
//   0x020e268c  0x00004664   field offset (plain immediate, no reloc)
//   0x020e2690  0x00004686   field offset (plain immediate, no reloc)
//   0x020e2694  0x00004668   field offset (plain immediate, no reloc)
//   0x020e2698  0x02082214   data_02082214 (relocs.txt: kind:load, module main)
//
// ---- CALLEES: THE HOLE DOES NOT CASCADE ----------------------------------
//
// Three distinct callees, seven call sites, all in main, ALL already
// decompiled. Identical set to the sibling's, and to CT1's for 0x020e1854.
//
//   0x0203d744  _ZN4cstd4sqrtEy               x3   src/_ZN4cstd4sqrtEy.c
//   0x0203b4dc  _ZN4cstd5atan2E5Fix12IiES1_   x3   src/_ZN4cstd5atan2E5Fix12IiES1_.c
//   0x02012718  func_02012718                 x1   src/func_02012718.c
//
// ---- THE SHELL RECORD ----------------------------------------------------
//
// Five shells, stride 0x2c, based at this+0x4660. Cross-checked field for
// field against the two matched siblings src/func_ov006_020e2868.c and
// src/func_ov006_020e2c08.c; the table is repeated in the sibling
// transcription's header rather than referenced, because a reader of either
// file needs it. This function is the one that establishes +0x4668 and
// +0x4686 as a SPEED and a HEADING, because it is where they are combined
// through the angle table into a velocity and taken apart again.
//
//   +0x4660   s32   position, axis A (Fix12)
//   +0x4664   s32   position, axis B (Fix12)
//   +0x4668   s32   speed (Fix12)
//   +0x4686   u16   heading (DS angle)
//   +0x4688   u8    motion state
//   +0x4689   u8    shell is on the board
//   +0x468b   u8    "fast" flag
//
// THE MOTION-STATE GATE IS THIS FUNCTION'S OWN, and the sibling does not have
// it: a candidate is skipped when +0x4688 is 0 or 3. 020e2c08 writes 1 there
// on launch and 020e2868 writes 2 when the shell's speed decays to zero, so 0
// and 3 are the two states this function refuses to touch and 1 and 2 are the
// two it collides. Transcribed as the two separate compares the ROM emits.
//
// ---- THE CONSTANTS -------------------------------------------------------
//
//   0x18    = 24         contact distance (two half-widths of 12)
//   0xc     = 12         shell half-width, used against the walls
//   0x1b000 = 27 << 12   separation distance, in Fix12 this time. The sibling
//                        spells its 26 as a whole number and shifts after the
//                        multiply; this one folds the shift into the constant.
//                        Different spelling, and a DIFFERENT VALUE (27 vs 26).
//                        Both kept as found.
//   0xc000 / 0xf4000     the axis-A walls, 12 and 244
//   -0xd4000             the axis-B low wall, -212
//   0x3800               the "fast" threshold, the same one 020e2c08 uses
//   0xe8                 the clack, the same id the sibling plays
//
// ---- FOUR THINGS THE ROM DOES THAT A TIDY VERSION WOULD NOT --------------
//
// 1. THE ZERO-COMPONENT SUBSTITUTION AT 0x020e22e4. The instruction is
//        MOVEQ r3, lr, ASR #1
//    on the flags left by the ORRS at 0x020e2280 that finished computing the
//    struck shell's axis-B velocity component. In plain terms: if the struck
//    shell's B component comes out exactly zero -- which is the ordinary case,
//    because a shell sitting still has zero speed -- it is replaced by HALF the
//    striker's B component before the exchange runs. It is transcribed, not
//    removed. Three reasons it is not read as a decode error: the encoding is
//    unambiguous (0x01a030ce, cond EQ, MOV, Rd=r3, Rm=r14 ASR #1), the value it
//    writes is live (r3 is read twice at 0x020e2380 and 0x020e2384), and no
//    branch separates the flag-setter from the use. Whether it is the ROM
//    expressing "if the target is at rest, split" or a compiler artifact of a
//    ternary is not decidable from the bytes, so the bytes stand.
//
// 2. THE WALL TEST REUSES A CLOBBERED REGISTER. r1 holds the axis-A integer
//    position for the low-wall test; the low-wall branch overwrites it with a
//    Fix12 delta, and the HIGH-wall test that follows compares whatever is in
//    it. So a shell pushed off the low wall is high-wall-tested on a different
//    quantity. It is harmless -- the delta is large and negative there, so the
//    high test cannot fire, which is the right answer anyway -- but it is the
//    ROM's flow and the transcription keeps the same variable doing the same
//    two jobs rather than quietly splitting it in two.
//
// 3. THERE ARE ONLY THREE WALL CLAMPS, NOT FOUR. Axis A is clamped at both
//    ends; axis B only at the low end. src/func_ov006_020e2868.c, matched, has
//    all four (0xb4000 as well). Not repaired here.
//
// 4. THE DISTANCE TEST AND THE ANGLE DISAGREE ABOUT DIRECTION, AND USE
//    DIFFERENT SCALES. The distance is measured struck-minus-striker on the
//    >>12 whole-unit parts; the collision angle is measured striker-minus-
//    struck on the raw Fix12 values. atan2 does not care about the scale, and
//    the direction difference is real and load-bearing -- the separation below
//    pushes the STRIKER back out along it. Both kept exactly.
//
// SEMANTICS, NOT BYTES. A host copy shaped for MSVC, not a byte match. It
// retires when a byte match lands in src/.
//
// NO GAME LOGIC WAS INVENTED. Every constant, branch, field offset and call is
// carried over from the disassembly, and each block carries its ROM address.
#include <stdio.h>
#include <stdlib.h>

extern "C" {

unsigned int _ZN4cstd4sqrtEy(unsigned long long v);
int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
void func_02012718(void *id, int pos);

extern short data_02082214[];

unsigned port_mg_curling_collide_020e20bc_entries(void);
void port_mg_curling_collide_020e20bc(char *c, int idx);

}

namespace {

enum {
    SHELL_COUNT  = 5,
    SHELL_STRIDE = 0x2c,
    F_POS_A      = 0x4660,
    F_POS_B      = 0x4664,
    F_SPEED      = 0x4668,
    F_HEADING    = 0x4686,
    F_STATE      = 0x4688,
    F_ONBOARD    = 0x4689,
    F_FAST       = 0x468b,

    CONTACT      = 0x18,
    HALF_WIDTH   = 0xc,
    SEPARATION   = 0x1b000,   /* 27 units, already in Fix12 */
    WALL_A_LO    = 0xc000,
    WALL_A_HI    = 0xf4000,
    WALL_B_LO    = -0xd4000,
    FAST_SPEED   = 0x3800
};

inline int *posA(char *c, int i)  { return (int *)(c + F_POS_A + i * SHELL_STRIDE); }
inline int *posB(char *c, int i)  { return (int *)(c + F_POS_B + i * SHELL_STRIDE); }
inline int *speed(char *c, int i) { return (int *)(c + F_SPEED + i * SHELL_STRIDE); }
inline unsigned short *heading(char *c, int i)
{
    return (unsigned short *)(c + F_HEADING + i * SHELL_STRIDE);
}
inline unsigned char *state(char *c, int i)
{
    return (unsigned char *)(c + F_STATE + i * SHELL_STRIDE);
}
inline int onBoard(char *c, int i)
{
    return *(unsigned char *)(c + F_ONBOARD + i * SHELL_STRIDE);
}
inline unsigned char *fastFlag(char *c, int i)
{
    return (unsigned char *)(c + F_FAST + i * SHELL_STRIDE);
}

/* The ROM's rounded Fix12 multiply. Every one of the twenty-odd sites in this
   function is the same five instructions: a 64-bit multiply, add 0x800 with
   carry, take bits 12..43. Six of them are spelled umull-plus-sign-correction
   rather than smull, which is the same arithmetic. */
inline int rom_fmul(int a, int b)
{
    return (int)(((long long)a * (long long)b + 0x800) >> 12);
}

/* One component pair of the angle table, at (angle >> 4). Not named sin and
   cos: nothing in this function or in the matched siblings distinguishes the
   two members beyond which position axis each pairs with, and naming them
   would be inventing a fact. See the sibling's header. */
struct Rot { int m0, m1; };

inline Rot rot_at(int angle)
{
    Rot r;
    int k = angle >> 4;
    r.m0 = data_02082214[k * 2];
    r.m1 = data_02082214[k * 2 + 1];
    return r;
}

/* A shell's velocity, taken apart into its two position axes exactly the way
   src/func_ov006_020e2868.c puts it back together: axis A moves by
   tbl[2k+1] * speed and axis B by tbl[2k] * speed. */
struct Vel { int a, b; };

inline Vel velocity_of(char *c, int i)
{
    Rot m = rot_at(*heading(c, i));
    Vel v;
    v.a = rom_fmul(m.m1, *speed(c, i));
    v.b = rom_fmul(m.m0, *speed(c, i));
    return v;
}

/* Rotate a velocity by the frame `m` describes. The ROM builds two of these,
   one from the negated collision angle and one from the angle itself, and
   applies them in that order with the first components exchanged between the
   two shells -- an equal-mass elastic exchange along the contact normal. */
inline Vel rotate(Rot m, Vel v)
{
    Vel r;
    r.a = rom_fmul(m.m1, v.a) - rom_fmul(m.m0, v.b);
    r.b = rom_fmul(m.m0, v.a) + rom_fmul(m.m1, v.b);
    return r;
}

void collide_020e20bc_body(char *c, int idx)
{
    for (int j = 0; j < SHELL_COUNT; j++) {
        /* 0x020e20f0..0x020e2118 -- four separate refusals, in the ROM's own
           order, including the two-compare state gate. */
        if (!onBoard(c, j)) continue;
        if (idx == j) continue;
        int stj = *state(c, j);
        if (stj == 0) continue;
        if (stj == 3) continue;

        /* 0x020e211c..0x020e2150 -- contact test, on the whole-unit parts,
           struck minus striker. */
        {
            int dB = (*posB(c, j) - *posB(c, idx)) >> 12;
            int dA = (*posA(c, j) - *posA(c, idx)) >> 12;
            int d2 = dB * dB + dA * dA;
            int dist = (int)_ZN4cstd4sqrtEy((unsigned long long)(long long)d2);
            if (dist > CONTACT) continue;
        }

        /* 0x020e2154..0x020e21a4 -- the collision angle, striker minus struck,
           on the RAW Fix12 values this time. Note 4 in the header. */
        int collAngle = (int)(unsigned short)_ZN4cstd5atan2E5Fix12IiES1_(
                            *posB(c, idx) - *posB(c, j),
                            *posA(c, idx) - *posA(c, j));

        /* 0x020e21a8..0x020e2280 -- both velocities, from heading and speed. */
        Vel vIdx = velocity_of(c, idx);
        Vel vJ   = velocity_of(c, j);

        /* 0x020e22e4 -- NOTE 1 IN THE HEADER. Kept because the bytes say so. */
        if (vJ.b == 0) vJ.b = vIdx.b >> 1;

        /* 0x020e2284..0x020e22d0 -- the two rotation frames, from the negated
           angle and from the angle. Both are re-truncated to 16 bits, which is
           what keeps the >>4 inside the table. */
        Rot into = rot_at((int)(unsigned short)(-collAngle));
        Rot back = rot_at((int)(unsigned short)collAngle);

        /* 0x020e22d4..0x020e2420 -- into the contact frame. */
        Vel rIdx = rotate(into, vIdx);
        Vel rJ   = rotate(into, vJ);

        /* 0x020e2424..0x020e2494 -- EXCHANGE the normal components and rotate
           back. Each shell keeps its own tangential half and takes the other's
           normal half; that is the whole collision. */
        Vel nIdx, nJ;
        {
            Vel swappedIdx; swappedIdx.a = rJ.a;   swappedIdx.b = rIdx.b;
            Vel swappedJ;   swappedJ.a   = rIdx.a; swappedJ.b   = rJ.b;
            nIdx = rotate(back, swappedIdx);
            nJ   = rotate(back, swappedJ);
        }

        /* 0x020e2498..0x020e24bc -- the striker's new heading and speed. */
        *heading(c, idx) = (unsigned short)_ZN4cstd5atan2E5Fix12IiES1_(nIdx.b,
                                                                      nIdx.a);
        *speed(c, idx) = (int)_ZN4cstd4sqrtEy(
            (unsigned long long)((long long)nIdx.b * nIdx.b
                                 + (long long)nIdx.a * nIdx.a));

        /* 0x020e24c0..0x020e2534 -- and the striker is pushed back out to
           SEPARATION units from the shell it hit, along the contact angle.
           The ROM moves the STRIKER here, not the struck shell; the sibling
           moves the other one. */
        *posA(c, idx) = *posA(c, j) + rom_fmul(back.m1, SEPARATION);
        *posB(c, idx) = *posB(c, j) + rom_fmul(back.m0, SEPARATION);

        /* 0x020e2538..0x020e25ec -- keep the striker inside the rink, and
           give the struck shell whatever the wall took off, so the pair does
           not end up overlapping again against the boards.

           `wallA` is note 2 in the header: one variable, two jobs, clobbered
           in between exactly as the ROM's r1 is. */
        {
            int wallA = *posA(c, idx) >> 12;
            int zi    = *posB(c, idx) >> 12;

            if (wallA - HALF_WIDTH < 0) {
                wallA = *posA(c, idx) - WALL_A_LO;
                *posA(c, j) += wallA;
                *posA(c, idx) = WALL_A_LO;
            }
            if (wallA + HALF_WIDTH > 0x100) {
                *posA(c, j) += *posA(c, idx) - WALL_A_HI;
                *posA(c, idx) = WALL_A_HI;
            }
            if (zi - HALF_WIDTH < -0xe0) {
                *posB(c, j) += *posB(c, idx) - WALL_B_LO;
                *posB(c, idx) = WALL_B_LO;
            }
        }

        /* 0x020e25f0..0x020e2620 -- the struck shell's new heading and speed.
           The ROM does these AFTER the wall work, which is why they are here
           and not beside the striker's. */
        *heading(c, j) = (unsigned short)_ZN4cstd5atan2E5Fix12IiES1_(nJ.b,
                                                                    nJ.a);
        *speed(c, j) = (int)_ZN4cstd4sqrtEy(
            (unsigned long long)((long long)nJ.b * nJ.b
                                 + (long long)nJ.a * nJ.a));

        /* 0x020e2624..0x020e264c -- both shells are moving now, and the
           struck one's "fast" flag is recomputed on the same 0x3800 threshold
           src/func_ov006_020e2c08.c uses at launch. */
        *state(c, idx) = 1;
        *state(c, j) = 1;
        *fastFlag(c, j) = (unsigned char)(*speed(c, j) >= FAST_SPEED ? 1 : 0);

        /* 0x020e2650..0x020e265c -- the clack, positioned at the striker. */
        func_02012718((void *)(size_t)0xe8u, *posA(c, idx));
        return;
    }
    /* 0x020e267c..0x020e2684 -- nothing was in reach. */
}

unsigned g_entries;

}  /* namespace */

extern "C" unsigned port_mg_curling_collide_020e20bc_entries(void)
{
    return g_entries;
}

extern "C" void port_mg_curling_collide_020e20bc(char *c, int idx)
{
    static int on = -1;
    static int traced;
    if (on < 0) {
        const char *e = getenv("SM64DS_MG_CURLING_TRACE");
        on = (e && *e && *e != '0') ? 1 : 0;
    }

    ++g_entries;

    if (on && !traced) {
        traced = 1;
        fprintf(stderr,
                "  [scene] dScMgCurling_c SHELL COLLISION 0x020e20bc ENTERED "
                "(port_mg_curling_collide_020e20bc, host transcription, "
                "port/unmatched/MgCurling_Collide_020e20bc.cpp)\n");
        fflush(stderr);
    }

    collide_020e20bc_body(c, idx);
}

/* src/func_ov006_020e285c.c IS NOT HOSTED, and the reason is worth writing
 * down because the first attempt at this lane DID host it and the gate refused
 * the result.
 *
 * The veneer is
 *     ldr ip,[pc]; bx ip; .word 0x020e20bc
 * and its src spells both itself and its callee `(void)`. On ARM that is
 * correct -- r0 and r1 still hold self and the shell index across the bx.
 * unmatched/MgCurling_StateDispatch.cpp used to call it with no arguments and
 * its header argued, correctly at the time, that a callee which ignores its
 * arguments cannot read the wrong ones. That argument died with this file:
 * 020e20bc wants both arguments now.
 *
 * THE HOST ANSWER IS THE ONE THE PORT ALREADY BUILT FOR THIS EXACT SHAPE, and
 * it is not a host copy. MSVC compiles a one-call forwarder as a JMP, which
 * reuses the caller's own cdecl frame, so the two arguments the dispatch
 * pushes are still at [esp+4] and [esp+8] when 020e20bc reads them -- the same
 * mechanism that makes func_0204f76c hand its riders to func_0205ac5c, and the
 * same one roughly fifty ov007 rows depend on. The dispatch now declares
 * 020e285c as (char *, int) and passes (c, a); the matched src TU is untouched
 * and STAYS IN THE LINK, which is where the first attempt went wrong. Taking
 * it out of port/slice_mg1.txt and hosting it here cost a linked TU and turned
 * the gate red at 5830.
 *
 * NOTHING ABOUT THAT IS SAFE BY ACCIDENT. port/tools/tailjump_guard.py carries
 * a CLASS C row for it, checked against every map on every build, so /Od, /Ob0
 * or one added statement in the veneer fails the build instead of silently
 * feeding this body a garbage `this`. */
