// MIS-SEATED SHARED FILL, FIXED. ov72_fill_shared used to seat bodies at the
// wrong slot indices for BOTH classes it feeds. Slots 13/14/15 were rotated
// and every slot from 20 up was written one position low, which left 29 and 30
// NULL. The correct numbering is the ROM's own, read out of the two vtable
// spans' relocations in config/arm9/overlays/ov072/relocs.txt with the targets
// resolved through config/arm9/symbols.txt. Both spans agree slot for slot:
//
//   slot  daBgSnwmn_c 0x02122978   BabyPenguin 0x02122a90   what the ROM names
//    0    0x02120a44 (own)          0x02121e84 (own)         InitResources
//    1    0x02011268                0x02011268               Actor::BeforeInitResources
//    2    0x02011244                0x02011244               Actor::AfterInitResources(u32)
//    3    0x02120980 (own)          0x02121d6c (own)         CleanupResources
//    4    0x02011220                0x02011220               Actor::BeforeCleanupResources
//    5    0x02011214                0x02011214               Actor::AfterCleanupResources(u32)
//    6    0x02120a08 (own)          0x02121e08 (own)         Behavior
//    7    0x02010fd4                0x02010fd4               Actor::BeforeBehavior
//    8    0x02010fc8                0x02010fc8               Actor::AfterBehavior(u32)
//    9    0x021209c0 (own)          0x02121db4 (own)         Render
//   10    0x02010f78                0x02010f78               Actor::BeforeRender
//   11    0x02010f6c                0x02010f6c               Actor::AfterRender(u32)
//   12    0x021209bc (own)          0x02121db0 (own)         OnPendingDestroy
//   13    0x0204357c                0x0204357c               ActorBase::Virtual34(u32,u32)
//   14    0x0204349c                0x0204349c               ActorBase::Virtual38(u32,u32)
//   15    0x02043494                0x02043494               ActorBase::OnHeapCreated
//   16    0x02120824 (own)          0x02120c58 (own)         D1
//   17    0x02120874 (own)          0x02120ca0 (own)         D0
//   18    0x02010160                0x02120cfc (own)         Actor::OnYoshiTryEat
//   19    0x02010154                0x02121fa0 (own)         Actor::OnTurnIntoEgg(Player&)
//   20    0x0201014c                0x0201014c               Actor::Virtual50
//   21    0x02010148                0x02010148               Actor::OnGroundPounded(Actor&)
//   22    0x02010144                0x02010144               Actor::OnAttacked1(Actor&)
//   23    0x02010140                0x02010140               Actor::OnAttacked2(Actor&)
//   24    0x0201013c                0x0201013c               Actor::OnKicked(Actor&)
//   25    0x02010138                0x02010138               Actor::OnPushed(Actor&)
//   26    0x02010134                0x02010134               Actor::OnHitByCannonBlastedChar(Actor&)
//   27    0x02010130                0x02010130               Actor::OnHitByMegaChar(Player&)
//   28    0x0201012c                0x0201012c               Actor::OnHitFromUnderneath(Actor&)
//   29    0x02010124                0x02010124               Actor::OnAimedAtWithEgg
//   30    0x020100dc                0x020100dc               Actor::OnAimedAtWithEggReturnVec
//
// The two tables differ in exactly the seven own-body slots (0/3/6/9/12/16/17)
// plus BabyPenguin's own 18 and 19. Every shared slot is byte-identical, which
// is what makes one shared fill legitimate. Verified independently per class,
// not assumed.
//
// TWO OF THE FOUR WERE LIVE STACK SMASHES, not merely contract violations.
//
// SLOT 28, the higher-probability one. The old fill put ov72_aimed (the
// zero-argument OnAimedAtWithEgg body) at slot 28, whose real contract is
// OnHitFromUnderneath(Actor&), one argument. src/func_ov002_020eeca8.cpp
// dispatches slot 28 (a->m70(arg)) on whatever actor the player's mesh
// collision resolves to when the hit-ceiling flag is set, and five Player
// state mains reach it (St_Jump_Main, St_WallJump_Main, St_SideFlip_Main,
// St_Shell_Main, St_MetalWaterWater_Main). Jumping into the underside of a
// BabyPenguin's collision therefore called a body that pops nothing where the
// caller pushed one word.
//
// SLOT 13, the lower-probability one, and NOT dead scaffolding. The old fill
// put ov72_heap where ActorBase::Virtual34(u32,u32) belongs, so the callee
// popped nothing where the caller pushed two words.
// port/hal/actor_classes_bob_world.cpp:101 records that a heap-pressure path
// dispatches Virtual34 on EVERY actor on the behaviour list, which is exactly
// why that file traps the slot instead of leaving it. Either ov072 class on
// that list during heap pressure took the same desync as slot 28, on a frame
// with nothing to do with the class. Slot 14 (Virtual38) had the identical
// shape. Do not read "its body is not linked into the port" as "the slot is
// not reachable": dispatch here is indirect through the table, so the ROM
// relocations to those addresses being kind:load says nothing either way.
//
// In both cases the caller pops short and its own ret takes a saved register
// as a return address. The desync is DETERMINISTIC given the dispatch, not
// state-dependent. The c0000005 signature and the level 10 concentration make
// this a strong candidate for that crash family, but no runtime reproduction
// of the specific player crash was performed. What this file does claim, and
// what is verified from the ROM relocs, from the repo's own slot maps and
// from the emitted x86 independently, is that the seating was wrong and is
// now right.
//
// SLOTS 29 AND 30 WERE NULL BUT NOT REACHABLE. Slot 30 has a single
// dispatcher and it fires only against one already-chosen egg target resolved
// by uniqueID, never over nearby actors, and eligibility needs mFlags bit
// 0x10000000 at +0xb0 which no ov072 file ever sets. Neither ov072 class can
// be an egg target, so those two NULLs are seated correctly here because the
// ROM says so, not because they were crashing.
//
// SLOT 19 IS CLEAN, and this note used to say otherwise. bp_egg
// (OnTurnIntoEgg(Player&)) has to pop the argument the caller pushed, and it
// does: it is declared with the stack parameter and emits ret 4. That fix is
// owned by the port-c2-player-stack lane, which is why nothing here touches
// it, and that lane landed AHEAD of this file in the 0.1.3 wave. abicheck
// over the merged tree reads zero UNDERPOP and zero OVERPOP, so there is no
// outstanding pop-contract defect in this file.
//
// GATE 193: the actual SNOWMAN (272), ov072. Fresh per-symbol mount
// (port/ov072_syms.txt), the ov079/ov080/ov081 convention. A plain
// 31-slot Actor table (no Kill -- Actor-derived, not Platform-derived).
// Full slot derivation, the class-identity correction (id 272 is the
// UNNAMED daBgSnwmn_c block, NOT SnowmanBody or SnowmanHead) and the
// vtable-slot-count correction are documented in port/ov072_syms.txt's
// own header. Its vtable was read directly out of
// extracted/overlays/overlay_0072.bin (ROM-byte verified, not inferred
// from relocs alone).
//
// SNOWMAN'S OWN PMF-FREE SHAPE: no state-cell dispatch of its own -- its
// nine files are all plain calls, matching MotherPenguin's gate-191
// shape (a plain Actor with a real MSVC-synthesised D1 destructor and a
// D0 that stores its own table and never overwrites it).
//
// PREPARE SHORT-1 BUG: func_ov072_02120a44's InitResources calls
// TextureSequence::Prepare with the MotherPenguin (gate 191) two-argument
// shape; HOST COPY port/unmatched/Snowman_InitResources.cpp fixes it.
//
// SNOWMAN_HEAD (273) COUPLING: daBgSnwmn_c::InitResources spawns
// SnowmanHead (Actor::Spawn(0x111, ...)) unconditionally when star 5 is
// not yet collected, then marks itself for destruction the same frame.
// SnowmanHead stays UNREGISTERED this gate -- the Spawn call reaches the
// registry's own "not registered" skip path, not a crash (verified at
// runtime, see the commit message).
//
// BABY_PENGUIN (256) IS REGISTERED (babypenguin-crash lane). The crash
// that excluded it from gate 193 is ROOT-CAUSED and fixed: the matched
// src _ZN11BabyPenguin6RenderEv.cpp dispatched ModelAnim slot 5 through a
// local six-virtual ROM-order shadow, which the host _ZTV9ModelAnim
// numbers as Virtual18 (the documented ModelAnim slot-5 collision, the
// Whomp/Butterfly/Fish case) -- Virtual18's __fastcall trampoline pops
// TWO stack words where the shadow call pushed one, the skewed caller
// rets into its own spilled `self`, and the jump surfaces as a DEP
// execute-violation inside port_actor_process's frame (which is why it
// first read as a "shared actor-Process pipeline" bug -- the pipeline
// was the victim frame, not the culprit). Fix: the matched Render TU is
// dropped from slice_gate193.txt (byte-locked in src/ as proof) and
// _ZN11BabyPenguin6RenderEv is a HOST COPY in
// port/unmatched/ModelAnim_Renders.cpp with the dispatch spelled as the
// qualified ModelAnim::Render. The six-cell PMF state table, the
// func_ov072_02121d50 SHORT-1 fixes and both state host copies were
// verified INNOCENT during the hunt (all seat-checked valid at the
// moment of the crash). daBgSnwmn_c is not exposed: its +0xd4/+0x124
// members are plain 0x50-stride Models and _ZTV5Model[5] is dual-filled.
//
// FindWithActorID(0x101) MOTHER COUPLING (BabyPenguin::Behavior): looks
// up MOTHER_PENGUIN (id 257, hosted since gate 191) by actor id when in
// range and on-ground; null-guarded at the call site -- CLEAN even
// before 191, MOOT now that 191 hosts the mother. Documented here even
// though BABY_PENGUIN is unregistered this gate, since the fill and the
// PMF work are complete and this finding stands on its own.
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
#include <cstdlib>

