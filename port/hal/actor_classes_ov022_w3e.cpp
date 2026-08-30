// RUN rel0215 WAVE 3 (lane w3-e): THE REST OF LETHAL LAVA LAND'S CAST (ov022,
// level 14). Wave 8 (hal/actor_classes_ov022.cpp) seated five of ov022's
// eleven ids and wave 16 seated a sixth; this file seats the last five --
// ROLLING_LOG_LLL (70), LAVA_BRIDGE (73), LAVA_SEESAW (77),
// ROTATING_PLATFORM_LLL (80) and VOLCANO_FIRE (243) -- and closes the overlay.
//
// Read hal/actor_classes_ov022.cpp first: the cast map, the naming shift, the
// width derivation and the ten host-array rulings are all there and are not
// repeated. What follows is only what is NEW for these five.
//
// ---- WHY THE FOUR "BLOCKED" IDS UNBLOCKED ---------------------------------
//
// That file refused 73/77/80 on the inferred-stub ratchet: sixteen of their
// slot bodies carry the `recovered from vtable slot identity` marker. The
// marker records how the NAME was recovered, not where the BODY came from, and
// the wave-16 route answers the real question. FOURTEEN bodies were rebuilt
// with tools/match.py at 2004/b56 against extracted/overlays/overlay_0022.bin
// with --strict-relocs --module ov022:
//
//   80   func_ov022_021115f8 0211165c 02111670                        (3)
//   73   func_ov022_021119c4 02111a64 02111aa8 02111ad0 02111bdc      (5)
//   77   func_ov022_02111cf0 02111d90 02111dd4 02111dfc 02111ea0      (5)
//  243   func_ov022_021126ac                                          (1)
//
// ALL FOURTEEN ARE MATCHING -- the recovered source reproduces the ROM's bytes
// and every relocation destination. The rulings are in
// port/tools/inferred_stub_adjudicated.txt, one line each, with the run.
//
// ROLLING_LOG_LLL's second blocker was already gone when this lane opened.
// Wave 8 deferred it because its slot 9 (func_ov080_02127124), its slot 27
// (func_ov080_02127058) and the func_ov080_021274ac its InitResources
// tail-calls were unlinked. port/slice_ov030cast.txt seats RollingLogTtm --
// the SAME ROM class in ov030 -- and sliced all three plus five more ov080
// bodies. Read out of walk_window.map on this lane's own baseline, not assumed.
//
// ---- THE FIVE TABLES, ALL HOST ARRAYS -------------------------------------
//
//   id   table       width  slot 0 / 3 / 6 / 9 / 16 / 17     RTTI at [-1]
//    80  0x02113de8    32   02111670 0211165c ov002 ov002    16daObjFl_Koma_D_c
//                            021115a8 021115f8
//    73  0x02113f70    32   02111bdc 02111a64 02111ad0       16daObjFl_London_c
//                            02111aa8 02111980 021119c4
//    77  0x02114034    32   02111ea0 02111d90 02111dfc       16daObjFl_Seesaw_c
//                            02111dd4 02111cac 02111cf0
//    70  0x021143a4    32   02112590 0211254c 02112560       15daObjFlMaruta_c
//                            ov080    02112498 021124e8   + slot 27 ov080
//   243  0x0211447c    31   021128b8 021127e0 02112800       21daObj_volcanoCannon_c
//                            arm9     021125e0 02112610
//
// Widths re-pinned here out of extracted/overlays/overlay_0022.bin, both ways
// the wf slot-31 bug demands. Four of the five end
//   slot 29 0x02010124 Actor::OnAimedAtWithEgg
//   slot 30 0x020100dc Actor::OnAimedAtWithEggReturnVec
//   slot 31 0x020ee55c Platform::Kill
// and the next dsd symbol lands exactly on each computed end (0x02113e68,
// 0x02113ff0, 0x021140b4, 0x02114424). VOLCANO_FIRE is an ACTOR, not a
// Platform: its run stops after slot 30 and the raw words at 0x021144f8 and
// 0x021144fc are ZERO, so its array is [31] and it takes no Kill.
//
// TWO SLOT OVERRIDES THE SHARED FILL DOES NOT COVER, both read off the ROM:
//   70  slot 27 = func_ov080_02127058 (OnHitByMegaChar), not Actor's
//   243 slot  9 = 0x02043af0 _ZN9ActorBase6RenderEv, reached as an MSVC method
//
// ---- THE FACTORIES: THREE STORE THEIR OWN TABLE, TWO STORE `VT1` ----------
//
// 73 and 77 name their table directly (data_ov022_02113f70 / _02114034) and
// are registered as-is. 80 and 70 end with the FloatingFloor shape -- the ROM
// stores the BASE table first and its OWN table second, and the recovered
// source has the RTTI name on the base store and the shared-header `VT1`
// placeholder on the own store:
//
//   RotatingPlatformLll_Spawn  021116bc = 0x021091d4 (ov002 base)  <- named
//                              021116c0 = 0x02113de8 (own)         <- VT1
//   RollingLogLll_Spawn        021125d8 = 0x02128338 (ov079/80 base) <- named
//                              021125dc = 0x021143a4 (own)         <- VT1
//
// Both were disassembled: `ldr r1,[pc]; ldr r0,[pc]; str r1,[r4]; str r0,[r4]`
// -- two stores back to back, so the first is provably dead. Rather than a
// port_factory wrapper that re-seats p[0] after the fact, each Spawn gets a
// PER-SOURCE `-DVT1=<host array>` (port/CMakeLists.txt), which makes the ROM's
// own final store write the host table. The dead first store is then the same
// value, and both Spawns register directly.
//
// 243's Spawn stores its own table once, under the dsd name
// _ZTV13RollingLogLll, which IS this file's host array.
//
// ---- VOLCANO_FIRE IS LIVE, AND IT IS NOT ON A PLACEMENT ROW ---------------
//
// 243 appears on no mounted level's object list. It is registered anyway
// because VolcanoRing's Behavior -- src/func_ov022_021112ac.c, linked and
// running on level 14 since wave 8 -- calls Actor_Spawn(0xf3, ...) for the
// ring's fireball. Wave 8's note that 243 "gets no row (the ov019 rule)" read
// the placement list and not the caller.
//
// ---- ITS POINTER-TO-MEMBER STATE RECORDS ---------------------------------
//
// 243 keeps a pointer at +0x108 to a pair of mwcc pointer-to-member records,
// and __sinit_ov022_021130bc builds that pair in bss 0x02114690 by copying the
// two .data pairs at 0x02114424 ({func_ov022_021126ac, 0}) and 0x0211442c
// ({func_ov022_02112710, 0}). Two TUs dispatch through it:
//   src/func_ov022_02112790.cpp        sets the pointer and calls record 0
//   src/_ZN13RollingLogLll8BehaviorEv.cpp  calls the record at +8
// Both are compiled with /vmg /vmm (port/CMakeLists.txt), which is the wave-18
// ruling: that pair of flags gives MSVC the ROM's own 8-byte {fn, delta}
// representation, so `pp + 1` strides eight onto the second record's function
// word instead of onto the adj word of record 0 (`jmp 0`).
//
// THE FLAGS FIX THE STRIDE AND NOT THE RECEIVER. A pointer-to-member call is
// __thiscall -- `mov ecx, this+delta; jmp fn` -- and both ROM bodies are
// plain cdecl functions taking the actor as their first stack argument. So the
// fill seats the TWO FUNCTION WORDS with __fastcall thunks (ecx is arg 1),
// AFTER the sinit has copied the DS addresses in: the CrazedCrate re-seat in
// hal/actor_classes_ov063.cpp, same order, same reason. Deltas are ROM ZERO on
// both records and are written back as zero.
//
// ---- ONE TU HELD OUT, AND IT IS NOT A REFUSAL OF THE BODY ----------------
//
// src/_ZN13RollingLogLllD1Ev.cpp (243's slot 16) is a real MSVC-synthesised
// destructor over a shadow class with no vtable store of its own; MSVC mangles
// it ??1RollingLogLll@@UAE@XZ and never as the Itanium name the slot needs.
// That is the MotherPenguin / OneUpLogo / BabyPenguin shape gate 191, gate 193
// and slice_ov030cast.txt (for this class's ov030 twin) all keep out. The
// chain is spelled in vf_d1 below, HIGH ADDRESS FIRST, from the ROM body at
// 0x021125e0: MovingCylinderClsn at +0xd4 then Actor::D2, with the vptr
// restore the ROM does first.
//
// ---- LANE OWNERSHIP -------------------------------------------------------
//
// This file does not touch hal/actor_overlays.cpp either. It calls wave 8's
// port_ov22_bringup() (mount pack-check, syms patch and the four sinits that
// lane's classes need) and then runs the FIVE this lane's classes need, under
// its own DSSTATE-bracketed guard, on the same argument wave 8 records: the
// guard lives in .dsstate so a save-state restore re-runs the pass exactly
// when its results were rolled away.

