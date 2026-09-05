/* ov090, THE WATER ENEMY PACK -- SKEETER (230, 9daMenbo_c), MANTA_RAY
 * (226, 9daManta_c), CHEEP_CHEEP (227, 12daPukupuku_c) and SHARK
 * (225, 9daShark_c). Run rel0215 wave 2, lane cast-ov090.
 * Fresh per-symbol mount (port/ov090_syms.txt), slice port/slice_ov090.txt,
 * the ov077/ov079/ov080/ov081/ov072/ov094 convention.
 *
 * THE CAST MAP, THE FOUR 31-SLOT WIDTHS, THE ID-230 ATTRIBUTION AND THE THREE
 * NAME RACES ARE ALL DERIVED IN port/ov090_syms.txt's HEADER. That file is the
 * evidence; this one is the seat. In brief:
 *
 *   id   class       SpawnInfo   vtable      slots  spawn fn
 *  225   Shark       0x021343c8  0x021343ec    31   0x02133ca0
 *  226   MantaRay    0x02134218  0x0213423c    31   0x02132fe8
 *  227   CheepCheep  0x02134300  0x02134324    31   0x02133634
 *  230   Skeeter     0x02134144  0x02134168    31   0x02132654
 *
 * ALL FOUR TABLES ARE ROM-SHAPED (T1's default). Every one of the four derives
 * from Enemy -- each _Spawn calls _ZN5EnemyC2Ev and every typeinfo record's
 * base word is the same ov002 record 0x021081c0 -- and none derives from
 * Model or ModelAnim; they EMBED a ModelAnim at +0x30c. So the Model/ModelAnim
 * exception to T1 does not apply and the tables stay in ROM order.
 *
 * WHERE THE EMBEDDED ModelAnim IS DISPATCHED, IT COLLIDES, and all four
 * Renders are host copies for exactly that reason (the ov077 Render finding,
 * the Whomp/Fish/Spiny case): each source draws through a six-virtual local
 * shadow over +0x30c and so reaches index 5 in MSVC's own numbering, while
 * hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in MSVC numbering where slot 5
 * is Virtual18. unmatched/{Skeeter,MantaRay,CheepCheep,Shark}_HostSites.cpp
 * name ModelAnim::Render outright.
 *
 * SLOT 30 IS TRAPPED ON ALL FOUR. It is Actor::OnAimedAtWithEggReturnVec
 * (0x020100dc), an SRET method no __fastcall(void*, void*) thunk shape models
 * -- the repo-wide Enemy trap ov077/ov094/ov081/bob_world/flame all take.
 * Slots 13 and 14 (ActorBase::Virtual34/Virtual38) are trapped for the
 * standing reason: neither body is linked anywhere in this port. These are
 * LOUD faces, not silent stubs, and none is reachable for these four classes
 * (ov094's note: slot 30's single dispatcher needs mFlags bit 0x10000000 at
 * +0xb0, which no ov090 body sets).
 *
 * SKEETER IS THE ONLY ONE OF THE FOUR WITH OWN 18/19/29 BODIES
 * (func_ov090_0213264c returns 4, _02132620 gives coins and kills,
 * _02132618 returns 0x20000). MantaRay, CheepCheep and Shark take the arm9
 * Actor defaults in all three, which is what their own relocations say
 * (0x02010160 / 0x02010154 / 0x02010124 at slots 18/19/29).
 *
 * THREE RTTI-SPELLING BRIDGES, AND WHY THEY ARE SAFE. Each D0 stores its table
 * under the class's RTTI name rather than its C++ name -- Skeeter's D0 spells
 * _ZTV9daMenbo_c, CheepCheep's _ZTV12daPukupuku_c, Shark's _ZTV9daShark_c --
 * while each D1 and each _Spawn spells _ZTV7Skeeter / _ZTV10CheepCheep /
 * _ZTV5Shark. They are ONE table each under two spellings: every table's own
 * vtable[-1] typeinfo record was read through to its name string and gives
 * exactly the RTTI spelling (0x02134138 -> "9daMenbo_c", 0x021342e4 ->
 * "12daPukupuku_c", 0x021343bc -> "9daShark_c"). The SkiLift / MotherPenguin /
 * HootTheOwl / Lakitu shape. MANTA RAY HAS NO SECOND SPELLING -- config has no
 * _ZTV9daManta_c row and every MantaRay TU spells _ZTV8MantaRay -- so it gets
 * no bridge, and inventing one would be an alias with nothing on either side.
 * All six names are declared in the config and DEFINED NOWHERE ELSE IN THE
 * LINK -- checked before these were written, because an /alternatename whose
 * LHS is already defined is defeated SILENTLY (the wave-5 R1/R2 class, and the
 * same failure mode this lane had to retire MgBomroom_Faces.cpp's alias over).
 * port/tools/alternatename_guard.py re-checks that against walk_window.map
 * after every link. None of the three belongs in
 * port/tools/alternatename_baseline.txt: that file is the allowlist of aliases
 * whose LHS IS deliberately defined, and adding an alias that is not defeated
 * would blunt the guard rather than record anything.
 *
 * THE STATE SEAT. All four classes run a pointer-to-member state machine and
 * all eight of their dispatch sites are host copies (the four *_HostSites.cpp
 * files: a record-0 ENTER setter and a record-1 TICK call per class). The
 * mounted PMF SOURCE records hold DS CODE addresses -- ovdata.py's pointer
 * pass rebases only pointers into other MOUNTED DATA -- so
 * port_ov090_states_seat() below rewrites each of the sixteen records' fn word
 * with its HOST body's address, VALIDATING the mounted word against the ROM's
 * own address first so a wrong mount aborts loudly instead of seating garbage.
 * It must run BEFORE the four sinits copy those records into the eight bss
 * cells, which is why hal/actor_overlays.cpp calls it between
 * port_ov090_syms_patch() and the first __sinit_ov090_*. The
 * MrBlizzard/BabyPenguin/Unagi/HootTheOwl/Lakitu order.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Actor.h"
#include "ActorBase.h"
#include "CheepCheep.h"
#include "Shark.h"

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

/* ---- the shared arm9 half all four tables name ------------------------- */
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
void _ZN5Actor19OnHitFromUnderneathERS_(void *self, void *o);  /* slot 28 */
int _ZN5Actor16OnAimedAtWithEggEv(void *self);                 /* slot 29 */

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* ---- the four host vtables (the mount excludes all four spans) ---------- */
int _ZTV7Skeeter[31];      /* == _ZTV9daMenbo_c     */
int _ZTV8MantaRay[31];     /* no second spelling    */
int _ZTV10CheepCheep[31];  /* == _ZTV12daPukupuku_c */
int _ZTV5Shark[31];        /* == _ZTV9daShark_c     */

