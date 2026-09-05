// RUN LINKW WAVE 9, lane w9-reg: TILTING_PLATFORM_LLL (id 72, ov064).
//
// The single largest skipped class in the wave-8 census: FOURTEEN instances
// across two levels (44 ex_m_map x10, 48 ex_w_map x4), and the overlay it
// lives in is ALREADY mounted twice over (gate 177 whole-of-data per symbol,
// gate 178 its second block), so this is registration work with no new mount
// in front of it.
//
// It is also the exact twin of hal/actor_classes_ov045.cpp's
// TILTING_PLATFORM_BFS (id 141): same 848-byte Platform object, same Model at
// +0xd4 and MovingMeshCollider at +0x124, the same two ov002 tilting bodies in
// slots 6/9, the same base-then-own factory trap, and the same three-word file
// table feeding two Init/Cleanup veneers. That file is the exemplar this one is
// built on and every departure from it is called out below.
//
// ---- WHICH CLASS THIS IS, and the five reads that settle it ---------------
//
// dsd lays each mwcc class block out as [bodies..., Spawn] and names the block
// as if it STARTED at the Spawn, so the bodies of id 72 are spelled
// _ZN12MetalNetLift* and its table is spelled _ZTV12MetalNetLift -- both are the
// NEIGHBOUR's names. id 69 (METAL_NET_LIFT, hosted by gate 178) is a different
// class with a different table (_ZTV17daObjFl_Amilift_c, 0x0211bc68) and the
// func_ov064_02117* bodies. hal/actor_classes_ov064_gate178.cpp's own header
// says so in as many words and defers id 72 to "a follow-up gate"; this is it.
//
//   route 1  ACTOR_SPAWN_TABLE 0x02090864 + 72*4 = 0x02090984 -> 0x0211bd08
//   route 2  the raw record at 0x0211bd08, read out of
//            extracted/overlays/overlay_0064.bin at base 0x02115ee0 (ov064 is
//            compressed:true in overlays.yaml, so the dsd export is the
//            COMPRESSED image and halfword reads out of it are noise):
//              word[0] = 0x021180fc  TiltingPlatformLll_Spawn, INSIDE ov064
//                                    [0x02115ee0, 0x0211c640)
//              +4 halfword = 72      the id the registry itself cross-checks
//            BOTH attribution routes, which is the rule -- sibling overlays
//            share this window and either route alone has produced a wrong
//            answer before.
//   route 3  +6 list priority = 280, Nintendo's, left alone
//   route 4  the Spawn's own vtable-store reloc (below) names 0x0211bd2c
//   route 5  the typeinfo word at 0x0211bd28 (vtable[-1]) resolves to
//            0x0211bce8, whose name string is `14daObjFl_Gura_c` -- gura, the
//            tilting one, the same RTTI root as ov045's TiltingPlatformBfs
//            (`15daObjKm2_Gura_c`). The RTTI exists and was read; it is not
//            assumed absent.
//
// ---- THE WIDTH: THIRTY-TWO, pinned twice ----------------------------------
//
// The reloc run at 0x0211bd2c is 32 entries and ends at 0x0211bdac; the next
// dsd data symbol is data_ov064_0211bdac, landing EXACTLY on that end. Both
// pins agree, so the width is 32 and slot 31 (ov002 0x020ee55c,
// _ZN8Platform4KillEv, already in walk_window.map) is real rather than a read
// past the end -- the [31] bug hal/actor_classes_wf.cpp's header describes.
//
// ---- SLOTS 6 AND 9 ARE ov002's, NOT ov064's -------------------------------
//
// Behavior and Render are func_ov002_020b616c / func_ov002_020b6144, the shared
// TILTING pair -- the SAME two bodies ov045's id 141 dispatches through, both
// already linked, so they cost this lane no new TU. A fill that only wrote
// 0/3/16/17 out of ov064 would leave this class's Behavior and Render null.
//
// ---- THE FACTORY TRAP: base store then own store --------------------------
//
// src/TiltingPlatformLll_Spawn.c ends `p[0] = _ZTV14daObjFl_Gura_c; p[0] = VT1;`
// and the recovered NAMING IS SHIFTED against what the ROM does. Disassembling
// the body (ov064 0x021180fc, 0x3c bytes) settles which store is which:
//
//     021180fc  STMFD sp!, {r4, lr}
//     02118100  MOV   r0, #0x350            <- 848, the object size
//     02118104  BL    0x02043444            <- ActorBase::operator new
//     02118108  MOVS  r4, r0
//     0211810c  BEQ   0x02118124
//     02118110  BL    0x020eea50            <- Platform's constructor (ov002)
//     02118114  LDR   r1, =0x02109084       <- the ov002 BASE table
//     02118118  LDR   r0, =0x0211bd2c       <- id 72's OWN table
//     0211811c  STR   r1, [r4, #0]          <- base FIRST
//     02118120  STR   r0, [r4, #0]          <- own SECOND
//
// base first, own second. So in the recovered source the RTTI spelling
// _ZTV14daObjFl_Gura_c sits on the BASE store and the zeroed shared-header
// placeholder VT1 (hal/auto_bss.cpp) sits on the OWN one. A raw spawn therefore
// returns a live object dispatching through a zeroed array. The factory wrapper
// below calls the real Spawn and reseats p[0] onto this class's host table,
// which is the ROM's final value -- exactly port_factory_tilting_platform_bfs.
//
// The alias points _ZTV14daObjFl_Gura_c at the OWN table, which is what the
// destructors need and what the relocs say: D1 (0x02118020) and D0 (0x02118070)
// each carry the pool triple 0x0211bd2c / 0x02109084 / 0x0210ae38, so their
// three transient stores are own-table, then _ZTV10dBgActor_c (the ov002 base,
// already defined port-wide), then VT2. The one cost of the alias is that the
// Spawn's transient BASE store writes the own table instead; nothing dispatches
// between the two stores, and the wrapper fixes the final value either way.
//
// ---- THE D1/D0 ARE LINKED, NOT REPLACED -----------------------------------
//
// Both are dying-object writes: three vptr stores with no dispatch between
// them, then MovingMeshCollider::~ at +0x124, Model::~ at +0xd4, Actor::D2 (and
// in D0, Memory::Deallocate against G0, the game heap already aliased to
// _data_020a0eac). That is the same reading hal/actor_classes_ov045.cpp records
// for its own ten D1/D0 bodies, so these two TUs are called directly.
//
// ---- RESOURCES: ONE SINIT, AND IT IS THIS LANE'S TO RUN -------------------
//
// InitResources and CleanupResources are two-line cross-overlay veneers onto
// ov002's tilting base pair (func_ov002_020b6244 / func_ov002_020b60fc, both
// already linked) through the three-word file table data_ov064_0211adb0, whose
// words relocate to
//     [0] 0x0211c768  the model SharedFilePtr
//     [1] 0x0211c770  the collision SharedFilePtr
//     [2] 0x0211bb8c  the CLPS record (0x20 bytes)
// and __sinit_ov064_0211b078 is the ov064 initialiser that builds exactly those
// two SFPs (model dtor func_02017ab4 through node 0x0211c778, clsn dtor
// 0x02017b34 through node 0x0211c784) and nothing else. port/ov064_syms.txt
// already records that the eight un-run ov064 sinits have a CLEAN reach audit --
// each touches only ov064-internal bss and arm9 main, none carries a
// cross-overlay reloc -- "so a follow-up gate can run its own subset without a
// hazard". This runs that one subset, and its four bss cells are disjoint from
// every sinit actor_overlays.cpp already runs (checked cell by cell).
//
// WHY THE BRING-UP IS IN THIS FILE. hal/actor_overlays.cpp is where ov064's
// pack_check/syms_patch and gate 177/178's sinits live and is the right home for
// this one too, but no lane owns that file in this wave, so the bring-up rides
// the first registry fill behind a done-guard -- the ov045 shape exactly. The
// ordering that makes it safe is the same one that file measured:
// port_actor_overlays_sinits() runs to completion BEFORE
// port_actor_registry_install(), so ov064's mount patch has already run when
// this fires; the SFPs it builds are read by InitResources, which runs at spawn,
// after registration. THE HANDOFF: whoever next owns hal/actor_overlays.cpp
// should move this one call beside the gate-178 block and cut the guard here.
//
// ---- THE VTABLE IS HOST STORAGE -------------------------------------------
//
// 0x0211bd2c is EXCLUDED from the ov064 per-symbol mount and declared here as a
// host array -- the ov015/ov016/ov080 rule, since a mounted vtable hands the
// factory DS code addresses. Its two names (the dsd _ZTV12MetalNetLift and the
// RTTI _ZTV14daObjFl_Gura_c) are aliased onto the one array.
#include <cstdio>

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include "dsstate_seg.h"

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half, slots 1..30 */
int _ZN5Actor19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                   /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                     /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                    /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);   /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                         /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);      /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);          /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);          /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);              /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);              /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */
void _ZN8Platform4KillEv(void *self);                              /* slot 31 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* id 72's own four bodies, spelled with the neighbour's class name (the dsd
   block shift documented above). Init/Cleanup are the ov002 veneers. */
