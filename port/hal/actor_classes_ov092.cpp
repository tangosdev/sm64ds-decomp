/* ov092, THE TOX_BOX PACK -- TOX_BOX (309, 8daOnms_c), the crushing block on
 * level 16 (Shifting Sand Land). Run rel0215 wave 2, lane cast-lvl16pair,
 * sub-job 1 of 2. Fresh per-symbol mount (port/ov092_syms.txt), slice
 * port/slice_ov092.txt, the ov017/ov077/ov079/ov080/ov081/ov072/ov094
 * convention.
 *
 * THE CAST MAP, THE 32-SLOT WIDTH, THE FOOTPRINT ARGUMENT AND THE THREE
 * AGREEING IDENTITY ROUTES ARE ALL DERIVED IN port/ov092_syms.txt's HEADER.
 * That file is the evidence; this one is the seat. In brief:
 *
 *   id   class   SpawnInfo   vtable      slots  spawn fn
 *  309   ToxBox  0x021322ac  0x021322d0    32   0x02132018
 *
 * THE TABLE IS ROM-SHAPED (T1's default), and this class is PLATFORM-DERIVED,
 * not plain Actor. Slot 30 is Actor::OnAimedAtWithEggReturnVec (0x020100dc,
 * the last Actor slot) and slot 31 is 0x020ee55c, which
 * config/arm9/overlays/ov002/symbols.txt:1175 names _ZN8Platform4KillEv. So it
 * is a 32-slot Platform close, the ov017 ShipWater shape, not the 31-slot
 * Actor close ov077's three enemies take. ToxBox_Spawn confirms it without the
 * relocations: it calls _ZN8PlatformC2Ev before it stores _ZTV6ToxBox, and
 * both destructors restore _ZTV8Platform on the way down.
 *
 * NOTHING IN THIS OVERLAY DISPATCHES A VIRTUAL ON THE ACTOR ITSELF through a
 * local shadow, so the ov077 slot-29 question does not arise here. The one
 * shadow dispatch in the overlay is ToxBox::Render's, and it is on the MODEL
 * member at +0xd4, which hal/cxxname_bridge.cpp:511 dual-fills for exactly
 * this shape -- see port/slice_ov092.txt for why that is the ov017 case and
 * not the ov077 ModelAnim collision.
 *
 * SLOT 30 IS TRAPPED. It is Actor::OnAimedAtWithEggReturnVec, an SRET method
 * no __fastcall(void*, void*) thunk shape models -- the repo-wide trap
 * ov017/ov094/ov081/bob_world/flame all take. Slots 13 and 14
 * (ActorBase::Virtual34/Virtual38) are trapped for the standing reason:
 * neither body is linked anywhere in this port. These are LOUD faces, not
 * silent stubs, and none is reachable for this class (slot 30's single
 * dispatcher needs mFlags bit 0x10000000 at +0xb0, which no ov092 body sets).
 *
 * NO RTTI-SPELLING BRIDGE, AND THAT IS A MEASURED RESULT. dsd gives 0x021322d0
 * exactly ONE name, _ZTV6ToxBox -- there is no _ZTV9daOnms_c row to reconcile,
 * unlike ov077's three pairs and ov017's one. The RTTI string is still read
 * through and recorded (vtable[-1] -> 0x021322a0 -> 0x02132288 -> "8daOnms_c")
 * because the wave-1 lesson is that identity shifts are the NORM and the check
 * is what establishes there is none, not the absence of a second config row.
 * Both destructors' pools AND the factory's pool all relocate to 0x021322d0
 * (relocs 0x02130f54, 0x02130fc0, 0x02132068), so all three routes install one
 * table under one spelling. This file therefore carries NO /alternatename at
 * all and adds no row to port/tools/alternatename_baseline.txt.
 *
 * THE STATE SEAT. ToxBox runs a nine-state pointer-to-member machine and its
 * ONE dispatch site is a host copy (port/unmatched/ToxBox_HostSites.cpp). The
 * mounted PMF SOURCE records hold DS CODE addresses -- ovdata.py's pointer pass
 * rebases only pointers into other MOUNTED DATA -- so port_ov092_states_seat()
 * below rewrites each record's fn word with its HOST body's address, VALIDATING
 * the mounted word against the ROM's own address first so a wrong mount aborts
 * loudly instead of seating garbage. It must run BEFORE the sinit copies those
 * records into bss, which is why port_ov92_bringup() calls it between
 * port_ov092_syms_patch() and __sinit_ov092_021320cc(). The MrBlizzard/
 * BabyPenguin/Unagi/HootTheOwl/ov077 order.
 */
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
#include "dsstate_seg.h"

