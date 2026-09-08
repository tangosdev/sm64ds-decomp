// Gate 17: the level overlay's own static initialisers, and the two tables
// they leave holding DS addresses.
//
// ov009 carries four __sinit functions. They are ordinary matched src and the
// port runs them exactly where the DS runs them -- after the overlay is
// mounted and before anything spawns -- because every SharedFilePtr the
// level's own actors load through is constructed there, and so are the three
// seven-element Vector3 arrays the moat spawns its bubbles along.
//
// What they leave behind that a host cannot use as-is is the Bird's behaviour
// table: four mwcc {function, delta} pairs copied out of the overlay's own
// statics, so four DS CODE ADDRESSES. Same treatment as the sign's state
// machine and the mushroom's type table (port/unmatched/): check the word the
// sinit left against the ROM address the host body was compiled from, then
// seat the host body over it. A disagreement means the mount is pointing at
// the wrong bytes and says so rather than calling into the overlay image.
#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"
#include "Bird.h"      /* for the cdecl face at the end of this file */

/* THE GUARD IS CAPTURED, and it is the same defect the level mount cache had.
 *
 * Everything port_ov009_sinits does lands in .dsstate: port_ov009_syms_patch
 * rebases the overlay image's own words, and the four __sinit functions
 * construct SharedFilePtrs and Vector3 arrays that are hosted DS globals. A
 * save-state restore rolls all of that back. While `done` was a plain host
 * static it did NOT roll back, so the pass never ran again and the world held
 * RAW DS pointers for the rest of the session -- every access ov009 made
 * through data_ov009_02112bc4+20 landing on the zero-filled DS reservation at
 * 0x02112238 instead of the hosted block.
 *
 * That was measured before it was fixed. Three runs, one binary
 * (RELOADRV's ov009_hazard.log, re-run here): a plain castle-grounds boot
 * reads the record at 00CFB3E8 with "storage in .dsstate (captured)"; a
 * level-2 boot (ov009 never mounted) reads 02112238, "NOT CAPTURED"; and a
 * castle-grounds boot that HAD run the sinits, then restored the level-2
 * state, went back to 02112238 and stayed there.
 *
 * Bracketing makes the flag and the bytes it describes roll together, so the
 * pass re-runs exactly when its results were rolled away and not otherwise --
 * the same choice, for the same reason, as g_level_mounted in
 * hal/level_boot.cpp, which has the long form of the argument. */
DSSTATE_BEGIN
static int g_ov009_sinits_done;
DSSTATE_END

/* the fix-off half of the A/B; see port_mount_cache_stash in hal/level_boot.cpp */
static int g_ov009_sinits_done_stash;

extern "C" void port_ov009_sinit_stash(void)
{ g_ov009_sinits_done_stash = g_ov009_sinits_done; }

extern "C" void port_ov009_sinit_unstash(void)
{ g_ov009_sinits_done = g_ov009_sinits_done_stash; }

extern "C" {

void __sinit_ov009_02112458(void);
void __sinit_ov009_02112524(void);
void __sinit_ov009_02112a5c(void);
void __sinit_ov009_02112ac8(void);
void port_ov009_pack_check(void);
void port_ov009_syms_patch(void);

/* the Bird's four states, in the order the sinit seats them */
void func_ov009_021116ec(void *);   /* 0 */
void func_ov009_021115d8(void *);   /* 1 */
void func_ov009_02111234(void *);   /* 3 */

struct PortPmf { unsigned fn; int ptr; };
extern PortPmf data_ov009_02113c48[];

}  /* extern "C" */

/* The Bird's state 2 (ov009 0x0211145c, the "fly away" setup) is a
   NONMATCHING asm hatch in src -- an mwcc `asm { }` block rather than C -- so
   there is no body MSVC can compile. Seated by name: the two birds on the
   castle grounds start in state 0 and the walk says so if one ever gets here. */
static void port_bird_state2(void *)
{
    std::fprintf(stderr, "FATAL: Bird state 2 (ov009 0x0211145c) is an asm "
                 "hatch, not C -- no host body exists\n");
    std::abort();
}

static const struct { unsigned rom; void (*host)(void *); } g_bird[4] = {
    {0x021116ec, func_ov009_021116ec},
    {0x021115d8, func_ov009_021115d8},
    {0x0211145c, port_bird_state2},
    {0x02111234, func_ov009_02111234},
};

extern "C" void port_ov009_sinits(void)
{
    if (g_ov009_sinits_done)
        return;
    g_ov009_sinits_done = 1;
    port_ov009_pack_check();
    /* the per-symbol mount's own internal pointers (the BTA's
       four, the CLPS chain) rebased onto host storage */
    port_ov009_syms_patch();
    __sinit_ov009_02112458();
    __sinit_ov009_02112524();
    __sinit_ov009_02112a5c();
    __sinit_ov009_02112ac8();
    for (int i = 0; i < 4; ++i) {
        if (data_ov009_02113c48[i].fn != g_bird[i].rom) {
            std::fprintf(stderr, "FATAL: Bird state %d: the sinit left %08x, "
                         "the ROM's own table says %08x -- WRONG BYTES\n", i,
                         data_ov009_02113c48[i].fn, g_bird[i].rom);
            std::abort();
        }
        /* Every one of the four is the plain nonvirtual { function, 0 } form
           -- verified against the overlay image, all four deltas zero -- so
           the host reads the first word back as a function taking `this`. */
        if (data_ov009_02113c48[i].ptr != 0) {
            std::fprintf(stderr, "FATAL: Bird state %d is not a plain "
                         "pointer-to-member (delta %d)\n", i,
                         data_ov009_02113c48[i].ptr);
            std::abort();
        }
        data_ov009_02113c48[i].fn = (unsigned)(size_t)g_bird[i].host;
    }
}

/* THE FACE, and why this row needed nothing else (run link100, lane PMFB1).

   src/_ZN4Bird8BehaviorEv.cpp is on port/slice_pmfc.txt now and the host copy
   that used to stand here is gone. Everything that row needed was already in
   this file: port_ov009_sinits above runs the four ov009 sinits, then checks
   each of data_ov009_02113c48's four slots against the ROM address the reloc
   table promises AND against a zero delta, aborting loudly on either, and
   only then writes the HOST body address into the code word. So by the time
   anything can dispatch, the word the matched TU reads is a host address.

   The matched TU does not use a pointer-to-member at all. It reads the pair
   as two ints, tests bit 0 itself, and calls the code word as a raw cdecl
   void (*)(void *) with the receiver PUSHED -- so unlike the CALL-shaped rows
   of this family it needs no __fastcall receiver face, and unlike the thirteen
   tail-jump rows it reads no table record, so the /Zp4 stride correction in
   block R9d does not apply to it either. Its `obj = this + (ptr >> 1)` arm is
   the ROM's own arithmetic and the seat has already proven every ptr zero.

   What it DOES need is a name. The matched TU defines the real C++ method
   Bird::Behavior(), so its object publishes ?Behavior@Bird@@QAEHXZ, while every
   caller in the port spells the C name _ZN4Bird8BehaviorEv that the host copy
   used to define. That is a one-line cdecl face, the shape hal/fold2_faces.cpp
   uses for thirty of these. The call is qualified so it cannot become a
   virtual dispatch. */
extern "C" {
void Vec3_Asr(void *dst, const void *src, int n);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short a);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, short a);
int _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *m, int rad, int h, unsigned u);
int _ZN9Animation7AdvanceEv(void *a);
extern int data_020a0e68[12];

int _ZN4Bird8BehaviorEv(void *s)   { return ((Bird *)s)->Bird::Behavior(); }
}  /* extern "C" */
