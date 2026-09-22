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
// The dead first store each _Spawn/D1 makes (an ov002 base table or _ZTV10dBgActor_c)
// and the recovered VT0/VT1/VT2 placeholders are bound per source in
// port/CMakeLists.txt, because the same recovered name means different addresses
// in different TUs (daObjWc_Obj07_c_classInit's _ZTV15daObjWc_Obj07_c is the
// ov002 base, the RectangleD1 TU's is 0x02114018).

#include "port_d16.h"

#include <cstdio>

/* hal/actor_slot30_seat.cpp -- the shared seat for vtable slot 30,
   Actor::OnAimedAtWithEggReturnVec. The ROM word in slot 30 of every vtable
   this file fills IS the arm9 base body 0x020100dc (checked against
   config/<module>/relocs.txt at vtable+30*4), and that body is now in the
   link from src/_ZN8dActor_c25OnAimedAtWithEggReturnVecEv.cpp on slice_gate50.
   The three-parameter __fastcall is the sret contract MSVC uses for a
   thiscall member returning a 12-byte struct: this in ecx, the hidden result
   pointer the one (callee-popped) stack argument. Same shape as whomp_s30. */
extern "C" void *__fastcall port_actor_s30_base(void *self, void *, void *out);
#include "dsstate_seg.h"
#include "dtor_faces_cpp.h"
#include "dActor_c.h"
#include "fBase_c.h"

extern "C" {
/* shared arm9 Actor/ActorBase bodies (slots 1..30), same set the ov025 lane
   forwards, all already in walk_window.map */
int  _ZN8dActor_c19BeforeInitResourcesEv(void *self);
void _ZN8dActor_c18AfterInitResourcesEj(void *self, unsigned a);
int  _ZN8dActor_c14BeforeBehaviorEv(void *self);
int  _ZN8dActor_c12BeforeRenderEv(void *self);
int  _ZN8dActor_c13OnYoshiTryEatEv(void *self);
void _ZN8dActor_c13OnTurnIntoEggER6Player(void *self, void *p);
int  _ZN8dActor_c9Virtual50Ev(void *self);
void _ZN8dActor_c15OnGroundPoundedERS_(void *self, void *o);
void _ZN8dActor_c11OnAttacked1ERS_(void *self, void *o);
void _ZN8dActor_c11OnAttacked2ERS_(void *self, void *o);
void _ZN8dActor_c8OnKickedERS_(void *self, void *o);
void _ZN8dActor_c8OnPushedERS_(void *self, void *o);
void _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void *self, void *o);
void _ZN8dActor_c15OnHitByMegaCharER6Player(void *self, void *p);
void _ZN8dActor_c19OnHitFromUnderneathERS_(void *self, void *o);
int  _ZN8dActor_c16OnAimedAtWithEggEv(void *self);
void _ZN10dBgActor_c4KillEv(void *self);

const char *port_actor_class_name(unsigned id);
void port_actor_slot_decline(const char *what);
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

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
int _ZN17daObjFloatBoard_c16CleanupResourcesEv(void *self);
int _ZN17daObjFloatBoard_c8BehaviorEv(void *self);
int _ZN17daObjFloatBoard_c6RenderEv(void *self);
int _ZN16daObjKaitendai_c8BehaviorEv(void *self);
int _ZN16daObjKaitendai_c6RenderEv(void *self);

/* id 95 FloatOnWaterPlatformWdwSquare own bodies (unnamed; s17 D0 is a guessed
   body, trapped not seated -- no extern) */
