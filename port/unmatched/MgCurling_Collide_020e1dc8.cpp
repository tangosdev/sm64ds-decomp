// HOST TRANSCRIPTION of func_ov006_020e1dc8 -- dScMgCurling_c's SHOT-TIME
// SHELL SEPARATION. Run link60, lane CUR2.
//
// WHAT IT IS, PLAINLY. The moment you let go of a shell, this checks whether
// it is already overlapping another shell on the ice and, if it is, shoves
// that other shell clear -- and then checks whether the shell it just shoved
// has landed on top of a third one, and shoves that one too. It does not
// change anybody's speed or heading; it only un-stacks them. The velocity
// half of the collision is its sibling, func_ov006_020e20bc, transcribed in
// MgCurling_Collide_020e20bc.cpp.
//
// WHY IT EXISTS. It is one of the two return-0 faces in hal/scene_mg_faces.cpp
// that the owner's 2026-08-15 curling session walked into:
//     [scene] UNMATCHED ov004/ov006 body entered: func_ov006_020e1dc8
// banked at
//     the link60 run curling review log (private run dir)
// with the report "no collision against each other on the shells". Both faces
// are that one defect; see port/curling_round2.txt.
//
// ITS ONE CALLER IS MATCHED AND SETTLES THE ARGUMENTS.
// src/func_ov006_020e2c08.c is the release handler: it decides the tap landed
// on shell `idx`, writes that shell's heading (+0x4686), speed (+0x4668) and
// state (+0x4688 = 1), and THEN calls func_ov006_020e1dc8(self, idx) before
// playing the launch sound 0x1d3. So `idx` is the shell that was just shot,
// and this runs once per shot, not per frame.
//
// PROVENANCE. Not a banked near-miss; nearmiss/db.jsonl has no draft for this
// symbol. Read straight off a capstone listing of
//   extracted/overlays/overlay_0006.bin   at base 0x020BFEC0
// (ov006 base_address 34340544 from extracted/dsd/arm9_overlays/overlays.yaml
// id:6).
//
// THE BASE IS PROVED, NOT ASSUMED, and by this function's own relocations.
// config/arm9/overlays/ov006/relocs.txt:3440 says
//     from:0x020e2d6c kind:arm_call to:0x020e1dc8 module:overlay(6)
// and at that base the word at 0x020e2d6c is 0xebfffc15, a BL whose
// displacement resolves to exactly 0x020e1dc8. The sibling's anchor agrees:
// relocs.txt:3430 from:0x020e2b44 -> 0x020e20bc decodes the same way, and the
// plain word at 0x020e2864 (relocs.txt:3425, kind:load) READS 0x020e20bc. The
// dsd export extracted/dsd/arm9_overlays/ov006.bin was not used for anything.
//
// EXTENT. config/arm9/overlays/ov006/symbols.txt:608
//     func_ov006_020e1dc8 kind:function(arm,size=0x2f4) addr:0x020e1dc8
//   0x020e1dc8..0x020e20af   code, 186 ARM instructions
//   0x020e20b0..0x020e20bb   literal pool, 3 words (resolved below)
//   0x020e20bc               func_ov006_020e20bc begins. The extent is tight.
//
// ---- THE LITERAL POOL, RESOLVED ------------------------------------------
//
//   0x020e20b0  0x00004660   field offset, a plain immediate
//   0x020e20b4  0x00004664   field offset, a plain immediate
//   0x020e20b8  0x02082214   data_02082214, the arm9 angle table
//
// The first two carry NO reloc row, which is the evidence that they are
// offsets and not addresses; the third is relocs.txt:3411,
// `from:0x020e20b8 kind:load to:0x02082214 module:main`.
//
// ---- CALLEES: THE HOLE DOES NOT CASCADE ----------------------------------
//
// Three distinct callees, five call sites, all in the main module, and ALL
// THREE ALREADY HAVE DECOMPILED BODIES. Nothing had to be invented.
//
//   0x0203d744  _ZN4cstd4sqrtEy               x2   src/_ZN4cstd4sqrtEy.c
//   0x0203b4dc  _ZN4cstd5atan2E5Fix12IiES1_   x2   src/_ZN4cstd5atan2E5Fix12IiES1_.c
//   0x02012718  func_02012718                 x1   src/func_02012718.c
//
// That is the same callee set CT1 found for func_ov006_020e1854, plus nothing.
// The sibling in MgCurling_Collide_020e20bc.cpp has the same three and no
// others either.
//
// ---- THE SHELL RECORD, READ OUT OF THIS FUNCTION AND ITS MATCHED SIBLINGS -
//
// Five shells, stride 0x2c, based at this+0x4660. Every offset below is
// cross-checked against src/func_ov006_020e2868.c (the matched per-frame
// physics tick) and src/func_ov006_020e2c08.c (the matched release handler),
// which reach the same fields by the same arithmetic:
//
//   +0x4660   s32   position, axis A (Fix12).  020e2868 advances it by
//                   tbl[(ang>>4)*2+1] * speed and walls it at 0xc000/0xf4000
//   +0x4664   s32   position, axis B (Fix12).  020e2868 advances it by
//                   tbl[(ang>>4)*2]   * speed and walls it at 0xb4000/-0xd4000
//   +0x4668   s32   speed (Fix12)
//   +0x4686   u16   heading, a DS angle
//   +0x4688   u8    motion state (020e2c08 writes 1 on launch)
//   +0x4689   u8    shell is on the board
//   +0x468b   u8    "fast" flag, set when speed >= 0x3800
//
// AXIS A AND AXIS B ARE NOT NAMED X AND Z HERE, deliberately. Nothing in this
// function or in the two matched siblings distinguishes them beyond which
// half of the angle table each pairs with, and guessing a world axis would be
// inventing a fact. The pairing IS established, twice over: 020e2868 moves
// +0x4660 by tbl[2k+1] and +0x4664 by tbl[2k], and this function offsets them
// the same way round.
//
// ---- THE ANGLE TABLE -----------------------------------------------------
//
// data_02082214 is the arm9's shared angle table, s16 pairs indexed by
// (angle >> 4) -- 4096 entries over the DS's 16-bit turn. Matched callers
// that pin the layout: Matrix4x3_FromRotationX/Y/Z pass data_02082214[a*2]
// and [a*2+1] as the two components of a rotation, Camera_UpdateMatrices and
// Vec3_RotateYAndTranslate the same. This file indexes it exactly as
// func_ov006_020e2868.c does and does not name the two members, for the
// reason above.
//
// ---- THE TWO CONSTANTS ---------------------------------------------------
//
//   0x18 = 24   the contact test. Two shells touch at 24 units apart, and the
//               matched sibling agrees: func_ov006_020e20bc uses 0x18 for the
//               same test and clamps each shell's position with a HALF-width
//               of 0xc = 12 against the rink walls. 12 + 12 = 24.
//   0x1a = 26   the separation distance. Two units of slack past contact, so
//               the shell that was pushed is outside the contact radius and
//               the test does not fire again on the same pair next frame.
//
// ---- WHAT THE ROM DOES THAT LOOKS LIKE A MISTAKE, KEPT ANYWAY ------------
//
// 1. THE PUSHED SHELL IS PLACED RELATIVE TO THE PUSHER, NOT DISPLACED.
//    The ROM reads the PUSHER's position, adds the 26-unit offset and stores
//    the result into the PUSHED shell. It does not add anything to the pushed
//    shell's own position. So a shell that was 3 units away and a shell that
//    was 20 units away both end up at exactly 26 units, on the line between
//    them. That is a snap, not a nudge, and it is what the bytes say.
//
// 2. THE SOUND ONLY PLAYS ON A TWO-DEEP CASCADE. The call to func_02012718
//    is reached ONLY by falling out of the inner loop's hit path. If the shot
//    shell pushes one shell and that shell touches nobody, the inner loop runs
//    to exhaustion and returns at 0x020e2088 with no sound. The clack for the
//    ordinary one-on-one case comes from the sibling instead, which plays the
//    same id 0xe8 on every hit it handles. Transcribed as found.
//
// 3. THE CASCADE STOPS AT DEPTH TWO, and both loops stop at their first hit.
//    Three separate limits, all in the bytes: the outer loop returns after
//    handling one collision, the inner loop returns after handling one, and
//    there is no third level.
//
// SEMANTICS, NOT BYTES. This is a host copy shaped for MSVC, not a byte match.
// When a byte match lands in src/ this file retires per the port rule.
//
// NO GAME LOGIC WAS INVENTED. Every constant, branch, field offset and call
// below is carried over from the disassembly, and each block carries the ROM
// address it came from.
#include <stdio.h>
#include <stdlib.h>