#include "Actor.h"
#include "dtor_faces_cpp.h"
#include "ActorBase.h"
#include "BabyPenguin.h"
#include "SnowmanBody.h"
#include "SnowmanHead.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);
int _ZN5Actor14BeforeBehaviorEv(void *self);
int _ZN5Actor12BeforeRenderEv(void *self);
int _ZN5Actor13OnYoshiTryEatEv(void *self);
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);
int _ZN5Actor9Virtual50Ev(void *self);
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);
void _ZN5Actor8OnKickedERS_(void *self, void *o);
void _ZN5Actor8OnPushedERS_(void *self, void *o);
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o);
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);
int _ZN5Actor16OnAimedAtWithEggEv(void *self);
void _ZN5ActorD2Ev(void *self);
extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);  /* hal/actor_registry */
void port_actor_slot_decline(const char *what);  /* func_02043fdc_hostcopy.cpp */
void _ZN9ModelAnimD1Ev(void *self);
void _ZN11ShadowModelD1Ev(void *self);
void _ZN18MovingCylinderClsnD1Ev(void *self);
void _ZN12WithMeshClsnD1Ev(void *self);
extern void *data_020a0eac;
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
void port_actor_render_probe(const char *cls, void *model);
}

// ---- the trap --------------------------------------------------------------
// Slots 13/14 are ActorBase::Virtual34/Virtual38 and slot 30 is
// Actor::OnAimedAtWithEggReturnVec. All three are named by the ROM relocs
// above, and none of the three can be serviced by a __fastcall(void*, void*)
// thunk: 13/14 take two u32 arguments each and their bodies (solid-heap
// allocator work) are not linked into the port at all, and 30 is an SRET
// method whose hidden return pointer no thunk shape here models. The
// ccm/ov064/jrb/bbh convention: trap them by name rather than seat a body
// with the wrong contract.
static void ov72_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: vtable slot %d is not hosted (actor id %u %s, "
                 "phase %d, spawn step %d)\n",
                 slot, id, port_actor_class_name(id), data_02099f24[0],
                 (int)data_020a4b4c);
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall ov72_trap13(void *s, void *) { ov72_trap_report(s, 13); return 0; }
static int __fastcall ov72_trap14(void *s, void *) { ov72_trap_report(s, 14); return 0; }