#include <cstdio>
#include "dsstate_seg.h"

#include "Actor.h"
#include "ActorBase.h"
/* Six of this cast's bodies are REAL C++ METHODS in src/ (MSVC decorates them
   ?InitResources@FallBlockLll@@QAEHXZ and friends), so they are called as
   methods against the generated headers rather than through a C name that does
   not exist. The rest are extern "C" in src/ and are declared as C names.
   Which is which was read off the sources, not assumed:
     70  InitResources / CleanupResources / Behavior   -> methods
     70  D1 / D0                                       -> C names
    243  InitResources / Behavior                      -> methods
    243  CleanupResources / D0                         -> C names  */
#include "FallBlockLll.h"
#include "RollingLogLll.h"

extern "C" {
/* the arm9 shared half -- the same slot-for-slot set the four wave-8 tables
   take, re-read here off this lane's own five reloc runs. */
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

const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);
void port_actor_render_probe(const char *cls, void *model);

/* wave 8's bring-up: the mount pack-check, the syms patch and its four sinits */
void port_ov22_bringup(void);
/* the five ov022 sinits THIS lane's classes need */
void __sinit_ov022_02112d14(void);   /* 80's model/collision pair  */
void __sinit_ov022_02112dec(void);   /* 73's                       */
void __sinit_ov022_02112e58(void);   /* 77's                       */
void __sinit_ov022_02113050(void);   /* 70's                       */
void __sinit_ov022_021130bc(void);   /* 243's two PMF state records */

