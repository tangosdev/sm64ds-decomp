/* ov096, THE POKEY PACK -- POKEY (240, daSanbo_c), POKEY_SEGMENT (241, THE
 * SAME CLASS under a second spawn entry point) and TORNADO (308, daTor_c),
 * level 16's Shifting Sand Land cast. Run rel0215 wave 2, lane cast-lvl16pair,
 * sub-job 2 of 2. Fresh per-symbol mount (port/ov096_syms.txt), slice
 * port/slice_ov096.txt, the ov017/ov077/ov079/ov080/ov081/ov072/ov094
 * convention.
 *
 * THE CAST MAP, THE THREE LIVE CONTESTED ADDRESSES, THE PokeySegment IDENTITY
 * PROOF, THE TWO 31-SLOT WIDTHS AND THE TWO RTTI PAIRS ARE ALL DERIVED IN
 * port/ov096_syms.txt's HEADER. That file is the evidence; this one is the
 * seat. In brief:
 *
 *   id   class          SpawnInfo   vtable      slots  spawn fn
 *  240   Pokey          0x02137998  0x021379d8    31   0x02136d60
 *  241   PokeySegment   0x021379b4  0x021379d8    31   0x02136d10   SAME TABLE
 *  308   Tornado        0x02137a6c  0x02137a90    31   0x021376bc
 *
 * TWO TABLES, THREE IDS. POKEY_SEGMENT is not a class: both factories are
 * byte-for-byte the same body under two names, both allocate 944 bytes, both
 * store 0x021379d8, and PokeySegment's own record's typeinfo word points at
 * POKEY's __si_class_type_info. One fill serves both ids; the two registry
 * rows differ only in their id and their factory. The ov030 UKIKI_THIEF /
 * UKIKI_STAR (267/268) shape.
 *
 * BOTH TABLES ARE ROM-SHAPED (T1's default) and both are the plain 31-slot
 * Actor close -- slot 30 is Actor::OnAimedAtWithEggReturnVec and neither class
 * is Platform-derived, so neither carries the 32nd slot ov092's ToxBox has in
 * the other half of this lane.
 *
 * ROUTE B LIES ON TORNADO, and the seat is where that matters: dsd's
 * next-symbol landing for 0x02137a90 is .bss at 0x02137b20, giving 0x90 = 36
 * slots. Slots 31 through 35 read ZERO in the raw image -- .data's tail
 * padding to .bss's align-32 boundary, not five null virtuals -- and the reloc
 * run stops at slot 30. The host array below is 31, not 36. The ov077 HeaveHo
 * case with five pad words instead of three.
 *
 * SLOT 30 IS TRAPPED ON BOTH. It is Actor::OnAimedAtWithEggReturnVec
 * (0x020100dc), an SRET method no __fastcall(void*, void*) thunk shape models
 * -- the repo-wide Enemy trap ov017/ov094/ov081/bob_world/flame all take.
 * Slots 13 and 14 (ActorBase::Virtual34/Virtual38) are trapped for the
 * standing reason: neither body is linked anywhere in this port. These are
 * LOUD faces, not silent stubs, and none is reachable for these classes (slot
 * 30's single dispatcher needs mFlags bit 0x10000000 at +0xb0, which no ov096
 * body sets).
 *
 * TWO RTTI-SPELLING BRIDGES, AND WHY THEY ARE SAFE. The sources split the two
 * spellings cleanly: all three factories spell the C++ name (_ZTV5Pokey,
 * _ZTV7Tornado) and both D0s spell the RTTI name (_ZTV9daSanbo_c,
 * _ZTV7daTor_c). They are ONE table each: every table's own vtable[-1]
 * typeinfo record was read through to its name string and gives exactly the
 * RTTI spelling (0x021379d4 -> 0x0213798c -> 0x02137980 = "9daSanbo_c";
 * 0x02137a8c -> 0x02137a60 -> 0x02137a54 = "7daTor_c"), and every install
 * relocation on both routes reads the same address. The SkiLift/
 * MotherPenguin/HootTheOwl/ov077 shape. Both LHS are declared `extern int
 * ...[]` in include/decl_common.h (lines 841 and 771) and DEFINED NOWHERE
 * ELSE IN THE LINK -- checked against walk_window.map before these were
 * written, because an /alternatename whose LHS is already defined is defeated
 * SILENTLY (the wave-5 R1/R2 class). port/tools/alternatename_guard.py
 * re-checks that after every link. Neither belongs in
 * port/tools/alternatename_baseline.txt: that file is the allowlist of aliases
 * whose LHS IS deliberately defined, and adding an alias that is not defeated
 * would blunt the guard rather than record anything.
 *
 * NO BRIDGE IS NEEDED FOR THE THREE CONTESTED SpawnInfo ADDRESSES, which is a
 * different question and is answered in port/ov096_syms.txt: ov006 already
 * hosts all three today under data_ov006_* names, but the two mounts emit
 * DIFFERENT C names at different host storage and both the registry and the
 * sinits reach their records by C name, never by DS address. Nothing in this
 * lane spells a placeholder cxx_aliases.cpp binds.
 *
 * THE STATE SEAT. POKEY runs a six-state pointer-to-member machine over twelve
 * 8-byte records and its TWO dispatch sites are host copies
 * (port/unmatched/Pokey_HostSites.cpp). The mounted PMF SOURCE records hold DS
 * CODE addresses -- ovdata.py's pointer pass rebases only pointers into other
 * MOUNTED DATA -- so port_ov096_states_seat() below rewrites each record's fn
 * word with its HOST body's address, VALIDATING the mounted word against the
 * ROM's own address first so a wrong mount aborts loudly instead of seating
 * garbage. It must run BEFORE __sinit_ov096_0213770c copies the twelve records
 * into data_ov096_02137b48, which is why port_ov96_bringup() calls it between
 * port_ov096_syms_patch() and the first sinit. TORNADO has no state machine at
 * all: none of the twelve handlers is in its .text range and its slot 12 is
 * the arm9 ActorBase default.
 */
