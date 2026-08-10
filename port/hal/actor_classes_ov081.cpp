// GATE 192: the ov081 trio -- ICE_BLOCK (18), MR_BLIZZARD (223), SPINDRIFT
// (312). Fresh per-symbol mount (port/ov081_syms.txt), the ov079/ov080
// convention. Same law as every fill in this family -- ROM slot order,
// __fastcall thunks that call QUALIFIED or the class's own C body, unhosted
// slots trap by name.
//
// SPINDRIFT (daHuwa_c) and MR_BLIZZARD (daYuki_c) are both the standard
// 32-slot Enemy shape (own Kill at 31); ICE_BLOCK (daObjIceBlock_c) is the
// standard 32-slot Platform shape (own Kill at 31 too, per the scope
// report -- confirmed by reloc target, own body not the shared 0x020ee55c).
// Full slot derivation is in port/slice_gate192.txt and port/ov081_syms.txt.
//
// D1/D0: Spindrift and MrBlizzard's own D1/D0 store their own table
// directly (no shared-placeholder overwrite) and STAY IN THE SLICE, faced
// below. IceBlock's D1/D0 store their own table then overwrite with the
// shared _ZTV8Platform placeholder (the ShipUp/RockPillar/SkiLift shape)
// -- HOST THUNKS, out of the slice.
//
// MR_BLIZZARD'S PMF SEAT: a real ten-cell (5 states x enter/tick) function-
// pointer table, __sinit_ov081_02128154-copied from static const ROM
// records (DS code addresses) into the mutable instance table
// data_ov081_02128e14..ea4. func_ov081_02125488 installs a cell pointer at
// self+0x3f8 and dispatches its "enter" half immediately; MrBlizzard's own
// Behavior (matched src) dispatches the "tick" half every frame through the
// same pointer. Seated once, verify-then-abort on a ROM-address mismatch --
// the SoundObject/Cap treatment.
//
// SPINDRIFT'S PLAYER-BUMP HELPER (func_ov081_02123910, called every
// Behavior tick) reaches _ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player
// on one path, which in turn reads data_ov004_020beb68 -- Bob-omb
// Battlefield's OWN level tracker pointer (ov004), never mounted for
// level 10. Every reader of that pointer (func_ov004_020adc1c/020adc00,
// the KillByInvincibleChar body itself) null-guards it FIRST and returns
// doing nothing when it reads zero -- exactly what happens on the real ROM
// whenever BoB's own tracker isn't active, which is always true outside
// BoB. Declared here as a zero-initialized host global (never written by
// anything in this build) so those guards see the same zero the ROM's own
// unmounted-overlay BSS would read.
extern "C" int data_ov004_020beb68;
int data_ov004_020beb68 = 0;
/* data_0209b308[2] is read INSIDE the same function, but only after the
   guard above already returned -- dead code with data_ov004_020beb68 zero,
   still needs to link. Zero-initialized, never read at runtime. */
extern "C" int data_0209b308[4];
int data_0209b308[4] = {0, 0, 0, 0};

#include <cstdio>
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"

extern "C" {
int _ZN5Actor19BeforeInitResourcesEv(void *self);             /* slot 1  */
void _ZN5Actor18AfterInitResourcesEj(void *self, unsigned a);  /* slot 2  */
int _ZN5Actor14BeforeBehaviorEv(void *self);                  /* slot 7  */
int _ZN5Actor12BeforeRenderEv(void *self);                    /* slot 10 */
int _ZN5Actor13OnYoshiTryEatEv(void *self);                   /* slot 18 */
void _ZN5Actor13OnTurnIntoEggER6Player(void *self, void *p);  /* slot 19 */
int _ZN5Actor9Virtual50Ev(void *self);                        /* slot 20 */
void _ZN5Actor15OnGroundPoundedERS_(void *self, void *o);     /* slot 21 */
void _ZN5Actor11OnAttacked1ERS_(void *self, void *o);         /* slot 22 */
void _ZN5Actor11OnAttacked2ERS_(void *self, void *o);         /* slot 23 */
void _ZN5Actor8OnKickedERS_(void *self, void *o);             /* slot 24 */
void _ZN5Actor8OnPushedERS_(void *self, void *o);             /* slot 25 */
void _ZN5Actor24OnHitByCannonBlastedCharERS_(void *self, void *o); /* slot 26 */
void _ZN5Actor15OnHitByMegaCharER6Player(void *self, void *p);     /* slot 27 */
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o); /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                /* slot 29 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp, Platform's
                                                       own base table */
