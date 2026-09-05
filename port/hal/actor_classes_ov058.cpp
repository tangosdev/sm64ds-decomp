// RUN REL0215 WAVE 1 (lane cast-sweep1): OV058'S ONE CLASS (level 50, the Rec
// Room). Overlay 6 of 6 in this lane, and the smallest seat in it.
//
// Level 50 spawns 26 actors and skips exactly ONE, and it is this class. That
// makes ov058 the second overlay in this lane whose seat takes its level to
// zero skipped, after ov023 and level 15.
//
//   id   name               x on L50  factory                table       width
//   182  REC_ROOM_CUPBOARD   1        RecRoomCupboard_Spawn  0x02111a70   31
//
// A 0x21c-byte ACTOR -- not a Platform -- holding an ARRAY of five
// MovingCylinderClsnWithPos at +0xd4, stride 0x40, that its factory builds
// with func_020733a8 and both destructors tear down with __destroy_arr. It
// loads no file at all: ov058's .rodata, .init, .ctor and .bss are ALL EMPTY,
// so the overlay has zero static initialisers and this file's bring-up runs
// the pack check and the syms patch and nothing else. InitResources is a
// five-iteration loop sizing the cylinders; Behavior is the Rec Room's
// conversation and the minigame menu's entry point.
//
// ---- THE TABLE IS 31 SLOTS AND THE LANDING READS TWO ----------------------
//
// Slot 31 holds a literal zero, not a relocation, and slot 30 is
// Actor::OnAimedAtWithEggReturnVec -- the plain Actor close, not the 32-slot
// Platform one. Same shape as ov024's PYRAMID_TAG, and the second 31-slot
// table in this lane against five 32-slot ones; reading this width off any of
// them would write one word past the array. dsd's next-symbol landing is
// 0x02111a78, INTERIOR at slot 2, so it reads TWO -- and 0x02111abc (19) and
// 0x02111ad4 (25) are interior splits too. A two-slot host array would have
// left everything from CleanupResources onward off the end of storage.
//
// Slot 9 is _ZN9ActorBase6RenderEv, the BASE body: this class has no Render of
// its own, so the fill writes 0/3/6/16/17 and takes 9 from the base.
//
// ---- T1: THE TABLE STAYS ROM-SHAPED ---------------------------------------
//
// RecRoomCupboard derives from Actor and holds no Model or ModelAnim at all,
// so the default applies: [16] D1, [17] D0, ROM order.
//
// ---- NOTHING NEEDED RENAMING ----------------------------------------------
//
// The comment-stripped sweep for G0..G3 / VT / VT0..VT3 / HEAP / R0..R3 over
// all six TUs comes back empty, no TU spells a sibling overlay's name for an
// ov058 address, and dsd names the vtable once on the table its own factory
// installs. After ov017's three live races, ov023's three, ov033's four and
// ov035's two, this is the overlay that needed no per-source -D.
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
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
/* the arm9 shared half */
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

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */

/* the generated ov058 per-symbol mount (build/port/host-src/ov058_syms.c).
   ONE symbol: the SpawnInfo record. There is no sinit to run -- ov058 has no
   .init section at all -- and no hal_fill_platform_vtable either, because this
   class is Actor-derived and its destructors never install Platform's base
   vptr. */
void port_ov058_pack_check(void);
void port_ov058_syms_patch(void);

/* the class's own bodies (port/slice_sweep1_ov058.txt) */
int _ZN15RecRoomCupboard13InitResourcesEv(char *self);  /* slot 0  */
int _ZN15RecRoomCupboard16CleanupResourcesEv(void);     /* slot 3, `return 1` */
int _ZN15RecRoomCupboard8BehaviorEv(char *self);        /* slot 6  */
int _ZN15RecRoomCupboardD1Ev(char *self);               /* slot 16 */
void *_ZN15RecRoomCupboardD0Ev(char *self);             /* slot 17 */
void *RecRoomCupboard_Spawn(void);                      /* id 182 */

/* the host vtable, excluded from the mount. THIRTY-ONE, not thirty-two. */
DSSTATE_BEGIN
void *_ZTV15RecRoomCupboard[31];   /* 0x02111a70 */
DSSTATE_END
}

/* ---- THE MEASURED EXTERNAL GAP -----------------------------------------
   Five unresolved externals off the FIRST link of this slice, read out of
   the linker's own LNK2019/LNK2001 list rather than predicted. FOUR are the
   ov025 C++-MANGLED SPELLING class: both destructor TUs are //cpp files that
   declare their globals at file scope with no extern "C", so MSVC decorates
   each with its declared type while the mount, romdata and this file's own
   host array emit the one C name. Every LHS is declared and never defined,
   so alternatename_guard stays clean.

   The FIFTH, ??1Actor@@QAE@XZ, is not a spelling problem and is not aliased
   here: it is a __thiscall receiver against a cdecl body, and it is defined
   by port/unmatched/RecRoomCupboard_ActorDtorSeam.cpp -- a four-line
   forwarder, which costs one file instead of holding both destructor TUs
   out the way ov024's PyramidTag D1 had to be. */