/* the destructor chain vf_d1 spells out (243's held-out slot 16) */
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN5ActorD2Ev(void *self);
}

// ---- the trap --------------------------------------------------------------
static void ov22e_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov022 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov022 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV22E_TRAP(n) \
    static int __fastcall ov22e_trap##n(void *s, void *) \
    { ov22e_trap_report(s, n); return 0; }
OV22E_TRAP(13) OV22E_TRAP(14) OV22E_TRAP(17) OV22E_TRAP(30)
#undef OV22E_TRAP

static int __fastcall ov22e_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov22e_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov22e_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov22e_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov22e_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov22e_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov22e_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov22e_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov22e_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov22e_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov22e_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall ov22e_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov22e_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov22e_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov22e_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov22e_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov22e_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov22e_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov22e_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov22e_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov22e_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov22e_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov22e_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* Slots 1..30, every word read off this lane's own five reloc runs. The caller
   writes its own 0/3/6/9/16/17 and its 31 (four of five), and 70 also writes
   its 27. THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores
   bug, repro and dumpbin proof in hal/actor_classes_ov002g200.cpp: MSVC 19.44
   x86 /O2 can delete a static filler's stores when it is called with several
   distinct extern-array arguments. This helper has five callers. */
