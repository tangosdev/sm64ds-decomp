// GATES 64-69: the vtables of Whomp's Fortress' ENEMY-family cast (ov079).
//
// Same law as hal/actor_classes.cpp and hal/actor_classes_bob_enemy.cpp -- ROM
// slot order, __fastcall thunks that call QUALIFIED, unhosted slots trap by
// name -- and the THIRTY-ONE / THIRTY-TWO slot shape, because these are Actor
// subclasses (through Enemy or Platform), not the twenty-slot ActorBase shape.
//
// ---- what ov079 carries, and how each id was settled -----------------------
//
// Six classes, resolved id -> class -> vtable from the arm9 ACTOR_SPAWN_TABLE
// (0x02090864, entries relocate straight into this overlay) and each SpawnInfo's
// own +4 halfword. The task's shorthand ids were WRONG -- the ROM's WHOMP is
// 164, not 47 -- so every row below is the ROM's number:
//
//   WHOMP (164) and WHOMP_KING (165)  -> _ZTV5Whomp (0x02127c80, 31 slots).
//        ONE CLASS. Whomp_Spawn and WhompKing_Spawn build the same object with
//        the same table; the SpawnInfo param picks the king variant. Its
//        Behavior dispatches a pointer-to-member (host copy, see
//        port/unmatched/Whomp_Behavior.cpp).
//   BULLET_BILL (222)  -> _ZTV10BulletBill (0x02127ee8, also _ZTV7daKlr_c).
//        Its Behavior dispatches a two-entry PMF table (host copy,
//        port/unmatched/BulletBill_Behavior.cpp).
//   BILL_BLASTER (43)  -> data_ov079_02127fb8, a table dsd left as a PLAIN DATA
//        symbol (the RotatingPlatformWf case). All seven overrides are
//        func_ov079_* free functions; no host copy, no state seat.
//   FORTRESS_WALL (47) and FORTRESS_WALL_BREAKABLE (48)  -> _ZTV12FortressWall
//        (0x021280b0, 32 slots). ONE CLASS, a Platform. Both factories build
//        the same object; FortressWall::Behavior is plain (no PMF), so it goes
//        in the slice as matched src.
//
// ---- THE VTABLES ARE HOST STORAGE ------------------------------------------
//
// All four are excluded from the ov079 per-symbol mount (ov079_syms.txt) and
// declared here as host arrays the registry fills, the ov080/ov015 rule: a
// mounted vtable would hand a factory DS code addresses. Whomp's D1/D0 and
// BulletBill's D1/D0 restore their vtable by RTTI name mid-teardown, so the
// _ZTV7daKlr_c alias BulletBill answers to is aliased onto its host array.
//
// SLOT 30 TRAPS on the three Enemy classes (Whomp, BulletBill, BillBlaster) for
// the same reason the Bob-omb cast's does: Actor::OnAimedAtWithEggReturnVec
// returns a Vector3 by value, an ABI a thunk cannot bridge, and nothing aims a
// Yoshi egg at anything as Mario. FortressWall is a Platform whose slot 30 is
// the arm9 base body, filled by the shared pass.
#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);          /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a); /* slot 2 */
int _ZN5Actor14BeforeBehaviorEv(void *self);               /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                 /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                /* slot 18 */
int _ZN5Actor9Virtual50Ev(void *self);                     /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);  /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);      /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);      /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);          /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);          /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);      /* 28 */

extern int data_02099f24[];          /* the frame phase the lists are in */
extern unsigned char data_020a4b4c;  /* the spawn spine's own step */
const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
  void port_actor_slot_decline(const char *what);  /* func_02043fdc.cpp: per-actor decline */
void port_actor_render_probe(const char *cls, void *model); /* actor_classes */

/* the Enemy tier's own eight-entry death table, seated the first time an
   Enemy-family class registers (idempotent) -- port/unmatched/Enemy_UpdateDeath */
void port_enemy_death_states_seat(void);
}

// ---- the trap --------------------------------------------------------------
static void we_trap_report(void *self, int slot)
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
#define WE_TRAP(n) \
    static int __fastcall we_trap##n(void *s, void *) \
    { we_trap_report(s, n); return 0; }
WE_TRAP(13) WE_TRAP(14) WE_TRAP(16) WE_TRAP(17) WE_TRAP(19) WE_TRAP(30)
#undef WE_TRAP

// ---- the ten shared lifecycle halves plus Actor's tail ---------------------
static int __fastcall we_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall we_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall we_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall we_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall we_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall we_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall we_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall we_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall we_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall we_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall we_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall we_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall we_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall we_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall we_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall we_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall we_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall we_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall we_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall we_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }

/* The shared half of a 31-slot table: Actor's four Before/After pairs,
   ActorBase::OnHeapCreated/OnPendingDestroy, Actor::OnYoshiTryEat, Virtual50
   and the eight combat hooks, plus the traps. A caller writes its own
   0/3/6/9/16/17 and whichever of 12/18/19/26/27/29/30 it overrides. */
static void we31_fill_shared(void **vt)
{
    vt[1] = (void *)we_binit;
    vt[2] = (void *)we_ainit;
    vt[4] = (void *)we_bclean;
    vt[5] = (void *)we_aclean;
    vt[7] = (void *)we_bbeh;
    vt[8] = (void *)we_abeh;
    vt[10] = (void *)we_bren;
    vt[11] = (void *)we_aren;
    vt[12] = (void *)we_pdes;
    vt[13] = (void *)we_trap13;
    vt[14] = (void *)we_trap14;
    vt[15] = (void *)we_heap;
    vt[16] = (void *)we_trap16;
    vt[17] = (void *)we_trap17;
    vt[18] = (void *)we_yoshi;
    vt[19] = (void *)we_trap19;
    vt[20] = (void *)we_v50;
    vt[21] = (void *)we_pounded;
    vt[22] = (void *)we_atk1;
    vt[23] = (void *)we_atk2;
    vt[24] = (void *)we_kicked;
    vt[25] = (void *)we_pushed;
    vt[26] = (void *)we_cannon;
    vt[27] = (void *)we_mega;
    vt[28] = (void *)we_under;
    vt[29] = (void *)we_trap19;   /* overwritten where a class overrides it */
    vt[30] = (void *)we_trap30;
}

// ============================================================================
// WHOMP (actor 164) and WHOMP_KING (actor 165), _ZTV5Whomp @ ov079 0x02127c80
// ============================================================================
//
// One class. Behavior is a host copy (the PMF, port/unmatched/Whomp_Behavior).
// It overrides slots 27 (OnHitByMegaChar), 29 (OnAimedAtWithEgg) and 30's
// neighbour -- read the table: 27 is func_ov079_02123e60(self, player), 29 is
// func_ov079_02123b60(self) and 30 is func_ov079_02123b54(void). D1/D0 are C-
// named free functions; InitResources/Render/CleanupResources are real methods
// faced below.
//
// Object layout, from Whomp_Spawn: WithMeshClsn at 0x110, ModelAnim at 0x2cc,
// TextureSequence at 0x330, ShadowModel at 0x344, MovingMeshCollider at 0x418,
// 0x610 bytes.
extern "C" {
int _ZN5Whomp13InitResourcesEv(void *self);   /* face: below */
int _ZN5Whomp6RenderEv(void *self);           /* face: below */
int _ZN5Whomp16CleanupResourcesEv(void *self);/* face: below */
int _ZN5Whomp8BehaviorEv(void *self);         /* host copy */
int *_ZN5WhompD1Ev(int *self);
int *_ZN5WhompD0Ev(int *self);
void func_ov079_02123e60(char *self, void *player);  /* slot 27 */
int func_ov079_02123b60(char *self);                 /* slot 29 */
void func_ov079_02123b54(void);                      /* slot 30 veneer */
void func_ov079_02123d4c(int *out, char *self);      /* slot 30, the body */
void *_ZTV5Whomp[31];
}

static int __fastcall whomp_init(void *s, void *)
{ return _ZN5Whomp13InitResourcesEv(s); }
static int __fastcall whomp_clean(void *s, void *)
{ return _ZN5Whomp16CleanupResourcesEv(s); }
static int __fastcall whomp_behavior(void *s, void *)
{ return _ZN5Whomp8BehaviorEv(s); }
static int __fastcall whomp_render(void *s, void *)
{ port_actor_render_probe("WHOMP", (char *)s + 0x2cc);
  return _ZN5Whomp6RenderEv(s); }
static int __fastcall whomp_d1(void *s, void *)
{ return (int)(size_t)_ZN5WhompD1Ev((int *)s); }
static int __fastcall whomp_d0(void *s, void *)
{ return (int)(size_t)_ZN5WhompD0Ev((int *)s); }
static int __fastcall whomp_mega(void *s, void *, void *p)
{ func_ov079_02123e60((char *)s, p); return 0; }
static int __fastcall whomp_aimed(void *s, void *)
{ return func_ov079_02123b60((char *)s); }
/* Slot 30 is OnAimedAtWithEggReturnVec(), which returns a Vector3 BY VALUE:
   MSVC passes the hidden result pointer as the one stack argument and the
   callee returns it in eax and pops it. func_ov079_02123b54 is the ROM's
   `ldr ip,[pc]; bx ip` veneer, which forwards r0/r1 untouched to
   func_ov079_02123d4c(out, self); the veneer's x86 transcription is declared
   void(void) and so drops both, which is why this dispatches the veneer's
   one target directly. Written with two parameters the thunk popped nothing
   and wrote through whatever the stale stack word pointed at. */