// ---- the shared 1..30 half, both 31-slot plain-Actor tables share it ----
static int __fastcall ov72_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov72_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov72_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov72_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov72_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov72_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov72_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov72_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov72_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov72_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov72_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov72_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov72_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov72_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov72_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov72_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov72_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov72_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov72_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov72_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov72_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Fills slots 1,2,4,5,7,8,10,11,13,14,15,18..30, every shared slot both
   31-slot plain-Actor tables share. The caller writes its own
   0/3/6/9/12/16/17 (and daBgSnwmn_c/BabyPenguin's slot 18/19 overrides
   afterward, where each has one).

   SLOT NUMBERING IS THE ROM'S, taken from the two reloc spans quoted in this
   file's header, not from the order the bodies happen to be declared in. The
   previous fill ran one slot low from 20 onward and had 13/14/15 rotated, so
   every seat from 13 up carried a body the ROM never put there. See the
   header for the fault list. */
static void ov72_fill_shared(void **vt)
{
    vt[1]  = (void *)ov72_binit;    /* Actor::BeforeInitResources    */
    vt[2]  = (void *)ov72_ainit;    /* Actor::AfterInitResources(u32) */
    vt[4]  = (void *)ov72_bclean;   /* Actor::BeforeCleanupResources */
    vt[5]  = (void *)ov72_aclean;   /* Actor::AfterCleanupResources(u32) */
    vt[7]  = (void *)ov72_bbeh;     /* Actor::BeforeBehavior */
    vt[8]  = (void *)ov72_abeh;     /* Actor::AfterBehavior(u32) */
    vt[10] = (void *)ov72_bren;     /* Actor::BeforeRender */
    vt[11] = (void *)ov72_aren;     /* Actor::AfterRender(u32) */
    vt[13] = (void *)ov72_trap13;   /* ActorBase::Virtual34(u32,u32), not linked */
    vt[14] = (void *)ov72_trap14;   /* ActorBase::Virtual38(u32,u32), not linked */
    vt[15] = (void *)ov72_heap;     /* ActorBase::OnHeapCreated */
    vt[18] = (void *)ov72_yoshi;    /* Actor::OnYoshiTryEat */
    vt[19] = (void *)ov72_egg;      /* Actor::OnTurnIntoEgg(Player&) */
    vt[20] = (void *)ov72_v50;      /* Actor::Virtual50 */
    vt[21] = (void *)ov72_pounded;  /* Actor::OnGroundPounded(Actor&) */
    vt[22] = (void *)ov72_atk1;     /* Actor::OnAttacked1(Actor&) */
    vt[23] = (void *)ov72_atk2;     /* Actor::OnAttacked2(Actor&) */
    vt[24] = (void *)ov72_kicked;   /* Actor::OnKicked(Actor&) */
    vt[25] = (void *)ov72_pushed;   /* Actor::OnPushed(Actor&) */
    vt[26] = (void *)ov72_cannon;   /* Actor::OnHitByCannonBlastedChar(Actor&) */
    vt[27] = (void *)ov72_mega;     /* Actor::OnHitByMegaChar(Player&) */
    vt[28] = (void *)ov72_under;    /* Actor::OnHitFromUnderneath(Actor&) */
    vt[29] = (void *)ov72_aimed;    /* Actor::OnAimedAtWithEgg */
    vt[30] = (void *)port_actor_s30_base;   /* Actor::OnAimedAtWithEggReturnVec, SRET */
}

// ============================================================================
// SNOWMAN (daBgSnwmn_c, 272), a plain Actor, 31 slots. Own overrides:
// 0 Init (HOST COPY, Prepare SHORT-1 fix), 3 Cleanup, 6 Behavior,
// 9 Render, 12 OnPendingDestroy (own empty body), 16 D1, 17 D0. No own
// slot 18+ override -- confirmed by the raw vtable dump (words 18..30 all
// match the shared arm9 defaults byte for byte).
// ============================================================================
extern "C" {
int func_ov072_02120a44(char *self);       /* slot 0, HOST COPY (Snowman_InitResources.cpp) */
int func_ov072_02120980(void);             /* slot 3, .c body takes void */
int func_ov072_02120a08(char *self);       /* slot 6 */
int func_ov072_021209c0(char *self);       /* slot 9 */
void func_ov072_021209bc(void);            /* slot 12, own, empty body */
int *func_ov072_02120824(int *self);       /* slot 16, D1 */
int *func_ov072_02120874(int *self);       /* slot 17, D0 */
void *func_ov072_02120c00(void);           /* installs the class's own (unnamed) vtable */
int _ZTV11daBgSnwmn_c[31];   /* vtspan: data_ov072_02122978 */                 /* the class's own vtable, host array -- ALL matched src spells it by this mangled name directly, no alias needed */
}

static int __fastcall snm_init(void *s, void *)
{ return func_ov072_02120a44((char *)s); }
static int __fastcall snm_clean(void *s, void *)
{ (void)s; return func_ov072_02120980(); }
static int __fastcall snm_behavior(void *s, void *)
{ return func_ov072_02120a08((char *)s); }
static int __fastcall snm_render(void *s, void *)
{ port_actor_render_probe("SNOWMAN", (char *)s + 0xd4);
  return func_ov072_021209c0((char *)s); }
static int __fastcall snm_pdes(void *s, void *)
{ (void)s; func_ov072_021209bc(); return 0; }
static int __fastcall snm_d1(void *s, void *)
{ return (int)(size_t)func_ov072_02120824((int *)s); }
static int __fastcall snm_d0(void *s, void *)
{ return (int)(size_t)func_ov072_02120874((int *)s); }

