// RUN REL0215 WAVE 1 (lane cast-sweep1): OV033'S TWO CLASSES (level 25,
// Tiny-Huge Island's small side). Overlay 3 of 6 in this lane.
//
// Level 25 spawns 87 actors and skips 3 across three ids on tip f77f01169.
// TWO of the three are this overlay's own cast; the third (id 239) belongs to
// an unmounted pack and stays skipped.
//
//   id   name         x on L25  factory          table                width
//   104  TINY_COVER    1        TinyCover_Spawn  0x0211237c (unnamed)  32
//   105  TINY_WATER    1        TinyWater_Spawn  0x02112440            32
//
// ---- THE DSD LABEL IS ON THE WRONG TABLE ----------------------------------
//
// dsd names exactly one vtable here, _ZTV9TinyCover at 0x02112440, and it is
// the WATER class's. Three independent routes agree:
//   the factories   TinyCover_Spawn (0x021113a4) allocates 800 and installs
//                   0x0211237c; TinyWater_Spawn (0x02111690) allocates 832,
//                   installs 0x02112440 and constructs a TextureTransformer at
//                   this+0x320.
//   the RTTI        0x0211237c -> "13daObjTtFuta_c" (futa = lid/cover)
//                   0x02112440 -> "14daObjTtWater_c"
//   the records     0x02112358 (id 104) -> TinyCover_Spawn
//                   0x0211241c (id 105) -> TinyWater_Spawn
// So every _ZN9TinyCover* method body in src/ is TINY_WATER's and every
// func_ov033_* lifecycle body is TINY_COVER's. The rows below are wired by
// address; the dsd spellings survive only as host array names, which is what
// lets the matched TUs link unchanged. Same shift ov035 carries, one overlay
// along. The derivation is in port/ov033_syms.txt.
//
// ---- ID 104 HAS SEVEN OWN SLOTS, NOT SIX ----------------------------------
//
// Its table overrides slot 21, OnGroundPounded, with func_ov033_0211123c:
// spawn particle 0x28 at the pounder's position, play sound 0xf,
// Event::SetBit(0xe), mark self destroyed. Ground-pounding the lid is what
// opens the well, and it is the class's whole reason to exist. A fill that let
// the shared half write Actor's do-nothing into slot 21 would have left the
// lid indestructible with no fault and no log line -- so the fill writes 21
// explicitly and the shared helper is called first, not after.
//
// ---- T1: BOTH TABLES STAY ROM-SHAPED --------------------------------------
//
// Neither class derives from Model or ModelAnim (both HOLD a Model at +0xd4 as
// a member and id 105 also holds a TextureTransformer at +0x320), so the
// default applies: ROM-shaped, [16] D1, [17] D0, and the 32-slot Platform
// close on slot 31. Every slot verified against
// config/arm9/overlays/ov033/relocs.txt before a word was replaced. dsd's
// landings read 26 and THREE.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE -------------------------------
//
// The ov025/ov035/ov045/ov052/ov056/ov073 lane-ownership pattern, behind one
// DSSTATE-bracketed done-guard. It runs the pack check, the syms patch,
// hal_fill_platform_vtable (all four destructors install ov002 0x0210ae38 as
// the base vptr on the way out) and then BOTH ov033 sinits, which is all of
// them -- neither class here is without reach.
#include <cstdio>
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
void _ZN8Platform4KillEv(void *self);                              /* slot 31 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp */

/* the generated ov033 per-symbol mount (build/port/host-src/ov033_syms.c) */
void port_ov033_pack_check(void);
void port_ov033_syms_patch(void);
/* both sinits: id 104's files 1733/1734 and id 105's 1735/1736 */
void __sinit_ov033_021119e8(void);
void __sinit_ov033_02111a54(void);

/* id 104 TINY_COVER's own bodies -- dsd left the class unnamed, so all seven
   are func_ov033_*. Their "recovered from vtable slot identity" markers are
   NAME recoveries over real decompiled bodies (T5, adjudicated in
   port/ov033_syms.txt); none is a stub. */
int func_ov033_02111310(char *self);       /* slot 0,  InitResources */
int func_ov033_02111280(void *self);       /* slot 3,  CleanupResources */
int func_ov033_021112ec(void *self);       /* slot 6,  Behavior */
int func_ov033_021112c4(void *self);       /* slot 9,  Render */
int *func_ov033_021111a0(int *self);       /* slot 16, D1 */
int *func_ov033_021111e4(int *self);       /* slot 17, D0 */
void func_ov033_0211123c(char *self, char *other); /* slot 21, OnGroundPounded */
void *TinyCover_Spawn(void);               /* id 104 */

/* id 105 TINY_WATER's own bodies -- these are the ones dsd spelled
   _ZN9TinyCover*, and they belong to the WATER class. */
int *_ZN9TinyCoverD1Ev(int *self);         /* slot 16 */
int *_ZN9TinyCoverD0Ev(int *self);         /* slot 17 */
int _ZN9TinyCover8BehaviorEv(void *self);  /* slot 6, a plain .c body */
void *TinyWater_Spawn(void);               /* id 105 */

