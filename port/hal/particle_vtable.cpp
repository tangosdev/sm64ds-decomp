// The thirteen Particle::Callback vtables, as host storage.
//
// Particle::SysTracker's constructor builds fourteen callback sub-objects
// inside itself and points each one's vptr at one of these:
//
//     _ZN5dPa_c7level_c16simpleCallback_cC2Ev(p)  ->  *(void **)p = &_ZTVN5dPa_c7level_c10callback_cE;   /* then f3c4 */
//     _ZN5dPa_c7level_c15scaleCallback_cC1Ev(p)  ->  _ZN5dPa_c7level_c16simpleCallback_cC2Ev(p); p->vt = &_ZTVN5dPa_c7level_c15scaleCallback_cE;
//
// so the vptr is the ADDRESS of the symbol, and the two words at and just
// after it are the class's two virtual functions. The dispatch that matters
// is in _ZN8Particle10SysTracker8Contents6UpdateEv, the tracker's per-frame walk over its 0x40 system
// slots:
//
//     e->f10->vt->m[1](e->f10, e->fc, m)
//
// -- plain cdecl through slot 1. Slot 0 (SpawnParticles) is reached the same
// way from the spawn path.
//
// WHY THIS FILE EXISTS AT ALL. On the ROM these are .data words in arm9 at
// 0x0208f3a4..0x0208f464 holding DS code addresses, and the port has been
// bitten more than once by exactly that shape -- a table of raw DS addresses
// that must never be called directly. Here the fix is the simplest form of
// the seat: the port never loads that region (romdata.py's NAMED list does
// not carry it, and hal/auto_bss.cpp would otherwise hand out zeroed storage,
// which is a null call on the first frame), so instead of patching ROM words
// after the fact this file DEFINES the thirteen vtables outright, with host
// function pointers, in the ROM's own layout.
//
// THE LAYOUT IS THE ROM'S, NOT A GUESS. Each vtable is Itanium-shaped --
// [offset-to-top][typeinfo][slot0][slot1] -- and the vptr the constructors
// install points at slot0, four words in. Only slot0 and slot1 are ever
// read, so each vtable is self-contained in its own symbol as a two-element
// array and no adjacency between symbols is required (which is why this
// needs no romdata.py CONTIG run).
//
// EVERY PAIR BELOW IS THE ROM'S OWN RELOCATION. config/arm9/relocs.txt gives
// the destination of all 26 words; the comment on each line is the exact
// from:/to: pair, so this table can be re-derived and re-checked against the
// ROM at any time. port_particle_vtables_check() re-asserts the shape at
// runtime before the first frame.
//
// Slot 0 is SpawnParticles, slot 1 is OnUpdate. Note that slot 0 is SHARED:
// eight of the thirteen classes never override SpawnParticles and take either
// Particle::Callback's (a bare `return`) or Particle::SimpleCallback's. That
// sharing is the ROM's, not a simplification -- see the to: addresses.
// Five seat their own: Callback (f3b4), SimpleCallback (f3c4),
// EndingStarGlitter (f3d4), CheckLava (f404), Scale (f414). Of the eight that
// inherit, three take SimpleCallback's (Bubble f3a4, Splash f3e4,
// FitWaterSimple f444) and five take Callback's (CheckWater f3f4,
// CheckWaterRipple f424, Clip f434, FitWater f454, CleanParticle f464) -- so
// SimpleCallback::SpawnParticles is slot 0 of FOUR tables and Callback's is
// slot 0 of six.
//
// THAT SPLIT HAS BEEN CHALLENGED ONCE, AND IT HELD. A 2026-08-12 vtable-head
// sweep (the group-5 note in hal/w2_dtor_heads.cpp) reported that "the four
// particle tables put Particle::SimpleCallback::SpawnParticles at index 0 in
// the ROM where hal/particle_vtable.cpp seats the BASE
// Particle::Callback::SpawnParticles." Re-derived mechanically, all 26 slots
// agree with the ROM and none of the four is mis-seated; the seats have been
// these since the file was written. Re-check takes two greps, which is the
// whole reason every line carries its from:/to: pair:
//
//     config/arm9/relocs.txt   from:0x0208f3a4 -> to:0x02022640
//     config/arm9/symbols.txt  0x02022640 = SimpleCallback::SpawnParticles
//                              0x020226d0 = Callback::SpawnParticles (size 4)
//
// WHAT THE SWEEP WAS PROBABLY SEEING, because it is a real and separate fact:
// the matched TU src/_ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE
// .cpp does NOT link. It is one of the three ARM argument ride-throughs
// (slice_gate29.txt keeps it out on purpose), so the body those four slot-0
// seats actually reach is the host copy in
// port/unmatched/Particle_RideThroughs.cpp -- same C name, cdecl, arguments
// made explicit. So "the decompiled SimpleCallback::SpawnParticles is not in
// the binary" is true, and "the vtable seats the wrong class" is not. The
// distinction is the whole difference between a fidelity bug and a documented
// host-ABI exception, and slot 0 here is the second one.
#include <cstdio>
#include <cstdlib>
#include "dsstate_seg.h"