extern "C" void hal_fill_snowman_vtable(void)
{
    void **vt = (void **)_ZTV11daBgSnwmn_c;
    ov72_fill_shared(vt);
    vt[0]  = (void *)snm_init;
    vt[3]  = (void *)snm_clean;
    vt[6]  = (void *)snm_behavior;
    vt[9]  = (void *)snm_render;
    vt[12] = (void *)snm_pdes;
    vt[16] = (void *)snm_d1;
    vt[17] = (void *)snm_d0;
    /* no slot 31: a plain Actor, 31 slots total, ends here */
}

// ============================================================================
// BABY_PENGUIN (daPgBby_c, 256), a plain Actor, 31 slots
// (_ZTV11BabyPenguin == _ZTV9daPgBby_c). Own overrides: 0 Init, 3 Cleanup,
// 6 Behavior, 9 Render, 12 OnPendingDestroy (own, empty body --
// func_ov072_02121db0), 16 D1 (real MSVC-synthesised empty destructor, no
// vtable-store -- chain spelled directly, the MotherPenguin/OneUpLogo
// shape), 17 D0 (stores its own table, never overwrites -- stays in
// slice), 18 own OnYoshiTryEat (returns 7, NOT the shared default),
// 19 own OnTurnIntoEgg (a tail-call veneer straight to
// ActorBase::MarkForDestruction). Six-cell PMF state table seated below
// before InitResources can dispatch through it.
// ============================================================================
extern "C" {
int _ZN11BabyPenguin13InitResourcesEv(void *self);      /* slot 0, faced */
int _ZN11BabyPenguin16CleanupResourcesEv(void);         /* slot 3, .c body takes void */
int _ZN11BabyPenguin8BehaviorEv(void *self);             /* slot 6, faced */
int _ZN11BabyPenguin6RenderEv(void *self);                /* slot 9, faced */
void _ZN11BabyPenguin16OnPendingDestroyEv(void);        /* slot 12, own, .c body takes void */
int *_ZN11BabyPenguinD0Ev(int *self);                    /* slot 17, spells its own table */
int func_ov072_02120cfc(void);                            /* slot 18, own OnYoshiTryEat, returns 7 */
void func_ov072_02121fa0(void *self);                     /* slot 19, own OnTurnIntoEgg */
void *BabyPenguin_Spawn(void);                             /* installs _ZTV11BabyPenguin */
int _ZTV11BabyPenguin[31];

/* the six-cell PMF state table: each cell is TWO 8-byte {fn,delta} pairs
   back to back (the .a=enter half, .b=tick half __sinit_ov072_02122414's
   own struct shows), 0x10 bytes/cell -- data_ov072_02122d6c[6]. ROM-byte
   verified against overlay_0072.bin (see this file's own header), every
   delta 0. */
struct PortBabyPenguinPair { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
extern PortBabyPenguinPair data_ov072_02122d6c[6];

/* the twelve real state-handler bodies the six cells' enter/tick halves
   point at -- all already matched src in the slice (func_ov072_021217ac
   is the ONE exception: its host copy, BabyPenguin_LandTransition.cpp,
   supplies this exact symbol name instead, replacing the func_ov072_02121d50
   SHORT-1 caller -- same address, same signature, transparent to the seat
   below). All twelve take one pointer, return int -- the uniform shape
   every state handler in this table shares. */
int func_ov072_02121c94(char *p);
int func_ov072_02121a84(void *c);
int func_ov072_02121a28(char *c);
int func_ov072_021218dc(char *self);
int func_ov072_02121890(char *c);
int func_ov072_021217ac(void *thiz);
int func_ov072_02121758(char *c);
int func_ov072_02121670(char *c);
int func_ov072_02121640(char *c);
int func_ov072_021214dc(void *thiz);
int func_ov072_02121368(char *self);
int func_ov072_021212c0(void *thiz);
}

/* {ROM address the sinit's own source cell carries, verified against
   overlay_0072.bin before the rewrite, host body -- the SoundObject/Cap/
   MrBlizzard seat shape: a mount pointing at the wrong bytes aborts
   instead of silently calling into garbage}. Order matches
   __sinit_ov072_02122414's own field order (NOT sorted by address). */
typedef int (*PortBpFn)(void *);
static const struct { unsigned enter_rom, tick_rom; PortBpFn enter_host, tick_host; }
g_bp_cells[6] = {
    {0x02121c94, 0x02121a84, (PortBpFn)func_ov072_02121c94, (PortBpFn)func_ov072_02121a84},
    {0x02121a28, 0x021218dc, (PortBpFn)func_ov072_02121a28, (PortBpFn)func_ov072_021218dc},
    {0x02121890, 0x021217ac, (PortBpFn)func_ov072_02121890, (PortBpFn)func_ov072_021217ac},
    {0x02121758, 0x02121670, (PortBpFn)func_ov072_02121758, (PortBpFn)func_ov072_02121670},
    {0x02121640, 0x021214dc, (PortBpFn)func_ov072_02121640, (PortBpFn)func_ov072_021214dc},
    {0x02121368, 0x021212c0, (PortBpFn)func_ov072_02121368, (PortBpFn)func_ov072_021212c0},
};

/* Seat the six cells with HOST function addresses, the MrBlizzard shape:
   verify the sinit copied the ROM's own {address,0} pairs (WRONG BYTES
   aborts instead of silently calling into garbage), then overwrite
   enter_fn/tick_fn with the host bodies' own addresses -- BabyPenguin_
   StateEnter.cpp / BabyPenguin_StateTick.cpp dispatch through these same
   fields as plain function pointers, so they must hold host code, not
   the ROM's DS addresses. */
extern "C" void port_baby_penguin_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 6; ++i) {
        PortBabyPenguinPair &cell = data_ov072_02122d6c[i];
        if (cell.enter_fn != g_bp_cells[i].enter_rom || cell.enter_delta != 0 ||
            cell.tick_fn != g_bp_cells[i].tick_rom || cell.tick_delta != 0) {
            std::fprintf(stderr, "FATAL: BabyPenguin state cell %d: the sinit "
                         "left %08x/%u %08x/%u, the ROM's own records say "
                         "%08x/0 %08x/0 -- WRONG BYTES\n", i,
                         cell.enter_fn, cell.enter_delta, cell.tick_fn, cell.tick_delta,
                         g_bp_cells[i].enter_rom, g_bp_cells[i].tick_rom);
            std::abort();
        }
        cell.enter_fn = (unsigned)(size_t)g_bp_cells[i].enter_host;
        cell.tick_fn = (unsigned)(size_t)g_bp_cells[i].tick_host;
    }
}