extern "C" {

/* The three matched callees, spelled the way the rest of the port spells
   them. cstd::sqrt takes a u64 -- the ROM sign-extends the 32-bit sum of
   squares into r0/r1 and calls it, and src/func_0203d5dc.c is where the
   (u64)(s64) call idiom is taken from. */
unsigned int _ZN4cstd4sqrtEy(unsigned long long v);
int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
void func_02012718(void *id, int pos);

extern short data_02082214[];

unsigned port_mg_curling_collide_020e1dc8_entries(void);
void port_mg_curling_collide_020e1dc8(char *c, int idx);

}

namespace {

enum {
    SHELL_COUNT  = 5,
    SHELL_STRIDE = 0x2c,
    F_POS_A      = 0x4660,
    F_POS_B      = 0x4664,
    F_ONBOARD    = 0x4689,
    CONTACT      = 0x18,   /* 24 units: two shells of half-width 12 touching */
    SEPARATION   = 0x1a    /* 26 units: contact plus two of slack */
};

inline int *posA(char *c, int i)
{
    return (int *)(c + F_POS_A + i * SHELL_STRIDE);
}
inline int *posB(char *c, int i)
{
    return (int *)(c + F_POS_B + i * SHELL_STRIDE);
}
inline int onBoard(char *c, int i)
{
    return *(unsigned char *)(c + F_ONBOARD + i * SHELL_STRIDE);
}

/* The ROM's rounded Fix12 multiply, in the exact 64-bit shape it emits:
   umull/smull into a pair, add 0x800 with carry, take bits 12..43. Written as
   a long long so MSVC produces the same value; the ROM's own sequence is a
   64-bit multiply and a 64-bit shift, not a 32-bit one. */
inline int rom_fmul(int a, int b)
{
    return (int)(((long long)a * (long long)b + 0x800) >> 12);
}

/* `<< 12` through a helper for the reason CT1's rom_lsl12 exists: the values
   shifted here are signed and can be negative, and a bare left shift of a
   negative int is UB even though MSVC emits the LSL the ROM wants. */
inline int rom_lsl12(int v)
{
    return (int)((unsigned)v << 12);
}

/* 0x020e1e20..0x020e1e6c, and again at 0x020e1f70..0x020e1fbc. The two are the
   same seven instructions with different shells in them, so they are one
   function here.

   Both deltas come out as OTHER minus SELF and the angle is atan2(dB, dA) --
   the B-axis delta is the first argument. Both of those are the ROM's own
   operand order and both matter: the angle is used to offset A by tbl[2k+1]
   and B by tbl[2k], so swapping either rotates every separation a quarter
   turn.

   The distance is computed on the >>12 integer parts, not on the Fix12
   values, so it is in whole units and the 0x18 test is a whole-unit test. */
struct Contact { int hit; int angle; };

Contact probe(char *c, int self, int other)
{
    Contact r;
    int dB = (*posB(c, other) - *posB(c, self)) >> 12;
    int dA = (*posA(c, other) - *posA(c, self)) >> 12;
    int d2 = dB * dB + dA * dA;
    /* The ROM sign-extends d2 into a 64-bit argument (asr r1, r0, #0x1f) and
       calls the u64 sqrt with it. */
    int dist = (int)_ZN4cstd4sqrtEy((unsigned long long)(long long)d2);
    /* The atan2 result is truncated to 16 bits before use (lsl #16, lsr #16),
       which is what makes the >>4 below land inside the table. The B delta is
       the FIRST argument -- the ROM's own operand order. */
    r.angle = (int)(unsigned short)_ZN4cstd5atan2E5Fix12IiES1_(dB, dA);
    r.hit = (dist <= CONTACT);
    return r;
}

/* 0x020e1e70..0x020e1f54, and again at 0x020e1fc0..0x020e2058. Place `other`
   at SEPARATION units from `self` along the contact angle. */
void separate(char *c, int self, int other, int angle)
{
    int k = angle >> 4;
    *posA(c, other) = *posA(c, self)
                    + rom_lsl12(rom_fmul(data_02082214[k * 2 + 1], SEPARATION));
    *posB(c, other) = *posB(c, self)
                    + rom_lsl12(rom_fmul(data_02082214[k * 2], SEPARATION));
}

/* The transcription proper. Kept static and kept PURE: the entry counter and
   the trace live in the wrapper below, so nothing observational sits inside
   the body being transcribed. */
void collide_020e1dc8_body(char *c, int idx)
{
    /* 0x020e1dc8..0x020e1e04 -- prologue: the shot shell's two position
       fields, and the outer loop's cursor. */
    for (int i = 0; i < SHELL_COUNT; i++) {
        /* 0x020e1e08..0x020e1e1c */
        if (!onBoard(c, i)) continue;
        if (i == idx) continue;

        /* 0x020e1e20..0x020e1e6c */
        Contact hit = probe(c, idx, i);
        if (!hit.hit) continue;

        /* 0x020e1e70..0x020e1f54 -- shell i is snapped clear of the shot
           shell. */
        separate(c, idx, i, hit.angle);

        /* 0x020e1f58..0x020e2084 -- and shell i may now be sitting on a
           third. ONE level, and the loop leaves on its first hit. */
        for (int j = 0; j < SHELL_COUNT; j++) {
            if (!onBoard(c, j)) continue;
            if (i == j) continue;

            Contact hit2 = probe(c, i, j);
            if (!hit2.hit) continue;

            separate(c, i, j, hit2.angle);

            /* 0x020e205c..0x020e2074 -- and ONLY here. See note 2 in the
               header: a one-on-one push returns silently from the loop exit
               below, and the sibling is what clacks in that case. The
               position handed over is the SHOT shell's, not either of the
               two that were just moved. */
            func_02012718((void *)(size_t)0xe8u, *posA(c, idx));
            return;
        }
        /* 0x020e2088..0x020e2090 -- inner loop exhausted: return, no sound. */
        return;
    }
    /* 0x020e20a4..0x020e20ac */
}

unsigned g_entries;

}  /* namespace */

extern "C" unsigned port_mg_curling_collide_020e1dc8_entries(void)
{
    return g_entries;
}

/* The counter and the opt-in trace are the same shape CT1 gave
   MgCurling_State_020e1854.cpp, and for the same reason: "the body ran" has to
   be a measurement rather than an assumption, and the refusal line this file
   removes was the only signal anyone had. OFF unless SM64DS_MG_CURLING_TRACE
   is set, so every selftest frame is byte-identical with it compiled in. */
extern "C" void port_mg_curling_collide_020e1dc8(char *c, int idx)
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
                "  [scene] dScMgCurling_c SHOT SEPARATION 0x020e1dc8 ENTERED "
                "(port_mg_curling_collide_020e1dc8, host transcription, "
                "port/unmatched/MgCurling_Collide_020e1dc8.cpp)\n");
        fflush(stderr);
    }

    collide_020e1dc8_body(c, idx);
}
