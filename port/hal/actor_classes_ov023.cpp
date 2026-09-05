// RUN REL0215 WAVE 1 (lane cast-sweep1): OV023'S ONE CLASS (level 15, Lethal
// Lava Land's volcano interior). Overlay 4 of 6 in this lane.
//
// Level 15 spawns 55 actors and skips exactly ONE, and it is this class. That
// makes ov023 the only overlay in this lane whose seat takes its level to zero
// skipped classes.
//
//   id  name      x on L15  factory         table                  width
//   84  SQUASHER   1        Squasher_Spawn  0x02111fe4 _ZTV8Squasher  32
//
// An 892-byte Platform: Model at +0xd4, MovingMeshCollider at +0x124, the
// collider transform source at +0x2ec and a ShadowModel at +0x324 the factory
// constructs and both destructors tear down first. Its InitResources loads
// model 1558 and KCL 1559, hands the collider a CLPS block that lives in
// ANOTHER overlay, installs MeshColliderBase::UpdatePosWithTransform as the
// BeforeClsn callback (the contested-slot family c9a1731da seats) and enables
// the collider against itself.
//
// ---- THE DSD LANDING OVER-READS BY SEVEN SLOTS ----------------------------
//
// _ZTV8Squasher's next dsd symbol is the .bss base 0x9c away, so the landing
// reads THIRTY-NINE slots against a real 32. That is the opposite of ov024's
// and ov033's under-reads and it is the quieter failure for a host array:
// seven words of storage nothing ever writes, where a reader walking past slot
// 31 finds zeros instead of a fault. The width is pinned by the ROM's own
// close on slot 31, _ZN8Platform4KillEv at 0x020ee55c, verified slot by slot
// against config/arm9/overlays/ov023/relocs.txt.
//
// ---- T1: THE TABLE STAYS ROM-SHAPED ---------------------------------------
//
// Squasher does not derive from Model or ModelAnim -- it HOLDS a Model at
// +0xd4 as a member -- so the default applies: [16] D1, [17] D0, ROM order.
//
// ---- THREE ALIAS RACES, ALL PINNED TO THE ROM -----------------------------
//
// The InitResources body is the worst-spelled in this lane. Its own literal
// pool and the relocations over it settle every argument:
//   pool 0x02111718 -> 0x02112088   model SharedFilePtr, spelled correctly
//   pool 0x0211171c -> 0x02112080   collision SharedFilePtr, spelled
//                                   &_ZN32FloatOnWaterPlatformWdwRectangleD1Ev
//                                   -- an OV029 FUNCTION name at the same
//                                   window address. Left alone,
//                                   MeshCollider::LoadFile takes the address
//                                   of a destructor as a file pointer.
//   call 0x021116a4 -> 0x02111308   ov023's own transform helper, spelled
//                                   func_ov026_02111308. Lane W1-A is seating
//                                   ov026 in this same wave, so that name is
//                                   about to become DEFINED and the call would
//                                   land in another level's overlay.
//   pool 0x02111720 -> 0x0211ba4c   module:overlay(64). NOT a race. The CLPS
//                                   block really is in the Lethal Lava Land
//                                   actor pack, and one line was appended to
//                                   port/ov064_syms.txt to host it -- the
//                                   ov046 shape.
// And both destructors carry the ov033 inversion: the source stores
// _ZTV10dBgActor_c first and VT1 second, the ROM stores the OWN table first
// (pool 0x021111e4 = 0x02111fe4) and Platform's base second (0x021111e8 =
// 0x0210ae38). All of it is routed by per-source -D in port/CMakeLists.txt
// onto the private names this file binds below.
//
// ---- WHY THE MOUNT BRING-UP IS IN THIS FILE -------------------------------
//
// The ov025/ov033/ov035/ov045/ov052/ov056/ov073 lane-ownership pattern, behind
// one DSSTATE-bracketed done-guard: pack check, syms patch,
// hal_fill_platform_vtable (the destructors install ov002 0x0210ae38 on the
// way out and that array is zeroed storage until the fill runs), then ov023's
// single sinit, which is all of them.
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
void _ZN8Platform4KillEv(void *self);                              /* slot 31 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp */

/* the generated ov023 per-symbol mount (build/port/host-src/ov023_syms.c) */
void port_ov023_pack_check(void);
void port_ov023_syms_patch(void);
/* ov023's single sinit: files 1558 (model) and 1559 (collision) and their two
   chain nodes. */
void __sinit_ov023_02111aa8(void);

/* the bodies src spells with C names (port/slice_sweep1_ov023.txt) */
int *_ZN8SquasherD1Ev(int *self);                 /* slot 16 */
int *_ZN8SquasherD0Ev(int *self);                 /* slot 17 */
int _ZN8Squasher8BehaviorEv(void *self);          /* slot 6, a plain .c body */
void *Squasher_Spawn(void);                       /* id 84 */

/* the host vtable, excluded from the mount */
DSSTATE_BEGIN
void *_ZTV8Squasher[32];      /* 0x02111fe4 */
DSSTATE_END
}

/* THE FIVE RENAMED SPELLINGS, all bound onto real objects here. Every LHS is
   declared and never defined, so none of these aliases can be defeated and
   alternatename_guard stays clean.

   port_ov023_vt_own / _vt_base   the destructors' two vtable stores, whose
        resolver labels are INVERTED: the ROM writes the own table first (pool
        0x021111e4 = 0x02111fe4) and Platform's base second (0x021111e8 =
        0x0210ae38). A direct -D onto _ZTV10dBgActor_c is impossible anyway --
        it is a live host array for every Platform in the port.
   port_ov023_sq_file0 / _file1   CleanupResources's G0 and G1, the model and
        collision SharedFilePtrs in the pool order the ROM releases them
        (0x02111380 then 0x02111384). G0 is declared in that TU OUTSIDE its
        extern "C" block so MSVC mangles it; G1 comes from decl_common.h which
        is inside one. Hence the two different LHS shapes -- both measured off
        the link's unresolved list, not guessed.
   port_ov023_clsn_file           InitResources's collision file, spelled
        &_ZN32FloatOnWaterPlatformWdwRectangleD1Ev by the resolver -- an ov029
        function name at ov023's data address.
   port_ov023_xform               InitResources's call to ov023's own transform
        helper, spelled func_ov026_02111308. */
#pragma comment(linker, "/alternatename:_port_ov023_vt_own=__ZTV8Squasher")
#pragma comment(linker, "/alternatename:_port_ov023_vt_base=__ZTV10dBgActor_c")
#pragma comment(linker, "/alternatename:?port_ov023_sq_file0@@3PAHA=_data_ov023_02112088")
#pragma comment(linker, "/alternatename:_port_ov023_sq_file1=_data_ov023_02112080")
#pragma comment(linker, "/alternatename:_port_ov023_clsn_file=_data_ov023_02112080")
#pragma comment(linker, "/alternatename:_port_ov023_xform=_func_ov023_02111308")

/* The three bodies src defines as real C++ methods, faced here -- the
   ov013/ov024/ov025/ov033/ov035 recipe. Render rides FROM SRC as a plain-Model
   slot-5 bare-call shadow over +0xd4; _ZTV5Model[5] is dual-filled in
   hal/cxxname_bridge.cpp and this class holds no ModelAnim, so it is the
   Tree/ov013 case, not the ModelAnim slot-5 collision. */
#include "Squasher.h"
extern "C" {
int _ZN8Squasher13InitResourcesEv(void *self)
{ return ((Squasher *)self)->Squasher::InitResources(); }
int _ZN8Squasher16CleanupResourcesEv(void *self)
{ return ((Squasher *)self)->Squasher::CleanupResources(); }
int _ZN8Squasher6RenderEv(void *self)
{ return ((Squasher *)self)->Squasher::Render(); }
}