static void *__fastcall whomp_s30(void *s, void *, void *out)
{ func_ov079_02123d4c((int *)out, (char *)s); return out; }

extern "C" void port_whomp_states_seat(void);   /* port/unmatched/Whomp_Behavior */

static void hal_fill_whomp_vtable(void)
{
    void **vt = _ZTV5Whomp;
    port_enemy_death_states_seat();
    port_whomp_states_seat();
    we31_fill_shared(vt);
    vt[0] = (void *)whomp_init;
    vt[3] = (void *)whomp_clean;
    vt[6] = (void *)whomp_behavior;
    vt[9] = (void *)whomp_render;
    vt[16] = (void *)whomp_d1;
    vt[17] = (void *)whomp_d0;
    vt[27] = (void *)whomp_mega;
    vt[29] = (void *)whomp_aimed;
    vt[30] = (void *)whomp_s30;
}

// ============================================================================
// BULLET_BILL (actor 222), _ZTV10BulletBill / _ZTV7daKlr_c @ ov079 0x02127ee8
// ============================================================================
//
// Behavior is a host copy (the two-entry PMF, port/unmatched/BulletBill_Behavior).
// It overrides slot 29 (func_ov079_021266fc, self-less). InitResources/Cleanup
// and D1/D0 are C-named free functions; only Render is a method faced below.
//
// Object layout, from BulletBill_Spawn: MovingCylinderClsnWithPos at 0x110,
// WithMeshClsn at 0x150, Model at 0x30c and 0x35c, ShadowModel at 0x3ac,
// 0x3e0 bytes.
extern "C" {
int _ZN10BulletBill13InitResourcesEv(void *self);   /* C-free */
int _ZN10BulletBill16CleanupResourcesEv(void);       /* C-free */
int _ZN10BulletBill6RenderEv(void *self);            /* face: below */
int _ZN10BulletBill8BehaviorEv(void *self);          /* host copy */
int *_ZN10BulletBillD1Ev(int *self);
int *_ZN10BulletBillD0Ev(int *self);
int func_ov079_021266fc(void);                       /* slot 29 */
void *_ZTV10BulletBill[31];
}
#pragma comment(linker, "/alternatename:__ZTV7daKlr_c=__ZTV10BulletBill")

static int __fastcall klr_init(void *s, void *)
{ return _ZN10BulletBill13InitResourcesEv(s); }
static int __fastcall klr_clean(void *, void *)
{ return _ZN10BulletBill16CleanupResourcesEv(); }
static int __fastcall klr_behavior(void *s, void *)
{ return _ZN10BulletBill8BehaviorEv(s); }
static int __fastcall klr_render(void *s, void *)
{ port_actor_render_probe("BULLET_BILL", (char *)s + 0x30c);
  return _ZN10BulletBill6RenderEv(s); }
static int __fastcall klr_d1(void *s, void *)
{ return (int)(size_t)_ZN10BulletBillD1Ev((int *)s); }
static int __fastcall klr_d0(void *s, void *)
{ return (int)(size_t)_ZN10BulletBillD0Ev((int *)s); }
static int __fastcall klr_aimed(void *, void *)
{ return func_ov079_021266fc(); }

extern "C" void port_bullet_bill_states_seat(void); /* port/unmatched/BulletBill_Behavior */

static void hal_fill_bullet_bill_vtable(void)
{
    void **vt = _ZTV10BulletBill;
    port_enemy_death_states_seat();
    port_bullet_bill_states_seat();
    we31_fill_shared(vt);
    vt[0] = (void *)klr_init;
    vt[3] = (void *)klr_clean;
    vt[6] = (void *)klr_behavior;
    vt[9] = (void *)klr_render;
    vt[16] = (void *)klr_d1;
    vt[17] = (void *)klr_d0;
    vt[29] = (void *)klr_aimed;
}

