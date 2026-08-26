// RUN LINKW WAVE 7, LANE w7-c: THE DARK WORLD ARENA'S OWN CLASS (ov044, level 36).
//
// ONE class, and it is the only cast either new arena needs. Level 40's three
// ids (167 BOWSER_SKY_PLATFORM x10, 279 BOWSER x1, 284 SPIKE_BOMB x5) are all
// ov060 and all registered since wave 6, so that arena is a pure mount. Level
// 36 places 279 x1 and 284 x8 from the same ov060 pack -- and 301 x16, which is
// not an ov060 class at all.
//
// ---- WHY 301 IS ov044's, MEASURED BOTH WAYS -------------------------------
//
// The level's object table (LVL_Overlay 0x021113b0 -> objTable 0x02111374, one
// Standard sub-table of 25 entries at area 0, ids through
// data_ov002_0210cbf4[raw]) yields 279 x1, 284 x8, 301 x16. For 301:
//
//   arm9 ACTOR_SPAWN_TABLE 0x02090864 + 301*4 = 0x02090d18  ->  0x021115e0
//   overlay_0044.bin @0x021115e0 word[0] = 0x021112dc  (OrangeBallBillboard_Spawn)
//   overlay_0044.bin @0x021115e0 word[1] & 0xffff = 0x012d = 301
//
// BOTH tests, because sibling overlays share the load window: the spawnFunc
// 0x021112dc is inside ov044's own 0x021111a0..0x02111680 (overlays.yaml
// ram_size 1248), AND the id halfword matches the slot it was reached from.
// Bytes come from extracted/overlays/overlay_0044.bin -- ov044 is
// compressed:true, so the dsd export is the compressed image and its halfwords
// are noise.
//
// ---- CLASS IDENTITY: RTTI, NOT THE LABEL ----------------------------------
//
// _ZTV19OrangeBallBillboard is at 0x02111604; its vtable[-1] (0x02111600)
// points at 0x021115bc, and that typeinfo's name string (0x021115c8) reads
// "19daObjKb1Billboard_c". kb1 = koopa boss 1 -- this arena. The asset agrees
// independently: __sinit_ov044_02111314 constructs the class's SharedFilePtr on
// handle 1570, which build/assets/handles.tsv resolves to
// data/special_obj/kb1_ball/kb1_ball.bmd. So "OrangeBallBillboard" is the
// config's chosen spelling of daObjKb1Billboard_c, NOT a shifted label -- ov044
// contains exactly one class, so there is no neighbour for a shift to land on.
// No RTTI alias is needed here: the only two TUs that restore the table by name
// (D0 and Spawn) both spell it _ZTV19OrangeBallBillboard.
//
// ---- THE WIDTH: THIRTY-ONE, PINNED TWICE ----------------------------------
//
// config/arm9/overlays/ov044/relocs.txt carries an unbroken run of `load`
// relocs at 0x02111604, 0x02111608, ... 0x0211167c -- thirty-one consecutive
// words -- and NOTHING at 0x02111680. delinks.txt ends .data at 0x02111680 and
// starts .bss there. Reloc run and next-symbol landing agree: 31 slots, the
// plain Actor shape, no Platform::Kill tail. (A slot-31 undersize has taken a
// level before; a slot-31 OVERSIZE here would put a host pointer into bss.)
//
// The shared half is byte-identical to the arena class the port already hosts:
// slots 1,2,4,5,7,8,10..15 and 18..30 of this table hold exactly the same arm9
// addresses as _ZTV18BowserFireSeaArena's (0x0211a8b0, ov060) -- compared word
// for word on this lane. One slot differs from that class's shape and is
// INHERITED here rather than own:
//   slot  6  0x02043b24  _ZN9ActorBase8BehaviorEv        (no Behavior of its own)
// (slot 12, 0x02043ac0 _ZN9ActorBase16OnPendingDestroyEv, is the SAME word in
// both tables -- inherited in each; the review's word-for-word recomparison
// puts the full differing set at {0, 3, 6, 9, 16, 17}.)
// Own slots are 0 / 3 / 9 / 16 / 17 only. Sixteen static billboards that turn
// to face the camera do not need a per-frame Behavior, and the ROM agrees.
//
// ---- D1/D0 NUMBERING ------------------------------------------------------
//
// This seat uses dsd's Itanium numbering, and the ROM settles which is which
// rather than the names doing it: 0x021111d0 (dsd: D0) ends in
// Memory::Deallocate (src/_ZN19OrangeBallBillboardD0Ev.c, and reloc 0x021111fc
// -> 0x0203c1e8), 0x021111a0 (dsd: D1) does not (its only calls are
// 0x02016d20 Model::~Model and 0x020112c8 Actor::D2). So D1 = complete-object,
// slot 16; D0 = deleting, slot 17. That is the arena/spikebomb seating.
//
// ---- THE ONE TU HELD OUT OF THE SLICE -------------------------------------
//
// src/_ZN19OrangeBallBillboardD1Ev.cpp is a real MSVC destructor over a SHADOW
// class (`struct OrangeBallBillboard : Actor { Model m0; }` declared inside its
// own .cpp), so MSVC emits ??1OrangeBallBillboard@@UAE@XZ and auto-generates
// calls to ??1Model@@QAE@XZ and ??1Actor@@UAE@XZ, neither of which exists
// anywhere in this build -- the BigBooIcon / HauntedChair / SPIKE_BOMB case.
// obb_d1 below is the chain its matched sibling D0 spells minus the
// Deallocate, which relocs.txt confirms instruction for instruction.
//
// ---- THE ONE CROSS-OVERLAY SPELLING ---------------------------------------
//
// src/_ZN19OrangeBallBillboard13InitResourcesEv.c reaches the class's
// SharedFilePtr as `data_ov059_02111680` while its sibling
// CleanupResources and the sinit both spell the SAME cell `data_ov044_02111680`
// -- dsd's shared-window naming race (0x02111680 is inside twenty overlays'
// windows; ov059 is not mounted anywhere in this build). One storage, sibling
// name aliased onto it, the ov060 data_ov066_* recipe. Getting this wrong is
// not a link error but a silent one: the sinit would construct one
// SharedFilePtr and InitResources would load a model through a different,
// zeroed one.
//
// ---- LANE OWNERSHIP -------------------------------------------------------
//
// No lane owns hal/actor_overlays.cpp this wave, so the ov044 mount bring-up
// and its single sinit ride the registry fill (port_ov44_bringup, the
// ov045/ov060 shape). Whoever next owns actor_overlays.cpp should move the body
// beside the ov013/ov045/ov060 blocks and cut the guard here to a call.