int  _ZN14daObjWcObj01_c13InitResourcesEv(char *self);   /* s0 InitResources */
int *_ZN14daObjWcObj01_cD1Ev(int *self);    /* s16 D1 */
int *_ZN14daObjWcObj01_cD0Ev(int *self);    /* s17 D0 -- GATE 229, ruled and seated */
/* id 96 ArrowLift, live table _ZTV15daObjWc_Obj02_c (mangled bodies) */
int _ZN15daObjWc_Obj02_c13InitResourcesEv(void *self);
int _ZN15daObjWc_Obj02_c16CleanupResourcesEv(void *self);
int _ZN15daObjWc_Obj02_c8BehaviorEv(void *self);
int _ZN15daObjWc_Obj02_c6RenderEv(void *self);
int *_ZN15daObjWc_Obj02_cD1Ev(int *self);
int *_ZN15daObjWc_Obj02_cD0Ev(int *self);
/* id 97 WaterDiamond, live table _ZTV9ArrowLift (Actor 31) */
int _ZN15daObjWc_Obj03_c13InitResourcesEv(void *self);
int _ZN15daObjWc_Obj03_c16CleanupResourcesEv(void *self);
int _ZN15daObjWc_Obj03_c8BehaviorEv(void *self);
int _ZN15daObjWc_Obj03_c6RenderEv(void *self);
int *_ZN15daObjWc_Obj03_cD0Ev(int *self);   /* D1 trapped: see WaterDiamond fill */
/* id 99 CageLift (link100 SPAWN2). Its six own bodies, every one taken from the
   reloc at _ZTV15daObjWc_Obj05_c + 4*slot and confirmed by a
   kind:function(arm,size=..) record at exactly that address in
   config/arm9/overlays/ov029/symbols.txt:
     slot 0  0x02113e74 -> 0x02111d6c  _ZN15daObjWc_Obj05_c13InitResourcesEv size 0xd4   (.cpp)
     slot 3  0x02113e80 -> 0x02111b60  _ZN15daObjWc_Obj05_c16CleanupResourcesEv size 0x44
     slot 6  0x02113e8c -> 0x02111bcc  _ZN15daObjWc_Obj05_c8BehaviorEv size 0x1a0
     slot 9  0x02113e98 -> 0x02111ba4  _ZN15daObjWc_Obj05_c6RenderEv size 0x28   (.cpp)
     slot 16 0x02113eb4 -> 0x02111ac4  _ZN15daObjWc_Obj05_cD1Ev size 0x44
     slot 17 0x02113eb8 -> 0x02111b08  _ZN15daObjWc_Obj05_cD0Ev size 0x58
   Five of the six carry the "recovered from vtable slot identity" marker and
   lane STUBADJ ruled ALL FIVE REAL_DECOMP against the ROM (match.py 2004/b56
   strict-reloc MATCH, 0 WRONG-DEST), which is the fact that retires the note
   that used to stand where this block is. The sixth (the D1) never carried a
   marker. */
int  _ZN15daObjWc_Obj05_c13InitResourcesEv(char *self);   /* s0  InitResources */
int  _ZN15daObjWc_Obj05_c16CleanupResourcesEv(void *self);   /* s3  CleanupResources */
int  _ZN15daObjWc_Obj05_c8BehaviorEv(void *self);   /* s6  Behavior */
int  _ZN15daObjWc_Obj05_c6RenderEv(void *self);   /* s9  Render */
int *_ZN15daObjWc_Obj05_cD1Ev(int *self);    /* s16 D1 */
int *_ZN15daObjWc_Obj05_cD0Ev(int *self);    /* s17 D0 */
/* id 100 FloatOnWaterPlatformWdwRectangle own bodies (unnamed; s17 D0 guessed,
   trapped not seated) */
