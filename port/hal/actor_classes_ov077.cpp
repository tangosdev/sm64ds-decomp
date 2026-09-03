/* ov077, THE SHARED ENEMY PACK -- HEAVE_HO (238, daPopoi_c), SPINY (260,
 * daTgz_c) and LAKITU (265, daJgm_c). Run rel0215 wave 1, lane cast-ov077.
 * Fresh per-symbol mount (port/ov077_syms.txt), slice port/slice_ov077.txt,
 * the ov079/ov080/ov081/ov072/ov094 convention.
 *
 * THE CAST MAP, THE THREE 31-SLOT WIDTHS, THE FOUR WIDTH ROUTES AND THE THREE
 * CONTESTED ADDRESSES ARE ALL DERIVED IN port/ov077_syms.txt's HEADER. That
 * file is the evidence; this one is the seat. In brief:
 *
 *   id   class    SpawnInfo   vtable      slots  spawn fn
 *  238   HeaveHo  0x02127a74  0x02127a98    31   0x021271d4
 *  260   Spiny    0x02127960  0x02127984    31   0x021261f4
 *  265   Lakitu   0x02127848  0x0212786c    31   0x02124b04
 *
 * ALL THREE TABLES ARE ROM-SHAPED (T1's default). Every ov077 body that
 * dispatches a virtual on the ACTOR ITSELF does so through a local shadow
 * struct with thirty plain virtuals and NO destructor, reaching slot 29 --
 * func_ov077_02123d40, _02124038, _021251d0 and _02125480 all take that shape.
 * MSVC numbers a destructor-free shadow linearly 0..29, so index 29 lands on
 * ROM slot 29 in a ROM-shaped array and those four are correct straight out of
 * src/. Only dispatch on a MEMBER's table (mModelAnim's) collides, and all
 * three Renders are host copies for exactly that reason.
 *
 * SLOT 30 IS TRAPPED ON ALL THREE. It is Actor::OnAimedAtWithEggReturnVec
 * (0x020100dc), an SRET method no __fastcall(void*, void*) thunk shape models
 * -- the repo-wide Enemy trap ov094/ov081/bob_world/flame all take. Slots 13
 * and 14 (ActorBase::Virtual34/Virtual38) are trapped for the standing reason:
 * neither body is linked anywhere in this port. These are LOUD faces, not
 * silent stubs, and none of them is reachable for these three classes (see
 * ov094's own note: slot 30's single dispatcher needs mFlags bit 0x10000000 at
 * +0xb0, which no ov077 body sets).
 *
 * THREE RTTI-SPELLING BRIDGES, AND WHY THEY ARE SAFE. Each D0 stores its
 * table under the class's RTTI name rather than its C++ name -- Lakitu's D0
 * spells _ZTV7daJgm_c, Spiny's _ZTV7daTgz_c, HeaveHo's _ZTV9daPopoi_c -- while
 * each D1 and each _Spawn spells _ZTV6Lakitu / _ZTV5Spiny / _ZTV7HeaveHo. They
 * are ONE table each under two spellings: every table's own vtable[-1]
 * typeinfo record was read through to its name string and gives exactly the
 * RTTI spelling (0x0212783c -> "7daJgm_c", 0x02127954 -> "7daTgz_c",
 * 0x02127a68 -> "9daPopoi_c"). The SkiLift/MotherPenguin/HootTheOwl shape.
 * All six names are declared `extern int ...[]` in include/decl_common.h and
 * DEFINED NOWHERE ELSE IN THE LINK -- checked before these were written,
 * because an /alternatename whose LHS is already defined is defeated SILENTLY
 * (the wave-5 R1/R2 class). port/tools/alternatename_guard.py re-checks that
 * against walk_window.map after every link. None of the three belongs in
 * port/tools/alternatename_baseline.txt: that file is the allowlist of aliases
 * whose LHS IS deliberately defined, and adding an alias that is not defeated
 * would blunt the guard rather than record anything.
 *
 * THE STATE SEAT. All three classes run a pointer-to-member state machine and
 * all six of their dispatch sites are host copies (port/unmatched/
 * Lakitu_HostSites.cpp, Spiny_HostSites.cpp, HeaveHo_HostSites.cpp). The
 * mounted PMF SOURCE records hold DS CODE addresses -- ovdata.py's pointer
 * pass rebases only pointers into other MOUNTED DATA -- so
 * port_ov077_states_seat() below rewrites each record's fn word with its HOST
 * body's address, VALIDATING the mounted word against the ROM's own address
 * first so a wrong mount aborts loudly instead of seating garbage. It must run
 * BEFORE the three sinits copy those records into bss, which is why
 * hal/actor_overlays.cpp calls it between port_ov077_syms_patch() and the
 * first __sinit_ov077_*. The MrBlizzard/BabyPenguin/Unagi/HootTheOwl order.
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Actor.h"
#include "dtor_faces_cpp.h"
#include "ActorBase.h"
#include "Lakitu.h"
#include "Spiny.h"
#include "HeaveHo.h"

extern "C" {

/* ---- the shared arm9 half these three tables all name ------------------- */
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

