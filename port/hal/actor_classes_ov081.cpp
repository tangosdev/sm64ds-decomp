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
// Behavior tick) reaches ROM address 0x020ada40 on one path.
//
// RUN LINK60 lane A2 CORRECTED WHERE THAT CALL LANDS. It used to land on the
// ov004 occupant of that address, because src/func_ov081_02123910.cpp takes
// the name from include/decl_Enemy.h:24 and that spelling is the ov004 body's.
// The ROM's reloc at 0x02123988 reads module:overlays(2,4) -- dsd cannot tell
// the two occupants apart -- and co-residency settles it: ov004 loads only
// with ov006 for a minigame scene, while ov081 is a level actor overlay that
// runs alongside ov002. The right occupant is ov002's 0x100-byte body, which
// is what every other level-overlay caller of that address already reaches
// through hal/bob_enemy_bridges.cpp's
// /alternatename:_func_020ada40=_func_ov002_020ada40. The routing is a
// per-source -D in port/CMakeLists.txt; the derivation is in the R-block there.
//
// The consequence of the old routing, for the record: the ov004 body's first
// statement is a null guard on data_ov004_020beb68, Bob-omb Battlefield's own
// level tracker pointer, which is the zero-initialized host global below and
// is never written by anything in this build. Spindrift's kill-by-invincible
// path returned doing nothing, every frame, and neither the byte gate nor the
// linkage count could see it.
//
// THE GLOBAL BELOW IS UNREAD AND IT STAYS; ITS ov004 SIBLING HAS GONE HOME.
// Their only reader was the ov004 KillByInvincibleChar TU, which had no other
// referrer in this build and left the link with the misroute (with
// func_ov004_020adc1c and func_ov004_020adc00, which only it called -- an
// honest linkage cost of 3, recorded rather than papered over). The comment
// here used to say both were kept for the day ov004 was mounted; run link60
// lane s2-m46 mounted it, so data_ov004_020beb68 now comes out of ov004's own
// .bss with the same four zero bytes over the same ROM span, and hosting it
// twice is a duplicate definition.
//
// data_0209b308 USED TO BE HOSTED HERE TOO, at 16 bytes, with the note "never
// read at runtime". That note was wrong: the record is the minigame sound row
// the dScMgBase_c framework reads at +0x28 (music id), +0x2c (enable) and
// +0x30, and the 16-byte host truncated every one of those reads -- one of the
// two causes of the curling "no music" report. Run link60 lane MUS1 re-hosted
// it at its true 0x34 span (data_0209b308 + data_0209b31c) next to the ov005
// table that seeds it, in hal/scene_mg_sound.cpp. Nothing here defines it now,
// so this file no longer needs the dsstate_seg bracket.

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
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */
void hal_fill_platform_vtable(void);              /* hal/actor_classes.cpp, Platform's
                                                       own base table */
extern int _ZTV8Platform[];
void _ZN18MovingMeshColliderD1Ev(void *);         /* MovingMeshCollider at +0x124 */
void _ZN18MovingCylinderClsnD1Ev(void *);        /* MovingCylinderClsn (Moneybag +0x1b0) */
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
int *_ZN8IceBlockD1Ev(int *self);                     /* slot 16, .c, DTOR-PAIRS seat (0x02127b34) */
int *_ZN8IceBlockD0Ev(int *self);                     /* slot 17, .c, DTOR-PAIRS seat (0x02127b80) */
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
/* D1/D0 (DTOR-PAIRS seat): the matched flat-C pair behind ecx->arg adapters,
   replacing the host copies of the chain that stood here. The "_ZTV8Platform
   placeholder" the copies were written to avoid is the ONE Platform base table
   by relocation (ov002 0x0210ae38, hosted as _ZTV8Platform / _ZTV10dBgActor_c,
   hal/lk2_platform_dtor_seat.cpp), and the first store is this table by its
   RTTI name, aliased above. */
static int __fastcall icb_d1(void *s, void *)
{ return (int)(size_t)_ZN8IceBlockD1Ev((int *)s); }
static int __fastcall icb_d0(void *s, void *)
{ return (int)(size_t)_ZN8IceBlockD0Ev((int *)s); }

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