static void ov22e_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov22e_binit;
    vt[2]  = (void *)ov22e_ainit;
    vt[4]  = (void *)ov22e_bclean;
    vt[5]  = (void *)ov22e_aclean;
    vt[7]  = (void *)ov22e_bbeh;
    vt[8]  = (void *)ov22e_abeh;
    vt[10] = (void *)ov22e_bren;
    vt[11] = (void *)ov22e_aren;
    vt[12] = (void *)ov22e_pdes;
    vt[13] = (void *)ov22e_trap13;
    vt[14] = (void *)ov22e_trap14;
    vt[15] = (void *)ov22e_heap;
    vt[17] = (void *)ov22e_trap17;
    vt[18] = (void *)ov22e_yoshi;
    vt[19] = (void *)ov22e_turn_egg;
    vt[20] = (void *)ov22e_v50;
    vt[21] = (void *)ov22e_pounded;
    vt[22] = (void *)ov22e_atk1;
    vt[23] = (void *)ov22e_atk2;
    vt[24] = (void *)ov22e_kicked;
    vt[25] = (void *)ov22e_pushed;
    vt[26] = (void *)ov22e_cannon;
    vt[27] = (void *)ov22e_mega;
    vt[28] = (void *)ov22e_under;
    vt[29] = (void *)ov22e_egg;
    vt[30] = (void *)ov22e_trap30;
}

// ---- the bring-up ----------------------------------------------------------
/* CAPTURED for hal/level_boot.cpp's g_level_mounted reason: everything this
   pass writes -- the mount's rebased pointers and the SharedFilePtrs the five
   sinits construct -- lives in .dsstate, and a restore rolls it back. A guard
   that does not roll back with it leaves the pass skipped forever. */
DSSTATE_BEGIN
static int g_ov22e_bringup_done;
DSSTATE_END

static void ov22e_bringup(void)
{
    port_ov22_bringup();          /* wave 8's: pack check, syms patch, 4 sinits */
    if (g_ov22e_bringup_done)
        return;
    g_ov22e_bringup_done = 1;
    __sinit_ov022_02112d14();     /* 80  */
    __sinit_ov022_02112dec();     /* 73  */
    __sinit_ov022_02112e58();     /* 77  */
    __sinit_ov022_02113050();     /* 70  */
    __sinit_ov022_021130bc();     /* 243 */
}

// ============================================================================
// ROTATING_PLATFORM_LLL (80) -- table 0x02113de8, unnamed func_ov022_* bodies
// ============================================================================
//
// 800-byte object; Model at +0xd4, MovingMeshCollider at +0x124. Its
// InitResources (0x02111670) and CleanupResources (0x0211165c) both reach the
// three-word resource table data_ov022_02113da4 = { 0x02114530 model SFP,
// 0x02114528 collision SFP, ov064 0x0211bb4c CLPS } -- which is why the CLPS
// block joins port/ov064_syms.txt with this lane. Slots 6 and 9 are ov002's
// shared platform bodies, already linked.
extern "C" {
int func_ov022_02111670(unsigned char *self);  /* slot 0  InitResources */
int func_ov022_0211165c(void *self);           /* slot 3  CleanupResources */
int func_ov002_020b6718(char *self);           /* slot 6  Behavior, ov002 */
int func_ov002_020b66f0(void *self);           /* slot 9  Render,   ov002 */
int *func_ov022_021115a8(int *self);           /* slot 16 D1 */
int *func_ov022_021115f8(int *self);           /* slot 17 D0 */
void *RotatingPlatformLll_Spawn(void);
DSSTATE_BEGIN
void *data_ov022_02113de8[32];
DSSTATE_END
}
/* 0x02113de8 answers to its RTTI name too; the D1 and D0 restore it by that
   spelling. One host array, both names -- the wave-8 daObjFl_Block_c
   treatment. (RotatingPlatformLll_Spawn ALSO spells this name, but there it is
   on the dead FIRST store of a `str r1,[r4]; str r0,[r4]` pair, so pointing it
   at the own table costs nothing; the live second store is the per-source
   -DVT1 in port/CMakeLists.txt.) */
