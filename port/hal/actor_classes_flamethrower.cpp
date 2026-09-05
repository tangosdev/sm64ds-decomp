// RUN linkw WAVE 6 (lane w6-f): FLAMETHROWER (318, ov095), RTTI
// daObjFlamethrower_c -- the wall-mounted jet of fire. Its three skips were the
// last non-143/147 residue on level 37 and it also skips on 13, 14 and 15, so
// it is the only class whose row clears instances on four mounted levels at
// once. Lane w5-C found the row BLOCKED (status/w5c.md) because the Behavior
// has no matched TU; this gate unblocks it with a host copy of that one method
// (port/unmatched/Flamethrower_Behavior.c) and slices the other four.
//
// Same law as hal/actor_classes_flame.cpp (gate 175, the structural twin: an
// Actor subclass with no overlay-side base machinery) -- MSVC slot order,
// __fastcall thunks, unhosted slots trap by name -- and THIRTY-ONE SLOTS, the
// plain Actor shape.
//
// THE SLOT MAP, re-derived for this lane rather than carried. Every word below
// was read out of extracted/overlays/overlay_0095.bin at _ZTV12Flamethrower
// (0x021376f0, file offset addr - 0x02135700) with
// config/arm9/overlays/ov095/relocs.txt applied, and every arm9 target was
// resolved by ADDRESS in config/arm9/symbols.txt (names lie in this port,
// addresses do not):
//
//   WIDTH = 31, pinned three independent ways. (1) The reloc run from
//   0x021376f0 is dense to 0x02137768 and STOPS there -- slot 30 is the last
//   relocated word. (2) The five words 0x0213776c..0x0213777c read as zero in
//   the raw image and 0x02137780 is the end of ov095's code+data section
//   (overlays.yaml: base 0x02135700 + code_size 0x2080), so there is no slot 31
//   to find. (3) _ZTV13UpDownLiftBbh in the same overlay has slot 30 at the
//   same 0x020100dc and then a REAL slot 31 (Platform::Kill 0x020ee55c),
//   because it is a Platform; Flamethrower derives straight from Actor and ends
//   one slot earlier. The RTTI agrees: vtable[-1] at 0x021376ec relocates to
//   0x021376a8 = { 0x0209a764 (__si_class_type_info), 0x021376b4
//   "19daObjFlamethrower_c", 0x0208e390 } -- a SINGLE-inheritance record, one
//   base, not the Platform chain.
//
//     slot  ROM word     owner
//        0  0x02136d60   Flamethrower::InitResources   OWN, matched src
//        1  0x02011268   Actor::BeforeInitResources
//        2  0x02011244   Actor::AfterInitResources
//        3  0x02043bf0   ActorBase::CleanupResources   base body, not overridden
//        4  0x02011220   Actor::BeforeCleanupResources
//        5  0x02011214   Actor::AfterCleanupResources
//        6  0x021368f0   Flamethrower::Behavior        OWN, HOST COPY
//        7  0x02010fd4   Actor::BeforeBehavior
//        8  0x02010fc8   Actor::AfterBehavior
//        9  0x02043af0   ActorBase::Render             base body, not overridden
//       10  0x02010f78   Actor::BeforeRender
//       11  0x02010f6c   Actor::AfterRender
//       12  0x02043ac0   ActorBase::OnPendingDestroy
//       13  0x0204357c   ActorBase::Virtual34          not linked -> trap
//       14  0x0204349c   ActorBase::Virtual38          not linked -> trap
//       15  0x02043494   ActorBase::OnHeapCreated
//       16  0x0213682c   Flamethrower::D1              OWN, matched src
//       17  0x02136884   Flamethrower::D0              OWN, matched src
//       18  0x02010160   Actor::OnYoshiTryEat          \
//       19  0x02010154   Actor::OnTurnIntoEgg          |
//       20  0x0201014c   Actor::Virtual50              |
//       21  0x02010148   Actor::OnGroundPounded        |
//       22  0x02010144   Actor::OnAttacked1            |  the interaction tail,
//       23  0x02010140   Actor::OnAttacked2            |  every one the ROM's
//       24  0x0201013c   Actor::OnKicked               |  own base body and
//       25  0x02010138   Actor::OnPushed               |  every one already
//       26  0x02010134   Actor::OnHitByCannonBlastedChar  linked, so BOUND
//       27  0x02010130   Actor::OnHitByMegaChar        |  rather than trapped
//       28  0x0201012c   Actor::OnHitFromUnderneath    |
//       29  0x02010124   Actor::OnAimedAtWithEgg       /
//       30  0x020100dc   Actor::OnAimedAtWithEggReturnVec  SRET -> trap
//
// The class overrides FOUR slots (0/6/16/17) and nothing else -- it does not
// override Render, which is the giveaway that the flame is drawn entirely by
// the particles its Behavior emits, the same reading gate 175 wrote down for
// daObjFire_c.
//
// THE MOUNT: ov095 is a per-symbol mount (gate 83 for SEESAW_BOB, extended by
// gate 173 and by w5-A). This gate adds Flamethrower_SpawnInfo (pinned 0x1c,
// the measured record stride -- see ov095_syms.txt) and the three tables the
// class indexes by segment: data_ov095_02136fb0 (12 x s32, InitResources and
// Behavior both), data_ov095_02136f80 and data_ov095_02136f98 (12 x s16, the
// particle arguments). All three are plain constants with no relocations, so
// they carry no pointer-rebase burden. The vtable stays OUT of the mount, the
// ov085/ov080/ov079 rule the ov095 header already states -- it is the host
// array defined below and the registry fills it.
//
// NO ALIAS IS ADDED BY THIS GATE. decl_common.h line 486 declares the table
// once, `extern int _ZTV12Flamethrower[]` at C linkage, and all three matched
// TUs that store it (Spawn, D1, D0) spell that same name -- unlike daUdlift_c
// or daObjFire_c there is no second RTTI spelling to launder. The two ov095
// tables InitResources reads are likewise declared inside decl_common.h's
// extern "C" block, so no C++ data spelling is emitted for them either.
//
// THE ID, cross-checked the way the flame gate did it: ACTOR_SPAWN_TABLE entry
// 0x02090d5c ((0x02090d5c - 0x02090864) / 4 = 318) relocates to 0x021376cc, and
// that record's raw bytes in overlay_0095.bin read
//     d4 6e 13 02  3e 01  3c 01  02 00 00 00 ...
// = spawn func 0x02136ed4 (Flamethrower_Spawn), +4 halfword 0x013e = 318, +6
// halfword 0x013c = 316. The record is `ambiguous` in config because ov006
// spans the same window; the raw ov095 image settles it, and the registry
// re-checks the +4 halfword at boot.
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
#include "ActorBase.h"
#include "Flamethrower.h"