// ============================================================================
// SNOWBALL (224), an Enemy, 31 slots (plain Actor tail -- no own Kill)
// ============================================================================
// LANE w3-b (run rel0215), the partial-module finish. Gate 192 mounted ov081
// for the trio and left SNOWBALL and MONEYBAG unregistered "since neither
// class is hosted"; this is the lane that hosts them. Nothing about the mount
// changes except that _ZTV8Snowball and _ZTV8Moneybag leave port/ov081_syms.txt
// and become the host arrays below -- the rule the same file already applies
// to _ZTV9Spindrift, _ZTV10MrBlizzard and _ZTV8IceBlock.
//
// WIDTH 31, NOT 32, and the difference is load-bearing: these two are plain
// Actor subclasses (slot 30 is Actor::OnAimedAtWithEggReturnVec, slot 31 is
// not a code pointer) where the trio are Enemy/Platform shapes with an own
// Kill at 31. `python port/tools/vtspan.py . _ZTV8Snowball` reads tail 31 /
// terminator 31 / next-dsd 31 and a RAW RUN of 32 -- that 32nd word is
// 0x021273e8, MONEYBAG's own state-2 enter body, reached because Moneybag's
// {function, 0} PMF SOURCE table begins at 0x02128b38 right where Snowball's
// table ends. Trap T4's fourth width trap, read off the real bytes rather
// than walked into. _ZTV8Moneybag's five routes all agree on 31.
//
// ov81_fill_shared writes 1..30 and is reused unchanged: slots 13/14 trap
// (ActorBase::Virtual34/38, the pair every sibling fill traps) and slot 30
// traps (Actor::OnAimedAtWithEggReturnVec, the SRET body no fill's thunk
// shape models -- hal/actor_classes_montymolerock.cpp's ruling, verbatim).
// Slot 31 is simply never written for these two, so their arrays are 31 long.
extern "C" {
int _ZN8Snowball13InitResourcesEv(void *self);        /* slot 0,  faced below */
int _ZN8Snowball16CleanupResourcesEv(void);           /* slot 3,  .c, takes void */
int _ZN8Snowball8BehaviorEv(void *self);              /* slot 6,  faced below */
int _ZN8Snowball6RenderEv(void *self);                /* slot 9,  faced below */
void _ZN8Snowball16OnPendingDestroyEv(void);          /* slot 12, OWN, .c, empty */
int _ZN8SnowballD1Ev(void *self);                     /* slot 16, .c, own table */
int _ZN8SnowballD0Ev(void *self);                     /* slot 17, .c, RTTI spelling */
void *Snowball_Spawn(void);                           /* installs _ZTV8Snowball */
int _ZTV8Snowball[31];

/* The class's TWO PMF halves. func_ov081_021261d4 (InitResources' last call)
   stores &data_ov081_02128eb4 at self+0x378 and dispatches the ENTER half
   (index 0) in the same statement; Snowball::Behavior reads the TICK half
   every frame as `((M *)cell)->pmf`, byte offset 8 of the same cell. Both are
   index-0 reads of an 8-byte {function, delta} record, so neither needs
   /vmg /vmm -- port/slice_w3b.txt has why that is a statement about run
   linkw wave 18's R9 predicate and not an assumption. */
struct PortSnowballPair { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
extern PortSnowballPair data_ov081_02128eb4;
/* enter = func_ov081_021261b8, ALREADY declared above as MrBlizzard's own Kill
   (slot 31): one real ROM address, two roles, the func_ov081_02124e64 shape
   this file already documents at length. Not redeclared. */
int func_ov081_021260fc(void *self);   /* tick */
}

/* D0 stores the RTTI spelling _ZTV12daSnowball_c (dsd dual-names it at the
   same address 0x02128abc; D1 and the factory both spell _ZTV8Snowball).
   Both references are C linkage, so point the underscore spelling at the one
   host array -- the daChoropu_c / daObj1UpLogo_c treatment. The LHS is
   UNDEFINED everywhere in this link now that the vtable has left the mount
   (ov081_syms.txt listed the dual-named pair ONCE, under _ZTV8Snowball, so
   dropping that line drops both spellings), which is what an /alternatename
   needs: a defined LHS is defeated silently, the R3 ov071/ov073 failure. */
#pragma comment(linker, "/alternatename:__ZTV12daSnowball_c=__ZTV8Snowball")

/* THE TWO HALVES OF THIS ONE CELL ARE REACHED THROUGH DIFFERENT CALLING
   CONVENTIONS ON THE HOST, so the seat writes a different kind of host address
   into each. That asymmetry is not a preference; it is what the two dispatch
   sites compile to, read out of the objs:

     ENTER, dispatched by func_ov081_021261d4 (the installer InitResources
       calls). Its `struct C` IS defined in that TU, so MSVC picks the 4-byte
       single-inheritance PMF, and the whole body is a one-call forwarder that
       MSVC compiles as a TAIL JUMP:
           mov eax,[ebp+0Ch] / mov ecx,[ebp+8] / mov [ecx+378h],eax
           mov eax,[eax] / test eax,eax / jne / mov eax,1 / ret
           pop ebp / jmp eax
       The jump reuses the forwarder's own cdecl frame, so the callee reads
       its receiver off the stack exactly where a plain
       `int f(void *self)` looks for it. The RAW matched body goes in this
       half. (This is the frame reuse port/tools/tailjump_guard.py exists to
       protect, and R9's genuine index-0 case.)

     TICK, dispatched by Snowball::Behavior mid-body. Its `struct Klass` is
       never completed, so the TU is compiled /vmg /vmm (block R10 in
       port/CMakeLists.txt, with the disassembly both ways) and MSVC emits a
       REAL CALL with the receiver in ECX:
           mov ecx,[esi+378h] / mov eax,[ecx+8] / test eax,eax / je
           mov ecx,[ecx+0Ch] / add ecx,esi / call eax
       A real call pushes a new frame, so nothing puts the receiver on the
       stack and the raw cdecl body would read the caller's saved edi as
       `self`. This half gets a __fastcall THUNK instead -- the same shape
       every vtable fill in this port uses, and ABI-compatible with the
       thiscall MSVC emits here: a no-argument member call passes ecx only and
       cleans nothing, __fastcall(void *, void *) takes ecx and edx and cleans
       nothing.

   THE ALTERNATIVE, AND WHY NOT. MrBlizzard's own Behavior one class up is a
   HOST COPY for the same mid-body dispatch, which is the precedent this could
   have followed. It costs a matched TU. The thunk costs nothing, keeps the
   decompiled Behavior running, and rests on a COMPILE OPTION that is pinned in
   CMakeLists rather than on codegen luck -- so this lane took it. If /vmg /vmm
   is ever dropped from that TU the width goes wrong again and the class faults
   on frame 1, loudly, which is how this was found in the first place. */
static int __fastcall sb_state_tick(void *s, void *)
{ return func_ov081_021260fc(s); }

/* {ROM address the sinit's own source record carries, host body}, verified
   before the rewrite -- the MrBlizzard seat above, one class over. */
extern "C" void port_snowball_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    PortSnowballPair &cell = data_ov081_02128eb4;
    if (cell.enter_fn != 0x021261b8u || cell.enter_delta != 0 ||
        cell.tick_fn != 0x021260fcu || cell.tick_delta != 0) {
        std::fprintf(stderr, "FATAL: Snowball state cell: the sinit left "
                     "%08x/%u %08x/%u, the ROM's own records say "
                     "021261b8/0 021260fc/0 -- WRONG BYTES\n",
                     cell.enter_fn, cell.enter_delta,
                     cell.tick_fn, cell.tick_delta);
        std::abort();
    }
    cell.enter_fn = (unsigned)(size_t)func_ov081_021261b8;   /* cdecl, tail-jumped */
    cell.tick_fn = (unsigned)(size_t)sb_state_tick;          /* __fastcall thunk */
}