/* ---- SKEETER (230) ----------------------------------------------------- */
int _ZN7Skeeter13InitResourcesEv(void *self);     /* matched src, flat      */
int _ZN7Skeeter8BehaviorEv(void *self);           /* unmatched/Skeeter_HostSites */
int _ZN7Skeeter6RenderEv(void *self);             /* unmatched/Skeeter_HostSites */
int _ZN7Skeeter16CleanupResourcesEv(void);        /* matched src, flat      */
void _ZN7Skeeter16OnPendingDestroyEv(void);       /* matched src, flat      */
int *_ZN7SkeeterD1Ev(int *self);                  /* matched src, flat      */
int *_ZN7SkeeterD0Ev(int *self);                  /* matched src, flat      */
int func_ov090_0213264c(void);                    /* slot 18, own: return 4 */
void func_ov090_02132620(void *self, void *p);    /* slot 19, own           */
int func_ov090_02132618(void);                    /* slot 29, own: 0x20000  */
void *Skeeter_Spawn(void);
extern unsigned char Skeeter_SpawnInfo[];

/* ---- MANTA_RAY (226) --------------------------------------------------- */
int _ZN8MantaRay13InitResourcesEv(void *self);    /* matched src, flat      */
int _ZN8MantaRay8BehaviorEv(void *self);          /* unmatched/MantaRay_HostSites */
int _ZN8MantaRay6RenderEv(void *self);            /* unmatched/MantaRay_HostSites */
int _ZN8MantaRay16CleanupResourcesEv(void);
void _ZN8MantaRay16OnPendingDestroyEv(void);
int *_ZN8MantaRayD1Ev(void *self);
int *_ZN8MantaRayD0Ev(void *self);
void *MantaRay_Spawn(void);
extern unsigned char MantaRay_SpawnInfo[];

/* ---- CHEEP_CHEEP (227) ------------------------------------------------- */
/* InitResources is a REAL C++ METHOD in src (int CheepCheep::InitResources()),
   so it is reached through include/CheepCheep.h, not by a flat name. */