/* ---- the three host vtables (the mount excludes all three spans) -------- */
int _ZTV6Lakitu[31];    /* == _ZTV7daJgm_c   */
int _ZTV5Spiny[31];     /* == _ZTV7daTgz_c   */
int _ZTV7HeaveHo[31];   /* == _ZTV9daPopoi_c */

/* ---- LAKITU (265) ------------------------------------------------------ */
int _ZN6Lakitu13InitResourcesEv(void *self);      /* unmatched/Lakitu_HostSites */
int _ZN6Lakitu8BehaviorEv(void *self);            /* face, bottom of file  */
int _ZN6Lakitu6RenderEv(void *self);              /* unmatched/Lakitu_HostSites */
int *_ZN6LakituD1Ev(int *self);                   /* unmatched/Lakitu_HostSites */
int _ZN6Lakitu16CleanupResourcesEv(void);         /* matched src, flat      */
void _ZN6Lakitu16OnPendingDestroyEv(void);        /* matched src, flat      */
int *_ZN6LakituD0Ev(int *self);                   /* matched src, flat      */
int func_ov077_02123804(void);                    /* slot 18, own: return 6 */
void func_ov077_02124aa4(void *self, void *p);    /* slot 19, own           */
int func_ov077_0212380c(void);                    /* slot 29, own: 0x3c000  */
void *Lakitu_Spawn(void);
extern unsigned char Lakitu_SpawnInfo[];

/* ---- SPINY (260) ------------------------------------------------------- */
int _ZN5Spiny13InitResourcesEv(void *self);       /* face                   */
int _ZN5Spiny8BehaviorEv(void *self);             /* face                   */
int _ZN5Spiny6RenderEv(void *self);               /* unmatched/Spiny_HostSites */
int *_ZN5SpinyD1Ev(int *self);                    /* unmatched/Spiny_HostSites */
int _ZN5Spiny16CleanupResourcesEv(void);
void _ZN5Spiny16OnPendingDestroyEv(void);
int *_ZN5SpinyD0Ev(int *self);
int func_ov077_02124c18(void);                    /* slot 18, own: return 6 */
void func_ov077_02126194(void *self, void *p);    /* slot 19, own           */
int func_ov077_02124c20(void);                    /* slot 29, own: 0x1e000  */
void *Spiny_Spawn(void);
extern unsigned char Spiny_SpawnInfo[];