static int __fastcall sb_init(void *s, void *)
{ return _ZN8Snowball13InitResourcesEv(s); }
static int __fastcall sb_clean(void *s, void *)
{ (void)s; return _ZN8Snowball16CleanupResourcesEv(); }
static int __fastcall sb_behavior(void *s, void *)
{ return _ZN8Snowball8BehaviorEv(s); }
/* Render is the MATCHED TU, not a host copy: its six-virtual ROM-order shadow
   sits over a plain Model at +0x300 (constructed by _ZN5ModelC1Ev in
   Snowball_Spawn, filled through ModelBase::SetFile in InitResources,
   destroyed by _ZN5ModelD1Ev in both destructors -- three witnesses, all
   matched src, the MontyMoleRock adjudication-from-construction), and
   _ZTV5Model is DUAL-FILLED at [4] and [5] by hal/cxxname_bridge.cpp so its
   slot 5 lands on Model::Render. The opposite ruling from Spindrift's Render
   in this same file; the member type is the only difference. */
static int __fastcall sb_render(void *s, void *)
{ port_actor_render_probe("SNOWBALL", (char *)s + 0x300);
  return _ZN8Snowball6RenderEv(s); }
static int __fastcall sb_pdes(void *s, void *)
{ (void)s; _ZN8Snowball16OnPendingDestroyEv(); return 0; }
static int __fastcall sb_d1(void *s, void *)
{ return _ZN8SnowballD1Ev(s); }
static int __fastcall sb_d0(void *s, void *)
{ return _ZN8SnowballD0Ev(s); }