int _ZN10CheepCheep8BehaviorEv(void *self);       /* unmatched/CheepCheep_HostSites */
int _ZN10CheepCheep6RenderEv(void *self);         /* unmatched/CheepCheep_HostSites */
int _ZN10CheepCheep16CleanupResourcesEv(void);
void _ZN10CheepCheep16OnPendingDestroyEv(void);
int *_ZN10CheepCheepD1Ev(int *self);
int *_ZN10CheepCheepD0Ev(int *self);
void *CheepCheep_Spawn(void);
extern unsigned char CheepCheep_SpawnInfo[];

/* ---- SHARK (225) ------------------------------------------------------- */
/* InitResources is a REAL C++ METHOD in src (int Shark::InitResources()). */
int _ZN5Shark8BehaviorEv(void *self);             /* unmatched/Shark_HostSites */
int _ZN5Shark6RenderEv(void *self);               /* unmatched/Shark_HostSites */
int _ZN5Shark16CleanupResourcesEv(void);
void _ZN5Shark16OnPendingDestroyEv(void);
int *_ZN5SharkD1Ev(int *self);
int *_ZN5SharkD0Ev(int *self);
void *Shark_Spawn(void);
extern unsigned char Shark_SpawnInfo[];

/* ---- the sixteen PMF source records the mount hosts --------------------- */
extern unsigned int data_ov090_021340e0[];
extern unsigned int data_ov090_021340e8[];
extern unsigned int data_ov090_021340f0[];
extern unsigned int data_ov090_021340f8[];
extern unsigned int data_ov090_02134100[];
extern unsigned int data_ov090_02134108[];
extern unsigned int data_ov090_02134110[];
extern unsigned int data_ov090_02134118[];
extern unsigned int data_ov090_021341e4[];
extern unsigned int data_ov090_021341ec[];
extern unsigned int data_ov090_021342b8[];
extern unsigned int data_ov090_021342c0[];
extern unsigned int data_ov090_021342c8[];
extern unsigned int data_ov090_021342d0[];
extern unsigned int data_ov090_021343a0[];
extern unsigned int data_ov090_021343a8[];

/* ---- the sixteen host state bodies ------------------------------------- */
int func_ov090_02131648(void *c);
int func_ov090_02131a74(void *c);
int func_ov090_02131608(void *c);
int func_ov090_02131ac4(void *c);
int func_ov090_02131584(void *c);
int func_ov090_02131c48(void *c);
int func_ov090_02131db0(void *c);
int func_ov090_02131b94(void *c);
int func_ov090_021327e4(void *c);
int func_ov090_02132a58(void *c);
int func_ov090_02133200(void *c);
int func_ov090_02133290(void *c);
int func_ov090_02133190(void *c);
int func_ov090_021331c4(void *c);
int func_ov090_0213387c(void *c);
int func_ov090_02133830(void *c);

}  /* extern "C" */

/* ============================================================================
 * THE STATE SEAT
 * Every `rom` column below is the word the ROM actually holds at that record,
 * read out of extracted/overlays/overlay_0090.bin (never dsd's copy, T4) and
 * cross-checked against the class's own .text block: Skeeter's eight all land
 * in 0x02130f00..0x02132654, MantaRay's two in 0x0213269c..0x02132fe8,
 * CheepCheep's four in 0x02133034..0x02133634 and Shark's two in
 * 0x0213367c..0x02133ca0. Every delta word is 0 in the ROM, which is what
 * makes a plain call through the fn word the same call the ROM makes.
 * ==========================================================================*/