extern int _ZTV8Platform[];
void _ZN18MovingMeshColliderD1Ev(void *);         /* MovingMeshCollider at +0x124 */
void _ZN5ModelD1Ev(void *);                       /* Model at +0xd4 */
void *_ZN5ActorD2Ev(void *);                      /* the Actor base D2 (returns void*) */
void _ZN6Memory10DeallocateEPvP4Heap(void *, void *);
extern void *data_020a0eac;                       /* the game heap, already hosted */
void *_ZN5EnemyD2Ev(void *);                      /* the Enemy base D2 (returns void*) */
}

// ---- the trap ----------------------------------------------------------
static void ov81_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov081 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov081 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
static int __fastcall ov81_trap13(void *s, void *) { ov81_trap_report(s, 13); return 0; }
static int __fastcall ov81_trap14(void *s, void *) { ov81_trap_report(s, 14); return 0; }
static int __fastcall ov81_trap30(void *s, void *) { ov81_trap_report(s, 30); return 0; }

// ---- the shared 1..30 half, both Enemy tables share it -------------------
static int __fastcall ov81_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov81_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov81_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov81_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov81_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov81_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov81_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov81_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov81_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov81_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov81_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov81_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov81_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov81_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov81_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov81_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov81_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov81_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov81_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov81_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov81_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov81_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }

/* Fills slots 1,2,4,5,7,8,10,11,13,14,15,18..30 -- every shared slot the
   standard 32-slot Enemy/Platform tables share. The caller writes its own
   0/3/6/9/12/16/17/(combat overrides)/31. Slot 12 (OnPendingDestroy) is
   included here for Spindrift/IceBlock, which use the shared default;
   MrBlizzard overrides it with its own body afterward. */
static void ov81_fill_shared(void **vt)
{
    vt[1]  = (void *)ov81_binit;
    vt[2]  = (void *)ov81_ainit;
    vt[4]  = (void *)ov81_bclean;
    vt[5]  = (void *)ov81_aclean;
    vt[7]  = (void *)ov81_bbeh;
    vt[8]  = (void *)ov81_abeh;
    vt[10] = (void *)ov81_bren;
    vt[11] = (void *)ov81_aren;
    vt[12] = (void *)ov81_pdes;
    vt[13] = (void *)ov81_trap13;
    vt[14] = (void *)ov81_trap14;
    vt[15] = (void *)ov81_heap;
    vt[18] = (void *)ov81_yoshi;
    vt[19] = (void *)ov81_egg;
    vt[20] = (void *)ov81_v50;
    vt[21] = (void *)ov81_pounded;
    vt[22] = (void *)ov81_atk1;
    vt[23] = (void *)ov81_atk2;
    vt[24] = (void *)ov81_kicked;
    vt[25] = (void *)ov81_pushed;
    vt[26] = (void *)ov81_cannon;
    vt[27] = (void *)ov81_mega;
    vt[28] = (void *)ov81_under;
    vt[29] = (void *)ov81_aimed;
    vt[30] = (void *)ov81_trap30;
}