extern "C" void hal_fill_snowball_vtable(void)
{
    /* seat the two PMF halves BEFORE InitResources can dispatch the enter
       half -- func_ov081_021261d4 dispatches it in the same statement that
       installs the cell pointer. */
    port_snowball_states_seat();
    void **vt = (void **)_ZTV8Snowball;
    ov81_fill_shared(vt);
    vt[0]  = (void *)sb_init;
    vt[3]  = (void *)sb_clean;
    vt[6]  = (void *)sb_behavior;
    vt[9]  = (void *)sb_render;
    vt[12] = (void *)sb_pdes;        /* own OnPendingDestroy, NOT the shared default */
    vt[16] = (void *)sb_d1;
    vt[17] = (void *)sb_d0;
}

// ============================================================================
// MONEYBAG (261), an Actor, 31 slots
// ============================================================================
extern "C" {
int _ZN8Moneybag13InitResourcesEv(void *self);        /* slot 0,  faced below */
int _ZN8Moneybag16CleanupResourcesEv(void);           /* slot 3,  .c, takes void */
int _ZN8Moneybag8BehaviorEv(void *self);              /* slot 6,  faced below */
int _ZN8Moneybag6RenderEv(void *self);                /* slot 9,  HOST COPY in
                                                           port/unmatched/
                                                           ModelAnim_Renders.cpp */
void _ZN8Moneybag16OnPendingDestroyEv(void);          /* slot 12, OWN, .c, empty */
/* slot 16 is a HOST THUNK below -- the matched _ZN8MoneybagD1Ev.cpp is the
   auto-emitted-member-dtor .cpp form (the MontyMole gate-174 case). */
int _ZN8MoneybagD0Ev(void *self);                     /* slot 17, .c, RTTI spelling */
int func_ov081_021265b8(void);                        /* slot 18, OnYoshiTryEat --
                                                           no args, returns 6 */
void func_ov081_02127a7c(void *self, void *p);        /* slot 19, OnTurnIntoEgg */
int func_ov081_021265c0(void);                        /* slot 29, OnAimedAtWithEgg --
                                                           no args, returns a radius */
void *Moneybag_Spawn(void);                           /* installs _ZTV8Moneybag */
int _ZTV8Moneybag[31];

void _ZN9ModelAnimD1Ev(void *);                       /* ModelAnim at +0xd4 */
void _ZN11ShadowModelD1Ev(void *);                    /* ShadowModel at +0x188 */
void _ZN12WithMeshClsnD1Ev(void *);                   /* WithMeshClsn at +0x1e4 */

/* NINE STATES x {enter, tick}: the eighteen records __sinit_ov081_021284f0
   copies out of the mounted source records at 0x02128b38..0x02128bc0 into the
   mutable table at 0x02128f40. func_ov081_0212777c sets
   `pp = &table + (state << 4)`; func_ov081_02127744 dispatches index 0
   (enter) and func_ov081_02127708 index 1 (tick -- the /vmg /vmm row run
   linkw wave 18 put in port/CMakeLists.txt ahead of this lane). */
struct PortMoneybagRec { unsigned fn, delta; };
extern PortMoneybagRec data_ov081_02128f40[18];
int func_ov081_021276b0(void *self);   /* state 0 enter */
int func_ov081_02127558(void *self);   /* state 0 tick  */
int func_ov081_021274c8(void *self);   /* state 1 enter */
int func_ov081_02127440(void *self);   /* state 1 tick  */
int func_ov081_021273e8(void *self);   /* state 2 enter */
int func_ov081_02127398(void *self);   /* state 2 tick  */
int func_ov081_02127314(void *self);   /* state 3 enter */
int func_ov081_02127240(void *self);   /* state 3 tick  */
int func_ov081_021271e8(void *self);   /* state 4 enter */
int func_ov081_02127188(void *self);   /* state 4 tick  */
int func_ov081_02127134(void *self);   /* state 5 enter */
int func_ov081_02127070(void *self);   /* state 5 tick  */
int func_ov081_02127044(void *self);   /* state 6 enter */
int func_ov081_02126fa4(void *self);   /* state 6 tick  */
int func_ov081_02126e28(void *self);   /* state 7 enter */
int func_ov081_02126d64(void *self);   /* state 7 tick  */
int func_ov081_02126c8c(void *self);   /* state 8 enter */
int func_ov081_02126c20(void *self);   /* state 8 tick  */
}

