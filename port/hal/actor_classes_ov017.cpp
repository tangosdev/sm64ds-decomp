// RUN REL0215 WAVE 1 (lane cast-sweep1): OV017'S ONE CLASS (level 9, Jolly
// Roger Bay's sunken ship). Overlay 5 of 6 in this lane.
//
// Level 9 spawned 14 actors and skipped two when this lane ran: TREASURE_CHEST
// (13, ov064's) and this one. Both are seated now -- run rel0215 wave 3 (lane
// w3-c) took id 13, and level 9's census reads 16 spawned (9 classes), 0
// skipped (0 classes).
//
//   id  name        x on L9  factory          table                   width
//   62  SHIP_WATER   1       ShipWater_Spawn  0x02111bf8 _ZTV9ShipWater  32
//
// An 832-byte Platform: Model at +0xd4, MovingMeshCollider at +0x124, the
// collider transform source at +0x2ec, and a TextureTransformer at +0x320 its
// factory constructs and both destructors tear down first. InitResources loads
// model 1673 and KCL 1674, prepares the scrolling BTA out of the six-word file
// descriptor at 0x02111a60, hands the collider the CLPS block at 0x02111a94,
// enables it against itself and saves the starting water height in +0x334.
//
// ---- THIS SEAT CLOSES A BLOCKER THE TREE WROTE DOWN -----------------------
//
// hal/level_boot.cpp's level-9 block (lines 143-152 on tip f77f01169) names
// SHIP_WATER as fully decompiled but deferred, because its InitResources
// "references its own overlay's statics by the ov055/ov056 symbol SPELLINGS
// (data_ov056_02111a60, data_ov055_02111a94)" and hosting it needed "a
// per-symbol ov017 mount ... PLUS an alias-by-address ... the 'propagate
// config renames BY ADDRESS' hazard". That is exactly what this lane built.
// The block names TWO raced spellings. There are THREE.
//
// ---- THE THIRD RACE IS LIVE ON THIS TIP -----------------------------------
//
// The ROM's own literal pool at 0x02111470..0x0211147c, with the relocations
// over it, says every one of InitResources's four objects is an OV017 symbol:
//     pool 0x02111470 -> 0x02111c88   spelled correctly
//     pool 0x02111474 -> 0x02111a60   spelled data_ov056_02111a60
//     pool 0x02111478 -> 0x02111c80   spelled &_ZN16FloatingFloorBfsD0Ev
//     pool 0x0211147c -> 0x02111a94   spelled data_ov055_02111a94
// src/_ZN16FloatingFloorBfsD0Ev.c is in port/slice_w1l4.txt and its symbol is
// in build/port/walk_window.map RIGHT NOW, so without the rename
// MeshCollider::LoadFile is handed the address of an ov045 destructor as a
// file pointer: it resolves, it fails nowhere at link, and it reads a function
// body as a file header. The other two spellings are undefined today (ov055
// has no per-symbol mount and ov056's list does not carry 0x02111a60) and are
// latent rather than live -- but ov056 IS mounted and a later lane adding
// either name turns them live with no warning. All three go through a
// per-source -D in port/CMakeLists.txt.
//
// ---- NO CLASS-IDENTITY SHIFT HERE -----------------------------------------
//
// Unlike ov033 and ov035, dsd's two names for 0x02111bf8 (_ZTV9ShipWater and
// _ZTV14daObjKsWater_c) are both for THIS table, the factory installs it, and
// the RTTI string agrees -- "14daObjKsWater_c", Ks for Kaizoku Ship. The
// destructors' two vtable stores are also in the RIGHT order for once: pool
// 0x021111e4 holds the own table and 0x021111e8 holds Platform's base, which
// is the order the source writes them. Only ov023's and ov033's pairs are
// inverted.
//
// ---- T1: THE TABLE STAYS ROM-SHAPED ---------------------------------------
//
// ShipWater does not derive from Model or ModelAnim -- it HOLDS a Model at
// +0xd4 and a TextureTransformer at +0x320 as members -- so the default
// applies: [16] D1, [17] D0, ROM order, 32-slot Platform close on slot 31.
// dsd's next-symbol landing reads TEN (0x02111c20 is slot 10, an interior
// split, and 0x02111c54 is another at slot 23).
//
// ---- A BEHAVIOURAL DIVERGENCE THIS SEAT EXPOSES ---------------------------
//
// RETIRED by run rel0215 wave 3 (lane w3-c). The divergence was real and this
// is the record of it, not a live warning.
//
// ShipWater::Behavior gates on TREASURE_CHEST: it walks
// Actor::FindWithActorID(0xd) and only begins draining once EVERY id-13 actor
// reads state 1 or 2. While level 9 skipped TREASURE_CHEST the walk found
// nothing, the "all opened" test was vacuously true on frame one, and the water
// started draining immediately instead of waiting for the player. That was
// never this class's bug -- it was the other class being unseated. Lane w3-c
// seated id 13 the way this paragraph asked for (a per-symbol ov064 class
// mount, port/ov064_syms.txt's wave-3 block), so the walk now finds the level's
// one chest and the gate is a real test again.
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