int  _ZN14daObjWcObj06_c13InitResourcesEv(char *self);   /* s0 */
int *_ZN14daObjWcObj06_cD1Ev(int *self);    /* s16 */
int *_ZN14daObjWcObj06_cD0Ev(int *self);    /* s17 D0 -- GATE 229, ruled and seated */
/* id 94 RotatingPlatformWdw, live table _ZTV15daObjWc_Obj07_c */
int _ZN15daObjWc_Obj07_c13InitResourcesEv(void *self);
int _ZN15daObjWc_Obj07_c16CleanupResourcesEv(void *self);
int *_ZN15daObjWc_Obj07_cD1Ev(int *self);
int *_ZN15daObjWc_Obj07_cD0Ev(int *self);
/* id 98 SwitchActivatedPlank, live table _ZTV20SwitchActivatedPlank */
int _ZN15daObjWc_Obj04_c13InitResourcesEv(void *self);
int _ZN15daObjWc_Obj04_c16CleanupResourcesEv(void *self);
int _ZN15daObjWc_Obj04_c8BehaviorEv(void *self);
int _ZN15daObjWc_Obj04_c6RenderEv(void *self);
int *_ZN15daObjWc_Obj04_cD1Ev(int *self);
int *_ZN15daObjWc_Obj04_cD0Ev(int *self);
/* id 101 WDW_Water, live table _ZTV19RotatingPlatformWdw */
int _ZN14daObjWc_Mizu_c13InitResourcesEv(void *self);
int _ZN14daObjWc_Mizu_c16CleanupResourcesEv(void *self);
int _ZN14daObjWc_Mizu_c8BehaviorEv(void *self);
int _ZN14daObjWc_Mizu_c6RenderEv(void *self);
int *_ZN14daObjWc_Mizu_cD1Ev(int *self);
int *_ZN14daObjWc_Mizu_cD0Ev(int *self);

/* the eight live vtables. Five carry the dsd _ZTV name (the matched _Spawn/D1
   TUs spell them); three are unnamed and hosted under their ROM address. Sized
   by vtspan: 32 slots for the Platform tables (Kill at 31), 31 for the Actor
   table (id 97, ends at slot 30). DSSTATE so a savestate restore refills. */