#include "Actor.h"
#include "ActorBase.h"

extern "C" {

/* ---- the shared arm9 half this table names ------------------------------ */
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

/* the generated ov092 per-symbol mount (build/port/host-src/ov092_syms.c) */
void port_ov092_pack_check(void);
void port_ov092_syms_patch(void);
/* ov092's single sinit: files 0x3c9 (model) and 0x3ca (collision), their two
   chain nodes, and the nine-entry PMF state table into bss. */
void __sinit_ov092_021320cc(void);

/* the bodies src spells with C names (port/slice_ov092.txt) */
int _ZN6ToxBox13InitResourcesEv(char *self);      /* slot 0, a plain .c body */
int _ZN6ToxBox8BehaviorEv(void *self);            /* slot 6, unmatched/ToxBox_HostSites */
int *_ZN6ToxBoxD1Ev(int *self);                   /* slot 16 */
int *_ZN6ToxBoxD0Ev(int *self);                   /* slot 17 */
void *ToxBox_Spawn(void);                         /* id 309 */
extern unsigned char ToxBox_SpawnInfo[];

/* the host vtable, excluded from the mount */
DSSTATE_BEGIN
void *_ZTV6ToxBox[32];        /* 0x021322d0 */
DSSTATE_END
}

/* The two bodies src defines as REAL C++ METHODS, faced here. Both TUs stay in
   the slice; MSVC mangles them ?CleanupResources@ToxBox@@QAEHXZ and
   ?Render@ToxBox@@QAEHXZ, so nothing answers to the flat name the fill uses.
   The BabyPenguin/IceSheet/OneUpLogo/HootTheOwl/ShipWater recipe.
   InitResources needs NO face -- src/_ZN6ToxBox13InitResourcesEv.c is a .c
   file and already defines the flat name. */
#include "ToxBox.h"
extern "C" {
int _ZN6ToxBox16CleanupResourcesEv(void *self)
{ return ((ToxBox *)self)->ToxBox::CleanupResources(); }
int _ZN6ToxBox6RenderEv(void *self)
{ return ((ToxBox *)self)->ToxBox::Render(); }
}

// ============================================================================
// THE STATE SEAT
// ============================================================================
extern "C" {
/* ToxBox's nine handler bodies, one per state. Eight return void;
   func_ov092_02131010 returns int. The ROM's own typedef is
   `void (ToxBox::*PMF)()` and the dispatch discards r0, so the seat table is
   typed void-returning to match it and the one int-returning body is reached
   through it on plain cdecl, where an ignored eax is the same discard. See
   port/unmatched/ToxBox_HostSites.cpp for the full note. */
void func_ov092_02130fcc(char *);
int func_ov092_02131010(unsigned char *);
void func_ov092_021311b0(void *);
void func_ov092_02131578(char *);
void func_ov092_021315ac(char *);
void func_ov092_02131620(void *);
void func_ov092_02131650(void *);
void func_ov092_02131680(void *);
void func_ov092_021316b0(void *);

/* the mounted SOURCE records, one 8-byte {fn, delta} pair each. */
extern unsigned int data_ov092_02132240[], data_ov092_02132248[],
    data_ov092_02132250[], data_ov092_02132258[], data_ov092_02132260[],
    data_ov092_02132268[], data_ov092_02132270[], data_ov092_02132278[],
    data_ov092_02132280[];
}