extern "C" {
/* the shared lifecycle halves, the same functions every fill writes */
int _ZN5Actor19BeforeInitResourcesEv(void *self);            /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                 /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                   /* slot 10 */
/* the Actor interaction tail, every one the ROM's own base body and every one
   already linked -- bound rather than trapped so a Yoshi lick, a ground pound
   or a cannon shot at the flamethrower runs the ROM's default */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                  /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p); /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                       /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);    /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);        /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);        /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);            /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);            /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);               /* slot 29 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */

/* Flamethrower's own bodies. InitResources is a real C++ method against
   include/Flamethrower.h (matched src, slice_w6f.txt) and is called qualified
   by its thunk; both destructors are the explicit-chain extern-"C" .cpp forms,
   so they are C names here. Behavior is the C-linkage HOST COPY
   (port/unmatched/Flamethrower_Behavior.c). It is NOT the one method of the
   class with no matched TU -- main matched it in PR #1474 and this branch is
   626 commits behind. It is the one method whose matched TU cannot be linked
   HERE: it needs main's include/Flamethrower.h + decl_common.h to compile
   (measured, cl.exe C2039) and a face to carry the C name into an MSVC-mangled
   C++ method. Run linkw wave 9, lane w9-harvest; the reading is in the host
   file's banner. */