int _ZN12MetalNetLift13InitResourcesEv(void *self);     /* slot 0  */
int _ZN12MetalNetLift16CleanupResourcesEv(void *self);  /* slot 3  */
int *_ZN12MetalNetLiftD1Ev(int *self);                  /* slot 16 */
int *_ZN12MetalNetLiftD0Ev(int *self);                  /* slot 17 */
/* slots 6 and 9: ov002's shared TILTING pair, already linked for ov045's id
   141. Behavior slerps the platform toward the rider's push; Render is its
   sibling. */
int func_ov002_020b616c(char *self);                    /* slot 6  */
int func_ov002_020b6144(void *self);                    /* slot 9  */
void *TiltingPlatformLll_Spawn(void);

/* the one ov064 initialiser this class needs (see the header) */
void __sinit_ov064_0211b078(void);

DSSTATE_BEGIN
void *_ZTV12MetalNetLift[32];
DSSTATE_END
}
/* gura = the tilting one. The D1/D0 restore the OWN table by this RTTI
   spelling; the relocs at 0x02118064 / 0x021180c4 say 0x0211bd2c, which is this
   array. */
#pragma comment(linker, "/alternatename:__ZTV14daObjFl_Gura_c=__ZTV12MetalNetLift")

// ---- the trap --------------------------------------------------------------
static void w9tp_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov064/w9 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m,
                    "unhosted ov064/w9 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define W9TP_TRAP(n) \
    static int __fastcall w9tp_trap##n(void *s, void *) \
    { w9tp_trap_report(s, n); return 0; }