// ============================================================================
// BILL_BLASTER (actor 43), vtable data_ov079_02127fb8 @ ov079 0x02127fb8
// ============================================================================
//
// dsd left the table a plain data symbol (the RotatingPlatformWf case), so it
// is excluded from the mount and declared here as the host array
// BillBlaster_Spawn installs. Every override is a func_ov079_* free function:
// slot 0 init (02127090), 3 cleanup (02126f04), 6 behavior (02126f8c -- plain,
// no PMF), 9 render (02126f64), 16/17 dtor (02126dbc/02126e00) and 27
// OnHitByMegaChar (02126ecc, self+player). No host copy, no state seat.
extern "C" {
int func_ov079_02127090(void *self);          /* slot 0  */
int func_ov079_02126f04(char *self);          /* slot 3  */
int func_ov079_02126f8c(char *self);          /* slot 6  */
int func_ov079_02126f64(void *self);          /* slot 9  */
int *func_ov079_02126dbc(int *self);          /* slot 16 */
int *func_ov079_02126e00(int *self);          /* slot 17 */
void func_ov079_02126ecc(char *self, void *player); /* slot 27 */
int func_ov079_02126e58(char *self);          /* slot 31, its own Kill */
/* THIRTY-TWO slots, not 31. BillBlaster is a dBgActor_c (Platform) subclass,
   so its table has Platform's tail slot, and here it is the class's OWN Kill
   (ov079 0x02126e58: a poof particle, dust, bank-3 sound 0xf, then
   ActorBase::MarkForDestruction). At 31 * 4 bytes slot 31 lay past the end and
   nothing ever wrote it -- the same wild call as the ROTATING_BRIDGE repro in
   hal/actor_classes_wf.cpp, in Whomp's Fortress, on a Bill Blaster that gets
   killed.
   Declared as bytes rather than void*[] because BillBlaster_Spawn installs it
   through a char* face; the count is the slot count times four, so it moves
   with the slot count. */
unsigned char data_ov079_02127fb8[32 * 4];    /* the host vtable BillBlaster_Spawn installs */
}

static int __fastcall blz_init(void *s, void *)
{ return func_ov079_02127090(s); }
static int __fastcall blz_clean(void *s, void *)
{ return func_ov079_02126f04((char *)s); }
static int __fastcall blz_behavior(void *s, void *)
{ return func_ov079_02126f8c((char *)s); }
static int __fastcall blz_render(void *s, void *)
{ return func_ov079_02126f64(s); }
static int __fastcall blz_d1(void *s, void *)
{ return (int)(size_t)func_ov079_02126dbc((int *)s); }
static int __fastcall blz_d0(void *s, void *)
{ return (int)(size_t)func_ov079_02126e00((int *)s); }
static int __fastcall blz_mega(void *s, void *, void *p)
{ func_ov079_02126ecc((char *)s, p); return 0; }
/* slot 31, the Platform tail. BillBlaster overrides Platform::Kill with its
   own body, which IS in the build (slice_gate64), so this runs the real thing
   rather than declining. */
static int __fastcall blz_kill(void *s, void *)
{ return func_ov079_02126e58((char *)s); }

static void hal_fill_bill_blaster_vtable(void)
{
    void **vt = (void **)data_ov079_02127fb8;
    we31_fill_shared(vt);
    vt[0] = (void *)blz_init;
    vt[3] = (void *)blz_clean;
    vt[6] = (void *)blz_behavior;
    vt[9] = (void *)blz_render;
    vt[16] = (void *)blz_d1;
    vt[17] = (void *)blz_d0;
    vt[27] = (void *)blz_mega;
    vt[31] = (void *)blz_kill;
}

// ============================================================================
// FORTRESS_WALL (actor 47) and FORTRESS_WALL_BREAKABLE (actor 48),
// _ZTV12FortressWall @ ov079 0x021280b0
// ============================================================================
//
// One class, a Platform (through Actor). THIRTY-TWO slots -- one past the usual
// 31 -- with slot 31 being func_ov079_02127280, a class-specific tail virtual.
// Behavior is PLAIN (no PMF): src/_ZN12FortressWall8BehaviorEv.cpp is matched
// and goes in the slice. InitResources/Render/CleanupResources are methods
// faced below; D1/D0 are C-named free functions. Slot 26 override is
// func_ov079_021272e0 (OnHitByCannonBlastedChar). It is NOT an Enemy, so no
// death-states seat.
extern "C" {
int _ZN12FortressWall13InitResourcesEv(void *self);   /* face: below */
int _ZN12FortressWall6RenderEv(void *self);           /* face: below */
int _ZN12FortressWall16CleanupResourcesEv(void *self);/* face: below */
int _ZN12FortressWall8BehaviorEv(void *self);         /* matched src (slice) */
int *_ZN12FortressWallD1Ev(int *self);
int *_ZN12FortressWallD0Ev(int *self);
void func_ov079_021272e0(void *self);                 /* slot 26 */
void func_ov079_02127280(char *self);                 /* slot 31 */
void *_ZTV12FortressWall[32];
}