/* ---- HEAVE_HO (238) ---------------------------------------------------- */
int _ZN7HeaveHo13InitResourcesEv(void *self);     /* face                   */
int _ZN7HeaveHo8BehaviorEv(void *self);           /* unmatched/HeaveHo_HostSites */
int _ZN7HeaveHo6RenderEv(void *self);             /* unmatched/HeaveHo_HostSites */
int _ZN7HeaveHo16CleanupResourcesEv(void);
void _ZN7HeaveHo16OnPendingDestroyEv(void);
int *_ZN7HeaveHoD1Ev(int *self);                  /* matched src, flat      */
int *_ZN7HeaveHoD0Ev(int *self);                  /* matched src, flat      */
void *HeaveHo_Spawn(void);
extern unsigned char HeaveHo_SpawnInfo[];

}  /* extern "C" */

/* THE THREE RTTI BRIDGES. Each LHS is declared in include/decl_common.h and
 * defined nowhere; alternatename_guard.py fails the build post-link if that
 * ever stops being true. */
#pragma comment(linker, "/alternatename:__ZTV7daJgm_c=__ZTV6Lakitu")
#pragma comment(linker, "/alternatename:__ZTV7daTgz_c=__ZTV5Spiny")
#pragma comment(linker, "/alternatename:__ZTV9daPopoi_c=__ZTV7HeaveHo")

// ============================================================================
// THE STATE SEAT
// ============================================================================
extern "C" {
/* Lakitu's ten handler bodies (five {enter,tick} states). */
int func_ov077_02124698(void *); int func_ov077_021243c0(void *);
int func_ov077_021241ac(void *); int func_ov077_02123fcc(void *);
int func_ov077_021242f0(void *); int func_ov077_02124118(void *);
int func_ov077_02124038(void *); int func_ov077_02124394(void *);
int func_ov077_021244d4(void *); int func_ov077_02124564(void *);
/* Spiny's twelve (six states). */
int func_ov077_021258dc(void *); int func_ov077_02125a54(void *);
int func_ov077_02125908(void *); int func_ov077_02125480(void *);
int func_ov077_02125b1c(void *); int func_ov077_02125bb4(void *);
int func_ov077_02125a0c(void *); int func_ov077_021253a4(void *);
int func_ov077_02125dd4(void *); int func_ov077_02125550(void *);
int func_ov077_021256b4(void *); int func_ov077_02125830(void *);
/* HeaveHo's ten (five states). */
int func_ov077_02126640(void *); int func_ov077_0212679c(void *);
int func_ov077_02126a04(void *); int func_ov077_02126930(void *);
int func_ov077_02126cd4(void *); int func_ov077_02126ad0(void *);
int func_ov077_02126758(void *); int func_ov077_02126a84(void *);
int func_ov077_02126a50(void *); int func_ov077_021269a8(void *);

/* the mounted SOURCE records, one 8-byte {fn, delta} pair each. */
extern unsigned int data_ov077_021277e0[], data_ov077_021277e8[],
    data_ov077_021277f0[], data_ov077_021277f8[], data_ov077_02127800[],
    data_ov077_02127808[], data_ov077_02127810[], data_ov077_02127818[],
    data_ov077_02127820[], data_ov077_02127828[];
extern unsigned int data_ov077_021278e8[], data_ov077_021278f0[],
    data_ov077_021278f8[], data_ov077_02127900[], data_ov077_02127908[],
    data_ov077_02127910[], data_ov077_02127918[], data_ov077_02127920[],
    data_ov077_02127928[], data_ov077_02127930[], data_ov077_02127938[],
    data_ov077_02127940[];
extern unsigned int data_ov077_02127a00[], data_ov077_02127a08[],
    data_ov077_02127a10[], data_ov077_02127a18[], data_ov077_02127a20[],
    data_ov077_02127a28[], data_ov077_02127a30[], data_ov077_02127a38[],
    data_ov077_02127a40[], data_ov077_02127a48[];
}

