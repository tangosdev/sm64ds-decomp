// ov029 -- level 21's own overlay (data/stage/water_city, the Dire Dire Docks
// city sublevel, course 10). Eight actors, ids 94..101, every one placed by the
// level-21 boot census. This file hosts their vtables; the SpawnInfo records,
// resource tables and sinit-built SharedFilePtr windows come from the per-symbol
// mount (port/ov029_syms.txt), and the slice enrolls the matched src bodies.
//
// THE NAME SHIFT (read before trusting any _ZTV name). The dsd _ZTV<PrettyName>
// symbols do NOT match <PrettyName>_Spawn: the SpawnInfo->_Spawn naming and the
// _ZTV/method naming are misaligned by one across the whole WDW platform chain.
// The authoritative fact for a live vtable is the LAST `STR rX,[obj,#0]` in the
// actor's own _Spawn, read by disassembly and confirmed by the table's own
// slot-16 destructor restoring that same table. Every row below is keyed on
// that, by ADDRESS, so the shifted names cost nothing here:
//
//   id  actor (_Spawn)                     live table (last store)      shape
//   94  RotatingPlatformWdw   0x02112168   0x02114018 _ZTV32Float..Rect Platform 32
//   95  FloatOnWaterPlatformWdwSquare      0x02113c2c (unnamed)         Platform 32
//   96  ArrowLift             0x021116f8   0x02113cf0 _ZTV29Float..Sq   Platform 32
//   97  WaterDiamond          0x02111a84   0x02113db4 _ZTV9ArrowLift    Actor 31 (no Kill)
//   98  SwitchActivatedPlank  0x02112964   0x021141a0 _ZTV20Switch..    Platform 32
//   99  CageLift              0x02111e74   0x02113e74 (unnamed)         Platform 32
//  100  FloatOnWaterPlatformWdwRectangle   0x02113f44 (unnamed)         Platform 32
//  101  WDW_Water             0x021125f8   0x021140dc _ZTV19Rotating..  Platform 32
//
// The dead first store each _Spawn/D1 makes (an ov002 base table or _ZTV8Platform)
// and the recovered VT0/VT1/VT2 placeholders are bound per source in
// port/CMakeLists.txt, because the same recovered name means different addresses
// in different TUs (RotatingPlatformWdw_Spawn's _ZTV15daObjWc_Obj07_c is the
// ov002 base, the RectangleD1 TU's is 0x02114018).

#include <cstdio>
#include "dsstate_seg.h"
#include "dtor_faces_cpp.h"
#include "Actor.h"
#include "ActorBase.h"

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
/* shared arm9 Actor/ActorBase bodies (slots 1..30), same set the ov025 lane
   forwards, all already in walk_window.map */
int  _ZN5Actor19BeforeInitResourcesEv(void *self);
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);
int  _ZN5Actor14BeforeBehaviorEv(void *self);
int  _ZN5Actor12BeforeRenderEv(void *self);
int  _ZN5Actor13OnYoshiTryEatEv(void *self);
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);
int  _ZN5Actor9Virtual50Ev(void *self);
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);
void _ZN5Actor8OnKickedERS_(void *self, void *o);
void _ZN5Actor8OnPushedERS_(void *self, void *o);
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o);
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);
int  _ZN5Actor16OnAimedAtWithEggEv(void *self);
void _ZN8Platform4KillEv(void *self);

const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);

/* the mount's own two passes and the eight SharedFilePtr sinits, in .ctor order */
void port_ov029_pack_check(void);
void port_ov029_syms_patch(void);
void __sinit_ov029_02112b38(void);
void __sinit_ov029_02112ba4(void);
void __sinit_ov029_02112c10(void);
void __sinit_ov029_02112c4c(void);
void __sinit_ov029_02112cb8(void);
void __sinit_ov029_02112d24(void);
void __sinit_ov029_02112d90(void);
void __sinit_ov029_02112dfc(void);

/* ov002 shared base-platform bodies the inherited slots 3/6/9 reach (in map) */
int func_ov002_020b5be0(void *self);
int func_ov002_020b5c4c(void *self);
int func_ov002_020b5c24(void *self);
int func_ov002_020b6718(void *self);
int func_ov002_020b66f0(void *self);