namespace {
struct Ov092Seat {
    unsigned int *rec;      /* the mounted source record        */
    unsigned int rom;       /* what its fn word must read first */
    void *host;             /* the host body to seat            */
    const char *name;
};
/* Each row's `rom` is the ROM's own relocation target for that record's word 0
   (config/arm9/overlays/ov092/relocs.txt, from:0x02132240 through
   from:0x02132280, resolved in port/ov092_syms.txt's data map). The seat
   REFUSES a record whose mounted word is not that. The comment on each row is
   the STATE INDEX __sinit_ov092_021320cc copies that record into, which is a
   permutation and not the address order -- see the table in
   port/unmatched/ToxBox_HostSites.cpp. */
const Ov092Seat g_ov092_seats[] = {
    {data_ov092_02132240, 0x02131680, (void *)func_ov092_02131680, "toxbox/02132240 state3"},
    {data_ov092_02132248, 0x021311b0, (void *)func_ov092_021311b0, "toxbox/02132248 state6"},
    {data_ov092_02132250, 0x02131650, (void *)func_ov092_02131650, "toxbox/02132250 state4"},
    {data_ov092_02132258, 0x021316b0, (void *)func_ov092_021316b0, "toxbox/02132258 state2"},
    {data_ov092_02132260, 0x02131620, (void *)func_ov092_02131620, "toxbox/02132260 state5"},
    {data_ov092_02132268, 0x021315ac, (void *)func_ov092_021315ac, "toxbox/02132268 state0"},
    {data_ov092_02132270, 0x02130fcc, (void *)func_ov092_02130fcc, "toxbox/02132270 state8"},
    {data_ov092_02132278, 0x02131010, (void *)func_ov092_02131010, "toxbox/02132278 state7"},
    {data_ov092_02132280, 0x02131578, (void *)func_ov092_02131578, "toxbox/02132280 state1"},
};
DSSTATE_BEGIN
bool g_ov092_seated = false;
DSSTATE_END
}  /* namespace */

extern "C" void port_ov092_states_seat(void)
{
    if (g_ov092_seated)
        return;
    for (unsigned i = 0; i < sizeof g_ov092_seats / sizeof g_ov092_seats[0]; ++i) {
        const Ov092Seat &s = g_ov092_seats[i];
        if (s.rec[0] != s.rom) {
            /* Either the mount is pointing at the wrong bytes or this record
               has already been seated. Both are worth stopping for: seating a
               second time over a host address would write a host pointer where
               the ROM check expects a DS one, and every later run of this
               function would then read as a wrong mount. */
            std::fprintf(stderr,
                "FATAL: ov092 state seat %s: mounted fn word reads 0x%08x, "
                "the ROM says 0x%08x\n", s.name, s.rec[0], s.rom);
            std::abort();
        }
        if (s.rec[1] != 0) {
            std::fprintf(stderr,
                "FATAL: ov092 state seat %s: delta word is 0x%08x, every ov092 "
                "PMF record's delta is 0 in the ROM\n", s.name, s.rec[1]);
            std::abort();
        }
        s.rec[0] = (unsigned int)(size_t)s.host;
    }
    g_ov092_seated = true;
}

// ---- the traps -------------------------------------------------------------
static void ov92_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov092 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov092 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV92_TRAP(n) \
    static int __fastcall ov92_trap##n(void *s, void *) \
    { ov92_trap_report(s, n); return 0; }
OV92_TRAP(13) OV92_TRAP(14)
#undef OV92_TRAP

static int __fastcall ov92_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov92_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov92_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov92_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov92_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov92_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov92_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov92_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
/* slot 12: the ROM word is 0x02043ac0, the arm9 ActorBase default. ToxBox has
   NO own OnPendingDestroy, unlike all three of ov077's classes. */
static int __fastcall ov92_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov92_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov92_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov92_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov92_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov92_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov92_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov92_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov92_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov92_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov92_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov92_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov92_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov92_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }
static int __fastcall ov92_kill(void *s, void *)
{ _ZN8Platform4KillEv(s); return 0; }