typedef int (*PortOv077Fn)(void *);
namespace {
struct Ov077Seat {
    unsigned int *rec;      /* the mounted source record        */
    unsigned int rom;       /* what its fn word must read first */
    PortOv077Fn host;       /* the host body to seat            */
    const char *name;
};
/* Each row's `rom` is the ROM's own relocation target for that record's word 0
   (config/arm9/overlays/ov077/relocs.txt, resolved in port/ov077_syms.txt's
   data map). The seat REFUSES a record whose mounted word is not that. */
const Ov077Seat g_ov077_seats[] = {
    /* Lakitu, 0x021277e0..0x02127830, copied into data_ov077_02127bc4[0..9]
       by __sinit_ov077_02127240. */
    {data_ov077_021277e0, 0x02124698, func_ov077_02124698, "lakitu/021277e0"},
    {data_ov077_021277e8, 0x021243c0, func_ov077_021243c0, "lakitu/021277e8"},
    {data_ov077_021277f0, 0x021241ac, func_ov077_021241ac, "lakitu/021277f0"},
    {data_ov077_021277f8, 0x02123fcc, func_ov077_02123fcc, "lakitu/021277f8"},
    {data_ov077_02127800, 0x021242f0, func_ov077_021242f0, "lakitu/02127800"},
    {data_ov077_02127808, 0x02124118, func_ov077_02124118, "lakitu/02127808"},
    {data_ov077_02127810, 0x02124038, func_ov077_02124038, "lakitu/02127810"},
    {data_ov077_02127818, 0x02124394, func_ov077_02124394, "lakitu/02127818"},
    {data_ov077_02127820, 0x021244d4, func_ov077_021244d4, "lakitu/02127820"},
    {data_ov077_02127828, 0x02124564, func_ov077_02124564, "lakitu/02127828"},
    /* Spiny, 0x021278e8..0x02127948, copied into data_ov077_02127c28 (the
       0x60-byte array this lane had to pin -- see port/ov077_syms.txt) by
       __sinit_ov077_0212749c. */
    {data_ov077_021278e8, 0x021258dc, func_ov077_021258dc, "spiny/021278e8"},
    {data_ov077_021278f0, 0x02125a54, func_ov077_02125a54, "spiny/021278f0"},
    {data_ov077_021278f8, 0x02125908, func_ov077_02125908, "spiny/021278f8"},
    {data_ov077_02127900, 0x02125480, func_ov077_02125480, "spiny/02127900"},
    {data_ov077_02127908, 0x02125b1c, func_ov077_02125b1c, "spiny/02127908"},
    {data_ov077_02127910, 0x02125bb4, func_ov077_02125bb4, "spiny/02127910"},
    {data_ov077_02127918, 0x02125a0c, func_ov077_02125a0c, "spiny/02127918"},
    {data_ov077_02127920, 0x021253a4, func_ov077_021253a4, "spiny/02127920"},
    {data_ov077_02127928, 0x02125dd4, func_ov077_02125dd4, "spiny/02127928"},
    {data_ov077_02127930, 0x02125550, func_ov077_02125550, "spiny/02127930"},
    {data_ov077_02127938, 0x021256b4, func_ov077_021256b4, "spiny/02127938"},
    {data_ov077_02127940, 0x02125830, func_ov077_02125830, "spiny/02127940"},
    /* HeaveHo, 0x02127a00..0x02127a50, copied into the five two-entry bss
       cells data_ov077_02127cd8/ce8/cf8/d08/d18 by __sinit_ov077_021275fc. */
    {data_ov077_02127a00, 0x02126640, func_ov077_02126640, "heaveho/02127a00"},
    {data_ov077_02127a08, 0x0212679c, func_ov077_0212679c, "heaveho/02127a08"},
    {data_ov077_02127a10, 0x02126a04, func_ov077_02126a04, "heaveho/02127a10"},
    {data_ov077_02127a18, 0x02126930, func_ov077_02126930, "heaveho/02127a18"},
    {data_ov077_02127a20, 0x02126cd4, func_ov077_02126cd4, "heaveho/02127a20"},
    {data_ov077_02127a28, 0x02126ad0, func_ov077_02126ad0, "heaveho/02127a28"},
    {data_ov077_02127a30, 0x02126758, func_ov077_02126758, "heaveho/02127a30"},
    {data_ov077_02127a38, 0x02126a84, func_ov077_02126a84, "heaveho/02127a38"},
    {data_ov077_02127a40, 0x02126a50, func_ov077_02126a50, "heaveho/02127a40"},
    {data_ov077_02127a48, 0x021269a8, func_ov077_021269a8, "heaveho/02127a48"},
};
bool g_ov077_seated = false;
}  /* namespace */