/* D0 stores the RTTI spelling _ZTV8daGmch_c; D1's ROM body and the factory
   both store _ZTV8Moneybag (0x02128c04, read off ov081's relocs at 0x02126550,
   0x021265b0 and Moneybag_Spawn's own store). Same alias argument as
   Snowball's above -- the LHS is undefined everywhere once the vtable leaves
   the mount, so the alias cannot be defeated. */
#pragma comment(linker, "/alternatename:__ZTV8daGmch_c=__ZTV8Moneybag")

/* TWO OF MONEYBAG'S TUs SPELL A MOUNTED C SYMBOL AS A TYPED C++ GLOBAL, so
   MSVC mangles the reference and the mount's C name does not answer it. Both
   came off the FIRST LINK'S OWN UNRESOLVED LIST rather than out of a reading
   (the measured-gap rule), and both are the montymolerock
   `?data_ov080_021283c8@@3USharedFilePtr@@A` shape:

     src/func_ov081_02126a20.cpp declares `extern Vector3
       data_ov081_02128ef8;` OUTSIDE its own extern "C" block -- the +0x2000
       Y-offset record __sinit_ov081_021284f0 initialises. Mounted by
       port/ov081_syms.txt.
     src/_ZN8Moneybag13InitResourcesEv.cpp declares `extern int
       data_ov002_0210d9b8[];` outside extern "C" -- the ov002 bss pair whose
       word 1 is the shared BMD file the plain Model at +0x138 is given.
       Mounted by port/ov002_syms.txt, and 8 bytes to its next config symbol
       (data_ov002_0210d9c0), so the `[1]` read stays inside its own span.

   An /alternatename and not a -D rename because the LHS here is an MSVC
   MANGLED name, which nothing else in this link can define -- the property
   the R3/R4/R5 rule demands and the exact reason those cases needed a -D
   instead. */
#pragma comment(linker, "/alternatename:?data_ov081_02128ef8@@3UVector3@@A=_data_ov081_02128ef8")
#pragma comment(linker, "/alternatename:?data_ov002_0210d9b8@@3PAHA=_data_ov002_0210d9b8")

/* {ROM address the sinit's own source record carries, host body}, in the
   sinit's OWN copy order (data_ov081_02128f40[i] <- the source record
   src/__sinit_ov081_021284f0.c names on line i), each fn resolved out of
   ov081's relocs.txt rather than inferred from the copy order. */