// ============================================================================
// SPINDRIFT (312), an Enemy, 32 slots (own Kill)
// ============================================================================
extern "C" {
int _ZN9Spindrift13InitResourcesEv(void *self);      /* slot 0, faced */
int _ZN9Spindrift16CleanupResourcesEv(void);          /* slot 3, .c, void */
int _ZN9Spindrift8BehaviorEv(void *self);             /* slot 6, faced */
int _ZN9Spindrift6RenderEv(void *self);               /* slot 9, faced */
int _ZN9SpindriftD1Ev(void *self);                    /* slot 16, .c, spells own table */
int _ZN9SpindriftD0Ev(void *self);                    /* slot 17, .c, spells own table */
int func_ov081_02124038(void);                        /* slot 18, OnYoshiTryEat -- no
                                                           args at all, returns int */
void func_ov081_02123fd8(void *self, void *p);        /* slot 19, OnTurnIntoEgg */
int func_ov081_021237e4(void);                        /* slot 29, OnAimedAtWithEgg --
                                                           no args, returns int */
int func_ov081_02124e64(void *self);                  /* slot 31, Kill -- returns int;
                                                           ALSO MrBlizzard's own PMF
                                                           cell-0 tick target, one real
                                                           ROM address serving both
                                                           roles, see the note below */
void *Spindrift_Spawn(void);                          /* installs _ZTV9Spindrift */
int _ZTV9Spindrift[32];
}

static int __fastcall spd_init(void *s, void *)
{ return _ZN9Spindrift13InitResourcesEv(s); }
static int __fastcall spd_clean(void *s, void *)
{ (void)s; return _ZN9Spindrift16CleanupResourcesEv(); }
static int __fastcall spd_behavior(void *s, void *)
{ return _ZN9Spindrift8BehaviorEv(s); }
static int __fastcall spd_render(void *s, void *)
{ port_actor_render_probe("SPINDRIFT", (char *)s + 0x110);
  return _ZN9Spindrift6RenderEv(s); }
static int __fastcall spd_d1(void *s, void *)
{ return _ZN9SpindriftD1Ev(s); }
static int __fastcall spd_d0(void *s, void *)
{ return _ZN9SpindriftD0Ev(s); }
static int __fastcall spd_yoshi(void *s, void *)
{ (void)s; return func_ov081_02124038(); }
static int __fastcall spd_egg(void *s, void *, void *p)
{ func_ov081_02123fd8(s, p); return 0; }
static int __fastcall spd_aimed(void *s, void *)
{ (void)s; return func_ov081_021237e4(); }
static int __fastcall spd_kill(void *s, void *)
{ return func_ov081_02124e64(s); }

extern "C" void hal_fill_spindrift_vtable(void)
{
    void **vt = (void **)_ZTV9Spindrift;
    ov81_fill_shared(vt);
    vt[0]  = (void *)spd_init;
    vt[3]  = (void *)spd_clean;
    vt[6]  = (void *)spd_behavior;
    vt[9]  = (void *)spd_render;
    vt[16] = (void *)spd_d1;
    vt[17] = (void *)spd_d0;
    vt[18] = (void *)spd_yoshi;      /* own OnYoshiTryEat, overrides the shared default */
    vt[19] = (void *)spd_egg;        /* own OnTurnIntoEgg */
    vt[29] = (void *)spd_aimed;      /* own OnAimedAtWithEgg */
    vt[31] = (void *)spd_kill;       /* own Kill */
}