#pragma comment(linker, "/alternatename:__ZTV16daObjFl_Koma_D_c=_data_ov022_02113de8")
static int __fastcall rp_init(void *s, void *)
{ return func_ov022_02111670((unsigned char *)s); }
static int __fastcall rp_clean(void *s, void *)
{ return func_ov022_0211165c(s); }
static int __fastcall rp_behavior(void *s, void *)
{ return func_ov002_020b6718((char *)s); }
static int __fastcall rp_render(void *s, void *)
{ port_actor_render_probe("ROTATING_PLATFORM_LLL", (char *)s + 0xd4);
  return func_ov002_020b66f0(s); }
static int __fastcall rp_d1(void *s, void *)
{ return (int)(size_t)func_ov022_021115a8((int *)s); }
static int __fastcall rp_d0(void *s, void *)
{ return (int)(size_t)func_ov022_021115f8((int *)s); }
extern "C" void hal_fill_rotating_platform_lll_vtable(void)
{
    ov22e_bringup();
    void *volatile *vt = (void *volatile *)data_ov022_02113de8;
    ov22e_fill_shared(vt);
    vt[0]  = (void *)rp_init;
    vt[3]  = (void *)rp_clean;
    vt[6]  = (void *)rp_behavior;
    vt[9]  = (void *)rp_render;
    vt[16] = (void *)rp_d1;
    vt[17] = (void *)rp_d0;
    vt[31] = (void *)ov22e_kill;
}

// ============================================================================
// LAVA_BRIDGE (73) -- table 0x02113f70, unnamed func_ov022_* bodies
// ============================================================================
//
// 800-byte object; Model at +0xd4, MovingMeshCollider at +0x124. x2 on level
// 14. Its InitResources (0x02111bdc) reads its SharedFilePtr pair
// 0x02114580 / 0x02114578 by literal and hands MovingMeshCollider::SetFile
// ov064's CLPS block 0x0211bb2c -- the second of the three this lane adds to
// port/ov064_syms.txt. Its CleanupResources (0x02111a64) spells that same pair
// as the shared-header G0/G1 and is bound per source (port/CMakeLists.txt);
// pool words 0x02111aa0 -> 0x02114580 and 0x02111aa4 -> 0x02114578, both
// module:overlay(22).
extern "C" {
int func_ov022_02111bdc(void *self);   /* slot 0  InitResources */
int func_ov022_02111a64(void *self);   /* slot 3  CleanupResources */
int func_ov022_02111ad0(void *self);   /* slot 6  Behavior */
int func_ov022_02111aa8(void *self);   /* slot 9  Render */
int *func_ov022_02111980(int *self);   /* slot 16 D1 */
int *func_ov022_021119c4(int *self);   /* slot 17 D0 */
void *LavaBridge_Spawn(void);
DSSTATE_BEGIN
void *data_ov022_02113f70[32];
DSSTATE_END
}
#pragma comment(linker, "/alternatename:__ZTV16daObjFl_London_c=_data_ov022_02113f70")
/* 73's InitResources (a .cpp) declares its two file pointers and its CLPS
   block as TYPED C++ globals, so MSVC decorates them and the plain C names the
   mounts define do not match. Data has no calling convention, so an alias onto
   the one object is exact -- hal/actor_faces_bob.cpp rule 1, and the same
   bridge hal/actor_classes_ov022.cpp already carries for 74's pair. Measured
   off the first link's own unresolved list, not guessed. */
#pragma comment(linker, "/alternatename:?data_ov022_02114580@@3USharedFilePtr@@A=_data_ov022_02114580")
#pragma comment(linker, "/alternatename:?data_ov022_02114578@@3USharedFilePtr@@A=_data_ov022_02114578")
#pragma comment(linker, "/alternatename:?data_ov064_0211bb2c@@3UCLPS_Block@@A=_data_ov064_0211bb2c")
static int __fastcall lb_init(void *s, void *)
{ return func_ov022_02111bdc(s); }
static int __fastcall lb_clean(void *s, void *)
{ return func_ov022_02111a64(s); }
static int __fastcall lb_behavior(void *s, void *)
{ return func_ov022_02111ad0(s); }
static int __fastcall lb_render(void *s, void *)
{ port_actor_render_probe("LAVA_BRIDGE", (char *)s + 0xd4);
  return func_ov022_02111aa8(s); }