/* the generated ov017 per-symbol mount (build/port/host-src/ov017_syms.c) */
void port_ov017_pack_check(void);
void port_ov017_syms_patch(void);
/* ov017's single sinit: files 1673 (model) and 1674 (collision) and their two
   chain nodes. */
void __sinit_ov017_0211198c(void);

/* the bodies src spells with C names (port/slice_sweep1_ov017.txt) */
int *_ZN9ShipWaterD1Ev(int *self);                /* slot 16 */
int *_ZN9ShipWaterD0Ev(int *self);                /* slot 17 */
int _ZN9ShipWater13InitResourcesEv(char *self);   /* slot 0, a plain .c body */
void *ShipWater_Spawn(void);                      /* id 62 */

/* the host vtable, excluded from the mount */
DSSTATE_BEGIN
void *_ZTV9ShipWater[32];     /* 0x02111bf8 */
DSSTATE_END
}

/* dsd's second name for the same table, which both destructors restore by.
   Both names are excluded from the mount, so this LHS is undefined everywhere
   and the alias cannot be defeated. The other word they store,
   _ZTV10dBgActor_c, is Platform's base table and is already a host array in
   hal/actor_classes.cpp -- and here, unlike ov023 and ov033, the source writes
   the two in the ROM's order, so neither needs renaming. */
#pragma comment(linker, "/alternatename:__ZTV14daObjKsWater_c=__ZTV9ShipWater")

/* CleanupResources's G0 and G1: the model and collision SharedFilePtrs, in the
   pool order the ROM releases them (0x02111288 = 0x02111c88, then 0x0211128c =
   0x02111c80). Private names because that TU includes decl_common.h, which
   already declares both objects -- a direct -D is a redeclaration, the ov100
   path-lift trap. G0 is declared in the TU itself OUTSIDE its extern "C" block
   so MSVC mangles it; G1 comes from decl_common.h, which is inside one. Both
   LHS are declared and never defined. */
#pragma comment(linker, "/alternatename:?port_ov017_sw_file0@@3PAHA=_data_ov017_02111c88")
#pragma comment(linker, "/alternatename:_port_ov017_sw_file1=_data_ov017_02111c80")

/* The three bodies src defines as real C++ methods, faced here. Render rides
   FROM SRC: it runs TextureTransformer::Update over the model's components at
   +0xdc and then the plain-Model slot-5 bare call over +0xd4. _ZTV5Model[5] is
   dual-filled in hal/cxxname_bridge.cpp and this class holds no ModelAnim, so
   it is the Tree/ov013 case and not the ModelAnim slot-5 collision. */
#include "ShipWater.h"

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN5Actor25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
extern "C" {
int _ZN9ShipWater16CleanupResourcesEv(void *self)
{ return ((ShipWater *)self)->ShipWater::CleanupResources(); }
int _ZN9ShipWater8BehaviorEv(void *self)
{ return ((ShipWater *)self)->ShipWater::Behavior(); }
int _ZN9ShipWater6RenderEv(void *self)
{ return ((ShipWater *)self)->ShipWater::Render(); }
}