static int __fastcall bp_init(void *s, void *)
{ return _ZN11BabyPenguin13InitResourcesEv(s); }
static int __fastcall bp_clean(void *s, void *)
{ (void)s; return _ZN11BabyPenguin16CleanupResourcesEv(); }
static int __fastcall bp_behavior(void *s, void *)
{ return _ZN11BabyPenguin8BehaviorEv(s); }
static int __fastcall bp_render(void *s, void *)
{ port_actor_render_probe("BABY_PENGUIN", (char *)s + 0xd4);
  return _ZN11BabyPenguin6RenderEv(s); }
static int __fastcall bp_pdes(void *s, void *)
{ (void)s; _ZN11BabyPenguin16OnPendingDestroyEv(); return 0; }
/* D1: a real MSVC-synthesised empty destructor over a LOCAL shadow class
   (no vtable-store write) -- the MotherPenguin/OneUpLogo shape, not
   compiled. Chain spelled directly, HIGH ADDRESS FIRST (WithMeshClsn
   +0x194, MovingCylinderClsn +0x160, ShadowModel +0x138, ModelAnim
   +0xd4), then Actor's own D2. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (bp_d1) spelled the same chain by hand. */
static int __fastcall bp_d0(void *s, void *)
{ return (int)(size_t)_ZN11BabyPenguinD0Ev((int *)s); }
static int __fastcall bp_yoshi(void *s, void *)
{ (void)s; return func_ov072_02120cfc(); }
/* Slot 19 is OnTurnIntoEgg(Player &player): the caller pushes the player, so
   the thunk needs the third parameter to pop it. The ov072 body takes
   nothing of its own, but it is the ROM's `ldr ip,[pc]; bx ip` veneer onto
   ActorBase::MarkForDestruction, and a veneer is a tail jump on the host
   too, so it forwards whatever this thunk pushed. Pushing nothing left
   MarkForDestruction reading this thunk's own return address as its self,
   marking a code address for destruction and leaving the penguin alive. */
static int __fastcall bp_egg(void *s, void *, void *)
{ func_ov072_02121fa0(s); return 0; }

extern "C" void hal_fill_baby_penguin_vtable(void)
{
    /* seat + verify the six PMF cells BEFORE InitResources can dispatch
       through them (func_ov072_02121d50/02121d18 dispatch the "enter"
       half from InitResources' own body). */
    port_baby_penguin_states_seat();
    void **vt = (void **)_ZTV11BabyPenguin;
    ov72_fill_shared(vt);
    vt[0]  = (void *)bp_init;
    vt[3]  = (void *)bp_clean;
    vt[6]  = (void *)bp_behavior;
    vt[9]  = (void *)bp_render;
    vt[12] = (void *)bp_pdes;
    vt[16] = (void *)hal_cppd1_BabyPenguin;
    vt[17] = (void *)bp_d0;
    vt[18] = (void *)bp_yoshi;   /* own OnYoshiTryEat, overrides the shared default */
    vt[19] = (void *)bp_egg;     /* own OnTurnIntoEgg, overrides the shared default */
    /* no slot 31: a plain Actor, 31 slots total, ends here */
}

// ---- method faces -----------------------------------------------------------
// _ZN11BabyPenguin13InitResourcesEv and _ZN11BabyPenguin8BehaviorEv are
// real MSVC methods against BabyPenguin.h (?InitResources@BabyPenguin@@...
// etc., not __ZN11BabyPenguin...) -- faced here, the IceSheet/OneUpLogo
// recipe. CleanupResources and OnPendingDestroy are plain C-linkage .c
// bodies taking (void) -- no face needed, declared extern "C" above and
// called directly.
// _ZN11BabyPenguin6RenderEv is NOT faced here: the matched Render TU
// dispatches ModelAnim slot 5 through a local six-virtual shadow (the ROM
// Render), which the host _ZTV9ModelAnim numbers as Virtual18. The TU is
// dropped from slice_gate193.txt and _ZN11BabyPenguin6RenderEv is the
// host copy in port/unmatched/ModelAnim_Renders.cpp, the Whomp/Butterfly/
// Fish/QuestionBlock case (see this file's own header).
extern "C" {
int _ZN11BabyPenguin13InitResourcesEv(void *self)
{ return ((BabyPenguin *)self)->BabyPenguin::InitResources(); }
int _ZN11BabyPenguin8BehaviorEv(void *self)
{ return ((BabyPenguin *)self)->BabyPenguin::Behavior(); }
}