typedef int (*PortMnFn)(void *);
static const struct { unsigned rom; PortMnFn host; } g_mn_recs[18] = {
    {0x021276b0, func_ov081_021276b0},   /* [ 0] <- 02128b58, state 0 enter */
    {0x02127558, func_ov081_02127558},   /* [ 1] <- 02128b68, state 0 tick  */
    {0x021274c8, func_ov081_021274c8},   /* [ 2] <- 02128b40, state 1 enter */
    {0x02127440, func_ov081_02127440},   /* [ 3] <- 02128b48, state 1 tick  */
    {0x021273e8, func_ov081_021273e8},   /* [ 4] <- 02128b38, state 2 enter */
    {0x02127398, func_ov081_02127398},   /* [ 5] <- 02128bc0, state 2 tick  */
    {0x02127314, func_ov081_02127314},   /* [ 6] <- 02128bb8, state 3 enter */
    {0x02127240, func_ov081_02127240},   /* [ 7] <- 02128bb0, state 3 tick  */
    {0x021271e8, func_ov081_021271e8},   /* [ 8] <- 02128ba8, state 4 enter */
    {0x02127188, func_ov081_02127188},   /* [ 9] <- 02128ba0, state 4 tick  */
    {0x02127134, func_ov081_02127134},   /* [10] <- 02128b88, state 5 enter */
    {0x02127070, func_ov081_02127070},   /* [11] <- 02128b98, state 5 tick  */
    {0x02127044, func_ov081_02127044},   /* [12] <- 02128b90, state 6 enter */
    {0x02126fa4, func_ov081_02126fa4},   /* [13] <- 02128b70, state 6 tick  */
    {0x02126e28, func_ov081_02126e28},   /* [14] <- 02128b50, state 7 enter */
    {0x02126d64, func_ov081_02126d64},   /* [15] <- 02128b60, state 7 tick  */
    {0x02126c8c, func_ov081_02126c8c},   /* [16] <- 02128b80, state 8 enter */
    {0x02126c20, func_ov081_02126c20},   /* [17] <- 02128b78, state 8 tick  */
};

extern "C" void port_moneybag_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < 18; ++i) {
        PortMoneybagRec &r = data_ov081_02128f40[i];
        if (r.fn != g_mn_recs[i].rom || r.delta != 0) {
            std::fprintf(stderr, "FATAL: Moneybag state record %d: the sinit "
                         "left %08x/%u, the ROM's own record says %08x/0 -- "
                         "WRONG BYTES\n", i, r.fn, r.delta, g_mn_recs[i].rom);
            std::abort();
        }
        r.fn = (unsigned)(size_t)g_mn_recs[i].host;
    }
}

static int __fastcall mn_init(void *s, void *)
{ return _ZN8Moneybag13InitResourcesEv(s); }
static int __fastcall mn_clean(void *s, void *)
{ (void)s; return _ZN8Moneybag16CleanupResourcesEv(); }
static int __fastcall mn_behavior(void *s, void *)
{ return _ZN8Moneybag8BehaviorEv(s); }
static int __fastcall mn_render(void *s, void *)
{ port_actor_render_probe("MONEYBAG", (char *)s + 0xd4);
  return _ZN8Moneybag6RenderEv(s); }
static int __fastcall mn_pdes(void *s, void *)
{ (void)s; _ZN8Moneybag16OnPendingDestroyEv(); return 0; }
/* D1 host thunk: the matched .cpp is the auto-emitted-member-dtor form. The
   chain is D0's own matched .c minus the Deallocate, in the ROM's order --
   confirmed against ov081's relocs for 0x02126504..0x02126554: the table
   store is 0x02128c04 (_ZTV8Moneybag, NOT the RTTI spelling D0 uses), then
   WithMeshClsn +0x1e4, MovingCylinderClsn +0x1b0, ShadowModel +0x188,
   Model +0x138, ModelAnim +0xd4, then Actor's own D2. */
static int __fastcall mn_d1(void *s, void *)
{
    char *t = (char *)s;
    *(void **)t = (void *)_ZTV8Moneybag;
    _ZN12WithMeshClsnD1Ev(t + 0x1e4);
    _ZN18MovingCylinderClsnD1Ev(t + 0x1b0);
    _ZN11ShadowModelD1Ev(t + 0x188);
    _ZN5ModelD1Ev(t + 0x138);
    _ZN9ModelAnimD1Ev(t + 0xd4);
    _ZN5ActorD2Ev(t);
    return (int)(size_t)s;
}
static int __fastcall mn_d0(void *s, void *)
{ return _ZN8MoneybagD0Ev(s); }
static int __fastcall mn_yoshi(void *s, void *)
{ (void)s; return func_ov081_021265b8(); }
static int __fastcall mn_egg(void *s, void *, void *p)
{ func_ov081_02127a7c(s, p); return 0; }
static int __fastcall mn_aimed(void *s, void *)
{ (void)s; return func_ov081_021265c0(); }