#include <cstdio>
#include <cstdlib>

#include "dsstate_seg.h"
#include "Actor.h"
#include "ActorBase.h"
#include "OrangeBallBillboard.h"

extern "C" {

/* ---- the shared arm9 half, slots 1..30 (see the width block above) ---- */
int  _ZN5Actor19BeforeInitResourcesEv(void *self);              /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);   /* slot 2  */
int  _ZN5Actor14BeforeBehaviorEv(void *self);                   /* slot 7  */
int  _ZN5Actor12BeforeRenderEv(void *self);                     /* slot 10 */
int  _ZN5Actor13OnYoshiTryEatEv(void *self);                    /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);    /* slot 19 */
int  _ZN5Actor9Virtual50Ev(void *self);                         /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);       /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);           /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);           /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);               /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);               /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o);  /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);      /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);       /* slot 28 */
int  _ZN5Actor16OnAimedAtWithEggEv(void *self);                     /* slot 29 */

/* ---- the class's own matched bodies ---- */
int  _ZN19OrangeBallBillboard13InitResourcesEv(void *self);     /* slot 0  */
int  _ZN19OrangeBallBillboard16CleanupResourcesEv(void);        /* slot 3  */
int *_ZN19OrangeBallBillboardD0Ev(int *self);                   /* slot 17 */
void *OrangeBallBillboard_Spawn(void);
void __sinit_ov044_02111314(void);