DSSTATE_BEGIN
void *_ZTV14daObjWcObj01_c[32];                    /* id 95  Square       */
void *_ZTV15daObjWc_Obj02_c[32];    /* id 96  ArrowLift     */
void *_ZTV9ArrowLift[31];                          /* id 97  WaterDiamond  */
void *_ZTV15daObjWc_Obj05_c[32];                    /* id 99  CageLift      */
void *_ZTV14daObjWcObj06_c[32];                    /* id 100 Rectangle     */
void *_ZTV15daObjWc_Obj07_c[32]; /* id 94  RotatingWdw   */
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
static int  __fastcall ov29_binit(void *s, void *) { return _ZN8dActor_c19BeforeInitResourcesEv(s); }
static void __fastcall ov29_ainit(void *s, void *, unsigned a) { _ZN8dActor_c18AfterInitResourcesEj(s, a); }
static int  __fastcall ov29_bclean(void *s, void *) { return ((dActor_c *)s)->dActor_c::BeforeCleanupResources(); }
static void __fastcall ov29_aclean(void *s, void *, unsigned a) { ((fBase_c *)s)->fBase_c::AfterCleanupResources(a); }
static int  __fastcall ov29_bbeh(void *s, void *) { return _ZN8dActor_c14BeforeBehaviorEv(s); }
static void __fastcall ov29_abeh(void *s, void *, unsigned a) { ((fBase_c *)s)->fBase_c::AfterBehavior(a); }
static int  __fastcall ov29_bren(void *s, void *) { return _ZN8dActor_c12BeforeRenderEv(s); }
static void __fastcall ov29_aren(void *s, void *, unsigned a) { ((fBase_c *)s)->fBase_c::AfterRender(a); }
static int  __fastcall ov29_pdes(void *s, void *) { ((fBase_c *)s)->fBase_c::OnPendingDestroy(); return 0; }
static int  __fastcall ov29_heap(void *s, void *) { return ((fBase_c *)s)->fBase_c::OnHeapCreated(); }
static int  __fastcall ov29_yoshi(void *s, void *) { return _ZN8dActor_c13OnYoshiTryEatEv(s); }
static int  __fastcall ov29_turn_egg(void *s, void *, void *p) { _ZN8dActor_c13OnTurnIntoEggER6Player(s, p); return 0; }
static int  __fastcall ov29_v50(void *s, void *) { return _ZN8dActor_c9Virtual50Ev(s); }
static int  __fastcall ov29_pounded(void *s, void *, void *o) { _ZN8dActor_c15OnGroundPoundedERS_(s, o); return 0; }
static int  __fastcall ov29_atk1(void *s, void *, void *o) { _ZN8dActor_c11OnAttacked1ERS_(s, o); return 0; }
static int  __fastcall ov29_atk2(void *s, void *, void *o) { _ZN8dActor_c11OnAttacked2ERS_(s, o); return 0; }
static int  __fastcall ov29_kicked(void *s, void *, void *o) { _ZN8dActor_c8OnKickedERS_(s, o); return 0; }
static int  __fastcall ov29_pushed(void *s, void *, void *o) { _ZN8dActor_c8OnPushedERS_(s, o); return 0; }
static int  __fastcall ov29_cannon(void *s, void *, void *o) { _ZN8dActor_c24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int  __fastcall ov29_mega(void *s, void *, void *p) { _ZN8dActor_c15OnHitByMegaCharER6Player(s, p); return 0; }
static int  __fastcall ov29_under(void *s, void *, void *o) { _ZN8dActor_c19OnHitFromUnderneathERS_(s, o); return 0; }
static int  __fastcall ov29_egg(void *s, void *) { return _ZN8dActor_c16OnAimedAtWithEggEv(s); }
static int  __fastcall ov29_kill(void *s, void *) { _ZN10dBgActor_c4KillEv(s); return 0; }

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
/* id 95 FloatOnWaterPlatformWdwSquare (_ZTV14daObjWcObj01_c, Platform 32). Its
   inherited s3/6/9 are the ov002 base-platform bodies. */
static int __fastcall sq_init(void *s, void *) { return _ZN14daObjWcObj01_c13InitResourcesEv((char *)s); }
static int __fastcall sq_clean(void *s, void *) { return _ZN17daObjFloatBoard_c16CleanupResourcesEv(s); }
static int __fastcall sq_beh(void *s, void *)   { return _ZN17daObjFloatBoard_c8BehaviorEv(s); }
static int __fastcall sq_ren(void *s, void *)   { return _ZN17daObjFloatBoard_c6RenderEv(s); }
static int __fastcall sq_d1(void *s, void *)    { return (int)(size_t)_ZN14daObjWcObj01_cD1Ev((int *)s); }
/* GATE 229: SLOT 17 IS THE ROM BODY NOW. _ZN14daObjWcObj01_cD0Ev carries the
   "recovered from vtable slot identity" marker, which is why the note that used
   to stand here trapped it -- and lane STUBADJ ruled that marker set against the
   ROM (REAL_DECOMP, port/tools/inferred_stub_adjudicated.txt), so it is a
   decompilation after all. Checked by address before seating, not by the
   ruling's own text: the table word 0x02113c70 is _ZTV14daObjWcObj01_c + 4*17 and
   relocates to 0x021111f0, and symbols.txt carries
   _ZN14daObjWcObj01_cD0Ev kind:function(arm,size=0x64) at exactly that address.
   Its THREE vptr stores are bound per TU in port/CMakeLists.txt out of its own
   literal pool (own table, the ov002 base 0x02108fdc, then _ZTV10dBgActor_c), which
   is the store the recovered placeholder names could not spell. Two-parameter
   face: slot 17 is a lifecycle slot and the caller pushes nothing. */
static int __fastcall sq_d0(void *s, void *)   { return (int)(size_t)_ZN14daObjWcObj01_cD0Ev((int *)s); }
extern "C" void hal_fill_float_on_water_platform_wdw_square_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV14daObjWcObj01_c;
    ov29_fill_shared(vt);
    vt[0]=(void *)sq_init; vt[3]=(void *)sq_clean; vt[6]=(void *)sq_beh;
    vt[9]=(void *)sq_ren;  vt[16]=(void *)PORT_D16(sq_d1);   vt[17]=(void *)sq_d0;
    vt[31]=(void *)ov29_kill;
}