// ---- the trap --------------------------------------------------------------
static void ov23_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov023 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov023 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV23_TRAP(n) \
    static int __fastcall ov23_trap##n(void *s, void *) \
    { ov23_trap_report(s, n); return 0; }
OV23_TRAP(13) OV23_TRAP(14)
#undef OV23_TRAP

static int __fastcall ov23_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov23_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov23_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov23_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov23_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov23_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov23_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov23_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov23_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov23_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov23_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov23_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov23_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov23_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov23_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov23_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov23_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov23_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov23_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov23_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov23_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov23_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov23_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

// ============================================================================
// SQUASHER (84) -- table 0x02111fe4, 32 slots.
// ============================================================================
static int __fastcall sq_init(void *s, void *)
{ return _ZN8Squasher13InitResourcesEv(s); }
static int __fastcall sq_clean(void *s, void *)
{ return _ZN8Squasher16CleanupResourcesEv(s); }
static int __fastcall sq_behavior(void *s, void *)
{ return _ZN8Squasher8BehaviorEv(s); }
static int __fastcall sq_render(void *s, void *)
{ port_actor_render_probe("SQUASHER", (char *)s + 0xd4);
  return _ZN8Squasher6RenderEv(s); }
static int __fastcall sq_d1(void *s, void *)
{ return (int)(size_t)_ZN8SquasherD1Ev((int *)s); }
static int __fastcall sq_d0(void *s, void *)
{ return (int)(size_t)_ZN8SquasherD0Ev((int *)s); }

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov23_bringup_done;
DSSTATE_END

extern "C" void port_ov23_bringup(void)
{
    if (g_ov23_bringup_done)
        return;
    g_ov23_bringup_done = 1;
    port_ov023_pack_check();
    port_ov023_syms_patch();
    hal_fill_platform_vtable();
    __sinit_ov023_02111aa8();
}

/* ONE table and ONE caller, so the shared half is written inline rather than
   through a helper -- the gate-200 elided-stores bug needs two call sites with
   distinct extern-array arguments to trigger, and there is only one here. The
   pointer is still volatile. */
extern "C" void hal_fill_squasher_vtable(void)
{
    port_ov23_bringup();
    void *volatile *vt = (void *volatile *)_ZTV8Squasher;
    vt[0]  = (void *)sq_init;
    vt[1]  = (void *)ov23_binit;
    vt[2]  = (void *)ov23_ainit;
    vt[3]  = (void *)sq_clean;
    vt[4]  = (void *)ov23_bclean;
    vt[5]  = (void *)ov23_aclean;
    vt[6]  = (void *)sq_behavior;
    vt[7]  = (void *)ov23_bbeh;
    vt[8]  = (void *)ov23_abeh;
    vt[9]  = (void *)sq_render;
    vt[10] = (void *)ov23_bren;
    vt[11] = (void *)ov23_aren;
    vt[12] = (void *)ov23_pdes;
    vt[13] = (void *)ov23_trap13;
    vt[14] = (void *)ov23_trap14;
    vt[15] = (void *)ov23_heap;
    vt[16] = (void *)sq_d1;
    vt[17] = (void *)sq_d0;
    vt[18] = (void *)ov23_yoshi;
    vt[19] = (void *)ov23_turn_egg;
    vt[20] = (void *)ov23_v50;
    vt[21] = (void *)ov23_pounded;
    vt[22] = (void *)ov23_atk1;
    vt[23] = (void *)ov23_atk2;
    vt[24] = (void *)ov23_kicked;
    vt[25] = (void *)ov23_pushed;
    vt[26] = (void *)ov23_cannon;
    vt[27] = (void *)ov23_mega;
    vt[28] = (void *)ov23_under;
    vt[29] = (void *)ov23_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[31] = (void *)ov23_kill;
}
