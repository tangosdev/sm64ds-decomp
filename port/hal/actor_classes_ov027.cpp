// run rel0215 wave 2, lane cast-ov027 (W2-ov027): ov027's SNOWMAN'S LAND cast
// -- level 19's own five ids on four vtables, fresh per-symbol mount
// (port/ov027_syms.txt), the ov036/ov072/ov094 dual-mount convention. Slice is
// port/slice_ov027cast.txt.
//
// ============================================================================
// READ port/ov027_syms.txt FIRST. TWO THINGS THERE DECIDE THIS FILE.
// ============================================================================
//
// (1) THERE ARE FIVE IDS, NOT THE FOUR THE TASK SHEET NAMED. Route 2 (the
// SpawnInfo's own word[1] idhalf, plus word[0] landing inside ov027's .text)
// cuts route 1's thirty-one candidates to five: 92, 93, 217, 258 and 275.
// Id 258's SpawnInfo and factory are both UNNAMED in config
// (data_ov027_02113a00 / func_ov027_0211207c), its ROM RTTI name is
// daPgDfdr_c, it owns sixteen of the overlay's 53 functions, and it is placed
// on level 19. The task sheet's own booby-trap-2 TU (func_ov027_02111d8c) is
// its CleanupResources, so seating four and not five would have left the
// flagged TU unlinked.
//
// (2) UNLIKE ov036 AND ov045, dsd's NAMES ARE CORRECT BY ADDRESS HERE. There
// is no off-by-one on this overlay -- every function that stores a table
// stores the one its own name predicts, checked against all four vtable[-1]
// typeinfo name strings. The pairing is still done BY ADDRESS below, but it
// agrees with the names rather than contradicting them.
//
//   id(s)  registry name   ROM RTTI name         host vtable array         slots
//   92,93  SLIDING_ICE*    17daObjSlIceBlock_c   _ZTV10SlidingIce           32
//    217   CHILL_BULLY     12daIDonketu_c        data_ov027_02113930        37
//    258   DA_PG_DFDR      10daPgDfdr_c          data_ov027_02113a90        32
//    275   SNOWMAN_BREATH  10daSnmBth_c          _ZTV13SnowmanBreath        31
//
// IDS 92 AND 93 SHARE ONE TABLE AND ONE FILL. SlidingIce_Spawn and
// SlidingIceSpawner_Spawn are the same twelve instructions -- allocate 812,
// Platform::C2, store 0x02113824 -- and the class branches on its own actor id
// (`mActorID == 0x5d`) inside InitResources, CleanupResources and Render. Two
// SpawnInfo records, two registry rows, two factories, ONE class. Only 92 is
// placed on level 19; 93 is what 92 spawns.
//
// TWO HOST ARRAY NAMES ARE THE CONFIG SPELLING, NOT THE ROM RTTI SPELLING, and
// that is not cosmetic: port/tools/vtspan.py --sweep sizes a host array against
// the ROM by looking its name up in config, and config names 0x02113930 and
// 0x02113a90 only data_ov027_02113930 / data_ov027_02113a90 --
// _ZTV12daIDonketu_c and _ZTV10daPgDfdr_c are host-only spellings out of
// include/decl_common.h. Declared under the host-only name, these two arrays
// would not have been sized by the one tool that sizes them (the ov036
// data_ov036_02113a98 correction). The TUs that spell the host-only names get
// a per-source -D onto these.
//
// ============================================================================
// THE SLOT MAP, out of config/arm9/overlays/ov027/relocs.txt with every target
// resolved through config/arm9/symbols.txt, cross-read against the raw bytes
// of extracted/overlays/overlay_0027.bin (never dsd's copy, trap T4).
// ============================================================================
//
// slot  92/93 SlIceBlock   217 IDonketu       258 PgDfdr        275 SnmBth      what the ROM names
//   0   02111440 own       0211181c own       02111eb4 own      021129dc own    InitResources
//   1   02011268           02011268           02011268          02011268        Actor::BeforeInitResources
//   2   02011244           02011244           02011244          02011244        Actor::AfterInitResources(u32)
//   3   02111264 own       02116ca0 ov064     02111d8c own      02112700 own    CleanupResources
//   4   02011220           02011220           02011220          02011220        Actor::BeforeCleanupResources
//   5   02011214           02011214           02011214          02011214        Actor::AfterCleanupResources(u32)
//   6   02111304 own       02111770 own       02111e34 own      021127a4 own    Behavior
//   7   02010fd4           02010fd4           02010fd4          02010fd4        Actor::BeforeBehavior
//   8   02010fc8           02010fc8           02010fc8          02010fc8        Actor::AfterBehavior(u32)
//   9   021112b4 own       02116cf0 ov064     02111e00 own      02112740 own    Render
//  10   02010f78           02010f78           02010f78          02010f78        Actor::BeforeRender
//  11   02010f6c           02010f6c           02010f6c          02010f6c        Actor::AfterRender(u32)
//  12   02043ac0           02043ac0           02111dfc own      0211273c own    OnPendingDestroy (258 and 275 override)
//  13   0204357c           0204357c           0204357c          0204357c        ActorBase::Virtual34(u32,u32)
//  14   0204349c           0204349c           0204349c          0204349c        ActorBase::Virtual38(u32,u32)
//  15   02043494           02043494           02043494          02043494        ActorBase::OnHeapCreated
//  16   021111a0 own       021115c4 own       021118c8 own      021120c4 own    D1
//  17   021111e4 own       02111618 own       02111924 own      02112104 own    D0
//  18..26  the arm9 shared half, byte-identical on all four
//  27   0211123c own       02010130           02010130          02010130        OnHitByMegaChar (92/93 override)
//  28   0201012c           0201012c           0201012c          0201012c        Actor::OnHitFromUnderneath
//  29   02010124           02115f84 ov062     02010124          02010124        OnAimedAtWithEgg (217 overrides)
//  30   020100dc           020100dc           020100dc          020100dc        Actor::OnAimedAtWithEggReturnVec
//  31   020ee55c           02111680 own       020ee55c          --              Platform::Kill / 217's own Enemy slot
//  32   --                 021116f0 own       --                --              217 only
//  33   --                 02116374 ov064     --                --              217 only
//  34   --                 02116360 ov064     --                --              217 only
//  35   --                 0211635c ov064     --                --              217 only
//  36   --                 02116348 ov064     --                --              217 only
//
// SNOWMAN_BREATH IS THE ONE PLAIN ACTOR: its Spawn calls Actor::C2, its base
// typeinfo is arm9 0x0208e390, and its table stops at Actor's slot 30 with
// zero padding to the EXACT .data end 0x02113be0. SLIDING_ICE and DA_PG_DFDR
// are Platform-derived (slot 31 Platform::Kill). CHILL_BULLY is Enemy-derived
// through Bully (ov064): its slots 3 and 9 ARE ov064's bodies, parked directly
// in the table, and slots 33..36 are Bully's own virtuals.
//
// ============================================================================
// TRAP T1: THESE TABLES STAY ROM-SHAPED, AND ONE OVERRIDE DEPENDS ON IT.
// ============================================================================
// None of the four derives from Model or ModelAnim -- two derive from Platform
// (dBgActor_c), one from Bully/Enemy and one from Actor -- so the default
// applies and the arrays are indexed exactly as the ROM indexes them, D1 at 16
// and D0 at 17. The Model/ModelAnim exception in hal/cxxname_bridge.cpp does
// not reach here. CHILL_BULLY and DA_PG_DFDR each hold a ModelAnim MEMBER (at
// +0x110 and +0x320); that is a different question and those members dispatch
// through cxxname_bridge's own MSVC-shaped tables, untouched by this file.
//
// THE OVERRIDE THAT DEPENDS ON IT is src/func_ov027_0211123c.cpp, SLIDING_ICE's
// slot 27. It declares a local shadow with THIRTY-TWO virtuals -- v0..v30 then
// m() -- and calls `c->m()`, so MSVC indexes m at 31, and 31 is exactly where
// the ROM parks Platform::Kill. The shadow declares NO destructor, so there is
// no D1/D0 pair for MSVC to fold and the shadow's numbering and the ROM's
// agree slot for slot. A table folded to MSVC numbering would have sent that
// call one slot off the end. Measured, not assumed: the shadow is in the TU.
//
// ============================================================================
// ov064's BULLY BASE TABLE IS HOST STORAGE HERE.
// ============================================================================
// CHILL_BULLY derives Bully (ov064 0x0211b768, 37 slots). port/ov064_syms.txt
// deliberately leaves that table OUT of its mount -- "its typeinfo word at
// 0x0211b764 relocates to 0x021138bc in the shared multi-overlay level window,
// so mounting it would bake a cross-level pointer" -- and nothing else in the
// tree defines it, so it is declared as a host array below. It is an
// intermediate installed only transiently: ChillBully_Spawn stores it right
// after Enemy::C2 and overwrites it with the derived table four member
// constructors later, and the two destructors install it between teardowns.
// None of those four member constructors dispatches a virtual on the receiver,
// so nothing reads it. It gets the shared half and traps for the rest, the
// ov036 hal_fill reading exactly: if a future class does dispatch through it,
// the trap says which slot.
//
// ============================================================================
// TRAP T5: TWELVE ov027 BODIES CARRY THE "recovered from vtable slot identity"
// MARKER. ALL TWELVE ARE REAL DECOMP; NINE OF THEM ARE SEATED HERE.
// ============================================================================
// func_ov027_0211123c, _02111618, _02111680, _021116f0, _02111770, _0211181c,
// _02111924, _02111d8c, _02111dfc, _02111e00, _02111e34, _02111eb4.
// Adjudicated per body: every one was put through the repo's own byte gate --
// tools/match.py at the canonical 2004/b56 with --strict-relocs on (the
// default), so every relocation SLOT'S DESTINATION is checked against
// config/arm9/overlays/ov027/relocs.txt rather than wildcarded, and
// --module ov027 so the target is this overlay's own image. ALL TWELVE REPORT
// MATCHING. A body that byte-reproduces the ROM at the canonical compiler with
// every reloc destination checked is not a behavioural guess.
//
// TWO of the twelve are the standing D1/D0-fold mislabel: _02111618 and
// _02111924 are both named "OnYoshiTryEat" by the marker and are both the
// DELETING DESTRUCTOR (slot 17) -- the NAME was guessed from a vtable slot and
// guessed wrong, the BODY is the ROM instruction for instruction. This is
// T5's first branch throughout: nothing here is a behavioural guess and
// nothing is de-seated.
//
// The twelve are recorded in port/tools/inferred_stub_adjudicated.txt, which is
// the guard's OWN route for a pre-seat ROM ruling. NOTHING was added to
// inferred_stub_baseline.txt. Each still OWES a decomp-side marker correction,
// and its adjudicated entry retires when the marker line goes; this lane does
// not edit src/, so that correction is owed and not done here.
//
// ============================================================================
// THREE TUs ARE NOT COMPILED.
// ============================================================================
// src/func_ov027_02111d38.cpp and src/func_ov027_02111cfc.cpp both spell
// `(c->**p)()` over an mwcc eight-byte member pointer. MSVC's is four bytes and
// cannot deliver `this` in ecx onto the plain cdecl state bodies, so both are
// host-copied in port/unmatched/DaPgDfdr_StateDispatch.cpp and dropped from the
// slice; the byte-matched originals stay in src/ as proof. That file carries
// the four addresses, the disassembly of both sites, and the reason
// port/tools/stategen.py REFUSED to generate it (this constructor spells the
// pair as `struct S2 { int w[2]; }`, one array field, and stategen's parser
// keys on two scalar fields -- a refusal, correctly, rather than a guess).
//
// THE THIRD is src/func_ov027_02111e00.cpp, DA_PG_DFDR's Render, and it is the
// T1 Model/ModelAnim exception biting. It dispatches ROM slot 5 of the
// ModelAnim at +0x320 through a six-virtual local shadow, and the host
// _ZTV9ModelAnim is MSVC-numbered with Virtual18 genuinely in that slot -- the
// Whomp/Fish case, and FLYING_CARPET's. This lane MEASURED it rather than
// predicting it: the first seated boot of level 19 faulted on frame 2 in
// Model::Virtual10 with a null, and faultmap resolved the chain
// port_actor_process -> pd_render -> func_ov027_02111e00 -> ModelAnim::Virtual18
// -> ModelAnim::Virtual10 -> Model::Virtual10. Host copy in
// port/unmatched/DaPgDfdr_Render.cpp, matched original byte-locked in src/.
// SWEPT for siblings: only two ov027 TUs declare a six-virtual `m()` shadow,
// this one and src/func_ov027_0211123c.cpp -- and that second one dispatches
// slot 31 of SLIDING_ICE's OWN ROM-shaped table, where the shadow's numbering
// and the ROM's already agree (see TRAP T1 above). It stays compiled.