int _ZN12Flamethrower8BehaviorEv(void *self);    /* slot 6, host copy */
int _ZN12FlamethrowerD1Ev(char *self);           /* slot 16, matched .cpp */
void *_ZN12FlamethrowerD0Ev(char *self);         /* slot 17, matched .cpp */
void *Flamethrower_Spawn(void);                  /* the ROM factory */

/* The array Flamethrower_Spawn installs (`*(void***)p = _ZTV12Flamethrower`)
   and both destructors store back; thirty-one slots, the ROM width derived in
   the banner. Defined here, not just declared: the `int` type and C linkage
   match the `extern int _ZTV12Flamethrower[]` in decl_common.h that all three
   matched TUs reference. */
int _ZTV12Flamethrower[31];
}

// ---- the trap --------------------------------------------------------------
static void ft_trap_report(void *self, int slot)
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
#define FT_TRAP(n) \
    static int __fastcall ft_trap##n(void *s, void *) \
    { ft_trap_report(s, n); return 0; }
/* 13/14 are ActorBase::Virtual34/38 (not linked -- their closure is the
   SolidHeap subsystem, the actor_classes_bob_world reading -- and no clean run
   of a flamethrower dispatches them); 30 is the SRET
   OnAimedAtWithEggReturnVec no thunk shape models. */
FT_TRAP(13) FT_TRAP(14)
#undef FT_TRAP

// ---- the shared half -------------------------------------------------------
static int __fastcall ft_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ft_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ft_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ft_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ft_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ft_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ft_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ft_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ft_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ft_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
/* slots 3 and 9 are ActorBase's own bodies in the ROM table -- the class
   overrides neither, the gate-175 shape */
static int __fastcall ft_clean_base(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::CleanupResources(); }
static int __fastcall ft_render_base(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }

// ---- the Actor interaction tail, the ROM's own base bodies -----------------
static int __fastcall ft_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
/* slot 19's second argument arrives on the stack, the cap_egg reading */
static int __fastcall ft_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ft_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ft_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ft_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ft_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ft_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ft_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ft_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ft_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ft_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ft_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

// ---- FLAMETHROWER's own four slots -----------------------------------------
static int __fastcall ft_init(void *s, void *)
{ return ((Flamethrower *)s)->Flamethrower::InitResources(); }
static int __fastcall ft_behavior(void *s, void *)
{ return _ZN12Flamethrower8BehaviorEv(s); }
static int __fastcall ft_d1(void *s, void *)
{ return _ZN12FlamethrowerD1Ev((char *)s); }
static int __fastcall ft_d0(void *s, void *)
{ return (int)(size_t)_ZN12FlamethrowerD0Ev((char *)s); }

extern "C" void hal_fill_flamethrower_vtable(void)
{
    void **vt = (void **)_ZTV12Flamethrower;
    vt[0]  = (void *)ft_init;
    vt[1]  = (void *)ft_binit;
    vt[2]  = (void *)ft_ainit;
    vt[3]  = (void *)ft_clean_base;
    vt[4]  = (void *)ft_bclean;
    vt[5]  = (void *)ft_aclean;
    vt[6]  = (void *)ft_behavior;
    vt[7]  = (void *)ft_bbeh;
    vt[8]  = (void *)ft_abeh;
    vt[9]  = (void *)ft_render_base;
    vt[10] = (void *)ft_bren;
    vt[11] = (void *)ft_aren;
    vt[12] = (void *)ft_pdes;
    vt[13] = (void *)ft_trap13;
    vt[14] = (void *)ft_trap14;
    vt[15] = (void *)ft_heap;
    vt[16] = (void *)ft_d1;
    vt[17] = (void *)ft_d0;
    vt[18] = (void *)ft_yoshi;
    vt[19] = (void *)ft_egg;
    vt[20] = (void *)ft_v50;
    vt[21] = (void *)ft_pounded;
    vt[22] = (void *)ft_atk1;
    vt[23] = (void *)ft_atk2;
    vt[24] = (void *)ft_kicked;
    vt[25] = (void *)ft_pushed;
    vt[26] = (void *)ft_cannon;
    vt[27] = (void *)ft_mega;
    vt[28] = (void *)ft_under;
    vt[29] = (void *)ft_aimed;
    vt[30] = (void *)port_actor_s30_base;
}