// ============================================================================
// LANE w3-d: the two classes gate 193 left unregistered.
//
// SNOWMAN_HEAD (daBgSnmHed_c, 273) and SNOWMAN_BODY (daBgSnmBdy_c, 274) are
// both plain 31-slot Actors, and neither has an own slot 18/19 override
// (unlike BabyPenguin) -- every slot outside the seven own ones is the shared
// arm9 default, byte for byte. So ov72_fill_shared is reused VERBATIM here
// rather than re-derived, and that reuse is legitimate for the same reason it
// was legitimate between daBgSnwmn_c and BabyPenguin: both tables were read
// out of extracted/overlays/overlay_0072.bin and compared slot for slot
// against the two this file already served, and all four agree on every
// shared slot.
//
//   slot  SnowmanBody 0x021227c0   SnowmanHead 0x021228bc   what the ROM names
//    0    0x0211fd54 (own)          0x0212069c (own)         InitResources
//    3    0x0211fccc (own)          0x021205f0 (own)         CleanupResources
//    6    0x0211fd24 (own)          0x0212066c (own)         Behavior
//    9    0x0211fcf4 (own)          0x02120638 (own)         Render
//   12    0x0211fcf0 (own)          0x02120634 (own)         OnPendingDestroy
//   16    0x0211f000 (own)          0x0211ff34 (own)         D1
//   17    0x0211f048 (own)          0x0211ff7c (own)         D0
//   18    0x02010160                0x02010160               Actor::OnYoshiTryEat
//   19    0x02010154                0x02010154               Actor::OnTurnIntoEgg
// (1/2/4/5/7/8/10/11/13/14/15/20..30 identical to the two tables above.)
//
// IDENTITY, BOTH ROUTES, for each class:
//   vtable[-1] -> typeinfo -> name string: 0x021227c0[-1] = 0x02122780 ->
//     "12daBgSnmBdy_c";  0x021228bc[-1] = 0x0212287c -> "12daBgSnmHed_c".
//   arm9 spawn table 0x02090864 + id*4: 0x02090cac -> SpawnInfo 0x0212279c
//     (word[1] 0x00850112, low halfword 0x112 = 274);  0x02090ca8 ->
//     SpawnInfo 0x02122898 (word[1] 0x00840111, low halfword 0x111 = 273).
//   The two routes agree for both classes.
//
// BOTH RENDERS ARE MATCHED SRC, NOT HOST COPIES, and that is the one
// judgement in this section that could have gone the other way. Each
// dispatches slot 5 through a local six-virtual ROM-order shadow -- the idiom
// that made _ZN11BabyPenguin6RenderEv a host copy and produced the DEP
// execute-violation this file's header dissects. The RECEIVER decides it, and
// here the receiver is a plain Model, not a ModelAnim: both D1 bodies name
// the member type outright (Model m0 at 0xd4, over struct Model with a 0x50
// pad), both D0 bodies call _ZN5ModelD1Ev on this+0xd4, and both generated
// headers put a 0x50-stride member there (SnowmanBody 0xd4 Model -> 0x124
// ShadowModel; SnowmanHead 0xd4 Model -> 0x124 TextureSequence). _ZTV5Model[5]
// is dual-filled, so the shadow call lands correctly. This is daBgSnwmn_c's
// case -- whose Render is in gate 193's own slice -- not BabyPenguin's.
//
// THE FOUR PMF DISPATCHERS ARE HOST COPIES
// (port/unmatched/Snowman_StateDispatch.cpp) and the STATE CELLS ARE SEATED
// WITH HOST ADDRESSES below. Both sinits copy {DS function address, 0} pairs
// out of mounted ROM data into bss; run raw, a dispatch through them jumps
// into unmapped host memory. See slice_gate193.txt and port/ov072_syms.txt.
// ============================================================================

extern "C" {
/* member destructors the two D1 chains need and this file did not yet name */
void _ZN5ModelD1Ev(void *self);
void _ZN15TextureSequenceD1Ev(void *self);

/* ---- SNOWMAN_BODY (274) own slots ---- */
int _ZN11SnowmanBody13InitResourcesEv(void *self);    /* slot 0, faced below */
int _ZN11SnowmanBody16CleanupResourcesEv(void);       /* slot 3, .c body takes void */
int _ZN11SnowmanBody8BehaviorEv(void *self);          /* slot 6, faced below */
int _ZN11SnowmanBody6RenderEv(void *self);            /* slot 9, faced below */
void _ZN11SnowmanBody16OnPendingDestroyEv(void);      /* slot 12, own empty body */
int *_ZN11SnowmanBodyD0Ev(int *self);                 /* slot 17, stores its own table */
void *SnowmanBody_Spawn(void);
extern unsigned char SnowmanBody_SpawnInfo[];         /* ov072 0x0212279c */
int _ZTV11SnowmanBody[31];   /* vtspan: 0x021227c0, 31 words to 0x0212283c */

/* ---- SNOWMAN_HEAD (273) own slots ---- */
int _ZN11SnowmanHead13InitResourcesEv(void *self);    /* slot 0, faced below */
int _ZN11SnowmanHead16CleanupResourcesEv(void);       /* slot 3, .c body takes void */
int _ZN11SnowmanHead8BehaviorEv(void *self);          /* slot 6, faced below */
int _ZN11SnowmanHead6RenderEv(void *self);            /* slot 9, faced below */
void _ZN11SnowmanHead16OnPendingDestroyEv(void);      /* slot 12, own empty body */
int *_ZN11SnowmanHeadD0Ev(int *self);                 /* slot 17, stores its own table */
void *SnowmanHead_Spawn(void);
extern unsigned char SnowmanHead_SpawnInfo[];         /* ov072 0x02122898 */
int _ZTV11SnowmanHead[31];   /* vtspan: 0x021228bc, 31 words to 0x02122938 */

/* the state cells both sinits build: TWO 8-byte {fn,delta} pairs per cell,
   0x10 bytes/cell -- the same unit PortBabyPenguinPair is above. */
struct PortSnowmanCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
extern PortSnowmanCell data_ov072_02122b64[6];   /* SnowmanBody, 6 cells */
extern PortSnowmanCell data_ov072_02122c00[4];   /* SnowmanHead, 4 cells */

/* SnowmanBody's twelve state handlers (six enter/tick pairs) */
int func_ov072_0211fb7c(char *c);
int func_ov072_0211fb14(void *self);
int func_ov072_0211faf0(char *c);
int func_ov072_0211fa08(char *c);
int func_ov072_0211f9c4(char *c);
int func_ov072_0211f81c(char *c);
int func_ov072_0211f804(char *p);
int func_ov072_0211f65c(unsigned char *thiz);
int func_ov072_0211f63c(char *c);
int func_ov072_0211f598(char *c);
int func_ov072_0211f578(char *c);
int func_ov072_0211f48c(char *c);

/* SnowmanHead's eight state handlers (four enter/tick pairs) */
int func_ov072_02120514(char *c);
bool func_ov072_02120450(void *self);
int func_ov072_02120430(char *p);
int func_ov072_02120358(void *thiz);
int func_ov072_02120308(char *base);
int func_ov072_021201d4(char *self);
int func_ov072_02120180(char *c);
int func_ov072_0212001c(char *c);
}