extern "C" void hal_fill_moneybag_vtable(void)
{
    /* seat the eighteen records BEFORE InitResources can dispatch through
       them -- its own func_ov081_0212777c(this, 0) call selects state 0 and
       runs the enter half immediately. */
    port_moneybag_states_seat();
    void **vt = (void **)_ZTV8Moneybag;
    ov81_fill_shared(vt);
    vt[0]  = (void *)mn_init;
    vt[3]  = (void *)mn_clean;
    vt[6]  = (void *)mn_behavior;
    vt[9]  = (void *)mn_render;
    vt[12] = (void *)mn_pdes;        /* own OnPendingDestroy, NOT the shared default */
    vt[16] = (void *)mn_d1;
    vt[17] = (void *)mn_d0;
    vt[18] = (void *)mn_yoshi;       /* own OnYoshiTryEat */
    vt[19] = (void *)mn_egg;         /* own OnTurnIntoEgg */
    vt[29] = (void *)mn_aimed;       /* own OnAimedAtWithEgg */
}

// ---- method faces ------------------------------------------------------
// The C-named references the thunks above take onto the real MSVC methods
// against include/, the IceSheet/gate-190 treatment.
#include "Spindrift.h"
#include "MrBlizzard.h"
#include "IceBlock.h"
#include "Snowball.h"
#include "Moneybag.h"
extern "C" {
int _ZN8Snowball13InitResourcesEv(void *self)
{ return ((Snowball *)self)->Snowball::InitResources(); }
int _ZN8Snowball8BehaviorEv(void *self)
{ return ((Snowball *)self)->Snowball::Behavior(); }
int _ZN8Snowball6RenderEv(void *self)
{ return ((Snowball *)self)->Snowball::Render(); }
int _ZN8Moneybag13InitResourcesEv(void *self)
{ return ((Moneybag *)self)->Moneybag::InitResources(); }
int _ZN8Moneybag8BehaviorEv(void *self)
{ return ((Moneybag *)self)->Moneybag::Behavior(); }
/* _ZN8Moneybag6RenderEv is NOT faced here -- the ModelAnim slot-5 collision.
   The matched Moneybag::Render dispatches ROM slot 5 through a six-virtual
   shadow off mModelAnim (+0xd4); host copy in
   port/unmatched/ModelAnim_Renders.cpp spells it as qualified
   ModelAnim::Render. */
int _ZN9Spindrift13InitResourcesEv(void *self)
{ return ((Spindrift *)self)->Spindrift::InitResources(); }
int _ZN9Spindrift8BehaviorEv(void *self)
{ return ((Spindrift *)self)->Spindrift::Behavior(); }
/* _ZN9Spindrift6RenderEv is NOT faced here -- the ModelAnim slot-5 collision.
   The matched Spindrift::Render dispatches ROM slot 5 through a six-virtual
   shadow off mModelAnim (+0x110); host copy in
   port/unmatched/ModelAnim_Renders.cpp spells it as qualified ModelAnim::Render. */
int _ZN10MrBlizzard13InitResourcesEv(void *self)
{ return ((MrBlizzard *)self)->MrBlizzard::InitResources(); }
int _ZN10MrBlizzard8BehaviorEv(void *self)
{ return ((MrBlizzard *)self)->MrBlizzard::Behavior(); }
int _ZN10MrBlizzard6RenderEv(void *self)
{ return ((MrBlizzard *)self)->MrBlizzard::Render(); }
int _ZN8IceBlock13InitResourcesEv(void *self)
{ return ((IceBlock *)self)->IceBlock::InitResources(); }
int _ZN8IceBlock16CleanupResourcesEv(void *self)
{ return ((IceBlock *)self)->IceBlock::CleanupResources(); }
int _ZN8IceBlock8BehaviorEv(void *self)
{ return ((IceBlock *)self)->IceBlock::Behavior(); }
int _ZN8IceBlock6RenderEv(void *self)
{ return ((IceBlock *)self)->IceBlock::Render(); }
}