/* ---- the destructor chain obb_d1 spells (relocs 0x021111b4 / 0x021111bc) ---- */
void  _ZN5ModelD1Ev(void *m);
void *_ZN5ActorD2Ev(void *self);

/* ---- the ov044 per-symbol mount (port/ov044_syms.txt) ---- */
void port_ov044_pack_check(void);
void port_ov044_syms_patch(void);

void port_actor_render_probe(const char *cls, void *model);
void port_actor_slot_decline(const char *what);
const char *port_actor_class_name(unsigned id);

/* HOST STORAGE for the one vtable, 31 slots (the width block above). Excluded
   from port/ov044_syms.txt: a mounted vtable hands a factory DS code
   addresses. */
DSSTATE_BEGIN
void *_ZTV19OrangeBallBillboard[31];   /* ROM 0x02111604..0x02111680 */
DSSTATE_END

}  /* extern "C" */

/* The InitResources spelling of the class's own SharedFilePtr, onto the one
   storage the sinit and CleanupResources use (the cross-overlay block above).
   Data aliases are exact -- there is no `this` to lose. */
#pragma comment(linker, "/alternatename:_data_ov059_02111680=_data_ov044_02111680")

// ---- the trap --------------------------------------------------------------
static void obb_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov044 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov044 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OBB_TRAP(n) \
    static int __fastcall obb_trap##n(void *s, void *) \
    { obb_trap_report(s, n); return 0; }
OBB_TRAP(13) OBB_TRAP(14) OBB_TRAP(30)
#undef OBB_TRAP

// ---- the shared half -------------------------------------------------------
static int __fastcall obb_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall obb_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall obb_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall obb_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall obb_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall obb_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall obb_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall obb_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall obb_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall obb_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall obb_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf_turn_egg contract. */
static int __fastcall obb_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall obb_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall obb_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall obb_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall obb_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall obb_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall obb_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall obb_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall obb_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall obb_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall obb_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
/* slot 6, INHERITED: the ROM's word is 0x02043b24 = ActorBase::Behavior, the
   base no-op. This class has no Behavior of its own. */
static int __fastcall obb_behavior(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Behavior(); }

// ---- the class's own slots -------------------------------------------------
static int __fastcall obb_init(void *s, void *)
{ return _ZN19OrangeBallBillboard13InitResourcesEv(s); }
/* slot 3 takes NO receiver: the matched body is a free function over the
   overlay's one SharedFilePtr (the SPIKE_BOMB / arena shape). */
static int __fastcall obb_clean(void *s, void *)
{ (void)s; return _ZN19OrangeBallBillboard16CleanupResourcesEv(); }
/* slot 9 is a REAL MSVC METHOD (src/..Render..cpp defines
   OrangeBallBillboard::Render over include/OrangeBallBillboard.h), so it is
   called through the class rather than by a C name that is never emitted.
   Its body is `((Sub *)&mModel)->m(0)` -- a six-virtual local shadow landing on
   _ZTV5Model slot 5, which hal/cxxname_bridge.cpp DUAL-FILLS (slots 4 and 5
   both Render) precisely for shadow-TU dispatch. Same reading as SPIKE_BOMB's,
   and this class has no ModelAnim, so the dual-fill collision does not apply. */
static int __fastcall obb_render(void *s, void *)
{
    port_actor_render_probe("ORANGE_BALL_BILLBOARD", (char *)s + 0xd4);
    return ((OrangeBallBillboard *)s)->Render();
}
/* slot 16, HOST THUNK, not the matched TU -- the shadow-class MSVC destructor
   (file header). The chain is what the matched D0 spells minus the Deallocate,
   and relocs.txt pins it: 0x021111cc loads 0x02111604 (the table), 0x021111b4
   calls 0x02016d20 (Model::~Model, member at +0xd4), 0x021111bc calls
   0x020112c8 (Actor::D2). Nothing else. */
