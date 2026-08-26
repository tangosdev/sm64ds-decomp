// RUN LINKW WAVE 12 (lane w12): CLAM (315), ov064.
//
// Nine instances -- five on level 8 (Jolly Roger Bay) and four on level 18 --
// on a table nothing in the port had filled. ov064 is mounted already, whole
// and per symbol, so this lane is nine symbols appended to port/ov064_syms.txt,
// nine TUs, one host vtable and one registry row.
//
//   id   class   x          table                       width
//   315  CLAM    L8 x5      0x0211c5c8 _ZTV4Clam          31
//                L18 x4                == _ZTV12daObjShell_c
//
// Both attribution routes agree: spawn-table site 0x02090d50 -> SpawnInfo
// 0x0211c5a4, word[0] = 0x0211ad70 (Clam_Spawn, inside ov064), +4 halfword =
// 315. Identity from the RTTI typeinfo at vtable[-1] (0x0211c5c4 -> 0x0211c588
// -> "12daObjShell_c"), not from a dsd label.
//
// ---- THE WIDTH IS 31, AND ITS NEIGHBOUR PROVES THE METHOD -----------------
//
// Clam's reloc run reads 31 and stops: slot 30 is
// Actor::OnAimedAtWithEggReturnVec and there is no slot 31, because Clam is an
// Actor and not a Platform (no Platform::Kill tail -- the ov060
// Bowser/BowserTail/BowserFire/SpikeBomb shape). The next dsd symbol
// under-reads at 18, dsd having split the table at 0x0211c610; the true end,
// 0x0211c644, is followed by 0x1c of ROM zeros to the .data end.
//
// The table NEXT DOOR is the cross-check, and it is the ov052 trap again:
// _ZTV11daObjTbox_c at 0x0211c50c has a reloc run of THIRTY-FOUR and a table of
// thirty-one. Its last three "slots" are {0x0209a764, 0x0211c594, arm9
// 0x0208e390} -- Clam's own Itanium typeinfo object at 0x0211c588, three
// relocated words the run walked straight into. Same overlay, same wave, same
// failure shape as ov052's id 174 reading 38.
//
// ---- TWO BODIES CANNOT BE CALLED AS THE MATCHED SOURCE WRITES THEM --------
//
// 1. _ZN4Clam6RenderEv is the ModelAnim slot-5 collision. Clam_Spawn
//    constructs a ModelAnim at +0xd4 (_ZN9ModelAnimC1Ev) and the matched
//    Render dispatches through a LOCAL six-virtual shadow class, which counts
//    in ROM/Itanium numbering, so its slot 5 is Render; the host
//    _ZTV9ModelAnim is MSVC-numbered and its slot 5 is Virtual18, which takes
//    two arguments where the shadow passes one. That is the measured
//    Butterfly / Fish / QuestionBlock / Whomp / BabyPenguin fault, and the fix
//    is theirs: a host copy in port/unmatched/Ov064_Clam.cpp with the dispatch
//    spelled as the qualified ModelAnim::Render. The matched TU is held out of
//    port/slice_w12c.txt.
//
// 2. src/_ZN4ClamD1Ev.cpp is a REAL MSVC DESTRUCTOR over structs declared
//    inside its own .cpp (`struct Clam : Actor { ModelAnim m0; ... };`), so it
//    emits ??1Clam@@UAE@XZ and never the C name _ZN4ClamD1Ev -- there is no
//    face to write, the gate-199/200/202 and ov045 ExtendingPlatform case.
//    clam_d1 below is the chain, transcribed instruction for instruction from
//    0x0211a930:
//        ldr r1, =0x0211c5c8 ; str r1, [r4]         restore the own table
//        add r0, r4, #0x138  ; bl MovingCylinderClsn::~
//        add r0, r4, #0xd4   ; bl ModelAnim::~
//        mov r0, r4          ; bl Actor::D2
//    and its sibling _ZN4ClamD0Ev, which IS a C name in matched src, spells
//    the identical chain plus Memory::Deallocate(t, data_020a0eac) -- so the
//    two agree and the host copy is the D0 minus its last call. That TU is
//    held out of the slice too.
//
// ---- WHAT THIS LANE DELIBERATELY DID NOT TAKE -----------------------------
//
// TREASURE_CHEST (id 13, eight instances on the same two levels plus level 9)
// shares this overlay and this wave's ranking put it beside Clam. It is
// BLOCKED and the block is measured, not suspected: its Behavior dispatches a
// six-record pointer-to-member state table at 0x0211c98c through
// func_ov064_0211a6ec / func_ov064_0211a734, and one of the six records,
// func_ov064_0211a4c4 (state 0's tick, 540 bytes at 0x0211a4c4), HAS NO
// MATCHED TU anywhere in src/ and is in no port host copy. State 0 is the
// state its InitResources leaves the chest in, so that body runs on the first
// frame a chest exists. Registering the class without it would seat a DS code
// address in a live dispatch table. The rest of the chest is ready -- both
// dispatchers' host copies are the ov073/ov060 recipe, the other five records
// are matched, and __sinit_ov064_0211b59c builds the table -- so the whole cost
// of that class is one verified host copy of 540 bytes of ARM.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half. Slots 1/2/4/5/7/8/10..15/18..30 are the same arm9
   words ov045's, ov052's and ov073's tables carry -- checked slot for slot. */
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

const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);
void port_actor_render_probe(const char *cls, void *model);

/* the one ov064 initialiser this lane adds: Clam's three SharedFilePtrs.
   port_ov064_pack_check / port_ov064_syms_patch are NOT called from here --
   hal/actor_overlays.cpp already runs them, and port_actor_overlays_sinits()
   completes before port_actor_registry_install(), so the mount's pointer pass
   has already run by the time this fill does. */