// ---- the trap --------------------------------------------------------------
static void ov17_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov017 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov017 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV17_TRAP(n) \
    static int __fastcall ov17_trap##n(void *s, void *) \
    { ov17_trap_report(s, n); return 0; }
OV17_TRAP(13) OV17_TRAP(14)
#undef OV17_TRAP

static int __fastcall ov17_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov17_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov17_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov17_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov17_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov17_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov17_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov17_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov17_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov17_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov17_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov17_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov17_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov17_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov17_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov17_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov17_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov17_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov17_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov17_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov17_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov17_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov17_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

// ============================================================================
// SHIP_WATER (62) -- table 0x02111bf8, 32 slots.
// ============================================================================
static int __fastcall sw_init(void *s, void *)
{ return _ZN9ShipWater13InitResourcesEv((char *)s); }
static int __fastcall sw_clean(void *s, void *)
{ return _ZN9ShipWater16CleanupResourcesEv(s); }
static int __fastcall sw_behavior(void *s, void *)
{ return _ZN9ShipWater8BehaviorEv(s); }
static int __fastcall sw_render(void *s, void *)
{ port_actor_render_probe("SHIP_WATER", (char *)s + 0xd4);
  return _ZN9ShipWater6RenderEv(s); }
static int __fastcall sw_d1(void *s, void *)
{ return (int)(size_t)_ZN9ShipWaterD1Ev((int *)s); }
static int __fastcall sw_d0(void *s, void *)
{ return (int)(size_t)_ZN9ShipWaterD0Ev((int *)s); }

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov17_bringup_done;
DSSTATE_END

extern "C" void port_ov17_bringup(void)
{
    if (g_ov17_bringup_done)
        return;
    g_ov17_bringup_done = 1;
    port_ov017_pack_check();
    port_ov017_syms_patch();
    hal_fill_platform_vtable();
    __sinit_ov017_0211198c();
}

/* ONE table and ONE caller, so the shared half is written inline rather than
   through a helper -- the gate-200 elided-stores bug needs two call sites with
   distinct extern-array arguments to trigger. The pointer is still volatile. */
extern "C" void hal_fill_ship_water_vtable(void)
{
    port_ov17_bringup();
    void *volatile *vt = (void *volatile *)_ZTV9ShipWater;
    vt[0]  = (void *)sw_init;
    vt[1]  = (void *)ov17_binit;
    vt[2]  = (void *)ov17_ainit;
    vt[3]  = (void *)sw_clean;
    vt[4]  = (void *)ov17_bclean;
    vt[5]  = (void *)ov17_aclean;
    vt[6]  = (void *)sw_behavior;
    vt[7]  = (void *)ov17_bbeh;
    vt[8]  = (void *)ov17_abeh;
    vt[9]  = (void *)sw_render;
    vt[10] = (void *)ov17_bren;
    vt[11] = (void *)ov17_aren;
    vt[12] = (void *)ov17_pdes;
    vt[13] = (void *)ov17_trap13;
    vt[14] = (void *)ov17_trap14;
    vt[15] = (void *)ov17_heap;
    vt[16] = (void *)sw_d1;
    vt[17] = (void *)sw_d0;
    vt[18] = (void *)ov17_yoshi;
    vt[19] = (void *)ov17_turn_egg;
    vt[20] = (void *)ov17_v50;
    vt[21] = (void *)ov17_pounded;
    vt[22] = (void *)ov17_atk1;
    vt[23] = (void *)ov17_atk2;
    vt[24] = (void *)ov17_kicked;
    vt[25] = (void *)ov17_pushed;
    vt[26] = (void *)ov17_cannon;
    vt[27] = (void *)ov17_mega;
    vt[28] = (void *)ov17_under;
    vt[29] = (void *)ov17_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[31] = (void *)ov17_kill;
}