static int __fastcall fw_init(void *s, void *)
{ return _ZN12FortressWall13InitResourcesEv(s); }
static int __fastcall fw_clean(void *s, void *)
{ return _ZN12FortressWall16CleanupResourcesEv(s); }
static int __fastcall fw_behavior(void *s, void *)
{ return _ZN12FortressWall8BehaviorEv(s); }
static int __fastcall fw_render(void *s, void *)
{ return _ZN12FortressWall6RenderEv(s); }
static int __fastcall fw_d1(void *s, void *)
{ return (int)(size_t)_ZN12FortressWallD1Ev((int *)s); }
static int __fastcall fw_d0(void *s, void *)
{ return (int)(size_t)_ZN12FortressWallD0Ev((int *)s); }
static int __fastcall fw_cannon(void *s, void *, void *)
{ func_ov079_021272e0(s); return 0; }
static int __fastcall fw_s31(void *s, void *)
{ func_ov079_02127280((char *)s); return 0; }

static void hal_fill_fortress_wall_vtable(void)
{
    void **vt = _ZTV12FortressWall;
    we31_fill_shared(vt);
    vt[0] = (void *)fw_init;
    vt[3] = (void *)fw_clean;
    vt[6] = (void *)fw_behavior;
    vt[9] = (void *)fw_render;
    vt[16] = (void *)fw_d1;
    vt[17] = (void *)fw_d0;
    /* slot 26 is FortressWall's own OnHitByCannonBlastedChar */
    vt[26] = (void *)fw_cannon;
    /* slot 31 is the class-specific tail virtual */
    vt[31] = (void *)fw_s31;
}

// ---- the registry entry points ---------------------------------------------
extern "C" void hal_fill_whomp_vtable_c(void) { hal_fill_whomp_vtable(); }
extern "C" void hal_fill_bullet_bill_vtable_c(void) { hal_fill_bullet_bill_vtable(); }
extern "C" void hal_fill_bill_blaster_vtable_c(void) { hal_fill_bill_blaster_vtable(); }
extern "C" void hal_fill_fortress_wall_vtable_c(void) { hal_fill_fortress_wall_vtable(); }

// ---- method faces ----------------------------------------------------------
// The C-named references the vtables take onto definitions that are real MSVC
// methods against include/. Everything else in ov079 is already a C-named free
// function in its own TU (the D1/D0, the func_ov079_*, the two Behaviors that
// became host copies).
#include "Whomp.h"
#include "BulletBill.h"
#include "FortressWall.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN5Whomp13InitResourcesEv(void *self)
{ return ((Whomp *)self)->Whomp::InitResources(); }
#else
int _ZN5Whomp13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN5Whomp13InitResourcesEv */
#endif /* _WIN32 */
/* Whomp::Render is NOT faced here: it dispatches ModelAnim slot 5 through a
   local six-virtual shadow (the ROM Render), which the host _ZTV9ModelAnim
   array numbers as Virtual18. src/_ZN5Whomp6RenderEv.cpp is dropped from
   slice_gate64.txt and _ZN5Whomp6RenderEv is the host copy in
   port/unmatched/ModelAnim_Renders.cpp, the Butterfly/Fish/QuestionBlock case. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN5Whomp16CleanupResourcesEv(void *self)
{ return ((Whomp *)self)->Whomp::CleanupResources(); }
#else
int _ZN5Whomp16CleanupResourcesEv(void *self);  /* Linux: real symbol from src/_ZN5Whomp16CleanupResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN10BulletBill6RenderEv(void *self)
{ return ((BulletBill *)self)->BulletBill::Render(); }
#else
int _ZN10BulletBill6RenderEv(void *self);  /* Linux: real symbol from src/_ZN10BulletBill6RenderEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN12FortressWall13InitResourcesEv(void *self)
{ return ((FortressWall *)self)->FortressWall::InitResources(); }
#else
int _ZN12FortressWall13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN12FortressWall13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN12FortressWall6RenderEv(void *self)
{ return ((FortressWall *)self)->FortressWall::Render(); }
#else
int _ZN12FortressWall6RenderEv(void *self);  /* Linux: real symbol from src/_ZN12FortressWall6RenderEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN12FortressWall16CleanupResourcesEv(void *self)
{ return ((FortressWall *)self)->FortressWall::CleanupResources(); }
#else
int _ZN12FortressWall16CleanupResourcesEv(void *self);  /* Linux: real symbol from src/_ZN12FortressWall16CleanupResourcesEv */
#endif /* _WIN32 */
}