W9TP_TRAP(13) W9TP_TRAP(14) W9TP_TRAP(17)
#undef W9TP_TRAP

static int __fastcall w9tp_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall w9tp_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall w9tp_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall w9tp_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall w9tp_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall w9tp_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall w9tp_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall w9tp_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall w9tp_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall w9tp_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall w9tp_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall w9tp_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall w9tp_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall w9tp_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall w9tp_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall w9tp_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall w9tp_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall w9tp_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall w9tp_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall w9tp_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall w9tp_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall w9tp_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall w9tp_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* Slots 1..30, every word read off the ROM table's relocations. The caller
   writes its own 0/3/6/9/16/17 and its 31. Slots 13/14 are the ActorBase
   Virtual34/Virtual38 traps and 30 declines -- the ov045 reading, 30's ROM body
   returns a Vector3 by value and the sret contract is unproved. Slot 17 traps
   here and the caller replaces it.

   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug, repro
   and dumpbin proof in hal/actor_classes_ov002g200.cpp. */
static void w9tp_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)w9tp_binit;
    vt[2]  = (void *)w9tp_ainit;
    vt[4]  = (void *)w9tp_bclean;
    vt[5]  = (void *)w9tp_aclean;
    vt[7]  = (void *)w9tp_bbeh;
    vt[8]  = (void *)w9tp_abeh;
    vt[10] = (void *)w9tp_bren;
    vt[11] = (void *)w9tp_aren;
    vt[12] = (void *)w9tp_pdes;
    vt[13] = (void *)w9tp_trap13;
    vt[14] = (void *)w9tp_trap14;
    vt[15] = (void *)w9tp_heap;
    vt[17] = (void *)w9tp_trap17;
    vt[18] = (void *)w9tp_yoshi;
    vt[19] = (void *)w9tp_turn_egg;
    vt[20] = (void *)w9tp_v50;
    vt[21] = (void *)w9tp_pounded;
    vt[22] = (void *)w9tp_atk1;
    vt[23] = (void *)w9tp_atk2;
    vt[24] = (void *)w9tp_kicked;
    vt[25] = (void *)w9tp_pushed;
    vt[26] = (void *)w9tp_cannon;
    vt[27] = (void *)w9tp_mega;
    vt[28] = (void *)w9tp_under;
    vt[29] = (void *)w9tp_egg;
    vt[30] = (void *)port_actor_s30_base;
}