/* id 95 FloatOnWaterPlatformWdwSquare own bodies (unnamed; s17 D0 is a guessed
   body, trapped not seated -- no extern) */
int  func_ov029_02111254(char *self);   /* s0 InitResources */
int *func_ov029_021111a0(int *self);    /* s16 D1 */
/* id 96 ArrowLift, live table _ZTV29FloatOnWaterPlatformWdwSquare (mangled bodies) */
int _ZN29FloatOnWaterPlatformWdwSquare13InitResourcesEv(void *self);
int _ZN29FloatOnWaterPlatformWdwSquare16CleanupResourcesEv(void *self);
int _ZN29FloatOnWaterPlatformWdwSquare8BehaviorEv(void *self);
int _ZN29FloatOnWaterPlatformWdwSquare6RenderEv(void *self);
int *_ZN29FloatOnWaterPlatformWdwSquareD1Ev(int *self);
int *_ZN29FloatOnWaterPlatformWdwSquareD0Ev(int *self);
/* id 97 WaterDiamond, live table _ZTV9ArrowLift (Actor 31) */
int _ZN9ArrowLift13InitResourcesEv(void *self);
int _ZN9ArrowLift16CleanupResourcesEv(void *self);
int _ZN9ArrowLift8BehaviorEv(void *self);
int _ZN9ArrowLift6RenderEv(void *self);
int *_ZN9ArrowLiftD0Ev(int *self);   /* D1 trapped: see WaterDiamond fill */
/* id 99 CageLift is not mounted (guessed bodies) -- no externs. */
/* id 100 FloatOnWaterPlatformWdwRectangle own bodies (unnamed; s17 D0 guessed,
   trapped not seated) */
int  func_ov029_02111f58(char *self);   /* s0 */
int *func_ov029_02111ea4(int *self);    /* s16 */
/* id 94 RotatingPlatformWdw, live table _ZTV32FloatOnWaterPlatformWdwRectangle */
int _ZN32FloatOnWaterPlatformWdwRectangle13InitResourcesEv(void *self);
int _ZN32FloatOnWaterPlatformWdwRectangle16CleanupResourcesEv(void *self);
int *_ZN32FloatOnWaterPlatformWdwRectangleD1Ev(int *self);
int *_ZN32FloatOnWaterPlatformWdwRectangleD0Ev(int *self);
/* id 98 SwitchActivatedPlank, live table _ZTV20SwitchActivatedPlank */
int _ZN20SwitchActivatedPlank13InitResourcesEv(void *self);
int _ZN20SwitchActivatedPlank16CleanupResourcesEv(void *self);
int _ZN20SwitchActivatedPlank8BehaviorEv(void *self);
int _ZN20SwitchActivatedPlank6RenderEv(void *self);
int *_ZN20SwitchActivatedPlankD1Ev(int *self);
int *_ZN20SwitchActivatedPlankD0Ev(int *self);
/* id 101 WDW_Water, live table _ZTV19RotatingPlatformWdw */
int _ZN19RotatingPlatformWdw13InitResourcesEv(void *self);
int _ZN19RotatingPlatformWdw16CleanupResourcesEv(void *self);
int _ZN19RotatingPlatformWdw8BehaviorEv(void *self);
int _ZN19RotatingPlatformWdw6RenderEv(void *self);
int *_ZN19RotatingPlatformWdwD1Ev(int *self);
int *_ZN19RotatingPlatformWdwD0Ev(int *self);

/* the eight live vtables. Five carry the dsd _ZTV name (the matched _Spawn/D1
   TUs spell them); three are unnamed and hosted under their ROM address. Sized
   by vtspan: 32 slots for the Platform tables (Kill at 31), 31 for the Actor
   table (id 97, ends at slot 30). DSSTATE so a savestate restore refills. */