// ============================================================================
// TOX_BOX (8daOnms_c, 309) -- table 0x021322d0, 32 slots. Own bodies at
// 0 Init, 3 Cleanup, 6 Behavior, 9 Render, 16 D1, 17 D0. Slots 18, 19 and 29
// are the arm9 Actor defaults (0x02010160 / 0x02010154 / 0x02010124), which is
// what its own relocations say -- the ov077 HeaveHo pattern, not the
// Lakitu/Spiny one.
// ============================================================================
static int __fastcall tb_init(void *s, void *)
{ return _ZN6ToxBox13InitResourcesEv((char *)s); }
static int __fastcall tb_clean(void *s, void *)
{ return _ZN6ToxBox16CleanupResourcesEv(s); }
static int __fastcall tb_behavior(void *s, void *)
{ return _ZN6ToxBox8BehaviorEv(s); }
static int __fastcall tb_render(void *s, void *)
{ port_actor_render_probe("TOX_BOX", (char *)s + 0xd4);
  return _ZN6ToxBox6RenderEv(s); }
static int __fastcall tb_d1(void *s, void *)
{ return (int)(size_t)_ZN6ToxBoxD1Ev((int *)s); }
static int __fastcall tb_d0(void *s, void *)
{ return (int)(size_t)_ZN6ToxBoxD0Ev((int *)s); }

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov92_bringup_done;
DSSTATE_END

extern "C" void port_ov92_bringup(void)
{
    if (g_ov92_bringup_done)
        return;
    g_ov92_bringup_done = 1;
    port_ov092_pack_check();
    port_ov092_syms_patch();
    hal_fill_platform_vtable();
    /* BEFORE the sinit: it copies the nine source records into bss, and the
       records must already carry host addresses when it does. */
    port_ov092_states_seat();
    __sinit_ov092_021320cc();
}

/* ONE table and ONE caller, so the shared half is written inline rather than
   through a helper -- the gate-200 elided-stores bug needs two call sites with
   distinct extern-array arguments to trigger. The pointer is still volatile.
   The ov017 shape. */
extern "C" void hal_fill_tox_box_vtable(void)
{
    port_ov92_bringup();
    void *volatile *vt = (void *volatile *)_ZTV6ToxBox;
    vt[0]  = (void *)tb_init;
    vt[1]  = (void *)ov92_binit;
    vt[2]  = (void *)ov92_ainit;
    vt[3]  = (void *)tb_clean;
    vt[4]  = (void *)ov92_bclean;
    vt[5]  = (void *)ov92_aclean;
    vt[6]  = (void *)tb_behavior;
    vt[7]  = (void *)ov92_bbeh;
    vt[8]  = (void *)ov92_abeh;
    vt[9]  = (void *)tb_render;
    vt[10] = (void *)ov92_bren;
    vt[11] = (void *)ov92_aren;
    vt[12] = (void *)ov92_pdes;
    vt[13] = (void *)ov92_trap13;
    vt[14] = (void *)ov92_trap14;
    vt[15] = (void *)ov92_heap;
    vt[16] = (void *)tb_d1;
    vt[17] = (void *)tb_d0;
    vt[18] = (void *)ov92_yoshi;
    vt[19] = (void *)ov92_turn_egg;
    vt[20] = (void *)ov92_v50;
    vt[21] = (void *)ov92_pounded;
    vt[22] = (void *)ov92_atk1;
    vt[23] = (void *)ov92_atk2;
    vt[24] = (void *)ov92_kicked;
    vt[25] = (void *)ov92_pushed;
    vt[26] = (void *)ov92_cannon;
    vt[27] = (void *)ov92_mega;
    vt[28] = (void *)ov92_under;
    vt[29] = (void *)ov92_egg;
    vt[30] = (void *)port_actor_s30_base;
    vt[31] = (void *)ov92_kill;
}