/* One seat row: the ROM addresses the sinit's own source pairs carry, and the
   host bodies that replace them. The order is the CELL order, which is NOT
   address order and NOT the source table's own order:
   __sinit_ov072_02122018 assigns data_ov072_02122b64.p0..p11 from the twelve
   source pairs at 0x02122720..0x02122778 in a scrambled order, and two
   consecutive p's make one cell. Every enter_rom/tick_rom below was read out
   of extracted/overlays/overlay_0072.bin AT THE SOURCE PAIR THE SINIT NAMES
   for that p, so a mount pointing at the wrong bytes aborts here instead of
   calling into garbage -- the MrBlizzard/BabyPenguin seat shape. */
typedef int (*PortSnFn)(void *);
struct PortSnRow { unsigned enter_rom, tick_rom; PortSnFn enter_host, tick_host; };

static const PortSnRow g_smb_cells[6] = {
    /* cell 0: p0  <- 0x02122750, p1  <- 0x02122728 */
    {0x0211fb7c, 0x0211fb14, (PortSnFn)func_ov072_0211fb7c, (PortSnFn)func_ov072_0211fb14},
    /* cell 1: p2  <- 0x02122720, p3  <- 0x02122730 */
    {0x0211faf0, 0x0211fa08, (PortSnFn)func_ov072_0211faf0, (PortSnFn)func_ov072_0211fa08},
    /* cell 2: p4  <- 0x02122748, p5  <- 0x02122738 */
    {0x0211f9c4, 0x0211f81c, (PortSnFn)func_ov072_0211f9c4, (PortSnFn)func_ov072_0211f81c},
    /* cell 3: p6  <- 0x02122740, p7  <- 0x02122778 */
    {0x0211f804, 0x0211f65c, (PortSnFn)func_ov072_0211f804, (PortSnFn)func_ov072_0211f65c},
    /* cell 4: p8  <- 0x02122770, p9  <- 0x02122758 */
    {0x0211f63c, 0x0211f598, (PortSnFn)func_ov072_0211f63c, (PortSnFn)func_ov072_0211f598},
    /* cell 5: p10 <- 0x02122768, p11 <- 0x02122760 */
    {0x0211f578, 0x0211f48c, (PortSnFn)func_ov072_0211f578, (PortSnFn)func_ov072_0211f48c},
};

/* SnowmanHead's four, from __sinit_ov072_021221f8's own assignment order into
   data_ov072_02122c00[0..7]. */
static const PortSnRow g_smh_cells[4] = {
    /* cell 0: [0] <- 0x02122874, [1] <- 0x02122854 */
    {0x02120514, 0x02120450, (PortSnFn)func_ov072_02120514, (PortSnFn)func_ov072_02120450},
    /* cell 1: [2] <- 0x0212286c, [3] <- 0x02122864 */
    {0x02120430, 0x02120358, (PortSnFn)func_ov072_02120430, (PortSnFn)func_ov072_02120358},
    /* cell 2: [4] <- 0x0212285c, [5] <- 0x02122844 */
    {0x02120308, 0x021201d4, (PortSnFn)func_ov072_02120308, (PortSnFn)func_ov072_021201d4},
    /* cell 3: [6] <- 0x0212284c, [7] <- 0x0212283c */
    {0x02120180, 0x0212001c, (PortSnFn)func_ov072_02120180, (PortSnFn)func_ov072_0212001c},
};

static void ov72_seat_cells(const char *who, PortSnowmanCell *cells,
                            const PortSnRow *rows, int n)
{
    for (int i = 0; i < n; ++i) {
        PortSnowmanCell &cell = cells[i];
        if (cell.enter_fn != rows[i].enter_rom || cell.enter_delta != 0 ||
            cell.tick_fn != rows[i].tick_rom || cell.tick_delta != 0) {
            std::fprintf(stderr, "FATAL: %s state cell %d: the sinit left "
                         "%08x/%u %08x/%u, the ROM's own records say "
                         "%08x/0 %08x/0 -- WRONG BYTES\n", who, i,
                         cell.enter_fn, cell.enter_delta,
                         cell.tick_fn, cell.tick_delta,
                         rows[i].enter_rom, rows[i].tick_rom);
            std::abort();
        }
        cell.enter_fn = (unsigned)(size_t)rows[i].enter_host;
        cell.tick_fn = (unsigned)(size_t)rows[i].tick_host;
    }
}

extern "C" void port_snowman_body_states_seat(void)
{
    static int done;
    if (done) return;
    done = 1;
    ov72_seat_cells("SnowmanBody", data_ov072_02122b64, g_smb_cells, 6);
}

extern "C" void port_snowman_head_states_seat(void)
{
    static int done;
    if (done) return;
    done = 1;
    ov72_seat_cells("SnowmanHead", data_ov072_02122c00, g_smh_cells, 4);
}

// ---- SNOWMAN_BODY (274) ----------------------------------------------------
static int __fastcall smb_init(void *s, void *)
{ return _ZN11SnowmanBody13InitResourcesEv(s); }
static int __fastcall smb_clean(void *s, void *)
{ (void)s; return _ZN11SnowmanBody16CleanupResourcesEv(); }
static int __fastcall smb_behavior(void *s, void *)
{ return _ZN11SnowmanBody8BehaviorEv(s); }
static int __fastcall smb_render(void *s, void *)
{ port_actor_render_probe("SNOWMAN_BODY", (char *)s + 0xd4);
  return _ZN11SnowmanBody6RenderEv(s); }