#include <cstdio>

#include "dsstate_seg.h"

#include "Actor.h"
#include "ActorBase.h"
#include "Bully.h"
#include "SlidingIce.h"
#include "SnowmanBreath.h"

// ---- two decorated references onto bodies the port already carries ---------
// src/_ZN13SnowmanBreath8BehaviorEv.cpp is a .cpp that declares two arm9/ov002
// entry points with TYPES, outside its own extern "C" block, so MSVC decorates
// the references while the host bodies carry the plain cdecl names. Each row
// is a decorated reference onto an EXISTING definition, not a rename of
// anything: the LHS is a decorated spelling nothing in this link defines,
// which is what keeps alternatename_guard's rule (an /alternatename whose LHS
// is also DEFINED is defeated silently) satisfied by construction. Both were
// taken off the FIRST LINK'S OWN unresolved list, not guessed:
//
//   ?_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj@@YAHPAX0IPBUVector3@@II@Z
//       -> __ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj, defined by
//          hal/message_probe.cpp:28. hal/cxx_aliases.cpp:1121 already routes a
//          DIFFERENT decorated spelling of the same body (@@YAXPAUPlayer@@...,
//          returning void and taking Player*); this TU's spelling returns int
//          and takes void*, so it needs its own row. Both are cdecl and the
//          argument slots line up: the two trailing u8s are pushed as full
//          words either way and the callee reads the low byte.
//   ?_ZN5Sound8PlayLongEjjjRK7Vector3j@@YAHHIIPAXI@Z
//       -> __ZN5Sound8PlayLongEjjjRK7Vector3j, defined by
//          hal/bob_enemy_bridges.cpp:27. hal/cxx_aliases.cpp:1233 already
//          routes @@YAIIIIPAXI@Z (unsigned return, unsigned first argument);
//          this TU spells int/int, same four bytes in the same slots.
#pragma comment(linker, "/alternatename:?_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj@@YAHPAX0IPBUVector3@@II@Z=__ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj")
#pragma comment(linker, "/alternatename:?_ZN5Sound8PlayLongEjjjRK7Vector3j@@YAHHIIPAXI@Z=__ZN5Sound8PlayLongEjjjRK7Vector3j")