void __sinit_ov064_0211b698(void);

/* Clam's own slots, and what clam_d1 spells by hand */
int _ZN4Clam13InitResourcesEv(void *self);   /* slot 0, faced at file bottom */
int _ZN4Clam16CleanupResourcesEv(void *self);/* slot 3  */
int _ZN4Clam8BehaviorEv(void *self);         /* slot 6  */
int _ZN4Clam6RenderEv(void *self);           /* slot 9, HOST COPY */
int *_ZN4ClamD0Ev(int *self);                /* slot 17 */
void *ClamSpawn_unused(void);
void *Clam_Spawn(void);
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN9ModelAnimD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);
DSSTATE_BEGIN
void *_ZTV4Clam[31];
DSSTATE_END
}
/* dsd names 0x0211c5c8 twice and both names are out of the mount; the RTTI
   spelling is the one _ZN4ClamD0Ev restores by. */
#pragma comment(linker, "/alternatename:__ZTV12daObjShell_c=__ZTV4Clam")

/* TWO C++-MANGLED DATA SPELLINGS, the data_02082128 / data_020a0e68 precedent.
   src/_ZN4Clam13InitResourcesEv.cpp declares two of Clam's three SharedFilePtrs
   at FILE SCOPE as `extern int data_ov064_0211c9bc[];` rather than inside its
   extern "C" block, so it emits decorated data names; the mount emits the one
   C-named array each, and __sinit_ov064_0211b698 (a .c) reaches the same two
   through the C names. LHS declared and never defined, so
   tools/alternatename_guard.py stays clean. */
#pragma comment(linker, "/alternatename:?data_ov064_0211c9bc@@3PAHA=_data_ov064_0211c9bc")
#pragma comment(linker, "/alternatename:?data_ov064_0211c9cc@@3PAHA=_data_ov064_0211c9cc")

// ---- the trap --------------------------------------------------------------
static void ov64w12_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov064 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov064 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV64W12_TRAP(n) \
    static int __fastcall ov64w12_trap##n(void *s, void *) \
    { ov64w12_trap_report(s, n); return 0; }
OV64W12_TRAP(13) OV64W12_TRAP(14) OV64W12_TRAP(30)
#undef OV64W12_TRAP

static int __fastcall cl_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall cl_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall cl_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall cl_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall cl_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall cl_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall cl_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall cl_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall cl_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall cl_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall cl_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall cl_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall cl_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall cl_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall cl_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall cl_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall cl_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall cl_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall cl_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall cl_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall cl_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall cl_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

static int __fastcall cl_init(void *s, void *)
{ return _ZN4Clam13InitResourcesEv(s); }
static int __fastcall cl_clean(void *s, void *)
{ return _ZN4Clam16CleanupResourcesEv(s); }
static int __fastcall cl_behavior(void *s, void *)
{ return _ZN4Clam8BehaviorEv(s); }
static int __fastcall cl_render(void *s, void *)
{ port_actor_render_probe("CLAM", (char *)s + 0xd4);
  return _ZN4Clam6RenderEv(s); }
/* slot 16, HOST CHAIN -- see this file's header. */
static int __fastcall cl_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV4Clam;
    _ZN18MovingCylinderClsnD1Ev(t + 0x138);
    _ZN9ModelAnimD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall cl_d0(void *s, void *)
{ return (int)(size_t)_ZN4ClamD0Ev((int *)s); }

/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "hal_fill_clam_vtable has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_clam_vtable_done;
DSSTATE_END

extern "C" void hal_fill_clam_vtable(void)
{
    if (!g_clam_vtable_done) {
        g_clam_vtable_done = 1;
        __sinit_ov064_0211b698();
    }
    void *volatile *vt = (void *volatile *)_ZTV4Clam;
    vt[0]  = (void *)cl_init;
    vt[1]  = (void *)cl_binit;
    vt[2]  = (void *)cl_ainit;
    vt[3]  = (void *)cl_clean;
    vt[4]  = (void *)cl_bclean;
    vt[5]  = (void *)cl_aclean;
    vt[6]  = (void *)cl_behavior;
    vt[7]  = (void *)cl_bbeh;
    vt[8]  = (void *)cl_abeh;
    vt[9]  = (void *)cl_render;
    vt[10] = (void *)cl_bren;
    vt[11] = (void *)cl_aren;
    vt[12] = (void *)cl_pdes;
    vt[13] = (void *)ov64w12_trap13;
    vt[14] = (void *)ov64w12_trap14;
    vt[15] = (void *)cl_heap;
    vt[16] = (void *)cl_d1;
    vt[17] = (void *)cl_d0;
    vt[18] = (void *)cl_yoshi;
    vt[19] = (void *)cl_turn_egg;
    vt[20] = (void *)cl_v50;
    vt[21] = (void *)cl_pounded;
    vt[22] = (void *)cl_atk1;
    vt[23] = (void *)cl_atk2;
    vt[24] = (void *)cl_kicked;
    vt[25] = (void *)cl_pushed;
    vt[26] = (void *)cl_cannon;
    vt[27] = (void *)cl_mega;
    vt[28] = (void *)cl_under;
    vt[29] = (void *)cl_egg;
    vt[30] = (void *)ov64w12_trap30;
    /* 31 slots: Clam is an Actor, not a Platform. No slot 31. */
}

// ---- method face -----------------------------------------------------------
// The one body src defines as a real C++ method. Its Cleanup, Behavior and D0
// are extern-C already; its Render is a host copy and its D1 has no C name.
#include "Clam.h"
extern "C" int _ZN4Clam13InitResourcesEv(void *self)
{ return ((Clam *)self)->Clam::InitResources(); }