// ============================================================================
// MR_BLIZZARD (223), an Enemy, 32 slots (own Kill, own OnPendingDestroy)
// ============================================================================
extern "C" {
int _ZN10MrBlizzard13InitResourcesEv(void *self);     /* slot 0, faced */
int _ZN10MrBlizzard16CleanupResourcesEv(void);        /* slot 3, .c body takes void */
int _ZN10MrBlizzard8BehaviorEv(void *self);           /* slot 6, faced */
int _ZN10MrBlizzard6RenderEv(void *self);             /* slot 9, faced */
void _ZN10MrBlizzard16OnPendingDestroyEv(void);       /* slot 12, own -- NOT shared, .c
                                                           body takes void, empty */
int _ZN10MrBlizzardD1Ev(void *self);                  /* slot 16, .c, spells own table */
int _ZN10MrBlizzardD0Ev(void *self);                  /* slot 17, .c, spells own table
                                                           under RTTI alias daSnowman_c */
int func_ov081_02125eb8(void);                        /* slot 29, OnAimedAtWithEgg --
                                                           no args, returns int */
int func_ov081_021261b8(void *self);                  /* slot 31, Kill -- returns int */
void *MrBlizzard_Spawn(void);                         /* installs _ZTV10MrBlizzard */
int _ZTV10MrBlizzard[32];

/* the ten state cells: each is TWO 8-byte {fn,delta} PMF pairs back to
   back (the .x=enter half, .y=tick half __sinit_ov081_02128154's own
   struct { S8 x, y; } shows) -- 0x10 bytes/cell, matching the real
   next-symbol stride (data_ov081_02128e14 -> 02128e24 = 0x10), sinit-
   copied as DS code addresses. func_ov081_02125488 installs a pointer to
   a cell's OWN address at self+0x3f8 and dispatches .x (enter)
   immediately; MrBlizzard's own Behavior (matched src) dispatches .y
   (tick) every frame through the same pointer + 1 PMF-width (8 bytes). */
struct PortMrBlizzardPair { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
extern PortMrBlizzardPair data_ov081_02128e14[10];
int func_ov081_02124ec0(void *self);   /* cell 0 (e14) enter */
/* cell 0 (e14) tick is func_ov081_02124e64, ALREADY declared above as
   Spindrift's own Kill (slot 31) -- one real ROM address serves both
   roles, see the note below. Not redeclared here. */
int func_ov081_0212479c(void *self);   /* cell 1 (e24) enter */
int func_ov081_021246a0(void *self);   /* cell 1 (e24) tick */
int func_ov081_02124dfc(void *self);   /* cell 2 (e34) enter */
int func_ov081_02124d50(void *self);   /* cell 2 (e34) tick */
int func_ov081_02124d14(void *self);   /* cell 3 (e44) enter */
int func_ov081_02124b98(void *self);   /* cell 3 (e44) tick */
int func_ov081_0212538c(void *self);   /* cell 4 (e54) enter */
int func_ov081_02125208(void *self);   /* cell 4 (e54) tick */
int func_ov081_02124b08(void *self);   /* cell 5 (e64) enter */
int func_ov081_021249f4(void *self);   /* cell 5 (e64) tick */
int func_ov081_02125200(void *self);   /* cell 6 (e74) enter */
int func_ov081_021250c8(void *self);   /* cell 6 (e74) tick */
int func_ov081_02125068(void *self);   /* cell 7 (e84) enter */
int func_ov081_02125038(void *self);   /* cell 7 (e84) tick */
int func_ov081_0212498c(void *self);   /* cell 8 (e94) enter */
int func_ov081_02124894(void *self);   /* cell 8 (e94) tick */
int func_ov081_02124f7c(void *self);   /* cell 9 (ea4) enter */
int func_ov081_02124f20(void *self);   /* cell 9 (ea4) tick */
}

/* NOTE ON func_ov081_02124e64: reloc-derivation independently identified
   this ONE ROM address (0x02124e64) as BOTH Spindrift's own vtable slot 31
   (Kill) AND MrBlizzard's PMF cell-0 tick target -- the reloc for
   Spindrift's slot 31 and the reloc for the sinit's source cell
   data_ov081_021288f8 (which __sinit_ov081_02128154 copies into cell 0's
   tick half) both resolve to 0x02124e64. func_ov081_02124e64's own body
   (src/func_ov081_02124e64.c) confirms it: it calls
   func_ov081_02125488(c, data_ov081_02128e34) -- MrBlizzard's own state-
   transition helper -- so the function genuinely IS a state handler, not
   merely misattributed by dsd's auto-namer. One real body, two roles,
   declared once above and reused by name in both fill sites below. */

/* {ROM address the sinit's own source cell carries, host body} --
   verified against the ROM's own record before the rewrite (the
   SoundObject/Cap seat shape: a mount pointing at the wrong bytes aborts
   instead of calling into the overlay image). Order matches
   __sinit_ov081_02128154's own field order (NOT sorted by address -- see
   port/ov081_syms.txt / that sinit's own body). */
typedef int (*PortMbFn)(void *);
static const struct { unsigned enter_rom, tick_rom; PortMbFn enter_host, tick_host; }
g_mb_cells[10] = {
    {0x02124ec0, 0x02124e64, func_ov081_02124ec0, func_ov081_02124e64},
    {0x0212479c, 0x021246a0, func_ov081_0212479c, func_ov081_021246a0},
    {0x02124dfc, 0x02124d50, func_ov081_02124dfc, func_ov081_02124d50},
    {0x02124d14, 0x02124b98, func_ov081_02124d14, func_ov081_02124b98},
    {0x0212538c, 0x02125208, func_ov081_0212538c, func_ov081_02125208},
    {0x02124b08, 0x021249f4, func_ov081_02124b08, func_ov081_021249f4},
    {0x02125200, 0x021250c8, func_ov081_02125200, func_ov081_021250c8},
    {0x02125068, 0x02125038, func_ov081_02125068, func_ov081_02125038},
    {0x0212498c, 0x02124894, func_ov081_0212498c, func_ov081_02124894},
    {0x02124f7c, 0x02124f20, func_ov081_02124f7c, func_ov081_02124f20},
};

extern "C" void port_mr_blizzard_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 10; ++i) {
        PortMrBlizzardPair &cell = data_ov081_02128e14[i];
        if (cell.enter_fn != g_mb_cells[i].enter_rom || cell.enter_delta != 0 ||
            cell.tick_fn != g_mb_cells[i].tick_rom || cell.tick_delta != 0) {
            std::fprintf(stderr, "FATAL: MrBlizzard state cell %d: the sinit "
                         "left %08x/%u %08x/%u, the ROM's own records say "
                         "%08x/0 %08x/0 -- WRONG BYTES\n", i,
                         cell.enter_fn, cell.enter_delta, cell.tick_fn, cell.tick_delta,
                         g_mb_cells[i].enter_rom, g_mb_cells[i].tick_rom);
            std::abort();
        }
        cell.enter_fn = (unsigned)(size_t)g_mb_cells[i].enter_host;
        cell.tick_fn = (unsigned)(size_t)g_mb_cells[i].tick_host;
    }
}