static int __fastcall smb_pdes(void *s, void *)
{ (void)s; _ZN11SnowmanBody16OnPendingDestroyEv(); return 0; }
/* D1: a real MSVC-synthesised destructor over LOCAL shadow classes (Model /
   ShadowModel / MovingCylinderClsn / WithMeshClsn declared with no bodies of
   their own), so the matched TU emits ??1Model@@QAE@XZ and friends nothing in
   the link defines -- the MotherPenguin/OneUpLogo/BabyPenguin shape, not
   compiled. Transcribed from the ROM listing at 0x0211f000:
     push {r4,lr} / mov r4,r0 / ldr r1,[pc,#0x34] (= 0x021227c0) /
     add r0,r4,#0x180 / str r1,[r4] / bl WithMeshClsn::~ /
     add r0,r4,#0x14c / bl MovingCylinderClsn::~ /
     add r0,r4,#0x124 / bl ShadowModel::~ /
     add r0,r4,#0xd4  / bl Model::~ / mov r0,r4 / bl Actor::~Actor /
     mov r0,r4 / pop {r4,lr} / bx lr
   UNLIKE bp_d1 this one DOES store its own table first, exactly as the ROM
   does; over the host array that is a no-op re-store. The same member set,
   order and offsets appear independently in _ZN11SnowmanBodyD0Ev.c. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (smb_d1) spelled the same chain by hand. */
static int __fastcall smb_d0(void *s, void *)
{ return (int)(size_t)_ZN11SnowmanBodyD0Ev((int *)s); }

extern "C" void hal_fill_snowman_body_vtable(void)
{
    /* seat the six cells BEFORE anything can dispatch through them */
    port_snowman_body_states_seat();
    void **vt = (void **)_ZTV11SnowmanBody;
    ov72_fill_shared(vt);
    vt[0]  = (void *)smb_init;
    vt[3]  = (void *)smb_clean;
    vt[6]  = (void *)smb_behavior;
    vt[9]  = (void *)smb_render;
    vt[12] = (void *)smb_pdes;
    vt[16] = (void *)hal_cppd1_SnowmanBody;
    vt[17] = (void *)smb_d0;
    /* no own 18/19 and no slot 31: a plain Actor, 31 slots, ends here */
}

// ---- SNOWMAN_HEAD (273) ----------------------------------------------------
static int __fastcall smh_init(void *s, void *)
{ return _ZN11SnowmanHead13InitResourcesEv(s); }
static int __fastcall smh_clean(void *s, void *)
{ (void)s; return _ZN11SnowmanHead16CleanupResourcesEv(); }
static int __fastcall smh_behavior(void *s, void *)
{ return _ZN11SnowmanHead8BehaviorEv(s); }
static int __fastcall smh_render(void *s, void *)
{ port_actor_render_probe("SNOWMAN_HEAD", (char *)s + 0xd4);
  return _ZN11SnowmanHead6RenderEv(s); }
static int __fastcall smh_pdes(void *s, void *)
{ (void)s; _ZN11SnowmanHead16OnPendingDestroyEv(); return 0; }
/* D1: same shape as smb_d1, transcribed from the ROM listing at 0x0211ff34
   (literal 0x021228bc). The member set differs -- TextureSequence at +0x124
   where SnowmanBody has a ShadowModel, and the two clsn members sit lower,
   +0x138 and +0x16c -- and _ZN11SnowmanHeadD0Ev.c carries the identical
   order and offsets. */
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (smh_d1) spelled the same chain by hand. */
static int __fastcall smh_d0(void *s, void *)
{ return (int)(size_t)_ZN11SnowmanHeadD0Ev((int *)s); }

extern "C" void hal_fill_snowman_head_vtable(void)
{
    port_snowman_head_states_seat();
    void **vt = (void **)_ZTV11SnowmanHead;
    ov72_fill_shared(vt);
    vt[0]  = (void *)smh_init;
    vt[3]  = (void *)smh_clean;
    vt[6]  = (void *)smh_behavior;
    vt[9]  = (void *)smh_render;
    vt[12] = (void *)smh_pdes;
    vt[16] = (void *)hal_cppd1_SnowmanHead;
    vt[17] = (void *)smh_d0;
    /* no own 18/19 and no slot 31: a plain Actor, 31 slots, ends here */
}

// ---- method faces for the two new classes -----------------------------------
// InitResources, Behavior and Render are real MSVC methods against
// SnowmanBody.h / SnowmanHead.h, so each needs the C-named face the ROM's own
// callers and tables use -- the IceSheet/OneUpLogo/BabyPenguin recipe.
// CleanupResources and OnPendingDestroy are plain C-linkage .c bodies taking
// (void), declared extern "C" above and called directly, no face.
// Render IS faced here, unlike BabyPenguin's: its receiver is a plain Model
// and _ZTV5Model[5]'s dual fill serves the shadow dispatch (see the section
// header for the derivation that settled it).
extern "C" {
int _ZN11SnowmanBody13InitResourcesEv(void *self)
{ return ((SnowmanBody *)self)->SnowmanBody::InitResources(); }
int _ZN11SnowmanBody8BehaviorEv(void *self)
{ return ((SnowmanBody *)self)->SnowmanBody::Behavior(); }
int _ZN11SnowmanBody6RenderEv(void *self)
{ return ((SnowmanBody *)self)->SnowmanBody::Render(); }
int _ZN11SnowmanHead13InitResourcesEv(void *self)
{ return ((SnowmanHead *)self)->SnowmanHead::InitResources(); }
int _ZN11SnowmanHead8BehaviorEv(void *self)
{ return ((SnowmanHead *)self)->SnowmanHead::Behavior(); }
int _ZN11SnowmanHead6RenderEv(void *self)
{ return ((SnowmanHead *)self)->SnowmanHead::Render(); }
}