/* the two host vtables, both excluded from the mount. The names are dsd's for
   the addresses, NOT for the classes -- see this file's header. */
DSSTATE_BEGIN
int data_ov033_0211237c[32];   /* 0x0211237c, id 104 TINY_COVER */
void *_ZTV9TinyCover[32];      /* 0x02112440, id 105 TINY_WATER */
DSSTATE_END
}

/* The RTTI spelling id 104's D1 restores its table by. It is excluded from the
   mount and defined nowhere else, so the alias cannot be defeated. */
#pragma comment(linker, "/alternatename:__ZTV13daObjTtFuta_c=_data_ov033_0211237c")

/* THE FOUR RESOLVER PLACEHOLDERS, all routed by per-source -D onto private
   names this lane owns, because each means a different object in a different
   TU and two of them collide with live host arrays:

     id 104's D0    VT0 -> the own table, VT1 -> Platform's base
     id 105's D1/D0 the source writes _ZTV10dBgActor_c FIRST and VT1 SECOND,
                    but the ROM writes the OWN table first (0x021113e4 stores
                    pool 0x02111418 = 0x02112440) and the base second
                    (0x021113f4 stores pool 0x0211141c = 0x0210ae38). The
                    resolver's two labels are inverted. Left alone the
                    destructor would install Platform's base before the
                    TextureTransformer teardown and leave the OWN table
                    installed after Actor::D2.
   A direct -D onto _ZTV10dBgActor_c is impossible either way: that name is a
   live host array (hal/actor_classes.cpp) for every Platform in the port. All
   four LHS below are declared and never defined, so the aliases cannot be
   defeated and alternatename_guard stays clean. */
#pragma comment(linker, "/alternatename:_port_ov033_tc_vt_own=_data_ov033_0211237c")
#pragma comment(linker, "/alternatename:_port_ov033_tw_vt_own=__ZTV9TinyCover")
#pragma comment(linker, "/alternatename:_port_ov033_vt_base=__ZTV10dBgActor_c")

/* The five bodies src defines as real C++ methods, faced here -- the
   ov013/ov024/ov025/ov035 recipe. Both Renders ride FROM SRC as plain-Model
   slot-5 bare-call shadows over +0xd4; _ZTV5Model[5] is dual-filled in
   hal/cxxname_bridge.cpp, and neither class holds a ModelAnim, so this is the
   Tree/ov013 case and not the ModelAnim slot-5 collision. */
#include "TinyCover.h"
extern "C" {
int _ZN9TinyCover13InitResourcesEv(void *self)
{ return ((TinyCover *)self)->TinyCover::InitResources(); }
int _ZN9TinyCover16CleanupResourcesEv(void *self)
{ return ((TinyCover *)self)->TinyCover::CleanupResources(); }
int _ZN9TinyCover6RenderEv(void *self)
{ return ((TinyCover *)self)->TinyCover::Render(); }
}

// ---- the trap --------------------------------------------------------------
static void ov33_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov033 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov033 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV33_TRAP(n) \
    static int __fastcall ov33_trap##n(void *s, void *) \
    { ov33_trap_report(s, n); return 0; }
OV33_TRAP(13) OV33_TRAP(14) OV33_TRAP(30)
#undef OV33_TRAP

static int __fastcall ov33_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov33_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov33_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov33_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov33_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov33_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov33_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov33_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov33_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov33_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov33_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov33_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov33_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov33_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov33_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov33_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov33_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov33_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov33_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov33_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov33_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov33_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov33_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

/* The shared half of both tables. The caller writes its own 0/3/6/9/16/17/31,
   and id 104 also writes 21 AFTER this returns.

   THE POINTER IS VOLATILE ON PURPOSE -- the gate-200 elided-stores bug, repro
   in hal/actor_classes_ov002g200.cpp. This helper has exactly two callers. */
static void ov33_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov33_binit;
    vt[2]  = (void *)ov33_ainit;
    vt[4]  = (void *)ov33_bclean;
    vt[5]  = (void *)ov33_aclean;
    vt[7]  = (void *)ov33_bbeh;
    vt[8]  = (void *)ov33_abeh;
    vt[10] = (void *)ov33_bren;
    vt[11] = (void *)ov33_aren;
    vt[12] = (void *)ov33_pdes;
    vt[13] = (void *)ov33_trap13;
    vt[14] = (void *)ov33_trap14;
    vt[15] = (void *)ov33_heap;
    vt[18] = (void *)ov33_yoshi;
    vt[19] = (void *)ov33_turn_egg;
    vt[20] = (void *)ov33_v50;
    vt[21] = (void *)ov33_pounded;
    vt[22] = (void *)ov33_atk1;
    vt[23] = (void *)ov33_atk2;
    vt[24] = (void *)ov33_kicked;
    vt[25] = (void *)ov33_pushed;
    vt[26] = (void *)ov33_cannon;
    vt[27] = (void *)ov33_mega;
    vt[28] = (void *)ov33_under;
    vt[29] = (void *)ov33_egg;
    vt[30] = (void *)ov33_trap30;
}

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov33_bringup_done;
DSSTATE_END