namespace {
struct Ov090Seat {
    unsigned int *rec;
    unsigned int rom;
    int (*host)(void *);
    const char *name;
};
const Ov090Seat g_ov090_seats[] = {
    /* Skeeter, 0x021340e0..0x02134120, copied into the four two-record bss
       cells data_ov090_021344e4/_021344f4/_02134504/_02134514 by
       __sinit_ov090_02133ce8. */
    {data_ov090_021340e0, 0x02131648, func_ov090_02131648, "skeeter/021340e0"},
    {data_ov090_021340e8, 0x02131a74, func_ov090_02131a74, "skeeter/021340e8"},
    {data_ov090_021340f0, 0x02131608, func_ov090_02131608, "skeeter/021340f0"},
    {data_ov090_021340f8, 0x02131ac4, func_ov090_02131ac4, "skeeter/021340f8"},
    {data_ov090_02134100, 0x02131584, func_ov090_02131584, "skeeter/02134100"},
    {data_ov090_02134108, 0x02131c48, func_ov090_02131c48, "skeeter/02134108"},
    {data_ov090_02134110, 0x02131db0, func_ov090_02131db0, "skeeter/02134110"},
    {data_ov090_02134118, 0x02131b94, func_ov090_02131b94, "skeeter/02134118"},
    /* MantaRay, 0x021341e4..0x021341f4, into data_ov090_0213454c by
       __sinit_ov090_02133ea8. */
    {data_ov090_021341e4, 0x021327e4, func_ov090_021327e4, "mantaray/021341e4"},
    {data_ov090_021341ec, 0x02132a58, func_ov090_02132a58, "mantaray/021341ec"},
    /* CheepCheep, 0x021342b8..0x021342d8, into data_ov090_02134584 and
       _02134594 by __sinit_ov090_02133f4c. */
    {data_ov090_021342b8, 0x02133200, func_ov090_02133200, "cheepcheep/021342b8"},
    {data_ov090_021342c0, 0x02133290, func_ov090_02133290, "cheepcheep/021342c0"},
    {data_ov090_021342c8, 0x02133190, func_ov090_02133190, "cheepcheep/021342c8"},
    {data_ov090_021342d0, 0x021331c4, func_ov090_021331c4, "cheepcheep/021342d0"},
    /* Shark, 0x021343a0..0x021343b0, into data_ov090_021345cc by
       __sinit_ov090_02134020. */
    {data_ov090_021343a0, 0x0213387c, func_ov090_0213387c, "shark/021343a0"},
    {data_ov090_021343a8, 0x02133830, func_ov090_02133830, "shark/021343a8"},
};
bool g_ov090_seated = false;
}  /* namespace */

extern "C" void port_ov090_states_seat(void)
{
    if (g_ov090_seated)
        return;
    for (unsigned i = 0; i < sizeof g_ov090_seats / sizeof g_ov090_seats[0]; ++i) {
        const Ov090Seat &s = g_ov090_seats[i];
        if (s.rec[0] != s.rom) {
            /* Either the mount is pointing at the wrong bytes or this record
               has already been seated. Both are worth stopping for: seating a
               second time over a host address would write a host pointer where
               the ROM check expects a DS one, and every later run of this
               function would then read as a wrong mount. */
            std::fprintf(stderr,
                "FATAL: ov090 state seat %s: mounted fn word reads 0x%08x, "
                "the ROM says 0x%08x\n", s.name, s.rec[0], s.rom);
            std::abort();
        }
        if (s.rec[1] != 0) {
            std::fprintf(stderr,
                "FATAL: ov090 state seat %s: delta word is 0x%08x, every ov090 "
                "PMF record's delta is 0 in the ROM\n", s.name, s.rec[1]);
            std::abort();
        }
        s.rec[0] = (unsigned int)(size_t)s.host;
    }
    g_ov090_seated = true;
}

// ============================================================================
// THE SHARED 31-SLOT ACTOR HALF (identical across all four tables; every
// shared slot was compared address for address against the other three and
// against ov077's own shared half before this was written to be one body)
// ============================================================================
namespace {
void ov90_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov090 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov090 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
}  /* namespace */
static int __fastcall ov90_trap13(void *s, void *) { ov90_trap_report(s, 13); return 0; }
static int __fastcall ov90_trap14(void *s, void *) { ov90_trap_report(s, 14); return 0; }

static int __fastcall ov90_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov90_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov90_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov90_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov90_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov90_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov90_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov90_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov90_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov90_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov90_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov90_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov90_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov90_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov90_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov90_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov90_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov90_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov90_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov90_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov90_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Fills 1,2,4,5,7,8,10,11,13,14,15,18..30 -- the standard 31-slot Actor half.
   18, 19 and 29 are the arm9 defaults here and are OVERWRITTEN below for
   SKEETER alone, which owns all three; MantaRay, CheepCheep and Shark keep all
   three defaults, which is what their own relocations say. */