extern "C" void port_ov077_states_seat(void)
{
    if (g_ov077_seated)
        return;
    for (unsigned i = 0; i < sizeof g_ov077_seats / sizeof g_ov077_seats[0]; ++i) {
        const Ov077Seat &s = g_ov077_seats[i];
        if (s.rec[0] != s.rom) {
            /* Either the mount is pointing at the wrong bytes or this record
               has already been seated. Both are worth stopping for: seating a
               second time over a host address would write a host pointer where
               the ROM check expects a DS one, and every later run of this
               function would then read as a wrong mount. */
            std::fprintf(stderr,
                "FATAL: ov077 state seat %s: mounted fn word reads 0x%08x, "
                "the ROM says 0x%08x\n", s.name, s.rec[0], s.rom);
            std::abort();
        }
        if (s.rec[1] != 0) {
            std::fprintf(stderr,
                "FATAL: ov077 state seat %s: delta word is 0x%08x, every ov077 "
                "PMF record's delta is 0 in the ROM\n", s.name, s.rec[1]);
            std::abort();
        }
        s.rec[0] = (unsigned int)(size_t)s.host;
    }
    g_ov077_seated = true;
}

// ============================================================================
// THE SHARED 31-SLOT ACTOR HALF (identical across all three tables; every
// shared slot was compared address for address against the other two and
// against ov072's own shared half before this was written to be one body)
// ============================================================================
namespace {
void ov77_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov077 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov077 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
}  /* namespace */
static int __fastcall ov77_trap13(void *s, void *) { ov77_trap_report(s, 13); return 0; }
static int __fastcall ov77_trap14(void *s, void *) { ov77_trap_report(s, 14); return 0; }
static int __fastcall ov77_trap30(void *s, void *) { ov77_trap_report(s, 30); return 0; }

static int __fastcall ov77_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov77_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov77_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov77_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov77_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov77_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov77_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov77_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
static int __fastcall ov77_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov77_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov77_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov77_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov77_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov77_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov77_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov77_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov77_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov77_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov77_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov77_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov77_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Fills 1,2,4,5,7,8,10,11,13,14,15,18..30 -- the standard 31-slot Actor half.
   18, 19 and 29 are the arm9 defaults here and are OVERWRITTEN below for
   Lakitu and Spiny, which own all three; HeaveHo keeps all three defaults,
   which is what its own relocations say (0x02010160 / 0x02010154 / 0x02010124
   at slots 18/19/29). */
static void ov77_fill_shared(void **vt)
{
    vt[1]  = (void *)ov77_binit;
    vt[2]  = (void *)ov77_ainit;
    vt[4]  = (void *)ov77_bclean;
    vt[5]  = (void *)ov77_aclean;
    vt[7]  = (void *)ov77_bbeh;
    vt[8]  = (void *)ov77_abeh;
    vt[10] = (void *)ov77_bren;
    vt[11] = (void *)ov77_aren;
    vt[13] = (void *)ov77_trap13;
    vt[14] = (void *)ov77_trap14;
    vt[15] = (void *)ov77_heap;
    vt[18] = (void *)ov77_yoshi;
    vt[19] = (void *)ov77_egg;
    vt[20] = (void *)ov77_v50;
    vt[21] = (void *)ov77_pounded;
    vt[22] = (void *)ov77_atk1;
    vt[23] = (void *)ov77_atk2;
    vt[24] = (void *)ov77_kicked;
    vt[25] = (void *)ov77_pushed;
    vt[26] = (void *)ov77_cannon;
    vt[27] = (void *)ov77_mega;
    vt[28] = (void *)ov77_under;
    vt[29] = (void *)ov77_aimed;
    vt[30] = (void *)ov77_trap30;
}