// ---- and two onto this lane's own mounted cells ----------------------------
// src/_ZN10SlidingIce16CleanupResourcesEv.cpp is the same shape: it declares
// `extern char func_ov030_02113be8[];` and its sibling at FILE scope, outside
// any extern "C", so after the per-source -D in port/CMakeLists.txt routes the
// wrong-overlay name onto ov027's own cell the reference is still decorated,
// while ovdata's emission carries the plain cdecl name. Its twin,
// src/_ZN10SlidingIce13InitResourcesEv.cpp, declares the same two INSIDE an
// extern "C" block and needs no row -- which is why only one of the two TUs
// appeared on the unresolved list.
#pragma comment(linker, "/alternatename:?data_ov027_02113be8@@3PADA=_data_ov027_02113be8")
#pragma comment(linker, "/alternatename:?data_ov027_02113be0@@3PADA=_data_ov027_02113be0")

extern "C" {
/* the arm9 shared half; every address read off this overlay's own reloc runs */
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
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p); /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);  /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                 /* slot 29 */
void _ZN8Platform4KillEv(void *self);                          /* slot 31 */

/* ---- SLIDING_ICE (92, 93), 0x02113824 ---- */
int _ZN10SlidingIce13InitResourcesEv(char *self);   /* slot 0, a C name */
int *_ZN10SlidingIceD1Ev(int *self);                /* slot 16 */
int *_ZN10SlidingIceD0Ev(int *self);                /* slot 17 */
void func_ov027_0211123c(void *self, void *player); /* slot 27 override */
void *SlidingIce_Spawn(void);
void *SlidingIceSpawner_Spawn(void);