/* id 96 ArrowLift (_ZTV15daObjWc_Obj02_c, Platform 32) */
static int __fastcall al_init(void *s, void *)  { return _ZN15daObjWc_Obj02_c13InitResourcesEv(s); }
static int __fastcall al_clean(void *s, void *) { return _ZN15daObjWc_Obj02_c16CleanupResourcesEv(s); }
static int __fastcall al_beh(void *s, void *)   { return _ZN15daObjWc_Obj02_c8BehaviorEv(s); }
static int __fastcall al_ren(void *s, void *)   { return _ZN15daObjWc_Obj02_c6RenderEv(s); }
static int __fastcall al_d1(void *s, void *)    { return (int)(size_t)_ZN15daObjWc_Obj02_cD1Ev((int *)s); }
static int __fastcall al_d0(void *s, void *)    { return (int)(size_t)_ZN15daObjWc_Obj02_cD0Ev((int *)s); }
extern "C" void hal_fill_arrow_lift_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV15daObjWc_Obj02_c;
    ov29_fill_shared(vt);
    vt[0]=(void *)al_init; vt[3]=(void *)al_clean; vt[6]=(void *)al_beh;
    vt[9]=(void *)al_ren;  vt[16]=(void *)PORT_D16(al_d1);   vt[17]=(void *)al_d0;
    vt[31]=(void *)ov29_kill;
}

/* id 97 WaterDiamond (_ZTV9ArrowLift, Actor 31 -- NO Kill slot 31) */
static int __fastcall wd_init(void *s, void *)  { return _ZN15daObjWc_Obj03_c13InitResourcesEv(s); }
static int __fastcall wd_clean(void *s, void *) { return _ZN15daObjWc_Obj03_c16CleanupResourcesEv(s); }
static int __fastcall wd_beh(void *s, void *)   { return _ZN15daObjWc_Obj03_c8BehaviorEv(s); }
static int __fastcall wd_ren(void *s, void *)   { return _ZN15daObjWc_Obj03_c6RenderEv(s); }
/* slot 16 (D1) is _ZN15daObjWc_Obj03_cD1Ev, a C++ virtual destructor: since lane
   DTOR-FACES-CPP it links from src through hal/dtor_faces_cpp.cpp's faces and
   is seated here (it was TRAPPED while the base dtors were unhosted). D0
   (slot 17) is the C-linkage _ZN15daObjWc_Obj03_cD0Ev, seated. */
static int __fastcall wd_d0(void *s, void *)    { return (int)(size_t)_ZN15daObjWc_Obj03_cD0Ev((int *)s); }
extern "C" void hal_fill_water_diamond_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV9ArrowLift;
    ov29_fill_shared(vt);
    vt[0]=(void *)wd_init;      vt[3]=(void *)wd_clean; vt[6]=(void *)wd_beh;
    vt[9]=(void *)wd_ren;  vt[16]=(void *)PORT_D16(hal_cppd1_ArrowLift); vt[17]=(void *)wd_d0;
    /* no slot 31: a plain Actor, 31 slots, ends at 30 */
}

/* id 99 CageLift (_ZTV15daObjWc_Obj05_c, Platform 32) -- link100 SPAWN2.
   THE NOTE THAT USED TO STAND HERE IS RETIRED, and by a ruling rather than by a
   rewrite: the five "recovered from vtable slot identity" bodies this class was
   held back for are each ruled REAL_DECOMP in port/tools/
   inferred_stub_adjudicated.txt, every one a match.py 2004/b56 strict-reloc
   MATCH against the ROM with 0 WRONG-DEST. So they are decompilations, not
   guesses, and InitResources can be seated -- which is the single thing the old
   note said the class was waiting on.
   THIS CLASS OWNS ALL SIX lifecycle slots (0/3/6/9/16/17); unlike the Square and
   the Rectangle two classes up it inherits nothing from the ov002 base. Slot 31
   is the shared Platform::Kill 0x020ee55c that every 32-slot table in this file
   carries, and slot 30 the SRET base seat.
   SLOT 9 IS THE SHADOW-DISPATCH SHAPE and it is safe here, adjudicated the way
   hal/actor_classes_ov043.cpp adjudicates DIAMOND_LIFT's byte-identical body:
   InitResources feeds Model::LoadFile into ModelBase::SetFile at +0xd4 and both
   destructors call _ZN5ModelD1Ev on +0xd4, so the object it dispatches slot 5 of
   is a plain Model and hal/cxxname_bridge.cpp's dual-filled _ZTV5Model[5] is
   Model::Render. _ZN19daObjKm1_Ukishima_c6RenderEv.cpp is the same eight lines and has been
   linked and shipping on level 35 since gate 206. */