static int __fastcall mb_init(void *s, void *)
{ return _ZN10MrBlizzard13InitResourcesEv(s); }
static int __fastcall mb_clean(void *s, void *)
{ (void)s; return _ZN10MrBlizzard16CleanupResourcesEv(); }
static int __fastcall mb_behavior(void *s, void *)
{ return _ZN10MrBlizzard8BehaviorEv(s); }
/* HISTORY: the func_0204488c raw-DMA fear that first excluded MR_BLIZZARD
   is RESOLVED (the dma verdict -- Part.fc/f8 are rebased at load; the
   garbage fc the original probe read was pre-rebase, not a bad forward).
   Registration was then attempted and found a DIFFERENT real blocker: a
   deterministic frame-0 register-state corruption in Behavior's callee
   chain (the `this` register comes back from a callee skewed to
   this+0x150, the mWithMeshClsn; the tail's +0x368 store then stomps the
   NEXT instance's render node). Full evidence chain + candidate list in
   the gate-192 registry comment (port/hal/actor_classes.inc). 223 stays
   unregistered until that seam is pinned; MrBlizzard::Render itself is
   clean (calls Model::Render BY NAME -- no ModelAnim slot-5 exposure). */
static int __fastcall mb_render(void *s, void *)
{ port_actor_render_probe("MR_BLIZZARD", (char *)s + 0x30c);
  return _ZN10MrBlizzard6RenderEv(s); }
static int __fastcall mb_pdes(void *s, void *)
{ (void)s; _ZN10MrBlizzard16OnPendingDestroyEv(); return 0; }
static int __fastcall mb_d1(void *s, void *)
{ return _ZN10MrBlizzardD1Ev(s); }
static int __fastcall mb_d0(void *s, void *)
{ return _ZN10MrBlizzardD0Ev(s); }
static int __fastcall mb_aimed(void *s, void *)
{ (void)s; return func_ov081_02125eb8(); }
static int __fastcall mb_kill(void *s, void *)
{ return func_ov081_021261b8(s); }