static void ov90_fill_shared(void **vt)
{
    vt[1]  = (void *)ov90_binit;
    vt[2]  = (void *)ov90_ainit;
    vt[4]  = (void *)ov90_bclean;
    vt[5]  = (void *)ov90_aclean;
    vt[7]  = (void *)ov90_bbeh;
    vt[8]  = (void *)ov90_abeh;
    vt[10] = (void *)ov90_bren;
    vt[11] = (void *)ov90_aren;
    vt[13] = (void *)ov90_trap13;
    vt[14] = (void *)ov90_trap14;
    vt[15] = (void *)ov90_heap;
    vt[18] = (void *)ov90_yoshi;
    vt[19] = (void *)ov90_egg;
    vt[20] = (void *)ov90_v50;
    vt[21] = (void *)ov90_pounded;
    vt[22] = (void *)ov90_atk1;
    vt[23] = (void *)ov90_atk2;
    vt[24] = (void *)ov90_kicked;
    vt[25] = (void *)ov90_pushed;
    vt[26] = (void *)ov90_cannon;
    vt[27] = (void *)ov90_mega;
    vt[28] = (void *)ov90_under;
    vt[29] = (void *)ov90_aimed;
    vt[30] = (void *)port_actor_s30_base;
}

// ============================================================================
// SKEETER (9daMenbo_c, 230). Own: 0 Init, 3 Cleanup, 6 Behavior, 9 Render,
// 12 OnPendingDestroy, 16 D1, 17 D0, 18, 19, 29.
// ============================================================================
static int __fastcall skt_init(void *s, void *)
{ return _ZN7Skeeter13InitResourcesEv(s); }
static int __fastcall skt_clean(void *s, void *)
{ (void)s; return _ZN7Skeeter16CleanupResourcesEv(); }
static int __fastcall skt_behavior(void *s, void *)
{ return _ZN7Skeeter8BehaviorEv(s); }
static int __fastcall skt_render(void *s, void *)
{ port_actor_render_probe("SKEETER", (char *)s + 0x30c);
  return _ZN7Skeeter6RenderEv(s); }
static int __fastcall skt_pdes(void *s, void *)
{ (void)s; _ZN7Skeeter16OnPendingDestroyEv(); return 0; }
static int __fastcall skt_d1(void *s, void *)
{ return (int)(size_t)_ZN7SkeeterD1Ev((int *)s); }
static int __fastcall skt_d0(void *s, void *)
{ return (int)(size_t)_ZN7SkeeterD0Ev((int *)s); }
static int __fastcall skt_yoshi(void *s, void *)
{ (void)s; return func_ov090_0213264c(); }
static int __fastcall skt_egg(void *s, void *, void *p)
{ func_ov090_02132620(s, p); return 0; }
static int __fastcall skt_aimed(void *s, void *)
{ (void)s; return func_ov090_02132618(); }

extern "C" void hal_fill_skeeter_vtable(void)
{
    port_ov090_states_seat();
    void **vt = (void **)_ZTV7Skeeter;
    ov90_fill_shared(vt);
    vt[0]  = (void *)skt_init;
    vt[3]  = (void *)skt_clean;
    vt[6]  = (void *)skt_behavior;
    vt[9]  = (void *)skt_render;
    vt[12] = (void *)skt_pdes;
    vt[16] = (void *)skt_d1;
    vt[17] = (void *)skt_d0;
    vt[18] = (void *)skt_yoshi;
    vt[19] = (void *)skt_egg;
    vt[29] = (void *)skt_aimed;
}

// ============================================================================
// MANTA_RAY (9daManta_c, 226). Own: 0, 3, 6, 9, 12, 16, 17. 18/19/29 arm9.
// ============================================================================
static int __fastcall mrp_init(void *s, void *)
{ return _ZN8MantaRay13InitResourcesEv(s); }
static int __fastcall mrp_clean(void *s, void *)
{ (void)s; return _ZN8MantaRay16CleanupResourcesEv(); }
static int __fastcall mrp_behavior(void *s, void *)
{ return _ZN8MantaRay8BehaviorEv(s); }
static int __fastcall mrp_render(void *s, void *)
{ port_actor_render_probe("MANTA_RAY", (char *)s + 0x30c);
  return _ZN8MantaRay6RenderEv(s); }
static int __fastcall mrp_pdes(void *s, void *)
{ (void)s; _ZN8MantaRay16OnPendingDestroyEv(); return 0; }
static int __fastcall mrp_d1(void *s, void *)
{ return (int)(size_t)_ZN8MantaRayD1Ev(s); }
static int __fastcall mrp_d0(void *s, void *)
{ return (int)(size_t)_ZN8MantaRayD0Ev(s); }