#include <cstdio>
#include <cstdlib>
#include "dsstate_seg.h"

#include "Actor.h"
#include "ActorBase.h"

extern "C" {

/* ---- the shared arm9 half both tables name ------------------------------ */
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

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* the generated ov096 per-symbol mount (build/port/host-src/ov096_syms.c) */
void port_ov096_pack_check(void);
void port_ov096_syms_patch(void);
/* ov096's two sinits, in the ROM's own .ctor order -- the words at 0x02137900
   and 0x02137904 read 0x0213770c then 0x02137894. */
void __sinit_ov096_0213770c(void);
void __sinit_ov096_02137894(void);

/* ---- POKEY (240) and POKEY_SEGMENT (241), one class -------------------- */
int _ZN5Pokey13InitResourcesEv(void *self);       /* face, bottom of file  */
int _ZN5Pokey16CleanupResourcesEv(void *self);    /* face                  */
int _ZN5Pokey8BehaviorEv(void *self);             /* face                  */
int _ZN5Pokey6RenderEv(void *self);               /* face; body rides from src */
void _ZN5Pokey16OnPendingDestroyEv(void *self);   /* face                  */
int *_ZN5PokeyD1Ev(int *self);                    /* unmatched/Pokey_HostSites */
int *_ZN5PokeyD0Ev(int *self);                    /* matched src, flat     */
int func_ov096_021357a4(void);                    /* slot 18, own: return 4 */
void func_ov096_02136cd0(void *self, void *p);    /* slot 19, own          */
int func_ov096_021357ac(void);                    /* slot 29, own: 0x3c000 */
void *Pokey_Spawn(void);
void *PokeySegment_Spawn(void);
extern unsigned char Pokey_SpawnInfo[];
extern unsigned char PokeySegment_SpawnInfo[];

/* ---- TORNADO (308) ----------------------------------------------------- */
int _ZN7Tornado13InitResourcesEv(void *self);     /* unmatched/Tornado_HostSites */
int _ZN7Tornado16CleanupResourcesEv(void *self);  /* matched src .c, flat  */
int _ZN7Tornado8BehaviorEv(void *self);           /* unmatched/Tornado_HostSites */
int _ZN7Tornado6RenderEv(void *self);             /* unmatched/Tornado_HostSites */
int *_ZN7TornadoD1Ev(int *self);                  /* unmatched/Tornado_HostSites */
int *_ZN7TornadoD0Ev(int *self);                  /* matched src, flat     */
void *Tornado_Spawn(void);
extern unsigned char Tornado_SpawnInfo[];

/* ---- the two host vtables (the mount excludes both spans) --------------- */
DSSTATE_BEGIN
void *_ZTV5Pokey[31];      /* 0x021379d8 == _ZTV9daSanbo_c */
void *_ZTV7Tornado[31];    /* 0x02137a90 == _ZTV7daTor_c   */
DSSTATE_END
}