static int __fastcall obb_d1(void *s, void *)
{
    *(void **)s = (void *)_ZTV19OrangeBallBillboard;
    _ZN5ModelD1Ev((char *)s + 0xd4);
    _ZN5ActorD2Ev(s);
    return (int)(size_t)s;
}
static int __fastcall obb_d0(void *s, void *)
{ return (int)(size_t)_ZN19OrangeBallBillboardD0Ev((int *)s); }

// ---- the mount bring-up ----------------------------------------------------
/* CAPTURED, and the argument is hal/level_boot.cpp's on g_level_mounted: this
   flag says "port_ov44_bringup has run", and everything that pass writes --
   the mount's rebased pointers and the SharedFilePtrs its static initialisers
   construct -- lives in .dsstate. A restore rolls that back. A guard that does
   not roll back with it leaves the pass skipped forever and the overlay
   holding raw DS pointers, which is the defect behind both of the RELOAD
   review's referrals. Bracketed, the pass re-runs exactly when its results
   were rolled away. */
DSSTATE_BEGIN
static int g_ov44_bringup_done;
DSSTATE_END

extern "C" void port_ov44_bringup(void)
{
    if (g_ov44_bringup_done)
        return;
    g_ov44_bringup_done = 1;
    port_ov044_pack_check();
    port_ov044_syms_patch();
    __sinit_ov044_02111314();
}

// ============================================================================
// ORANGE_BALL_BILLBOARD / daObjKb1Billboard_c (id 301) -- table 0x02111604,
// 31 slots, x16 on level 36
// ============================================================================
//
// Actor build: operator new(292 = 0x124) in OrangeBallBillboard_Spawn, Model at
// +0xd4, no collider. Spawn stores the table BY A REAL NAME
// (`p[0] = (int)_ZTV19OrangeBallBillboard`, not a VT placeholder), so the row
// registers the matched Spawn directly with no factory wrapper.
//
// THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug
// (hal/actor_classes_ov002g200.cpp).
extern "C" void hal_fill_orange_ball_billboard_vtable(void)
{
    port_ov44_bringup();
    void *volatile *vt = (void *volatile *)_ZTV19OrangeBallBillboard;
    vt[0]  = (void *)obb_init;
    vt[1]  = (void *)obb_binit;
    vt[2]  = (void *)obb_ainit;
    vt[3]  = (void *)obb_clean;
    vt[4]  = (void *)obb_bclean;
    vt[5]  = (void *)obb_aclean;
    vt[6]  = (void *)obb_behavior;   /* ActorBase::Behavior, inherited */
    vt[7]  = (void *)obb_bbeh;
    vt[8]  = (void *)obb_abeh;
    vt[9]  = (void *)obb_render;
    vt[10] = (void *)obb_bren;
    vt[11] = (void *)obb_aren;
    vt[12] = (void *)obb_pdes;       /* ActorBase::OnPendingDestroy, inherited */
    vt[13] = (void *)obb_trap13;     /* ActorBase::Virtual34, the wf/ov45 trap */
    vt[14] = (void *)obb_trap14;     /* ActorBase::Virtual38, likewise */
    vt[15] = (void *)obb_heap;
    vt[16] = (void *)obb_d1;
    vt[17] = (void *)obb_d0;
    vt[18] = (void *)obb_yoshi;
    vt[19] = (void *)obb_turn_egg;
    vt[20] = (void *)obb_v50;
    vt[21] = (void *)obb_pounded;
    vt[22] = (void *)obb_atk1;
    vt[23] = (void *)obb_atk2;
    vt[24] = (void *)obb_kicked;
    vt[25] = (void *)obb_pushed;
    vt[26] = (void *)obb_cannon;
    vt[27] = (void *)obb_mega;
    vt[28] = (void *)obb_under;
    vt[29] = (void *)obb_egg;
    vt[30] = (void *)obb_trap30;     /* declines, the wf/ov45 reading */
}