// ============================================================================
// LAKITU (daJgm_c, 265). Own: 0 Init, 3 Cleanup, 6 Behavior, 9 Render,
// 12 OnPendingDestroy, 16 D1, 17 D0, 18, 19, 29.
// ============================================================================
static int __fastcall lak_init(void *s, void *)
{ return _ZN6Lakitu13InitResourcesEv(s); }
static int __fastcall lak_clean(void *s, void *)
{ (void)s; return _ZN6Lakitu16CleanupResourcesEv(); }
static int __fastcall lak_behavior(void *s, void *)
{ return _ZN6Lakitu8BehaviorEv(s); }
static int __fastcall lak_render(void *s, void *)
{ port_actor_render_probe("LAKITU", (char *)s + 0xd4);
  return _ZN6Lakitu6RenderEv(s); }
static int __fastcall lak_pdes(void *s, void *)
{ (void)s; _ZN6Lakitu16OnPendingDestroyEv(); return 0; }
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (lak_d1) spelled the same chain by hand. */
static int __fastcall lak_d0(void *s, void *)
{ return (int)(size_t)_ZN6LakituD0Ev((int *)s); }
static int __fastcall lak_yoshi(void *s, void *)
{ (void)s; return func_ov077_02123804(); }
static int __fastcall lak_egg(void *s, void *, void *p)
{ func_ov077_02124aa4(s, p); return 0; }
static int __fastcall lak_aimed(void *s, void *)
{ (void)s; return func_ov077_0212380c(); }

extern "C" void hal_fill_lakitu_vtable(void)
{
    port_ov077_states_seat();
    void **vt = (void **)_ZTV6Lakitu;
    ov77_fill_shared(vt);
    vt[0]  = (void *)lak_init;
    vt[3]  = (void *)lak_clean;
    vt[6]  = (void *)lak_behavior;
    vt[9]  = (void *)lak_render;
    vt[12] = (void *)lak_pdes;
    vt[16] = (void *)hal_cppd1_Lakitu;
    vt[17] = (void *)lak_d0;
    vt[18] = (void *)lak_yoshi;
    vt[19] = (void *)lak_egg;
    vt[29] = (void *)lak_aimed;
}

// ============================================================================
// SPINY (daTgz_c, 260). Same own-slot set as Lakitu.
// ============================================================================
static int __fastcall spn_init(void *s, void *)
{ return _ZN5Spiny13InitResourcesEv(s); }
static int __fastcall spn_clean(void *s, void *)
{ (void)s; return _ZN5Spiny16CleanupResourcesEv(); }
static int __fastcall spn_behavior(void *s, void *)
{ return _ZN5Spiny8BehaviorEv(s); }
static int __fastcall spn_render(void *s, void *)
{ port_actor_render_probe("SPINY", (char *)s + 0x124);
  return _ZN5Spiny6RenderEv(s); }
static int __fastcall spn_pdes(void *s, void *)
{ (void)s; _ZN5Spiny16OnPendingDestroyEv(); return 0; }
/* slot 16 is the matched src D1 through hal/dtor_faces_cpp.cpp (lane DTOR-FACES-CPP);
   the transcribed thunk that stood here (spn_d1) spelled the same chain by hand. */
static int __fastcall spn_d0(void *s, void *)
{ return (int)(size_t)_ZN5SpinyD0Ev((int *)s); }
static int __fastcall spn_yoshi(void *s, void *)
{ (void)s; return func_ov077_02124c18(); }
static int __fastcall spn_egg(void *s, void *, void *p)
{ func_ov077_02126194(s, p); return 0; }
static int __fastcall spn_aimed(void *s, void *)
{ (void)s; return func_ov077_02124c20(); }