extern "C" void hal_fill_mr_blizzard_vtable(void)
{
    /* seat the ten PMF cells BEFORE InitResources can dispatch through them
       (func_ov081_02125488 dispatches the "enter" half immediately). */
    port_mr_blizzard_states_seat();
    void **vt = (void **)_ZTV10MrBlizzard;
    ov81_fill_shared(vt);
    vt[0]  = (void *)mb_init;
    vt[3]  = (void *)mb_clean;
    vt[6]  = (void *)mb_behavior;
    vt[9]  = (void *)mb_render;
    vt[12] = (void *)mb_pdes;        /* own OnPendingDestroy, NOT the shared default */
    vt[16] = (void *)mb_d1;
    vt[17] = (void *)mb_d0;
    vt[29] = (void *)mb_aimed;       /* own OnAimedAtWithEgg */
    vt[31] = (void *)mb_kill;        /* own Kill */
}

// ============================================================================
// ICE_BLOCK (18), a Platform, 32 slots (own Kill). D1/D0 host thunks: the
// matched src stores its own table then OVERWRITES with the shared
// _ZTV8Platform placeholder (the ShipUp/RockPillar/SkiLift shape).
// ============================================================================
extern "C" {
int _ZN8IceBlock13InitResourcesEv(void *self);        /* slot 0, faced */
int _ZN8IceBlock16CleanupResourcesEv(void *self);     /* slot 3, faced */
int _ZN8IceBlock8BehaviorEv(void *self);              /* slot 6, faced */
int _ZN8IceBlock6RenderEv(void *self);                /* slot 9, faced */
void func_ov081_02127ccc(void *self, void *p);        /* slot 27, OnHitByMegaChar */
void func_ov081_02127cf4(void *self);                 /* slot 31, Kill */
void *IceBlock_Spawn(void);                           /* installs _ZTV15daObjIceBlock_c */
int _ZTV8IceBlock[32];
extern int _ZTV15daObjIceBlock_c[];
}
/* IceBlock's own D0/D1 spell the RTTI name _ZTV15daObjIceBlock_c for their
   FIRST (later overwritten) store, but the factory installs it too --
   confirmed against IceBlock_Spawn.c. Alias the RTTI name onto this fill's
   host array (the OneUpLogo/daObj1UpLogo_c treatment). */
#pragma comment(linker, "/alternatename:__ZTV15daObjIceBlock_c=__ZTV8IceBlock")

static int __fastcall icb_init(void *s, void *)
{ return _ZN8IceBlock13InitResourcesEv(s); }
static int __fastcall icb_clean(void *s, void *)
{ return _ZN8IceBlock16CleanupResourcesEv(s); }
static int __fastcall icb_behavior(void *s, void *)
{ return _ZN8IceBlock8BehaviorEv(s); }
static int __fastcall icb_render(void *s, void *)
{ port_actor_render_probe("ICE_BLOCK", (char *)s + 0xd4);
  return _ZN8IceBlock6RenderEv(s); }
static int __fastcall icb_mega(void *s, void *, void *p)
{ func_ov081_02127ccc(s, p); return 0; }
static int __fastcall icb_kill(void *s, void *)
{ func_ov081_02127cf4(s); return 0; }
/* D1/D0 host thunks: store the derived table once, run the chain high-
   address first (MovingCylinderClsn +0x320, MovingMeshCollider +0x124,
   Model +0xd4), then Actor's own D2. D0 also frees on the game heap. */
extern "C" void _ZN18MovingCylinderClsnD1Ev(void *);
static int __fastcall icb_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV8IceBlock;
    _ZN18MovingCylinderClsnD1Ev(t + 0x320);
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall icb_d0(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV8IceBlock;
    _ZN18MovingCylinderClsnD1Ev(t + 0x320);
    _ZN18MovingMeshColliderD1Ev(t + 0x124);
    _ZN5ModelD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return (int)(size_t)s;
}