static int __fastcall lb_d1(void *s, void *)
{ return (int)(size_t)func_ov022_02111980((int *)s); }
static int __fastcall lb_d0(void *s, void *)
{ return (int)(size_t)func_ov022_021119c4((int *)s); }
extern "C" void hal_fill_lava_bridge_vtable(void)
{
    ov22e_bringup();
    void *volatile *vt = (void *volatile *)data_ov022_02113f70;
    ov22e_fill_shared(vt);
    vt[0]  = (void *)lb_init;
    vt[3]  = (void *)lb_clean;
    vt[6]  = (void *)lb_behavior;
    vt[9]  = (void *)lb_render;
    vt[16] = (void *)lb_d1;
    vt[17] = (void *)lb_d0;
    vt[31] = (void *)ov22e_kill;
}

// ============================================================================
// LAVA_SEESAW (77) -- table 0x02114034, unnamed func_ov022_* bodies
// ============================================================================
//
// 804-byte object, the same member layout. Its InitResources (0x02111ea0)
// reads 0x021145a8 / 0x021145a0 and passes ov064's CLPS block 0x0211bacc,
// which port/ov064_syms.txt already mounts (wave 8 added it for
// FLOATING_FLOOR_LLL). Its CleanupResources (0x02111d90) is the same G0/G1
// shape as 73's; pool 0x02111dcc -> 0x021145a8, 0x02111dd0 -> 0x021145a0.
extern "C" {
int func_ov022_02111ea0(void *self);   /* slot 0  InitResources */
int func_ov022_02111d90(void *self);   /* slot 3  CleanupResources */
int func_ov022_02111dfc(void *self);   /* slot 6  Behavior */
int func_ov022_02111dd4(void *self);   /* slot 9  Render */
int *func_ov022_02111cac(int *self);   /* slot 16 D1 */
int *func_ov022_02111cf0(int *self);   /* slot 17 D0 */
void *LavaSeesaw_Spawn(void);
DSSTATE_BEGIN
void *data_ov022_02114034[32];
DSSTATE_END
}
#pragma comment(linker, "/alternatename:__ZTV16daObjFl_Seesaw_c=_data_ov022_02114034")
/* 77's InitResources, same C++-decoration bridge as 73's above. */
#pragma comment(linker, "/alternatename:?data_ov022_021145a8@@3USharedFilePtr@@A=_data_ov022_021145a8")
#pragma comment(linker, "/alternatename:?data_ov022_021145a0@@3USharedFilePtr@@A=_data_ov022_021145a0")
#pragma comment(linker, "/alternatename:?data_ov064_0211bacc@@3UCLPS_Block@@A=_data_ov064_0211bacc")
static int __fastcall ls_init(void *s, void *)
{ return func_ov022_02111ea0(s); }
static int __fastcall ls_clean(void *s, void *)
{ return func_ov022_02111d90(s); }
static int __fastcall ls_behavior(void *s, void *)
{ return func_ov022_02111dfc(s); }
static int __fastcall ls_render(void *s, void *)
{ port_actor_render_probe("LAVA_SEESAW", (char *)s + 0xd4);
  return func_ov022_02111dd4(s); }