#pragma comment(linker, "/alternatename:?_ZTV15RecRoomCupboard@@3HA=__ZTV15RecRoomCupboard")
#pragma comment(linker, "/alternatename:?_ZN25MovingCylinderClsnWithPosD1Ev@@3HA=__ZN25MovingCylinderClsnWithPosD1Ev")
#pragma comment(linker, "/alternatename:?data_020a0eac@@3HA=_data_020a0eac")
/* Memory::Deallocate is a STATIC member, so MSVC gives it __cdecl under /Gd
   and the two calling conventions agree; only the decoration differs. */
#pragma comment(linker, "/alternatename:?Deallocate@Memory@@SAXPAXPAUHeap@@@Z=__ZN6Memory10DeallocateEPvP4Heap")

// ---- the trap --------------------------------------------------------------
static void ov58_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov058 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov058 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV58_TRAP(n) \
    static int __fastcall ov58_trap##n(void *s, void *) \
    { ov58_trap_report(s, n); return 0; }
OV58_TRAP(13) OV58_TRAP(14)
#undef OV58_TRAP

static int __fastcall ov58_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov58_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov58_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov58_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov58_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov58_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
/* slot 9 is the BASE Render: this class has no Render of its own. */
static int __fastcall ov58_render(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::Render(); }
static int __fastcall ov58_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov58_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov58_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov58_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov58_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov58_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov58_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov58_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov58_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov58_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov58_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov58_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov58_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov58_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov58_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov58_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

// ---- the class's own slots -------------------------------------------------
static int __fastcall rc_init(void *s, void *)
{ return _ZN15RecRoomCupboard13InitResourcesEv((char *)s); }
static int __fastcall rc_clean(void *s, void *)
{ (void)s; return _ZN15RecRoomCupboard16CleanupResourcesEv(); }
static int __fastcall rc_behavior(void *s, void *)
{ return _ZN15RecRoomCupboard8BehaviorEv((char *)s); }
static int __fastcall rc_d1(void *s, void *)
{ return _ZN15RecRoomCupboardD1Ev((char *)s); }
static int __fastcall rc_d0(void *s, void *)
{ return (int)(size_t)_ZN15RecRoomCupboardD0Ev((char *)s); }

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov58_bringup_done;
DSSTATE_END

extern "C" void port_ov58_bringup(void)
{
    if (g_ov58_bringup_done)
        return;
    g_ov58_bringup_done = 1;
    port_ov058_pack_check();
    port_ov058_syms_patch();
    /* No sinit and no hal_fill_platform_vtable: ov058 has no .init section and
       this class is Actor-derived, so its destructors never install Platform's
       base vptr. */
}

/* ONE table and ONE caller, so the shared half is written inline -- the
   gate-200 elided-stores bug needs two call sites with distinct extern-array
   arguments. The pointer is still volatile. THIRTY-ONE slots: there is no
   vt[31] to write, and writing one would run past the array. */
extern "C" void hal_fill_rec_room_cupboard_vtable(void)
{
    port_ov58_bringup();
    void *volatile *vt = (void *volatile *)_ZTV15RecRoomCupboard;
    vt[0]  = (void *)rc_init;
    vt[1]  = (void *)ov58_binit;
    vt[2]  = (void *)ov58_ainit;
    vt[3]  = (void *)rc_clean;
    vt[4]  = (void *)ov58_bclean;
    vt[5]  = (void *)ov58_aclean;
    vt[6]  = (void *)rc_behavior;
    vt[7]  = (void *)ov58_bbeh;
    vt[8]  = (void *)ov58_abeh;
    vt[9]  = (void *)ov58_render;
    vt[10] = (void *)ov58_bren;
    vt[11] = (void *)ov58_aren;
    vt[12] = (void *)ov58_pdes;
    vt[13] = (void *)ov58_trap13;
    vt[14] = (void *)ov58_trap14;
    vt[15] = (void *)ov58_heap;
    vt[16] = (void *)rc_d1;
    vt[17] = (void *)rc_d0;
    vt[18] = (void *)ov58_yoshi;
    vt[19] = (void *)ov58_turn_egg;
    vt[20] = (void *)ov58_v50;
    vt[21] = (void *)ov58_pounded;
    vt[22] = (void *)ov58_atk1;
    vt[23] = (void *)ov58_atk2;
    vt[24] = (void *)ov58_kicked;
    vt[25] = (void *)ov58_pushed;
    vt[26] = (void *)ov58_cannon;
    vt[27] = (void *)ov58_mega;
    vt[28] = (void *)ov58_under;
    vt[29] = (void *)ov58_egg;
    vt[30] = (void *)port_actor_s30_base;
}