/* ---- CHILL_BULLY (217), 0x02113930 ---- */
int func_ov027_0211181c(void *self);                /* slot 0  */
int func_ov027_02111770(char *self);                /* slot 6  */
int _ZN5Bully6RenderEv(void *self);                 /* slot 9, ov064, a C name */
int *func_ov027_021115c4(int *self);                /* slot 16, D1 */
int *func_ov027_02111618(int *self);                /* slot 17, D0 */
int func_ov062_02115f84(void *self);                /* slot 29 override, ov062 */
int func_ov027_02111680(void *self);                /* slot 31 */
int func_ov027_021116f0(void *self);                /* slot 32 */
int func_ov064_02116374(void *self);                /* slot 33 */
int func_ov064_02116360(void *self);                /* slot 34 */
int func_ov064_0211635c(void *self);                /* slot 35 */
int func_ov064_02116348(void *self);                /* slot 36 */
void *ChillBully_Spawn(void);

/* ---- DA_PG_DFDR (258), 0x02113a90 ---- */
int func_ov027_02111eb4(void *self);                /* slot 0  */
int func_ov027_02111d8c(char *self);                /* slot 3  */
int func_ov027_02111e34(void *self);                /* slot 6  */
int func_ov027_02111e00(void *self);                /* slot 9  */
int func_ov027_02111dfc(void *self);                /* slot 12 override */
int *func_ov027_021118c8(int *self);                /* slot 16, D1 */
int *func_ov027_02111924(int *self);                /* slot 17, D0 */
void *func_ov027_0211207c(void);                    /* the UNNAMED factory */