static int __fastcall ls_d1(void *s, void *)
{ return (int)(size_t)func_ov022_02111cac((int *)s); }
static int __fastcall ls_d0(void *s, void *)
{ return (int)(size_t)func_ov022_02111cf0((int *)s); }
extern "C" void hal_fill_lava_seesaw_vtable(void)
{
    ov22e_bringup();
    void *volatile *vt = (void *volatile *)data_ov022_02114034;
    ov22e_fill_shared(vt);
    vt[0]  = (void *)ls_init;
    vt[3]  = (void *)ls_clean;
    vt[6]  = (void *)ls_behavior;
    vt[9]  = (void *)ls_render;
    vt[16] = (void *)ls_d1;
    vt[17] = (void *)ls_d0;
    vt[31] = (void *)ov22e_kill;
}

// ============================================================================
// ROLLING_LOG_LLL (70) -- table 0x021143a4, bodies spelled _ZN12FallBlockLll*
// ============================================================================
//
// 836-byte object; Model at +0xd4, MovingMeshCollider at +0x124. This is
// RollingLogTtm's ROM twin: slots 9 and 27 are the SAME two ov080 bodies
// slice_ov030cast.txt already seats for that class, and its InitResources and
// CleanupResources are one tail call each into ov080's shared pair with the
// three-word resource table data_ov022_02112c9c = { 0x02114668, 0x02114670,
// ov064 0x0211bbcc } -- the third CLPS block this lane mounts.
extern "C" {
/* slots 0/3/6 are C++ methods -- see the note at the top of the file */
int *_ZN12FallBlockLllD1Ev(int *self);         /* slot 16 */
int *_ZN12FallBlockLllD0Ev(int *self);         /* slot 17 */
int func_ov080_02127124(void *self);           /* slot 9  Render,          ov080 */
void func_ov080_02127058(void *self, void *p); /* slot 27 OnHitByMegaChar, ov080 */
void *RollingLogLll_Spawn(void);
DSSTATE_BEGIN
void *_ZTV12FallBlockLll[32];
DSSTATE_END
}
/* 0x021143a4 answers to its RTTI name as well, and the D1/D0 restore it by
   that spelling. (RollingLogLll_Spawn spells it too, on the dead first store
   of its two-store pair; the live one is the per-source -DVT1.) */
#pragma comment(linker, "/alternatename:__ZTV15daObjFlMaruta_c=__ZTV12FallBlockLll")
static int __fastcall rl_init(void *s, void *)
{ return ((FallBlockLll *)s)->FallBlockLll::InitResources(); }
static int __fastcall rl_clean(void *s, void *)
{ return ((FallBlockLll *)s)->FallBlockLll::CleanupResources(); }
static int __fastcall rl_behavior(void *s, void *)
{ return ((FallBlockLll *)s)->FallBlockLll::Behavior(); }
static int __fastcall rl_render(void *s, void *)
{ port_actor_render_probe("ROLLING_LOG_LLL", (char *)s + 0xd4);
  return func_ov080_02127124(s); }
static int __fastcall rl_d1(void *s, void *)
{ return (int)(size_t)_ZN12FallBlockLllD1Ev((int *)s); }
static int __fastcall rl_d0(void *s, void *)
{ return (int)(size_t)_ZN12FallBlockLllD0Ev((int *)s); }
/* slot 27 takes the three-parameter shape so it emits `ret 4`, the same
   contract ov22e_mega has: the dispatch site pushes the Player. */
static int __fastcall rl_mega(void *s, void *, void *p)
{ func_ov080_02127058(s, p); return 0; }
extern "C" void hal_fill_rolling_log_lll_vtable(void)
{
    ov22e_bringup();
    void *volatile *vt = (void *volatile *)_ZTV12FallBlockLll;
    ov22e_fill_shared(vt);
    vt[0]  = (void *)rl_init;
    vt[3]  = (void *)rl_clean;
    vt[6]  = (void *)rl_behavior;
    vt[9]  = (void *)rl_render;
    vt[16] = (void *)rl_d1;
    vt[17] = (void *)rl_d0;
    vt[27] = (void *)rl_mega;
    vt[31] = (void *)ov22e_kill;
}