/* THE TWO RTTI BRIDGES. Each LHS is declared in include/decl_common.h and
 * defined nowhere; alternatename_guard.py fails the build post-link if that
 * ever stops being true. */
#pragma comment(linker, "/alternatename:__ZTV9daSanbo_c=__ZTV5Pokey")
#pragma comment(linker, "/alternatename:__ZTV7daTor_c=__ZTV7Tornado")

/* THE MEASURED CLOSURE GAP, four of its six rows. Taken off the first link's
 * own unresolved list, the way slice_vs.txt section 3 says to take it, not
 * guessed. The other two rows are not aliases: src/func_02016aac.c is a
 * MISSING arm9 TU and was added to port/slice_ov096.txt, and func_02112968 is
 * a wrong NAME for level data and is handled in
 * port/unmatched/Tornado_HostSites.cpp.
 *
 * THREE DATA ROWS (no calling convention to get wrong -- the ov077
 * category-1 / cxxname_bridge.cpp:260 shape). Three matched TUs declare data
 * with a C++ type and no extern "C", so MSVC decorates the reference while the
 * plain C name is what defines it:
 *   src/_ZN5Pokey13InitResourcesEv.cpp  `extern void *data_ov096_02137b20;`
 *       and the same for _02137b28 -- both DEFINED BY THIS LANE'S OWN MOUNT
 *       (port/ov096_syms.txt), the SharedFilePtrs for files 0x41b and 0x41a
 *   src/_ZN5Pokey13InitResourcesEv.cpp  `extern Block48 data_02082128;`
 *       -- an arm9 romdata symbol already in walk_window.map on the base tree
 *
 * ONE CODE ROW, and an alias is correct here where it would be WRONG for an
 * ordinary method. src/func_ov096_02136534.cpp declares a local shadow whose
 * Actor::Spawn is STATIC -- the decoration reads `SAX...`, S for static, so
 * MSVC emits a plain cdecl call with no ECX receiver and the alias cannot
 * misdeliver a `this` the way hal/method_faces.cpp's failure mode 3 does. The
 * tree already carries two other decorations of the SAME target onto the same
 * flat name (hal/actor_classes_ccm.cpp:419, hal/actor_faces_bob.cpp:161); this
 * is a third local spelling of one static method, not a third method.
 *
 * Every LHS is an MSVC decoration nothing in the link defines, so none can be
 * silently defeated the way an alias onto an already-defined LHS is;
 * port/tools/alternatename_guard.py re-checks that against walk_window.map
 * after every link, which is why none belongs in
 * port/tools/alternatename_baseline.txt. */
#pragma comment(linker, "/alternatename:?data_ov096_02137b20@@3PAXA=_data_ov096_02137b20")
#pragma comment(linker, "/alternatename:?data_ov096_02137b28@@3PAXA=_data_ov096_02137b28")
#pragma comment(linker, "/alternatename:?data_02082128@@3UBlock48@@A=_data_02082128")
#pragma comment(linker, "/alternatename:?Spawn@Actor@@SAXIIABUVector3@@PBUVector3_16@@HH@Z=__ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii")

// ============================================================================
// THE STATE SEAT -- POKEY's six states, twelve records
// ============================================================================
extern "C" {
/* Ten of the twelve return int and func_ov096_0213670c returns void. The ROM's
   own typedef is `void (C::*PMF)()` and the dispatch discards r0, so the seat
   table is typed void-returning to match it. See
   port/unmatched/Pokey_HostSites.cpp for the full note. */
int func_ov096_021360c4(void *); int func_ov096_02136134(void *);
int func_ov096_02136264(void *); int func_ov096_021363b4(void *);
int func_ov096_021363c4(void *); int func_ov096_0213640c(void *);
int func_ov096_02136434(void *); int func_ov096_02136534(void *);
int func_ov096_021365d4(void *); void func_ov096_0213670c(void *);
int func_ov096_02136754(void *); int func_ov096_021368a4(void *);

/* the mounted SOURCE records, one 8-byte {fn, delta} pair each. */
extern unsigned int data_ov096_02137920[], data_ov096_02137928[],
    data_ov096_02137930[], data_ov096_02137938[], data_ov096_02137940[],
    data_ov096_02137948[], data_ov096_02137950[], data_ov096_02137958[],
    data_ov096_02137960[], data_ov096_02137968[], data_ov096_02137970[],
    data_ov096_02137978[];
}