/* ---- SNOWMAN_BREATH (275), 0x02113b50 ---- */
int _ZN13SnowmanBreath16CleanupResourcesEv(void *self);   /* slot 3, a C name */
void _ZN13SnowmanBreath16OnPendingDestroyEv(void *self);  /* slot 12, a C name */
int _ZN13SnowmanBreathD1Ev(char *self);                   /* slot 16 */
void *_ZN13SnowmanBreathD0Ev(char *self);                 /* slot 17 */
void *SnowmanBreath_Spawn(void);

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy */
void port_actor_render_probe(const char *cls, void *model);
void port_dapgdfdr_states_check(void);   /* unmatched/DaPgDfdr_StateDispatch */
}

// ---- the five host vtable arrays -------------------------------------------
// THREE CARRY DS-SHAPED NAMES, so they have to sit inside the .dsstate span a
// save state captures (hal/dsstate_seg.h; dsstate_guard fails the build
// otherwise, which is how ov036's four were found rather than reasoned about).
// The two _ZTV*-named tables are not DS-shaped names and the guard does not
// ask for them. Grouped so one DSSTATE_BEGIN/END pair covers the set.
extern "C" {
DSSTATE_BEGIN
void *data_ov027_02113930[37];  /* vtspan: data_ov027_02113930, CHILL_BULLY */
void *data_ov027_02113a90[32];  /* vtspan: data_ov027_02113a90, DA_PG_DFDR */
void *data_ov064_0211b768[37];  /* vtspan: Bully/BigBully base, ov064 0x0211b768 */
DSSTATE_END

int _ZTV10SlidingIce[32];       /* vtspan: _ZTV10SlidingIce, SLIDING_ICE 92+93 */
int _ZTV13SnowmanBreath[31];    /* vtspan: _ZTV13SnowmanBreath, SNOWMAN_BREATH */
}

// ---- one __thiscall face onto an @@QAE body --------------------------------
// src/_ZN5Bully16CleanupResourcesEv.cpp (ov064's, LINKED via
// port/slice_gate177.txt) is a real C++ method, so the link carries only
// ?CleanupResources@Bully@@QAEHXZ and there is no _ZN5Bully16CleanupResourcesEv
// C name for slot 3 to take. include/Bully.h declares the class, so the call
// is spelled as the method it is rather than aliased: an /alternatename from a
// @@QAE LHS onto a cdecl body would hand it a `this` that never was one
// (hal/cxx_aliases.cpp's standing rule). Bully::Render IS a C name and is
// declared above with the others.

// ---- the traps -------------------------------------------------------------
// Slots 13/14 are ActorBase::Virtual34/Virtual38 and slot 30 is
// Actor::OnAimedAtWithEggReturnVec. The ov036/ov072/ov100pl convention: none of
// the three can be serviced by a __fastcall(void*, void*) thunk -- 13/14 take
// two u32 arguments each and their solid-heap bodies are not linked into the
// port at all, and 30 is an SRET method whose hidden return pointer no thunk
// shape here models. Trap them by name rather than seat a body with the wrong
// contract.
static void ov27_trap_report(void *self, int slot, const char *which)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d (%s) is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n",
                 slot, which, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[160];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d (%s) on id %u %s",
                    slot, which, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall ov27_trap13(void *s, void *)
{ ov27_trap_report(s, 13, "ActorBase::Virtual34"); return 0; }
static int __fastcall ov27_trap14(void *s, void *)
{ ov27_trap_report(s, 14, "ActorBase::Virtual38"); return 0; }
static int __fastcall ov27_trap30(void *s, void *)
{ ov27_trap_report(s, 30, "Actor::OnAimedAtWithEggReturnVec"); return 0; }
/* the Bully base table's own slots. Nothing dispatches through an intermediate
   installed only between two member teardowns; this says which slot instead of
   running a derived body on a base that has not finished constructing. */
static int __fastcall ov27_base_trap0(void *s, void *)
{ ov27_trap_report(s, 0, "Bully base InitResources"); return 0; }
static int __fastcall ov27_base_trap3(void *s, void *)
{ ov27_trap_report(s, 3, "Bully base CleanupResources"); return 0; }
static int __fastcall ov27_base_trap6(void *s, void *)
{ ov27_trap_report(s, 6, "Bully base Behavior"); return 0; }
static int __fastcall ov27_base_trap9(void *s, void *)
{ ov27_trap_report(s, 9, "Bully base Render"); return 0; }

// ---- the shared 1..30 half -------------------------------------------------
static int __fastcall ov27_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov27_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov27_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov27_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov27_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov27_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov27_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov27_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov27_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov27_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov27_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19 takes the three-parameter shape so it emits `ret 4`: the dispatch
   site pushes the Player the callee pops -- the wf/ov036/ov072 contract. */