// ============================================================================
// VOLCANO_FIRE (243) -- table 0x0211447c, THIRTY-ONE slots, bodies spelled
// _ZN13RollingLogLll* (THE SHIFT)
// ============================================================================
//
// 284-byte object and the only ACTOR in this file -- MovingCylinderClsn at
// +0xd4, no Model, no Platform tail. Slot 9 is arm9's ActorBase::Render
// (0x02043af0) rather than a Model render, which is consistent: there is no
// model to draw.
extern "C" {
/* slots 0/6 are C++ methods -- see the note at the top of the file */
int _ZN13RollingLogLll16CleanupResourcesEv(void *self); /* slot 3 */
int *_ZN13RollingLogLllD0Ev(int *self);                 /* slot 17 */
void *VolcanoFire_Spawn(void);
/* the two PMF state bodies the record seat installs (both matched, both on
   this lane's slice). ROM pairs, read out of overlay_0022.bin:
     0x02114424 = { func_ov022_021126ac, 0 }
     0x0211442c = { func_ov022_02112710, 0 }   */
int func_ov022_021126ac(char *self);
int func_ov022_02112710(char *self);
extern unsigned char data_ov022_02114690[];   /* the bss pair the sinit fills */
DSSTATE_BEGIN
void *_ZTV13RollingLogLll[31];
DSSTATE_END
}
#pragma comment(linker, "/alternatename:__ZTV21daObj_volcanoCannon_c=__ZTV13RollingLogLll")
static int __fastcall vf_init(void *s, void *)
{ return ((RollingLogLll *)s)->RollingLogLll::InitResources(); }
static int __fastcall vf_clean(void *s, void *)
{ return _ZN13RollingLogLll16CleanupResourcesEv(s); }
static int __fastcall vf_behavior(void *s, void *)
{ return ((RollingLogLll *)s)->RollingLogLll::Behavior(); }
static int __fastcall vf_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
/* slot 16: src/_ZN13RollingLogLllD1Ev.cpp is held out (MSVC mangles it
   ??1RollingLogLll@@UAE@XZ). The ROM body at 0x021125e0 restores the vptr,
   runs MovingCylinderClsn::D1 at +0xd4 and then Actor::D2 -- HIGH ADDRESS
   FIRST, one member, no Deallocate (that is D0's). */
static int __fastcall vf_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV13RollingLogLll;
    _ZN18MovingCylinderClsnD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall vf_d0(void *s, void *)
{ return (int)(size_t)_ZN13RollingLogLllD0Ev((int *)s); }
/* the two pointer-to-member records, as __fastcall (ecx is arg 1) so the
   __thiscall dispatch /vmg /vmm emits reaches these cdecl bodies. */
static int __fastcall vf_state0(void *s, void *)
{ return func_ov022_021126ac((char *)s); }
static int __fastcall vf_state1(void *s, void *)
{ return func_ov022_02112710((char *)s); }
extern "C" void hal_fill_volcano_fire_vtable(void)
{
    ov22e_bringup();
    void *volatile *vt = (void *volatile *)_ZTV13RollingLogLll;
    ov22e_fill_shared(vt);
    vt[0]  = (void *)vf_init;
    vt[3]  = (void *)vf_clean;
    vt[6]  = (void *)vf_behavior;
    vt[9]  = (void *)vf_render;
    vt[16] = (void *)vf_d1;
    vt[17] = (void *)vf_d0;
    /* ov22e_fill_shared wrote slot 30; this table has no slot 31 and the fill
       never touches one. */
    /* THE STATE-RECORD RE-SEAT. __sinit_ov022_021130bc ran in ov22e_bringup
       above and copied the ROM's two {fn, 0} pairs -- DS code addresses -- into
       0x02114690. Overwrite both function words with the host thunks and write
       the deltas back as the ROM's zero. */
    {
        static void *const seat[2] = { (void *)vf_state0, (void *)vf_state1 };
        int i;
        for (i = 0; i < 2; ++i) {
            *(void *volatile *)(data_ov022_02114690 + i * 8) = seat[i];
            *(int volatile *)(data_ov022_02114690 + i * 8 + 4) = 0;
        }
    }
}