extern "C" void hal_fill_spiny_vtable(void)
{
    port_ov077_states_seat();
    void **vt = (void **)_ZTV5Spiny;
    ov77_fill_shared(vt);
    vt[0]  = (void *)spn_init;
    vt[3]  = (void *)spn_clean;
    vt[6]  = (void *)spn_behavior;
    vt[9]  = (void *)spn_render;
    vt[12] = (void *)spn_pdes;
    vt[16] = (void *)hal_cppd1_Spiny;
    vt[17] = (void *)spn_d0;
    vt[18] = (void *)spn_yoshi;
    vt[19] = (void *)spn_egg;
    vt[29] = (void *)spn_aimed;
}

// ============================================================================
// HEAVE_HO (daPopoi_c, 238). Own: 0, 3, 6, 9, 12, 16, 17 only -- slots 18, 19
// and 29 are the arm9 Actor defaults, which is what its relocations say and
// what distinguishes it from the other two.
// ============================================================================
static int __fastcall hho_init(void *s, void *)
{ return _ZN7HeaveHo13InitResourcesEv(s); }
static int __fastcall hho_clean(void *s, void *)
{ (void)s; return _ZN7HeaveHo16CleanupResourcesEv(); }
static int __fastcall hho_behavior(void *s, void *)
{ return _ZN7HeaveHo8BehaviorEv(s); }
static int __fastcall hho_render(void *s, void *)
{ port_actor_render_probe("HEAVE_HO", (char *)s + 0x340);
  return _ZN7HeaveHo6RenderEv(s); }
static int __fastcall hho_pdes(void *s, void *)
{ (void)s; _ZN7HeaveHo16OnPendingDestroyEv(); return 0; }
static int __fastcall hho_d1(void *s, void *)
{ return (int)(size_t)_ZN7HeaveHoD1Ev((int *)s); }
static int __fastcall hho_d0(void *s, void *)
{ return (int)(size_t)_ZN7HeaveHoD0Ev((int *)s); }

extern "C" void hal_fill_heave_ho_vtable(void)
{
    port_ov077_states_seat();
    void **vt = (void **)_ZTV7HeaveHo;
    ov77_fill_shared(vt);
    vt[0]  = (void *)hho_init;
    vt[3]  = (void *)hho_clean;
    vt[6]  = (void *)hho_behavior;
    vt[9]  = (void *)hho_render;
    vt[12] = (void *)hho_pdes;
    vt[16] = (void *)hho_d1;
    vt[17] = (void *)hho_d0;
}

// ============================================================================
// THE METHOD FACES. Five ov077 TUs define REAL C++ METHODS against the
// generated class headers -- mwccarm mangles those Itanium, MSVC mangles them
// ?InitResources@Lakitu@@..., so nothing in the link answers to the flat name
// the vtable fill and the other TUs use. Faced here, the
// BabyPenguin/IceSheet/OneUpLogo/HootTheOwl recipe. The remaining four C++
// methods in this overlay (the three Renders and HeaveHo::Behavior) are NOT
// faced: they are host copies in port/unmatched/, which already define the
// flat name themselves.
// ============================================================================
/* Lakitu::InitResources is NOT faced here. It is a HOST COPY in
 * port/unmatched/Lakitu_HostSites.cpp -- the matched body drops the state
 * index the ROM passed in r1, which crashed all four LAKITU levels on this
 * seat's first boot. That file carries the ROM evidence for the value. */
extern "C" int _ZN6Lakitu8BehaviorEv(void *self)
{ return ((Lakitu *)self)->Lakitu::Behavior(); }
extern "C" int _ZN5Spiny13InitResourcesEv(void *self)
{ return ((Spiny *)self)->Spiny::InitResources(); }
extern "C" int _ZN5Spiny8BehaviorEv(void *self)
{ return ((Spiny *)self)->Spiny::Behavior(); }
extern "C" int _ZN7HeaveHo13InitResourcesEv(void *self)
{ return ((HeaveHo *)self)->HeaveHo::InitResources(); }