DSSTATE_BEGIN
void *data_ov029_02113c2c[32];                    /* id 95  Square       */
void *_ZTV29FloatOnWaterPlatformWdwSquare[32];    /* id 96  ArrowLift     */
void *_ZTV9ArrowLift[31];                          /* id 97  WaterDiamond  */
/* id 99 CageLift: not mounted (guessed bodies), no host table */
void *data_ov029_02113f44[32];                    /* id 100 Rectangle     */
void *_ZTV32FloatOnWaterPlatformWdwRectangle[32]; /* id 94  RotatingWdw   */
void *_ZTV20SwitchActivatedPlank[32];             /* id 98  SwitchPlank   */
void *_ZTV19RotatingPlatformWdw[32];              /* id 101 WDW_Water     */
DSSTATE_END
}

// ---- the trap: slots that no int __fastcall(void*,void*) thunk can serve ----
// 13/14 are ActorBase::Virtual34/38(u32,u32) (two args past `this`) and 30 is
// OnAimedAtWithEggReturnVec (hidden SRET pointer). None is reachable by a WDW
// platform in normal play; if one is called it is reported by name, not seated
// wrong.
static void ov29_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov029 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov029 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV29_TRAP(n) \
    static int __fastcall ov29_trap##n(void *s, void *) \
    { ov29_trap_report(s, n); return 0; }
OV29_TRAP(13) OV29_TRAP(14) OV29_TRAP(16) OV29_TRAP(17)
#undef OV29_TRAP