extern "C" void port_ov33_bringup(void)
{
    if (g_ov33_bringup_done)
        return;
    g_ov33_bringup_done = 1;
    port_ov033_pack_check();
    port_ov033_syms_patch();
    hal_fill_platform_vtable();
    __sinit_ov033_021119e8();
    __sinit_ov033_02111a54();
}

// ============================================================================
// TINY_COVER (104) -- table 0x0211237c, RTTI 13daObjTtFuta_c, 32 slots.
// ============================================================================
//
// The lid over the small island's well. An 800-byte Platform: Model at +0xd4,
// MovingMeshCollider at +0x124, collider transform source at +0x2ec.
// InitResources loads model 1733 and KCL 1734, hands the collider CLPS
// 0x02111bfc, and RETURNS Event::GetBit(0xe) == 0 -- so once the lid has been
// pounded open the class declines to initialise and never comes back.
// Slot 21 is the pound: particle 0x28 at the pounder's position, sound 0xf,
// Event::SetBit(0xe), self-destruct.
static int __fastcall tc_init(void *s, void *)
{ return func_ov033_02111310((char *)s); }
static int __fastcall tc_clean(void *s, void *)
{ return func_ov033_02111280(s); }
static int __fastcall tc_behavior(void *s, void *)
{ return func_ov033_021112ec(s); }
static int __fastcall tc_render(void *s, void *)
{ port_actor_render_probe("TINY_COVER", (char *)s + 0xd4);
  return func_ov033_021112c4(s); }
static int __fastcall tc_d1(void *s, void *)
{ return (int)(size_t)func_ov033_021111a0((int *)s); }
static int __fastcall tc_d0(void *s, void *)
{ return (int)(size_t)func_ov033_021111e4((int *)s); }
/* Slot 21, the OVERRIDE. Three parameters so it emits `ret 4`, the
   wf_turn_egg/OnGroundPounded contract the shared half's own slot 21 uses. */
static int __fastcall tc_pounded(void *s, void *, void *o)
{ func_ov033_0211123c((char *)s, (char *)o); return 0; }

extern "C" void hal_fill_tiny_cover_vtable(void)
{
    port_ov33_bringup();
    void *volatile *vt = (void *volatile *)data_ov033_0211237c;
    ov33_fill_shared(vt);
    vt[0]  = (void *)tc_init;
    vt[3]  = (void *)tc_clean;
    vt[6]  = (void *)tc_behavior;
    vt[9]  = (void *)tc_render;
    vt[16] = (void *)tc_d1;
    vt[17] = (void *)tc_d0;
    /* AFTER the shared fill, which writes Actor's do-nothing here. */
    vt[21] = (void *)tc_pounded;
    vt[31] = (void *)ov33_kill;
}

// ============================================================================
// TINY_WATER (105) -- table 0x02112440 (dsd _ZTV9TinyCover), RTTI
// 14daObjTtWater_c, 32 slots.
// ============================================================================
//
// The small island's water surface. An 832-byte Platform: Model at +0xd4,
// MovingMeshCollider at +0x124, and a TextureTransformer at +0x320 its factory
// constructs and both destructors tear down FIRST. InitResources loads model
// 1735 with LOD 0x14, prepares the scrolling BTA out of the six-word file
// descriptor 0x02111bc8, loads KCL 1736, hands the collider CLPS 0x02111c1c,
// enables it against itself and parks the surface height at mPosY - 0x3c000.
// Like the lid it returns Event::GetBit(0xe) == 0, so the water level and the
// lid share one event bit -- pounding the lid is what drains it.
static int __fastcall tw_init(void *s, void *)
{ return _ZN9TinyCover13InitResourcesEv(s); }
static int __fastcall tw_clean(void *s, void *)
{ return _ZN9TinyCover16CleanupResourcesEv(s); }
static int __fastcall tw_behavior(void *s, void *)
{ return _ZN9TinyCover8BehaviorEv(s); }
static int __fastcall tw_render(void *s, void *)
{ port_actor_render_probe("TINY_WATER", (char *)s + 0xd4);
  return _ZN9TinyCover6RenderEv(s); }
static int __fastcall tw_d1(void *s, void *)
{ return (int)(size_t)_ZN9TinyCoverD1Ev((int *)s); }
static int __fastcall tw_d0(void *s, void *)
{ return (int)(size_t)_ZN9TinyCoverD0Ev((int *)s); }

extern "C" void hal_fill_tiny_water_vtable(void)
{
    port_ov33_bringup();
    void *volatile *vt = (void *volatile *)_ZTV9TinyCover;
    ov33_fill_shared(vt);
    vt[0]  = (void *)tw_init;
    vt[3]  = (void *)tw_clean;
    vt[6]  = (void *)tw_behavior;
    vt[9]  = (void *)tw_render;
    vt[16] = (void *)tw_d1;
    vt[17] = (void *)tw_d0;
    vt[31] = (void *)ov33_kill;
}