static int __fastcall cl_init(void *s, void *)  { return _ZN15daObjWc_Obj05_c13InitResourcesEv((char *)s); }
static int __fastcall cl_clean(void *s, void *) { return _ZN15daObjWc_Obj05_c16CleanupResourcesEv(s); }
static int __fastcall cl_beh(void *s, void *)   { return _ZN15daObjWc_Obj05_c8BehaviorEv(s); }
static int __fastcall cl_ren(void *s, void *)
{ port_actor_render_probe("CAGE_LIFT", (char *)s + 0xd4);
  return _ZN15daObjWc_Obj05_c6RenderEv(s); }
static int __fastcall cl_d1(void *s, void *)    { return (int)(size_t)_ZN15daObjWc_Obj05_cD1Ev((int *)s); }
static int __fastcall cl_d0(void *s, void *)    { return (int)(size_t)_ZN15daObjWc_Obj05_cD0Ev((int *)s); }
extern "C" void hal_fill_cage_lift_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV15daObjWc_Obj05_c;
    ov29_fill_shared(vt);
    vt[0]=(void *)cl_init; vt[3]=(void *)cl_clean; vt[6]=(void *)cl_beh;
    vt[9]=(void *)cl_ren;  vt[16]=(void *)PORT_D16(cl_d1);   vt[17]=(void *)cl_d0;
    vt[31]=(void *)ov29_kill;
}

/* id 100 FloatOnWaterPlatformWdwRectangle (_ZTV14daObjWcObj06_c, Platform 32;
   inherited s3/6/9 = ov002 base, same as Square) */
static int __fastcall rc_init(void *s, void *)  { return _ZN14daObjWcObj06_c13InitResourcesEv((char *)s); }
static int __fastcall rc_clean(void *s, void *) { return _ZN17daObjFloatBoard_c16CleanupResourcesEv(s); }
static int __fastcall rc_beh(void *s, void *)   { return _ZN17daObjFloatBoard_c8BehaviorEv(s); }
static int __fastcall rc_ren(void *s, void *)   { return _ZN17daObjFloatBoard_c6RenderEv(s); }
static int __fastcall rc_d1(void *s, void *)    { return (int)(size_t)_ZN14daObjWcObj06_cD1Ev((int *)s); }
/* GATE 229: slot 17 is the ROM body, the same reading as the Square one class
   up. Table word 0x02113f88 = _ZTV14daObjWcObj06_c + 4*17 relocates to
   0x02111ef4, and symbols.txt has _ZN14daObjWcObj06_cD0Ev
   kind:function(arm,size=0x64) there. Same three-store binding out of its own
   pool, same two-parameter face. */
static int __fastcall rc_d0(void *s, void *)   { return (int)(size_t)_ZN14daObjWcObj06_cD0Ev((int *)s); }
extern "C" void hal_fill_float_on_water_platform_wdw_rectangle_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV14daObjWcObj06_c;
    ov29_fill_shared(vt);
    vt[0]=(void *)rc_init; vt[3]=(void *)rc_clean; vt[6]=(void *)rc_beh;
    vt[9]=(void *)rc_ren;  vt[16]=(void *)PORT_D16(rc_d1);   vt[17]=(void *)rc_d0;
    vt[31]=(void *)ov29_kill;
}

/* id 94 RotatingPlatformWdw (_ZTV15daObjWc_Obj07_c, Platform 32;
   s6/9 inherit the ov002 rotating base) */