// ---- the shared slots 1..30 (own 0/3/6/9/16/17 and 31 written by the caller) -
static int  __fastcall ov29_binit(void *s, void *) { return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov29_ainit(void *s, void *, unsigned a) { _ZN5Actor18AfterInitResourcesEj(s, a); }
static int  __fastcall ov29_bclean(void *s, void *) { return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov29_aclean(void *s, void *, unsigned a) { ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int  __fastcall ov29_bbeh(void *s, void *) { return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov29_abeh(void *s, void *, unsigned a) { ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int  __fastcall ov29_bren(void *s, void *) { return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov29_aren(void *s, void *, unsigned a) { ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int  __fastcall ov29_pdes(void *s, void *) { ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int  __fastcall ov29_heap(void *s, void *) { return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int  __fastcall ov29_yoshi(void *s, void *) { return _ZN5Actor13OnYoshiTryEatEv(s); }
static int  __fastcall ov29_turn_egg(void *s, void *, void *p) { _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int  __fastcall ov29_v50(void *s, void *) { return _ZN5Actor9Virtual50Ev(s); }
static int  __fastcall ov29_pounded(void *s, void *, void *o) { _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int  __fastcall ov29_atk1(void *s, void *, void *o) { _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int  __fastcall ov29_atk2(void *s, void *, void *o) { _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int  __fastcall ov29_kicked(void *s, void *, void *o) { _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int  __fastcall ov29_pushed(void *s, void *, void *o) { _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int  __fastcall ov29_cannon(void *s, void *, void *o) { _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int  __fastcall ov29_mega(void *s, void *, void *p) { _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int  __fastcall ov29_under(void *s, void *, void *o) { _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int  __fastcall ov29_egg(void *s, void *) { return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int  __fastcall ov29_kill(void *s, void *) { _ZN8Platform4KillEv(s); return 0; }

/* THE POINTER IS VOLATILE ON PURPOSE: the gate-200 elided-stores bug. */
static void ov29_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov29_binit;   vt[2]  = (void *)ov29_ainit;
    vt[4]  = (void *)ov29_bclean;  vt[5]  = (void *)ov29_aclean;
    vt[7]  = (void *)ov29_bbeh;    vt[8]  = (void *)ov29_abeh;
    vt[10] = (void *)ov29_bren;    vt[11] = (void *)ov29_aren;
    vt[12] = (void *)ov29_pdes;    vt[13] = (void *)ov29_trap13;
    vt[14] = (void *)ov29_trap14;  vt[15] = (void *)ov29_heap;
    vt[18] = (void *)ov29_yoshi;   vt[19] = (void *)ov29_turn_egg;
    vt[20] = (void *)ov29_v50;     vt[21] = (void *)ov29_pounded;
    vt[22] = (void *)ov29_atk1;    vt[23] = (void *)ov29_atk2;
    vt[24] = (void *)ov29_kicked;  vt[25] = (void *)ov29_pushed;
    vt[26] = (void *)ov29_cannon;  vt[27] = (void *)ov29_mega;
    vt[28] = (void *)ov29_under;   vt[29] = (void *)ov29_egg;
    vt[30] = (void *)port_actor_s30_base;
}

// ---- the bring-up: pack check, syms patch, the eight sinits in .ctor order --
// DSSTATE-guarded so a savestate restore re-runs it (everything it writes lives
// in .dsstate). Idempotent, so the driver and every fill may call it.
DSSTATE_BEGIN
static int g_ov29_bringup_done;
DSSTATE_END

extern "C" void port_ov29_bringup(void)
{
    if (g_ov29_bringup_done)
        return;
    g_ov29_bringup_done = 1;
    port_ov029_pack_check();
    port_ov029_syms_patch();
    __sinit_ov029_02112b38();
    __sinit_ov029_02112ba4();
    __sinit_ov029_02112c10();
    __sinit_ov029_02112c4c();
    __sinit_ov029_02112cb8();
    __sinit_ov029_02112d24();
    __sinit_ov029_02112d90();
    __sinit_ov029_02112dfc();
}

// ---- the eight per-actor override thunks + fills ----------------------------
/* id 95 FloatOnWaterPlatformWdwSquare (data_ov029_02113c2c, Platform 32). Its
   inherited s3/6/9 are the ov002 base-platform bodies. */
static int __fastcall sq_init(void *s, void *) { return func_ov029_02111254((char *)s); }
static int __fastcall sq_clean(void *s, void *) { return func_ov002_020b5be0(s); }
static int __fastcall sq_beh(void *s, void *)   { return func_ov002_020b5c4c(s); }
static int __fastcall sq_ren(void *s, void *)   { return func_ov002_020b5c24(s); }
static int __fastcall sq_d1(void *s, void *)    { return (int)(size_t)func_ov029_021111a0((int *)s); }
/* slot 17 (D0, the deleting destructor) is func_ov029_021111f0, which carries
   the "recovered from vtable slot identity" marker -- a guessed body, not a ROM
   decompilation, so it is TRAPPED rather than seated. D0 is not called across a
   level selftest (actors are freed at teardown), and the real D1 at slot 16 is
   seated. */
extern "C" void hal_fill_float_on_water_platform_wdw_square_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)data_ov029_02113c2c;
    ov29_fill_shared(vt);
    vt[0]=(void *)sq_init; vt[3]=(void *)sq_clean; vt[6]=(void *)sq_beh;
    vt[9]=(void *)sq_ren;  vt[16]=(void *)sq_d1;   vt[17]=(void *)ov29_trap17;
    vt[31]=(void *)ov29_kill;
}

/* id 96 ArrowLift (_ZTV29FloatOnWaterPlatformWdwSquare, Platform 32) */
static int __fastcall al_init(void *s, void *)  { return _ZN29FloatOnWaterPlatformWdwSquare13InitResourcesEv(s); }
static int __fastcall al_clean(void *s, void *) { return _ZN29FloatOnWaterPlatformWdwSquare16CleanupResourcesEv(s); }
static int __fastcall al_beh(void *s, void *)   { return _ZN29FloatOnWaterPlatformWdwSquare8BehaviorEv(s); }
static int __fastcall al_ren(void *s, void *)   { return _ZN29FloatOnWaterPlatformWdwSquare6RenderEv(s); }
static int __fastcall al_d1(void *s, void *)    { return (int)(size_t)_ZN29FloatOnWaterPlatformWdwSquareD1Ev((int *)s); }
static int __fastcall al_d0(void *s, void *)    { return (int)(size_t)_ZN29FloatOnWaterPlatformWdwSquareD0Ev((int *)s); }
extern "C" void hal_fill_arrow_lift_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV29FloatOnWaterPlatformWdwSquare;
    ov29_fill_shared(vt);
    vt[0]=(void *)al_init; vt[3]=(void *)al_clean; vt[6]=(void *)al_beh;
    vt[9]=(void *)al_ren;  vt[16]=(void *)al_d1;   vt[17]=(void *)al_d0;
    vt[31]=(void *)ov29_kill;
}

/* id 97 WaterDiamond (_ZTV9ArrowLift, Actor 31 -- NO Kill slot 31) */
static int __fastcall wd_init(void *s, void *)  { return _ZN9ArrowLift13InitResourcesEv(s); }
static int __fastcall wd_clean(void *s, void *) { return _ZN9ArrowLift16CleanupResourcesEv(s); }
static int __fastcall wd_beh(void *s, void *)   { return _ZN9ArrowLift8BehaviorEv(s); }
static int __fastcall wd_ren(void *s, void *)   { return _ZN9ArrowLift6RenderEv(s); }
/* slot 16 (D1) is _ZN9ArrowLiftD1Ev, a C++ virtual destructor: since lane
   DTOR-FACES-CPP it links from src through hal/dtor_faces_cpp.cpp's faces and
   is seated here (it was TRAPPED while the base dtors were unhosted). D0
   (slot 17) is the C-linkage _ZN9ArrowLiftD0Ev, seated. */
static int __fastcall wd_d0(void *s, void *)    { return (int)(size_t)_ZN9ArrowLiftD0Ev((int *)s); }
extern "C" void hal_fill_water_diamond_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV9ArrowLift;
    ov29_fill_shared(vt);
    vt[0]=(void *)wd_init;      vt[3]=(void *)wd_clean; vt[6]=(void *)wd_beh;
    vt[9]=(void *)wd_ren;  vt[16]=(void *)hal_cppd1_ArrowLift; vt[17]=(void *)wd_d0;
    /* no slot 31: a plain Actor, 31 slots, ends at 30 */
}

/* id 99 CageLift is NOT mounted. Five of its six lifecycle bodies
   (func_ov029_02111d6c InitResources, _02111b60 CleanupResources, _02111bcc
   Behavior, _02111ba4 Render, _02111b08 D0) carry the "recovered from vtable
   slot identity" marker -- they are guessed, not ROM decompilations, so seating
   them would fail inferred_stub_guard and put fabricated code behind a live
   vtable. Only its D1 (func_ov029_02111ac4) is real. With InitResources unable
   to be seated the class cannot load its model or collision, so it is left
   unregistered (id 99 stays skipped, as on baseline) rather than trap-mounted
   into a likely fault. It waits on a faithful decomp of those five bodies. */

/* id 100 FloatOnWaterPlatformWdwRectangle (data_ov029_02113f44, Platform 32;
   inherited s3/6/9 = ov002 base, same as Square) */
static int __fastcall rc_init(void *s, void *)  { return func_ov029_02111f58((char *)s); }
static int __fastcall rc_clean(void *s, void *) { return func_ov002_020b5be0(s); }
static int __fastcall rc_beh(void *s, void *)   { return func_ov002_020b5c4c(s); }
static int __fastcall rc_ren(void *s, void *)   { return func_ov002_020b5c24(s); }
static int __fastcall rc_d1(void *s, void *)    { return (int)(size_t)func_ov029_02111ea4((int *)s); }
/* slot 17 (D0) is func_ov029_02111ef4, a guess-marked body -- TRAPPED, like the
   Square D0. Real D1 at slot 16 is seated. */
extern "C" void hal_fill_float_on_water_platform_wdw_rectangle_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)data_ov029_02113f44;
    ov29_fill_shared(vt);
    vt[0]=(void *)rc_init; vt[3]=(void *)rc_clean; vt[6]=(void *)rc_beh;
    vt[9]=(void *)rc_ren;  vt[16]=(void *)rc_d1;   vt[17]=(void *)ov29_trap17;
    vt[31]=(void *)ov29_kill;
}

/* id 94 RotatingPlatformWdw (_ZTV32FloatOnWaterPlatformWdwRectangle, Platform 32;
   s6/9 inherit the ov002 rotating base) */
static int __fastcall rp_init(void *s, void *)  { return _ZN32FloatOnWaterPlatformWdwRectangle13InitResourcesEv(s); }
static int __fastcall rp_clean(void *s, void *) { return _ZN32FloatOnWaterPlatformWdwRectangle16CleanupResourcesEv(s); }
static int __fastcall rp_beh(void *s, void *)   { return func_ov002_020b6718(s); }
static int __fastcall rp_ren(void *s, void *)   { return func_ov002_020b66f0(s); }
static int __fastcall rp_d1(void *s, void *)    { return (int)(size_t)_ZN32FloatOnWaterPlatformWdwRectangleD1Ev((int *)s); }
static int __fastcall rp_d0(void *s, void *)    { return (int)(size_t)_ZN32FloatOnWaterPlatformWdwRectangleD0Ev((int *)s); }
extern "C" void hal_fill_rotating_platform_wdw_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV32FloatOnWaterPlatformWdwRectangle;
    ov29_fill_shared(vt);
    vt[0]=(void *)rp_init; vt[3]=(void *)rp_clean; vt[6]=(void *)rp_beh;
    vt[9]=(void *)rp_ren;  vt[16]=(void *)rp_d1;   vt[17]=(void *)rp_d0;
    vt[31]=(void *)ov29_kill;
}

/* id 98 SwitchActivatedPlank (_ZTV20SwitchActivatedPlank, Platform 32) */
static int __fastcall sp_init(void *s, void *)  { return _ZN20SwitchActivatedPlank13InitResourcesEv(s); }
static int __fastcall sp_clean(void *s, void *) { return _ZN20SwitchActivatedPlank16CleanupResourcesEv(s); }
static int __fastcall sp_beh(void *s, void *)   { return _ZN20SwitchActivatedPlank8BehaviorEv(s); }
static int __fastcall sp_ren(void *s, void *)   { return _ZN20SwitchActivatedPlank6RenderEv(s); }
static int __fastcall sp_d1(void *s, void *)    { return (int)(size_t)_ZN20SwitchActivatedPlankD1Ev((int *)s); }
static int __fastcall sp_d0(void *s, void *)    { return (int)(size_t)_ZN20SwitchActivatedPlankD0Ev((int *)s); }
extern "C" void hal_fill_switch_activated_plank_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV20SwitchActivatedPlank;
    ov29_fill_shared(vt);
    vt[0]=(void *)sp_init; vt[3]=(void *)sp_clean; vt[6]=(void *)sp_beh;
    vt[9]=(void *)sp_ren;  vt[16]=(void *)sp_d1;   vt[17]=(void *)sp_d0;
    vt[31]=(void *)ov29_kill;
}

/* id 101 WDW_Water (_ZTV19RotatingPlatformWdw, Platform 32) */
static int __fastcall ww_init(void *s, void *)  { return _ZN19RotatingPlatformWdw13InitResourcesEv(s); }
static int __fastcall ww_clean(void *s, void *) { return _ZN19RotatingPlatformWdw16CleanupResourcesEv(s); }
static int __fastcall ww_beh(void *s, void *)   { return _ZN19RotatingPlatformWdw8BehaviorEv(s); }
static int __fastcall ww_ren(void *s, void *)   { return _ZN19RotatingPlatformWdw6RenderEv(s); }
static int __fastcall ww_d1(void *s, void *)    { return (int)(size_t)_ZN19RotatingPlatformWdwD1Ev((int *)s); }
static int __fastcall ww_d0(void *s, void *)    { return (int)(size_t)_ZN19RotatingPlatformWdwD0Ev((int *)s); }
extern "C" void hal_fill_wdw_water_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV19RotatingPlatformWdw;
    ov29_fill_shared(vt);
    vt[0]=(void *)ww_init; vt[3]=(void *)ww_clean; vt[6]=(void *)ww_beh;
    vt[9]=(void *)ww_ren;  vt[16]=(void *)ww_d1;   vt[17]=(void *)ww_d0;
    vt[31]=(void *)ov29_kill;
}