extern "C" void hal_fill_manta_ray_vtable(void)
{
    port_ov090_states_seat();
    void **vt = (void **)_ZTV8MantaRay;
    ov90_fill_shared(vt);
    vt[0]  = (void *)mrp_init;
    vt[3]  = (void *)mrp_clean;
    vt[6]  = (void *)mrp_behavior;
    vt[9]  = (void *)mrp_render;
    vt[12] = (void *)mrp_pdes;
    vt[16] = (void *)mrp_d1;
    vt[17] = (void *)mrp_d0;
}

// ============================================================================
// CHEEP_CHEEP (12daPukupuku_c, 227). Own: 0, 3, 6, 9, 12, 16, 17.
// InitResources is a real C++ method and is called through the header.
// ============================================================================
static int __fastcall chp_init(void *s, void *)
{ return ((CheepCheep *)s)->CheepCheep::InitResources(); }
static int __fastcall chp_clean(void *s, void *)
{ (void)s; return _ZN10CheepCheep16CleanupResourcesEv(); }
static int __fastcall chp_behavior(void *s, void *)
{ return _ZN10CheepCheep8BehaviorEv(s); }
static int __fastcall chp_render(void *s, void *)
{ port_actor_render_probe("CHEEP_CHEEP", (char *)s + 0x30c);
  return _ZN10CheepCheep6RenderEv(s); }
static int __fastcall chp_pdes(void *s, void *)
{ (void)s; _ZN10CheepCheep16OnPendingDestroyEv(); return 0; }
static int __fastcall chp_d1(void *s, void *)
{ return (int)(size_t)_ZN10CheepCheepD1Ev((int *)s); }
static int __fastcall chp_d0(void *s, void *)
{ return (int)(size_t)_ZN10CheepCheepD0Ev((int *)s); }

extern "C" void hal_fill_cheep_cheep_vtable(void)
{
    port_ov090_states_seat();
    void **vt = (void **)_ZTV10CheepCheep;
    ov90_fill_shared(vt);
    vt[0]  = (void *)chp_init;
    vt[3]  = (void *)chp_clean;
    vt[6]  = (void *)chp_behavior;
    vt[9]  = (void *)chp_render;
    vt[12] = (void *)chp_pdes;
    vt[16] = (void *)chp_d1;
    vt[17] = (void *)chp_d0;
}

// ============================================================================
// SHARK (9daShark_c, 225). Own: 0, 3, 6, 9, 12, 16, 17.
// InitResources is a real C++ method and is called through the header.
// ============================================================================
static int __fastcall shk_init(void *s, void *)
{ return ((Shark *)s)->Shark::InitResources(); }
static int __fastcall shk_clean(void *s, void *)
{ (void)s; return _ZN5Shark16CleanupResourcesEv(); }
static int __fastcall shk_behavior(void *s, void *)
{ return _ZN5Shark8BehaviorEv(s); }
static int __fastcall shk_render(void *s, void *)
{ port_actor_render_probe("SHARK", (char *)s + 0x30c);
  return _ZN5Shark6RenderEv(s); }
static int __fastcall shk_pdes(void *s, void *)
{ (void)s; _ZN5Shark16OnPendingDestroyEv(); return 0; }
static int __fastcall shk_d1(void *s, void *)
{ return (int)(size_t)_ZN5SharkD1Ev((int *)s); }
static int __fastcall shk_d0(void *s, void *)
{ return (int)(size_t)_ZN5SharkD0Ev((int *)s); }

extern "C" void hal_fill_shark_vtable(void)
{
    port_ov090_states_seat();
    void **vt = (void **)_ZTV5Shark;
    ov90_fill_shared(vt);
    vt[0]  = (void *)shk_init;
    vt[3]  = (void *)shk_clean;
    vt[6]  = (void *)shk_behavior;
    vt[9]  = (void *)shk_render;
    vt[12] = (void *)shk_pdes;
    vt[16] = (void *)shk_d1;
    vt[17] = (void *)shk_d0;
}

/* THE THREE RTTI-SPELLING BRIDGES. Each is ONE table under two names; the
 * right-hand side is the array defined above and the left-hand side is
 * defined nowhere; alternatename_guard.py fails the build post-link if that
 * ever stops being true. MantaRay has no second spelling and gets no line. */
#pragma comment(linker, "/alternatename:__ZTV9daMenbo_c=__ZTV7Skeeter")
#pragma comment(linker, "/alternatename:__ZTV12daPukupuku_c=__ZTV10CheepCheep")
#pragma comment(linker, "/alternatename:__ZTV9daShark_c=__ZTV5Shark")