extern "C" void hal_fill_ice_block_vtable(void)
{
    void **vt = (void **)_ZTV8IceBlock;
    hal_fill_platform_vtable();
    ov81_fill_shared(vt);
    vt[0]  = (void *)icb_init;
    vt[3]  = (void *)icb_clean;
    vt[6]  = (void *)icb_behavior;
    vt[9]  = (void *)icb_render;
    vt[16] = (void *)icb_d1;
    vt[17] = (void *)icb_d0;
    vt[27] = (void *)icb_mega;       /* own OnHitByMegaChar, overrides the shared default */
    vt[31] = (void *)icb_kill;       /* own Kill */
}

// ---- method faces ------------------------------------------------------
// The C-named references the thunks above take onto the real MSVC methods
// against include/, the IceSheet/gate-190 treatment.
#include "Spindrift.h"
#include "MrBlizzard.h"
#include "IceBlock.h"
extern "C" {
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9Spindrift13InitResourcesEv(void *self)
{ return ((Spindrift *)self)->Spindrift::InitResources(); }
#else
int _ZN9Spindrift13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN9Spindrift13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN9Spindrift8BehaviorEv(void *self)
{ return ((Spindrift *)self)->Spindrift::Behavior(); }
#else
int _ZN9Spindrift8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN9Spindrift8BehaviorEv */
#endif /* _WIN32 */
/* _ZN9Spindrift6RenderEv is NOT faced here -- the ModelAnim slot-5 collision.
   The matched Spindrift::Render dispatches ROM slot 5 through a six-virtual
   shadow off mModelAnim (+0x110); host copy in
   port/unmatched/ModelAnim_Renders.cpp spells it as qualified ModelAnim::Render. */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN10MrBlizzard13InitResourcesEv(void *self)
{ return ((MrBlizzard *)self)->MrBlizzard::InitResources(); }
#else
int _ZN10MrBlizzard13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN10MrBlizzard13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN10MrBlizzard8BehaviorEv(void *self)
{ return ((MrBlizzard *)self)->MrBlizzard::Behavior(); }
#else
int _ZN10MrBlizzard8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN10MrBlizzard8BehaviorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN10MrBlizzard6RenderEv(void *self)
{ return ((MrBlizzard *)self)->MrBlizzard::Render(); }
#else
int _ZN10MrBlizzard6RenderEv(void *self);  /* Linux: real symbol from src/_ZN10MrBlizzard6RenderEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8IceBlock13InitResourcesEv(void *self)
{ return ((IceBlock *)self)->IceBlock::InitResources(); }
#else
int _ZN8IceBlock13InitResourcesEv(void *self);  /* Linux: real symbol from src/_ZN8IceBlock13InitResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8IceBlock16CleanupResourcesEv(void *self)
{ return ((IceBlock *)self)->IceBlock::CleanupResources(); }
#else
int _ZN8IceBlock16CleanupResourcesEv(void *self);  /* Linux: real symbol from src/_ZN8IceBlock16CleanupResourcesEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8IceBlock8BehaviorEv(void *self)
{ return ((IceBlock *)self)->IceBlock::Behavior(); }
#else
int _ZN8IceBlock8BehaviorEv(void *self);  /* Linux: real symbol from src/_ZN8IceBlock8BehaviorEv */
#endif /* _WIN32 */
#ifdef _WIN32 /* LINUX: this extern-C name IS the Itanium mangling of the C++ method it forwards to -> self-recurse on GCC. Keep the __cdecl->__thiscall converter on MSVC; on Linux fall to a plain decl and bind to the real src/ TU. */
int _ZN8IceBlock6RenderEv(void *self)
{ return ((IceBlock *)self)->IceBlock::Render(); }
#else
int _ZN8IceBlock6RenderEv(void *self);  /* Linux: real symbol from src/_ZN8IceBlock6RenderEv */
#endif /* _WIN32 */
}