static int __fastcall ov27_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov27_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov27_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov27_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov27_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov27_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov27_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov27_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov27_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov27_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov27_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov27_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* Fills every slot the four tables share: 1,2,4,5,7,8,10,11,12,13,14,15 and
   18..30. Each caller writes its own 0/3/6/9/16/17 afterwards (92/93 its own
   27, 217 its own 29 and 31..36, 258 and 275 their own 12, and the two
   Platform-derived their 31).

   SLOT NUMBERING IS THE ROM'S, taken from the four reloc spans quoted in this
   file's header. Slots 18..26, 28 and 30 are byte-identical across all four
   tables and across ov036's and ov072's reviewed fills, which is what makes
   one shared fill legitimate; verified per table against the raw image, not
   assumed. Slot 27 differs on 92/93 and slot 29 on 217, and both are written
   back by their own fill below. */
static void ov27_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov27_binit;    /* Actor::BeforeInitResources */
    vt[2]  = (void *)ov27_ainit;    /* Actor::AfterInitResources(u32) */
    vt[4]  = (void *)ov27_bclean;   /* Actor::BeforeCleanupResources */
    vt[5]  = (void *)ov27_aclean;   /* Actor::AfterCleanupResources(u32) */
    vt[7]  = (void *)ov27_bbeh;     /* Actor::BeforeBehavior */
    vt[8]  = (void *)ov27_abeh;     /* Actor::AfterBehavior(u32) */
    vt[10] = (void *)ov27_bren;     /* Actor::BeforeRender */
    vt[11] = (void *)ov27_aren;     /* Actor::AfterRender(u32) */
    vt[12] = (void *)ov27_pdes;     /* ActorBase::OnPendingDestroy */
    vt[13] = (void *)ov27_trap13;   /* ActorBase::Virtual34(u32,u32), not linked */
    vt[14] = (void *)ov27_trap14;   /* ActorBase::Virtual38(u32,u32), not linked */
    vt[15] = (void *)ov27_heap;     /* ActorBase::OnHeapCreated */
    vt[18] = (void *)ov27_yoshi;    /* Actor::OnYoshiTryEat */
    vt[19] = (void *)ov27_turn_egg; /* Actor::OnTurnIntoEgg(Player&) */
    vt[20] = (void *)ov27_v50;      /* Actor::Virtual50 */
    vt[21] = (void *)ov27_pounded;  /* Actor::OnGroundPounded(Actor&) */
    vt[22] = (void *)ov27_atk1;     /* Actor::OnAttacked1(Actor&) */
    vt[23] = (void *)ov27_atk2;     /* Actor::OnAttacked2(Actor&) */
    vt[24] = (void *)ov27_kicked;   /* Actor::OnKicked(Actor&) */
    vt[25] = (void *)ov27_pushed;   /* Actor::OnPushed(Actor&) */
    vt[26] = (void *)ov27_cannon;   /* Actor::OnHitByCannonBlastedChar(Actor&) */
    vt[27] = (void *)ov27_mega;     /* Actor::OnHitByMegaChar(Player&) */
    vt[28] = (void *)ov27_under;    /* Actor::OnHitFromUnderneath(Actor&) */
    vt[29] = (void *)ov27_aimed;    /* Actor::OnAimedAtWithEgg */
    vt[30] = (void *)ov27_trap30;   /* Actor::OnAimedAtWithEggReturnVec, SRET */
}

// ============================================================================
// ov064's Bully/BigBully base table, filled once.
// ============================================================================
/* THE `void **tabs[1]` SHAPE IS LOAD-BEARING, not stylistic.
   port/tools/vtspan.py --fills resolves a fill to the table it writes by
   reading the binding, and it recognises exactly two shapes: one
   `void **vt = <named array>;` and the `void **tabs[N] = { ... }` loop. A
   helper taking the array as a parameter, or a cast in the initialiser list,
   reports as "table binding not modelled" -- a GAP IN THE CHECK, not a pass.
   The array is declared `void *` above for the same reason: an `int` array
   needs a cast here, and a cast defeats the binding read. */
static void ov27_bully_base_bringup(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    void *volatile *vt = (void *volatile *)data_ov064_0211b768;
    ov27_fill_shared(vt);
    vt[0]  = (void *)ov27_base_trap0;
    vt[3]  = (void *)ov27_base_trap3;
    vt[6]  = (void *)ov27_base_trap6;
    vt[9]  = (void *)ov27_base_trap9;
    /* 16/17: an intermediate installed only between two member teardowns never
       reaches its own destructors either. */
    vt[16] = (void *)ov27_base_trap0;
    vt[17] = (void *)ov27_base_trap0;
    /* 31..36 are Enemy/Bully's own virtuals. The derived table overwrites all
       six one instruction later in the constructor and the two destructors
       never dispatch through them, so they trap rather than borrow 217's. */
    vt[31] = (void *)ov27_base_trap0;
    vt[32] = (void *)ov27_base_trap0;
    vt[33] = (void *)ov27_base_trap0;
    vt[34] = (void *)ov27_base_trap0;
    vt[35] = (void *)ov27_base_trap0;
    vt[36] = (void *)ov27_base_trap0;
}