/* ============================================================================
 * TEN LINKAGE ALIASES, ALL MEASURED OFF THE FIRST LINK, NONE PREDICTED.
 *
 * The gate-10 cxx_aliases shape (hal/cxx_aliases.cpp's own header states it):
 * a `//cpp` TU that declares a C-named symbol WITHOUT extern "C" emits an MSVC
 * decoration for what is a C name everywhere else, and the alias closes the gap
 * without touching src/. Every left-hand side below was copied VERBATIM out of
 * the link log's LNK2019 lines; every right-hand side is a symbol this link
 * already defines. None of the ten left-hand sides is defined anywhere, which
 * is the condition port/tools/alternatename_guard.py re-checks post-link -- a
 * defined LHS defeats an /alternatename silently, which is the failure this
 * whole lane had to retire MgBomroom_Faces.cpp's alias over.
 *
 * SIX ov090 DATA SYMBOLS this mount defines, each wanted under whatever struct
 * type its reading TU declared for it. The mount emits one flat C array per
 * symbol, so all six are the same object under a decorated name.
 */
#pragma comment(linker, "/alternatename:?data_ov090_021344e4@@3DA=_data_ov090_021344e4")
#pragma comment(linker, "/alternatename:?data_ov090_02134504@@3DA=_data_ov090_02134504")
#pragma comment(linker, "/alternatename:?data_ov090_02134490@@3UData134490@@A=_data_ov090_02134490")
#pragma comment(linker, "/alternatename:?data_ov090_021342d8@@3UVector3@@A=_data_ov090_021342d8")
#pragma comment(linker, "/alternatename:?data_ov090_02134564@@3USharedFilePtr@@A=_data_ov090_02134564")
#pragma comment(linker, "/alternatename:?data_ov090_0213455c@@3UAnimFilePtr@@A=_data_ov090_0213455c")

/* THREE arm9 FREE FUNCTIONS, cdecl on both sides (`@@YAX...@Z`), already in the
 * link under their flat names. These are three MORE decoration variants of the
 * same three functions hal/cxx_aliases.cpp:1450/1465/1910 and
 * hal/actor_classes_ov070.cpp:281 and hal/actor_faces_bob.cpp:100 already carry
 * -- each TU spells the parameter types slightly differently and each spelling
 * decorates differently. One line per variant is the tree's convention. */
#pragma comment(linker, "/alternatename:?_ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj@@YAXPAXPAUActor@@ABUVector3@@HHII@Z=__ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj")
#pragma comment(linker, "/alternatename:?_ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_@@YAXPAXPAUActor@@HHPAUVector3_16@@H@Z=__ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_")
#pragma comment(linker, "/alternatename:?_ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj@@YAXPAXPAUBCA_File@@HHI@Z=__ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj")

/* ONE __thiscall METHOD, AND IT IS SAFE FOR A REASON WORTH WRITING DOWN.
 * src/func_ov090_02132620.cpp (SKEETER's slot 19) declares its own
 * `class Player;` and calls Actor::GivePlayerCoins as a member, so it wants
 *     ?GivePlayerCoins@Actor@@QAEXAAVPlayer@@EI@Z     (class Player,  V)
 * while hal/actor_classes_ov070.cpp:782 already DEFINES the same method
 * against include/Player.h, where Player is a struct:
 *     ?GivePlayerCoins@Actor@@QAEXAAUPlayer@@EI@Z     (struct Player, U)
 * -- read out of that TU's own object file with dumpbin /symbols, not guessed.
 * Both are `QAE` (public __thiscall), same return, same three parameter widths;
 * only the class/struct tag differs, and MSVC's class-vs-struct tag has no ABI
 * meaning. So this alias is ABI-IDENTICAL, unlike the GivePlayerCoins alias
 * hal/actor_classes_ov070.cpp:286 correctly REFUSED -- that one would have
 * pointed a __thiscall reference at the cdecl C body, which is why ov070 wrote
 * the forwarding method instead. This one points a __thiscall reference at
 * ov070's __thiscall method. Same direction, opposite hazard. */
#pragma comment(linker, "/alternatename:?GivePlayerCoins@Actor@@QAEXAAVPlayer@@EI@Z=?GivePlayerCoins@Actor@@QAEXAAUPlayer@@EI@Z")