extern "C" {

/* the two base implementations, shared by most of the table */
void _ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE(void *, void *);
void _ZN5dPa_c7level_c10callback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE(void *, void *);
void _ZN5dPa_c7level_c16simpleCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);

/* the eleven overriding OnUpdates */
void _ZN5dPa_c7level_c16bubbleCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c20edStarKiraCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c20edStarKiraCallback_c14SpawnParticlesERN8Particle6SystemE(void *, void *);
void _ZN5dPa_c7level_c16splashCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c20checkWaterCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c20checkYoganCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c20checkYoganCallback_c14SpawnParticlesERN8Particle6SystemE(void *, void *);
void _ZN5dPa_c7level_c15scaleCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c15scaleCallback_c14SpawnParticlesERN8Particle6SystemE(void *, void *);
void _ZN5dPa_c7level_c26checkWaterRippleCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c14clipCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c24fitWaterSimpleCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c18fitWaterCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);
void _ZN5dPa_c7level_c23cleanParticleCallback_c8OnUpdateERN8Particle6SystemEb(void *, void *, int);

#define SPAWN(f) ((void *)&f)
#define UPDATE(f) ((void *)&f)

DSSTATE_BEGIN
/* BubbleCallback.  from:0x0208f3a4 to:0x02022640 | 0x0208f3a8 to:0x02022464 */
void *_ZTVN5dPa_c7level_c16bubbleCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c16bubbleCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* Callback (the base).
   from:0x0208f3b4 to:0x020226d0 | 0x0208f3b8 to:0x020226c8 */
void *_ZTVN5dPa_c7level_c10callback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c10callback_c8OnUpdateERN8Particle6SystemEb),
};
/* SimpleCallback.  from:0x0208f3c4 to:0x02022640 | 0x0208f3c8 to:0x02022630 */
void *_ZTVN5dPa_c7level_c16simpleCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c16simpleCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* EndingStarGlitterCallback.
   from:0x0208f3d4 to:0x0202222c | 0x0208f3d8 to:0x020221dc */
void *_ZTVN5dPa_c7level_c20edStarKiraCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c20edStarKiraCallback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c20edStarKiraCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* SplashCallback.  from:0x0208f3e4 to:0x02022640 | 0x0208f3e8 to:0x020224fc */
void *_ZTVN5dPa_c7level_c16splashCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c16splashCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* CheckWaterCallback.
   from:0x0208f3f4 to:0x020226d0 | 0x0208f3f8 to:0x02022160 */
void *_ZTVN5dPa_c7level_c20checkWaterCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c20checkWaterCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* CheckLavaCallback.
   from:0x0208f404 to:0x02022328 | 0x0208f408 to:0x020222f0 */
void *_ZTVN5dPa_c7level_c20checkYoganCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c20checkYoganCallback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c20checkYoganCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* ScaleCallback.  from:0x0208f414 to:0x020225d0 | 0x0208f418 to:0x020225a8 */
void *_ZTVN5dPa_c7level_c15scaleCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c15scaleCallback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c15scaleCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* CheckWaterRippleCallback.
   from:0x0208f424 to:0x020226d0 | 0x0208f428 to:0x020220a4 */
void *_ZTVN5dPa_c7level_c26checkWaterRippleCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c26checkWaterRippleCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* ClipCallback.  from:0x0208f434 to:0x020226d0 | 0x0208f438 to:0x02021e70 */
void *_ZTVN5dPa_c7level_c14clipCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c14clipCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* FitWaterSimpleCallback.
   from:0x0208f444 to:0x02022640 | 0x0208f448 to:0x02022418 */
void *_ZTVN5dPa_c7level_c24fitWaterSimpleCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c16simpleCallback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c24fitWaterSimpleCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* FitWaterCallback.
   from:0x0208f454 to:0x020226d0 | 0x0208f458 to:0x0202202c */
void *_ZTVN5dPa_c7level_c18fitWaterCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c18fitWaterCallback_c8OnUpdateERN8Particle6SystemEb),
};
/* CleanParticleCallback.
   from:0x0208f464 to:0x020226d0 | 0x0208f468 to:0x02021e40 */
void *_ZTVN5dPa_c7level_c23cleanParticleCallback_cE[2] = {
    SPAWN(_ZN5dPa_c7level_c10callback_c14SpawnParticlesERN8Particle6SystemE),
    UPDATE(_ZN5dPa_c7level_c23cleanParticleCallback_c8OnUpdateERN8Particle6SystemEb),
};