// ============================================================================
// SLIDING_ICE (92 SlidingIceSpawner, 93 SlidingIce) -- 17daObjSlIceBlock_c,
// 0x02113824, 32 slots, Platform-derived. ONE class, two ids, one fill.
// Own 0/3/6/9/16/17 and slot 27.
// ============================================================================
static int __fastcall si_init(void *s, void *)
{ return _ZN10SlidingIce13InitResourcesEv((char *)s); }
static int __fastcall si_clean(void *s, void *)
{ return ((SlidingIce *)s)->SlidingIce::CleanupResources(); }
static int __fastcall si_behavior(void *s, void *)
{ return ((SlidingIce *)s)->SlidingIce::Behavior(); }
static int __fastcall si_render(void *s, void *)
{ port_actor_render_probe("SLIDING_ICE", (char *)s + 0xd4);
  return ((SlidingIce *)s)->SlidingIce::Render(); }
static int __fastcall si_d1(void *s, void *)
{ return (int)(size_t)_ZN10SlidingIceD1Ev((int *)s); }
static int __fastcall si_d0(void *s, void *)
{ return (int)(size_t)_ZN10SlidingIceD0Ev((int *)s); }
/* slot 27 takes the three-parameter shape for slot 19's reason: the dispatch
   site pushes the Player the callee pops. The body itself dispatches slot 31
   back through this same table -- see TRAP T1 in the header. */
static int __fastcall si_mega(void *s, void *, void *p)
{ func_ov027_0211123c(s, p); return 0; }

extern "C" void hal_fill_sliding_ice_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV10SlidingIce;
    ov27_fill_shared(vt);
    vt[0]  = (void *)si_init;
    vt[3]  = (void *)si_clean;
    vt[6]  = (void *)si_behavior;
    vt[9]  = (void *)si_render;
    vt[16] = (void *)si_d1;
    vt[17] = (void *)si_d0;
    vt[27] = (void *)si_mega;
    vt[31] = (void *)ov27_kill;
}

// ============================================================================
// CHILL_BULLY (217) -- 12daIDonketu_c, 0x02113930, 37 slots, Bully <- Enemy.
// One on level 19. Own 0/6/16/17/29/31/32; 3 and 9 are ov064's Bully bodies
// the ROM parks directly in the table, and 33..36 are ov064's too.
// ============================================================================
static int __fastcall cb_init(void *s, void *)
{ return func_ov027_0211181c(s); }
static int __fastcall cb_clean(void *s, void *)
{ return ((Bully *)s)->Bully::CleanupResources(); }
static int __fastcall cb_behavior(void *s, void *)
{ return func_ov027_02111770((char *)s); }
static int __fastcall cb_render(void *s, void *)
{ port_actor_render_probe("CHILL_BULLY", (char *)s + 0x110);
  return _ZN5Bully6RenderEv(s); }
static int __fastcall cb_d1(void *s, void *)
{ return (int)(size_t)func_ov027_021115c4((int *)s); }
static int __fastcall cb_d0(void *s, void *)
{ return (int)(size_t)func_ov027_02111618((int *)s); }
static int __fastcall cb_aimed(void *s, void *)
{ return func_ov062_02115f84(s); }
static int __fastcall cb_v31(void *s, void *)
{ return func_ov027_02111680(s); }
static int __fastcall cb_v32(void *s, void *)
{ return func_ov027_021116f0(s); }
static int __fastcall cb_v33(void *s, void *)
{ return func_ov064_02116374(s); }
static int __fastcall cb_v34(void *s, void *)
{ return func_ov064_02116360(s); }
static int __fastcall cb_v35(void *s, void *)
{ return func_ov064_0211635c(s); }
static int __fastcall cb_v36(void *s, void *)
{ return func_ov064_02116348(s); }

extern "C" void hal_fill_chill_bully_vtable(void)
{
    ov27_bully_base_bringup();
    void *volatile *vt = (void *volatile *)data_ov027_02113930;
    ov27_fill_shared(vt);
    vt[0]  = (void *)cb_init;
    vt[3]  = (void *)cb_clean;
    vt[6]  = (void *)cb_behavior;
    vt[9]  = (void *)cb_render;
    vt[16] = (void *)cb_d1;
    vt[17] = (void *)cb_d0;
    vt[29] = (void *)cb_aimed;
    vt[31] = (void *)cb_v31;
    vt[32] = (void *)cb_v32;
    vt[33] = (void *)cb_v33;
    vt[34] = (void *)cb_v34;
    vt[35] = (void *)cb_v35;
    vt[36] = (void *)cb_v36;
}