/* The one ov064 sinit this lane runs; see the header for why it is here rather
   than in hal/actor_overlays.cpp and why running it late is safe. */
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "w9tp_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_w9tp_bringup_done;
DSSTATE_END

static void w9tp_bringup(void)
{
    if (g_w9tp_bringup_done)
        return;
    g_w9tp_bringup_done = 1;
    __sinit_ov064_0211b078();
}

// ============================================================================
// TILTING_PLATFORM_LLL (id 72) -- table 0x0211bd2c, the _ZN12MetalNetLift*
// bodies. x10 on level 44 (ex_m_map), x4 on level 48 (ex_w_map).
// ============================================================================
//
// 848-byte object built by Platform's constructor; Model at +0xd4,
// MovingMeshCollider at +0x124 (both straight out of the D1/D0 chains, which
// also settle the member order).
static int __fastcall w9tp_init(void *s, void *)
{ return _ZN12MetalNetLift13InitResourcesEv(s); }
static int __fastcall w9tp_clean(void *s, void *)
{ return _ZN12MetalNetLift16CleanupResourcesEv(s); }
static int __fastcall w9tp_behavior(void *s, void *)
{ return func_ov002_020b616c((char *)s); }
static int __fastcall w9tp_render(void *s, void *)
{ port_actor_render_probe("TILTING_PLATFORM_LLL", (char *)s + 0xd4);
  return func_ov002_020b6144(s); }
static int __fastcall w9tp_d1(void *s, void *)
{ return (int)(size_t)_ZN12MetalNetLiftD1Ev((int *)s); }
static int __fastcall w9tp_d0(void *s, void *)
{ return (int)(size_t)_ZN12MetalNetLiftD0Ev((int *)s); }

/* The factory ends on `p[0] = <base>; p[0] = VT1;` -- base store then the
   zeroed placeholder (the disassembly is in this file's header). The wrapper
   reseats the ROM's final value. */
extern "C" void *port_factory_tilting_platform_lll(void)
{
    void *p = TiltingPlatformLll_Spawn();
    if (p)
        *(void **)p = (void *)_ZTV12MetalNetLift;
    return p;
}

extern "C" void hal_fill_tilting_platform_lll_vtable(void)
{
    w9tp_bringup();
    void *volatile *vt = (void *volatile *)_ZTV12MetalNetLift;
    w9tp_fill_shared(vt);
    vt[0]  = (void *)w9tp_init;
    vt[3]  = (void *)w9tp_clean;
    vt[6]  = (void *)w9tp_behavior;
    vt[9]  = (void *)w9tp_render;
    vt[16] = (void *)w9tp_d1;
    vt[17] = (void *)w9tp_d0;
    vt[31] = (void *)w9tp_kill;
}