namespace {
struct Ov096Seat {
    unsigned int *rec;      /* the mounted source record        */
    unsigned int rom;       /* what its fn word must read first */
    void *host;             /* the host body to seat            */
    const char *name;
};
/* Each row's `rom` is the ROM's own relocation target for that record's word 0
   (config/arm9/overlays/ov096/relocs.txt, from:0x02137920 through
   from:0x02137978, resolved in port/ov096_syms.txt's data map). The seat
   REFUSES a record whose mounted word is not that. The comment on each row is
   the CELL __sinit_ov096_0213770c copies that record into -- a permutation,
   not the address order; the full table is in
   port/unmatched/Pokey_HostSites.cpp. */
const Ov096Seat g_ov096_seats[] = {
    {data_ov096_02137920, 0x02136434, (void *)func_ov096_02136434, "pokey/02137920 state2 tick"},
    {data_ov096_02137928, 0x021365d4, (void *)func_ov096_021365d4, "pokey/02137928 state1 tick"},
    {data_ov096_02137930, 0x02136534, (void *)func_ov096_02136534, "pokey/02137930 state2 enter"},
    {data_ov096_02137938, 0x021363b4, (void *)func_ov096_021363b4, "pokey/02137938 state4 enter"},
    {data_ov096_02137940, 0x02136754, (void *)func_ov096_02136754, "pokey/02137940 state0 tick"},
    {data_ov096_02137948, 0x021368a4, (void *)func_ov096_021368a4, "pokey/02137948 state0 enter"},
    {data_ov096_02137950, 0x0213670c, (void *)func_ov096_0213670c, "pokey/02137950 state1 enter"},
    {data_ov096_02137958, 0x021360c4, (void *)func_ov096_021360c4, "pokey/02137958 state5 tick"},
    {data_ov096_02137960, 0x02136134, (void *)func_ov096_02136134, "pokey/02137960 state5 enter"},
    {data_ov096_02137968, 0x02136264, (void *)func_ov096_02136264, "pokey/02137968 state4 tick"},
    {data_ov096_02137970, 0x021363c4, (void *)func_ov096_021363c4, "pokey/02137970 state3 tick"},
    {data_ov096_02137978, 0x0213640c, (void *)func_ov096_0213640c, "pokey/02137978 state3 enter"},
};
DSSTATE_BEGIN
bool g_ov096_seated = false;
DSSTATE_END
}  /* namespace */

extern "C" void port_ov096_states_seat(void)
{
    if (g_ov096_seated)
        return;
    for (unsigned i = 0; i < sizeof g_ov096_seats / sizeof g_ov096_seats[0]; ++i) {
        const Ov096Seat &s = g_ov096_seats[i];
        if (s.rec[0] != s.rom) {
            /* Either the mount is pointing at the wrong bytes or this record
               has already been seated. Both are worth stopping for: seating a
               second time over a host address would write a host pointer where
               the ROM check expects a DS one, and every later run of this
               function would then read as a wrong mount. */
            std::fprintf(stderr,
                "FATAL: ov096 state seat %s: mounted fn word reads 0x%08x, "
                "the ROM says 0x%08x\n", s.name, s.rec[0], s.rom);
            std::abort();
        }
        if (s.rec[1] != 0) {
            std::fprintf(stderr,
                "FATAL: ov096 state seat %s: delta word is 0x%08x, every ov096 "
                "PMF record's delta is 0 in the ROM\n", s.name, s.rec[1]);
            std::abort();
        }
        s.rec[0] = (unsigned int)(size_t)s.host;
    }
    g_ov096_seated = true;
}

// ============================================================================
// THE SHARED 31-SLOT ACTOR HALF (identical across both tables; every shared
// slot was compared address for address between the two and against ov077's
// own shared half before this was written to be one body)
// ============================================================================
namespace {
void ov96_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov096 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov096 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
}  /* namespace */
static int __fastcall ov96_trap13(void *s, void *) { ov96_trap_report(s, 13); return 0; }
static int __fastcall ov96_trap14(void *s, void *) { ov96_trap_report(s, 14); return 0; }
static int __fastcall ov96_trap30(void *s, void *) { ov96_trap_report(s, 30); return 0; }