static int __fastcall rp_init(void *s, void *)  { return _ZN15daObjWc_Obj07_c13InitResourcesEv(s); }
static int __fastcall rp_clean(void *s, void *) { return _ZN15daObjWc_Obj07_c16CleanupResourcesEv(s); }
static int __fastcall rp_beh(void *s, void *)   { return _ZN16daObjKaitendai_c8BehaviorEv(s); }
static int __fastcall rp_ren(void *s, void *)   { return _ZN16daObjKaitendai_c6RenderEv(s); }
static int __fastcall rp_d1(void *s, void *)    { return (int)(size_t)_ZN15daObjWc_Obj07_cD1Ev((int *)s); }
static int __fastcall rp_d0(void *s, void *)    { return (int)(size_t)_ZN15daObjWc_Obj07_cD0Ev((int *)s); }
extern "C" void hal_fill_rotating_platform_wdw_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV15daObjWc_Obj07_c;
    ov29_fill_shared(vt);
    vt[0]=(void *)rp_init; vt[3]=(void *)rp_clean; vt[6]=(void *)rp_beh;
    vt[9]=(void *)rp_ren;  vt[16]=(void *)PORT_D16(rp_d1);   vt[17]=(void *)rp_d0;
    vt[31]=(void *)ov29_kill;
}

/* id 98 SwitchActivatedPlank (_ZTV20SwitchActivatedPlank, Platform 32) */
static int __fastcall sp_init(void *s, void *)  { return _ZN15daObjWc_Obj04_c13InitResourcesEv(s); }
static int __fastcall sp_clean(void *s, void *) { return _ZN15daObjWc_Obj04_c16CleanupResourcesEv(s); }
static int __fastcall sp_beh(void *s, void *)   { return _ZN15daObjWc_Obj04_c8BehaviorEv(s); }
static int __fastcall sp_ren(void *s, void *)   { return _ZN15daObjWc_Obj04_c6RenderEv(s); }
static int __fastcall sp_d1(void *s, void *)    { return (int)(size_t)_ZN15daObjWc_Obj04_cD1Ev((int *)s); }
static int __fastcall sp_d0(void *s, void *)    { return (int)(size_t)_ZN15daObjWc_Obj04_cD0Ev((int *)s); }
extern "C" void hal_fill_switch_activated_plank_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV20SwitchActivatedPlank;
    ov29_fill_shared(vt);
    vt[0]=(void *)sp_init; vt[3]=(void *)sp_clean; vt[6]=(void *)sp_beh;
    vt[9]=(void *)sp_ren;  vt[16]=(void *)PORT_D16(sp_d1);   vt[17]=(void *)sp_d0;
    vt[31]=(void *)ov29_kill;
}

/* id 101 WDW_Water (_ZTV19RotatingPlatformWdw, Platform 32) */
static int __fastcall ww_init(void *s, void *)  { return _ZN14daObjWc_Mizu_c13InitResourcesEv(s); }
static int __fastcall ww_clean(void *s, void *) { return _ZN14daObjWc_Mizu_c16CleanupResourcesEv(s); }
static int __fastcall ww_beh(void *s, void *)   { return _ZN14daObjWc_Mizu_c8BehaviorEv(s); }
static int __fastcall ww_ren(void *s, void *)   { return _ZN14daObjWc_Mizu_c6RenderEv(s); }
static int __fastcall ww_d1(void *s, void *)    { return (int)(size_t)_ZN14daObjWc_Mizu_cD1Ev((int *)s); }
static int __fastcall ww_d0(void *s, void *)    { return (int)(size_t)_ZN14daObjWc_Mizu_cD0Ev((int *)s); }
extern "C" void hal_fill_wdw_water_vtable(void)
{
    port_ov29_bringup();
    void *volatile *vt = (void *volatile *)_ZTV19RotatingPlatformWdw;
    ov29_fill_shared(vt);
    vt[0]=(void *)ww_init; vt[3]=(void *)ww_clean; vt[6]=(void *)ww_beh;
    vt[9]=(void *)ww_ren;  vt[16]=(void *)PORT_D16(ww_d1);   vt[17]=(void *)ww_d0;
    vt[31]=(void *)ov29_kill;
}