/* ---- the two dispatch tables func_0204b028 / func_0204b244 index ---------
   THE SAME BUG SHAPE ONE MORE TIME, and this one crashed a play session
   (2026-08-05, ~frame 9750, warping into Bob-omb Battlefield): a table of raw
   DS code words, called directly.

   romdata.py carried data_02099fb4 and data_02099fbc in its NAMED list as
   "8-byte constants", so the port got the ROM's own bytes -- four DS
   addresses -- and the billboard builder called straight through them:

       data_02099fbc[g->axis](rotX, rotY, rot);   -- the axis rotation
       data_02099fb4[g->flag](fa, fb);            -- the blend/cull pair

   port_particle_render -> Particle::RenderAll -> func_02049ee8 ->
   func_0204a5c8 -> func_0204af3c -> func_0204b244 -> 0x0204c0a8, which on the
   host is unmapped memory. It survived this long because the four callees are
   only ever named by these two tables, so the linker discarded three of them
   as unreferenced and nothing pointed at the gap.

   Defined here with host pointers, in the ROM's own order. Every entry is the
   ROM's own relocation out of config/arm9/relocs.txt, quoted per line, and
   removed from romdata.py's list in exchange. */
void func_0204c0a8(int, int, void *);
void func_0204c0e8(int, int, void *);
void func_0204c24c(int, int);
void func_0204c194(int, int);

/* from:0x02099fbc to:0x0204c0a8 / from:0x02099fc0 to:0x0204c0e8 */
void (*data_02099fbc[2])(int, int, void *) = {
    func_0204c0a8,
    func_0204c0e8,
};

/* from:0x02099fb4 to:0x0204c24c / from:0x02099fb8 to:0x0204c194 */
void (*data_02099fb4[2])(unsigned char, unsigned char) = {
    (void (*)(unsigned char, unsigned char))func_0204c24c,
    (void (*)(unsigned char, unsigned char))func_0204c194,
};
DSSTATE_END

/* Called once before the first frame. Cheap, and it catches the two ways
   this file can rot: a slot left null (the symbol got zeroed storage from
   somewhere else and this file lost the tie-break), and a slot holding
   something in the DS address range instead of a host pointer -- which is
   what a raw ROM word looks like and is the bug class this file exists to
   prevent. */
void port_particle_vtables_check(void)
{
    void **vts[13] = {
        _ZTVN5dPa_c7level_c16bubbleCallback_cE, _ZTVN5dPa_c7level_c10callback_cE, _ZTVN5dPa_c7level_c16simpleCallback_cE, _ZTVN5dPa_c7level_c20edStarKiraCallback_cE,
        _ZTVN5dPa_c7level_c16splashCallback_cE, _ZTVN5dPa_c7level_c20checkWaterCallback_cE, _ZTVN5dPa_c7level_c20checkYoganCallback_cE, _ZTVN5dPa_c7level_c15scaleCallback_cE,
        _ZTVN5dPa_c7level_c26checkWaterRippleCallback_cE, _ZTVN5dPa_c7level_c14clipCallback_cE, _ZTVN5dPa_c7level_c24fitWaterSimpleCallback_cE, _ZTVN5dPa_c7level_c18fitWaterCallback_cE,
        _ZTVN5dPa_c7level_c23cleanParticleCallback_cE,
    };
    static const unsigned addr[13] = {
        0x0208f3a4, 0x0208f3b4, 0x0208f3c4, 0x0208f3d4, 0x0208f3e4,
        0x0208f3f4, 0x0208f404, 0x0208f414, 0x0208f424, 0x0208f434,
        0x0208f444, 0x0208f454, 0x0208f464,
    };
    for (int i = 0; i < 13; ++i) {
        for (int s = 0; s < 2; ++s) {
            const void *p = vts[i][s];
            if (!p) {
                std::fprintf(stderr, "FATAL: particle vtable %08x slot %d is "
                             "null -- nothing seated it\n", addr[i], s);
                std::abort();
            }
            /* a DS code word would land in 0x01ff0000..0x023fffff */
            const unsigned v = (unsigned)(unsigned long long)(unsigned long)p;
            if (v >= 0x01ff0000u && v < 0x02400000u) {
                std::fprintf(stderr, "FATAL: particle vtable %08x slot %d "
                             "holds a DS address %08x, not a host pointer\n",
                             addr[i], s, v);
                std::abort();
            }
        }
    }
    /* the two billboard dispatch tables, same test. These were raw ROM words
       until 2026-08-05 and this loop is what would have said so on frame 0
       instead of ten thousand frames into a play session. */
    const void *disp[4] = {
        (const void *)data_02099fbc[0], (const void *)data_02099fbc[1],
        (const void *)data_02099fb4[0], (const void *)data_02099fb4[1],
    };
    static const unsigned dispaddr[4] = {
        0x02099fbc, 0x02099fc0, 0x02099fb4, 0x02099fb8,
    };
    for (int i = 0; i < 4; ++i) {
        const unsigned v = (unsigned)(unsigned long long)(unsigned long)disp[i];
        if (!v || (v >= 0x01ff0000u && v < 0x02400000u)) {
            std::fprintf(stderr, "FATAL: billboard dispatch word %08x holds "
                         "%08x, not a host pointer\n", dispaddr[i], v);
            std::abort();
        }
    }
}

}  /* extern "C" */