static int __fastcall ov96_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov96_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov96_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov96_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov96_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov96_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov96_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov96_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
/* slot 12: TORNADO's ROM word is 0x02043ac0, the arm9 ActorBase default.
   POKEY overrides it with its own body below. */
static int __fastcall ov96_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov96_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov96_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov96_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov96_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov96_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov96_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov96_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov96_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov96_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov96_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov96_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov96_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov96_aimed(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* Fills 1,2,4,5,7,8,10,11,12,13,14,15,18..30 -- the standard 31-slot Actor
   half. 12, 18, 19 and 29 are the arm9 defaults here, which is what TORNADO's
   own relocations say (0x02043ac0 / 0x02010160 / 0x02010154 / 0x02010124);
   POKEY owns all four and OVERWRITES them below. */
static void ov96_fill_shared(void *volatile *vt)
{
    vt[1]  = (void *)ov96_binit;
    vt[2]  = (void *)ov96_ainit;
    vt[4]  = (void *)ov96_bclean;
    vt[5]  = (void *)ov96_aclean;
    vt[7]  = (void *)ov96_bbeh;
    vt[8]  = (void *)ov96_abeh;
    vt[10] = (void *)ov96_bren;
    vt[11] = (void *)ov96_aren;
    vt[12] = (void *)ov96_pdes;
    vt[13] = (void *)ov96_trap13;
    vt[14] = (void *)ov96_trap14;
    vt[15] = (void *)ov96_heap;
    vt[18] = (void *)ov96_yoshi;
    vt[19] = (void *)ov96_turn_egg;
    vt[20] = (void *)ov96_v50;
    vt[21] = (void *)ov96_pounded;
    vt[22] = (void *)ov96_atk1;
    vt[23] = (void *)ov96_atk2;
    vt[24] = (void *)ov96_kicked;
    vt[25] = (void *)ov96_pushed;
    vt[26] = (void *)ov96_cannon;
    vt[27] = (void *)ov96_mega;
    vt[28] = (void *)ov96_under;
    vt[29] = (void *)ov96_aimed;
    vt[30] = (void *)ov96_trap30;
}

// ---- the mount bring-up ----------------------------------------------------
DSSTATE_BEGIN
static int g_ov96_bringup_done;
DSSTATE_END

extern "C" void port_ov96_bringup(void)
{
    if (g_ov96_bringup_done)
        return;
    g_ov96_bringup_done = 1;
    port_ov096_pack_check();
    port_ov096_syms_patch();
    /* BEFORE the sinits: the first one copies the twelve source records into
       data_ov096_02137b48, and they must already carry host addresses. */
    port_ov096_states_seat();
    __sinit_ov096_0213770c();   /* POKEY: 2 SharedFilePtrs + the 12-record table */
    __sinit_ov096_02137894();   /* TORNADO: 2 SharedFilePtrs */
}

// ============================================================================
// POKEY (daSanbo_c, 240) and POKEY_SEGMENT (241) -- one table, 0x021379d8.
// Own: 0 Init, 3 Cleanup, 6 Behavior, 9 Render, 12 OnPendingDestroy, 16 D1,
// 17 D0, 18, 19, 29. The Lakitu/Spiny pattern plus an own slot 12.
// ============================================================================
static int __fastcall pky_init(void *s, void *)
{ return _ZN5Pokey13InitResourcesEv(s); }
static int __fastcall pky_clean(void *s, void *)
{ return _ZN5Pokey16CleanupResourcesEv(s); }
static int __fastcall pky_behavior(void *s, void *)
{ return _ZN5Pokey8BehaviorEv(s); }
static int __fastcall pky_render(void *s, void *)
{ port_actor_render_probe("POKEY", (char *)s + 0xd4);
  return _ZN5Pokey6RenderEv(s); }
static int __fastcall pky_pdes(void *s, void *)
{ _ZN5Pokey16OnPendingDestroyEv(s); return 0; }
static int __fastcall pky_d1(void *s, void *)
{ return (int)(size_t)_ZN5PokeyD1Ev((int *)s); }
static int __fastcall pky_d0(void *s, void *)
{ return (int)(size_t)_ZN5PokeyD0Ev((int *)s); }
static int __fastcall pky_yoshi(void *s, void *)
{ (void)s; return func_ov096_021357a4(); }
static int __fastcall pky_egg(void *s, void *, void *p)
{ func_ov096_02136cd0(s, p); return 0; }
static int __fastcall pky_aimed(void *s, void *)
{ (void)s; return func_ov096_021357ac(); }

extern "C" void hal_fill_pokey_vtable(void)
{
    port_ov96_bringup();
    void *volatile *vt = (void *volatile *)_ZTV5Pokey;
    ov96_fill_shared(vt);
    vt[0]  = (void *)pky_init;
    vt[3]  = (void *)pky_clean;
    vt[6]  = (void *)pky_behavior;
    vt[9]  = (void *)pky_render;
    vt[12] = (void *)pky_pdes;
    vt[16] = (void *)pky_d1;
    vt[17] = (void *)pky_d0;
    vt[18] = (void *)pky_yoshi;
    vt[19] = (void *)pky_egg;
    vt[29] = (void *)pky_aimed;
}

// ============================================================================
// TORNADO (daTor_c, 308) -- table 0x02137a90. Own: 0, 3, 6, 9, 16, 17 only.
// Slots 12, 18, 19 and 29 are the arm9 defaults, which is what its own
// relocations say and what distinguishes it from POKEY. The ov077 HeaveHo
// pattern.
// ============================================================================
static int __fastcall tor_init(void *s, void *)
{ return _ZN7Tornado13InitResourcesEv(s); }
static int __fastcall tor_clean(void *s, void *)
{ return _ZN7Tornado16CleanupResourcesEv(s); }
static int __fastcall tor_behavior(void *s, void *)
{ return _ZN7Tornado8BehaviorEv(s); }
static int __fastcall tor_render(void *s, void *)
{ port_actor_render_probe("TORNADO", (char *)s + 0x2c4);
  return _ZN7Tornado6RenderEv(s); }
static int __fastcall tor_d1(void *s, void *)
{ return (int)(size_t)_ZN7TornadoD1Ev((int *)s); }
static int __fastcall tor_d0(void *s, void *)
{ return (int)(size_t)_ZN7TornadoD0Ev((int *)s); }

extern "C" void hal_fill_tornado_vtable(void)
{
    port_ov96_bringup();
    void *volatile *vt = (void *volatile *)_ZTV7Tornado;
    ov96_fill_shared(vt);
    vt[0]  = (void *)tor_init;
    vt[3]  = (void *)tor_clean;
    vt[6]  = (void *)tor_behavior;
    vt[9]  = (void *)tor_render;
    vt[16] = (void *)tor_d1;
    vt[17] = (void *)tor_d0;
}

// ============================================================================
// THE FIVE METHOD FACES. Five ov096 TUs define REAL C++ METHODS against the
// generated class headers -- mwccarm mangles those Itanium, MSVC mangles them
// ?InitResources@Pokey@@QAEHXZ and so on, so nothing in the link answers to
// the flat name the vtable fills and the other TUs use. Faced here, the
// BabyPenguin/IceSheet/OneUpLogo/HootTheOwl/ShipWater/ToxBox recipe.
//
// Tornado::CleanupResources is NOT faced: src/_ZN7Tornado16CleanupResourcesEv.c
// is a .c file and already defines the flat name. Tornado::Render,
// Tornado::Behavior and Tornado::InitResources are not faced either: all three
// are HOST COPIES in
// port/unmatched/Tornado_HostSites.cpp, which defines their flat names itself
// -- Render for the ModelAnim slot-5 collision, Behavior for the implicit r0
// ride-through the ROM carries into its three state bodies, InitResources for
// the wrong-named level-overlay texture descriptor. Pokey::Render IS
// faced and its body RIDES FROM SRC -- see port/slice_ov096.txt for why the
// two Renders differ.
// ============================================================================
#include "Pokey.h"
#include "Tornado.h"
extern "C" int _ZN5Pokey13InitResourcesEv(void *self)
{ return ((Pokey *)self)->Pokey::InitResources(); }
extern "C" int _ZN5Pokey16CleanupResourcesEv(void *self)
{ return ((Pokey *)self)->Pokey::CleanupResources(); }
extern "C" int _ZN5Pokey8BehaviorEv(void *self)
{ return ((Pokey *)self)->Pokey::Behavior(); }
extern "C" int _ZN5Pokey6RenderEv(void *self)
{ return ((Pokey *)self)->Pokey::Render(); }
extern "C" void _ZN5Pokey16OnPendingDestroyEv(void *self)
{ ((Pokey *)self)->Pokey::OnPendingDestroy(); }