// ============================================================================
// DA_PG_DFDR (258) -- 10daPgDfdr_c, 0x02113a90, 32 slots, Platform-derived.
// One on level 19. Own 0/3/6/9/12/16/17. The registry name is the ROM's own
// RTTI string transliterated: config names neither its record nor its factory,
// nothing in the tree carries an English name for id 258, and inventing one
// would be a guess printed in every census. A later lane with the reference
// tree can rename it; the ADDRESSES are what this file binds.
// Its two-state pointer-to-member machine is hosted in
// port/unmatched/DaPgDfdr_StateDispatch.cpp.
// ============================================================================
static int __fastcall pd_init(void *s, void *)
{ return func_ov027_02111eb4(s); }
static int __fastcall pd_clean(void *s, void *)
{ return func_ov027_02111d8c((char *)s); }
static int __fastcall pd_behavior(void *s, void *)
{ return func_ov027_02111e34(s); }
static int __fastcall pd_render(void *s, void *)
{ port_actor_render_probe("DA_PG_DFDR", (char *)s + 0xd4);
  return func_ov027_02111e00(s); }
static int __fastcall pd_pdes(void *s, void *)
{ return func_ov027_02111dfc(s); }
static int __fastcall pd_d1(void *s, void *)
{ return (int)(size_t)func_ov027_021118c8((int *)s); }
static int __fastcall pd_d0(void *s, void *)
{ return (int)(size_t)func_ov027_02111924((int *)s); }

extern "C" void hal_fill_da_pg_dfdr_vtable(void)
{
    void *volatile *vt = (void *volatile *)data_ov027_02113a90;
    ov27_fill_shared(vt);
    vt[0]  = (void *)pd_init;
    vt[3]  = (void *)pd_clean;
    vt[6]  = (void *)pd_behavior;
    vt[9]  = (void *)pd_render;
    vt[12] = (void *)pd_pdes;
    vt[16] = (void *)pd_d1;
    vt[17] = (void *)pd_d0;
    vt[31] = (void *)ov27_kill;
    /* the constructor's copy of the four member-pointer pairs, checked once.
       Runs here rather than at sinit time because the fill is the first thing
       that happens after the sinits and before any instance exists. */
    port_dapgdfdr_states_check();
}

// ============================================================================
// SNOWMAN_BREATH (275) -- 10daSnmBth_c, 0x02113b50, 31 slots, a PLAIN ACTOR.
// One on level 19. Own 0/3/6/9/12/16/17. NO slot 31: its Spawn calls
// Actor::C2, not Platform::C2, and the table stops at Actor's slot 30.
// The object carries FIFTY sub-elements at +0xd4, stride 0x60, built by
// func_020733a8 and torn down by __destroy_arr -- which is why its Render
// loops func_ov027_02112424 fifty times.
// ============================================================================
static int __fastcall sb_init(void *s, void *)
{ return ((SnowmanBreath *)s)->SnowmanBreath::InitResources(); }
static int __fastcall sb_clean(void *s, void *)
{ return _ZN13SnowmanBreath16CleanupResourcesEv(s); }
static int __fastcall sb_behavior(void *s, void *)
{ return ((SnowmanBreath *)s)->SnowmanBreath::Behavior(); }
static int __fastcall sb_render(void *s, void *)
{ port_actor_render_probe("SNOWMAN_BREATH", (char *)s + 0xd4);
  return ((SnowmanBreath *)s)->SnowmanBreath::Render(); }
static int __fastcall sb_pdes(void *s, void *)
{ _ZN13SnowmanBreath16OnPendingDestroyEv(s); return 0; }
static int __fastcall sb_d1(void *s, void *)
{ return _ZN13SnowmanBreathD1Ev((char *)s); }
static int __fastcall sb_d0(void *s, void *)
{ return (int)(size_t)_ZN13SnowmanBreathD0Ev((char *)s); }

extern "C" void hal_fill_snowman_breath_vtable(void)
{
    void *volatile *vt = (void *volatile *)_ZTV13SnowmanBreath;
    ov27_fill_shared(vt);
    vt[0]  = (void *)sb_init;
    vt[3]  = (void *)sb_clean;
    vt[6]  = (void *)sb_behavior;
    vt[9]  = (void *)sb_render;
    vt[12] = (void *)sb_pdes;
    vt[16] = (void *)sb_d1;
    vt[17] = (void *)sb_d0;
}
