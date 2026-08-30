// RUN REL0215 WAVE 2 (lane w2-ov074): OV074, THE GOOMBOSS PACK.
//
// Two actor ids, ONE class. Level 45 (Mario's key arena, ex_mario, ov053) is
// the only level of the fifty-two that names ov074, and its skip list is
// exactly one row.
//
//   id   name               placed on L45  factory                 SpawnInfo
//   198  GOOMBOSS            1             Goomboss_Spawn          0x02122e78
//   199  EXPLOSION_GOOMBA    0             ExplosionGoomba_Spawn   0x02122e94
//
// src/Goomboss_Spawn.cpp and src/ExplosionGoomba_Spawn.cpp are the same body
// twice (0xd4 each): both allocate 0x610, both store _ZTV8Goomboss into p[0],
// and both run the identical member-construction sequence
// (4x MovingCylinderClsnWithPos at +0x110 stride 0x40, ModelAnim at +0x210,
// 3x ShadowModel at +0x274 stride 0x28, 3x 0xc-byte cells at +0x3ac,
// MaterialChanger +0x3d0, TextureSequence +0x3e4, TextureTransformer +0x3f8,
// WithMeshClsn +0x40c). ov074 has ONE vtable and ONE typeinfo record, so this
// file has two registry rows and ONE fill.
//
// ---- THE TABLE IS 31 SLOTS -----------------------------------------------
//
// _ZTV8Goomboss is at 0x02122eb8 and slot 30 is 0x020100dc
// Actor::OnAimedAtWithEggReturnVec, the plain Actor close. The class is not
// Platform-derived, so there is no 32nd slot. dsd's next-symbol landing agrees
// (data_ov074_02122f34 at slot 31), which is the rare case where both width
// routes give the same answer -- and the third check still had to be run,
// because what follows the table IS an 8-byte-looking run of relocations:
// 0x02123038 0x02112404 0x40000000 0x02123008 ... That is the overlay's
// THIRTEEN-ENTRY, 12-byte-stride file table, not a pointer-to-member source
// table. Stride 12 not 8, every third word a flag not a zero, and no dsd
// symbol at 8-byte stride inside it. See port/ov074_syms.txt.
//
// Own slots: 0 InitResources, 3 CleanupResources, 6 Behavior, 9 Render,
// 16 D1, 17 D0. Slot 12 is 0x02043ac0 ActorBase::OnPendingDestroy -- the base
// body, NOT an override, so the fill takes it from the base. Slots 18, 19 and
// 29 are the arm9 Actor defaults here (0x02010160 / 0x02010154 / 0x02010124),
// unlike ov077's Lakitu and Spiny which own all three.
//
// ---- T1: THE TABLE STAYS ROM-SHAPED --------------------------------------
//
// Goomboss derives from Actor and HOLDS a ModelAnim at +0x210 rather than
// deriving from one, so the default applies: [16] D1, [17] D0, ROM order. The
// two sites that dispatch that held ModelAnim's ROM slot 5 are host copies in
// port/unmatched/Goomboss_HostSites.cpp, the FlyingCarpet treatment.
//
// ---- THE NINE-STATE PMF MACHINE ------------------------------------------
//
// data_ov074_021230f8 is eighteen 8-byte {fn, 0} records = NINE {enter, tick}
// cells. src/__sinit_ov074_02122978.c copies them out of eighteen mounted
// .data records, and a mounted record's fn word is a RAW DS ADDRESS: ovdata's
// pointer pass only rebases pointers to other mounted DATA, and these point at
// CODE. port_ov074_states_seat() below rewrites each record's fn word with its
// HOST body's address, VALIDATING the mounted word against the ROM's own
// address first so a wrong mount aborts loudly instead of seating garbage. It
// must run BEFORE the sinit copies those records into bss, which is why
// hal/actor_overlays.cpp calls it between port_ov074_syms_patch() and
// __sinit_ov074_02122978(). The ov070/ov077/MrBlizzard/BabyPenguin order.
//
// ---- THREE MISSING BODIES, ALL FACED LOUDLY ------------------------------
//
// ov074 has 56 function symbols and 53 source TUs. Three symbols have neither
// a delink block in config/arm9/overlays/ov074/delinks.txt nor a src file
// anywhere in the tree:
//
//   func_ov074_021201f0  0x1f0  STATE 0's TICK. Its own relocations name
//        Camera::SetPos, Camera::SetLookAt, Camera::SetFlag_3,
//        Actor::ClosestPlayer, Sound::LoadAndSetMusic_Layer3,
//        Message::PrepareTalk, Player::ShowMessage, Player::StartTalk and
//        func_ov074_021203e4 (the state ENTER dispatcher) -- the fight's
//        opening cutscene, which ends by changing state.
//   func_ov074_021204c0  0x208  called from state 7's tick
//        (func_ov074_0211f5b8) and state 6's tick (func_ov074_0211fa74). Its
//        relocations name Particle::System::NewSimple and cstd::fdiv: a
//        particle-effect helper that returns a flag both callers branch on.
//   func_ov074_02121380  0x374  called UNCONDITIONALLY from
//        Goomboss::Behavior, right after func_ov074_02120d74. It calls only
//        func_ov074_02121270 and loads data_ov074_02122e24.
//
// Each is faced LOUDLY rather than stubbed, the hal/scene_boot.cpp l2_trap
// model and the ov030 func_ov030_021136b0 precedent: a silent stub here would
// read as "Goomboss is standing still" rather than as a hole.
//
// ---- NOTHING IN THIS OVERLAY NEEDED A PLACEHOLDER RENAME -----------------
//
// The comment-stripped sweep for G0..G3 / VT / VT0..VT3 / HEAP / R0..R3 over
// all 53 TUs comes back EMPTY. Both destructors spell _ZTV8Goomboss directly
// and both factories store that same table, so dsd's label sits on the table
// its own factory installs and the ov026/ov030/ov036 label shift does not fire
// (the vtable[-1] typeinfo at 0x02122e30 reads through to "12daKuriKing_c",
// checked in port/ov074_syms.txt). The two per-source -D renames this lane
// does add are on OTHER overlays' consumers, not on ov074's own TUs; both are
// in port/CMakeLists.txt beside the slice, and both are itemised in
// port/slice_ov074.txt sections 4a and 4b.
#include <cstdio>
#include "dsstate_seg.h"
#include <cstdlib>

#include "Actor.h"
#include "ActorBase.h"
#include "Goomboss.h"

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

const char *port_actor_class_name(unsigned id);   /* hal/actor_registry */
void port_actor_slot_decline(const char *what);   /* func_02043fdc_hostcopy.cpp */
void port_actor_slot_decline_for(void *actor, const char *what);  /* same file */
void port_actor_render_probe(const char *cls, void *model); /* hal/actor_classes */

/* the generated ov074 per-symbol mount (build/port/host-src/ov074_syms.c) */
void port_ov074_pack_check(void);
void port_ov074_syms_patch(void);
void __sinit_ov074_02122978(void);

/* the class's own bodies (port/slice_ov074.txt). InitResources, D1 and D0 are
   already extern "C" in their own sources -- InitResources by an explicit
   block, D1 and D0 because both are .c files -- so those three are reached
   directly. CleanupResources and Behavior are REAL C++ METHODS against
   include/Goomboss.h (`int Goomboss::Behavior()`), so MSVC mangles them
   ?Behavior@Goomboss@@QAEHXZ and the ROM's C name is not provided by the
   object at all; both get a thin face at the bottom of this file, the
   ov036 ShipWing/DonutBlock recipe. Render is a HOST COPY. */
int _ZN8Goomboss13InitResourcesEv(void *self);       /* slot 0, extern "C" src */
int _ZN8Goomboss16CleanupResourcesEv(void *self);    /* slot 3, face below  */
int _ZN8Goomboss8BehaviorEv(void *self);             /* slot 6, face below  */
int _ZN8Goomboss6RenderEv(void *self);               /* slot 9, HOST COPY  */
int _ZN8GoombossD1Ev(void *self);                    /* slot 16 */
int _ZN8GoombossD0Ev(void *self);                    /* slot 17 */
void *Goomboss_Spawn(void);                          /* id 198 */
void *ExplosionGoomba_Spawn(void);                   /* id 199 */
extern unsigned char Goomboss_SpawnInfo[];           /* mount, +4 = 198 */
extern unsigned char ExplosionGoomba_SpawnInfo[];    /* mount, +4 = 199 */

/* the host vtable, excluded from the mount along with its own prologue.
   THIRTY-ONE slots. The name is C linkage because src/_ZN8GoombossD1Ev.c and
   src/_ZN8GoombossD0Ev.c are .c files that spell it `extern int
   _ZTV8Goomboss[]` / `extern void *_ZTV8Goomboss[]`, and both factories store
   it as `void **`. One host object, three spellings, one decorated name. */
DSSTATE_BEGIN
void *_ZTV8Goomboss[31];   /* 0x02122eb8, == _ZTV12daKuriKing_c */
DSSTATE_END

/* ---- THE EIGHTEEN PMF SOURCE RECORDS the sinit copies -------------------
   Declared as byte arrays because that is what the mount emits (u8 name[8]).
   The seat below reads and rewrites word 0 of each. */
extern unsigned char data_ov074_02122d84[];
extern unsigned char data_ov074_02122d8c[];
extern unsigned char data_ov074_02122d94[];
extern unsigned char data_ov074_02122d9c[];
extern unsigned char data_ov074_02122da4[];
extern unsigned char data_ov074_02122dac[];
extern unsigned char data_ov074_02122db4[];
extern unsigned char data_ov074_02122dbc[];
extern unsigned char data_ov074_02122dc4[];
extern unsigned char data_ov074_02122dcc[];
extern unsigned char data_ov074_02122dd4[];
extern unsigned char data_ov074_02122ddc[];
extern unsigned char data_ov074_02122de4[];
extern unsigned char data_ov074_02122dec[];
extern unsigned char data_ov074_02122df4[];
extern unsigned char data_ov074_02122e0c[];
extern unsigned char data_ov074_02122e14[];
extern unsigned char data_ov074_02122e1c[];

/* ---- THE SEVENTEEN STATE BODIES THAT EXIST -----------------------------
   Every one is a mwcc pointer-to-member `void (C::*)()`: `this` in r0, no
   argument. The host bodies are cdecl and four of them (0212007c, 021201ec,
   021203e0, 0211fc34) are written `(void)` in their own recovered source,
   which is harmless -- the dispatcher passes the receiver, the callee ignores
   it, and cdecl leaves the caller to clean. They are declared uniformly here
   because this file only ever takes their ADDRESS; it never calls one. */
void func_ov074_0211f5b8(void *self);
void func_ov074_0211fa08(void *self);
void func_ov074_0211fa74(void *self);
void func_ov074_0211fb84(void *self);
void func_ov074_0211fbd0(void *self);
void func_ov074_0211fc34(void *self);
void func_ov074_0211fc38(void *self);
void func_ov074_0211fd48(void *self);
void func_ov074_0211fd74(void *self);
void func_ov074_0211ffac(void *self);
void func_ov074_0211ffcc(void *self);
void func_ov074_0212007c(void *self);
void func_ov074_02120080(void *self);
void func_ov074_0212016c(void *self);
void func_ov074_0212018c(void *self);
void func_ov074_021201ec(void *self);
void func_ov074_021203e0(void *self);

/* the ov053 whole-image mount's DS-to-host resolver, for the level-file seat
   below (build/port/host-src/ov053_data.c, generated from
   PORT_LEVEL_OVERLAYS). A pure function over a static array. */
void *port_ov053_at(unsigned ds);

/* the mount symbols the level-file seat rewrites */
extern unsigned char data_ov074_02122e5c[];   /* 7 BTA pointers  */
extern unsigned char data_ov074_02122f38[];   /* the file table's column 1 */

}  /* extern "C" */

/* ---- THE MEASURED EXTERNAL GAP, C++-MANGLED SPELLING HALF ----------------
   Eight unresolved externals off the FIRST link, all the ov025/ov058 class:
   a //cpp TU declares a global (or a static member) at file scope with no
   extern "C", so MSVC decorates each with its declared type while the mount,
   the ov084 mount and the port's own bodies emit the one C name. Fourteen more
   of the same shape came from src/_ZN8Goomboss13InitResourcesEv.c and were
   fixed at the source instead, by moving that host copy's declarations INSIDE
   its extern "C" block; these eight are in two slice TUs this lane does not
   own, so they are bridged.

   src/func_ov074_02121a4c.cpp declares the three file-table bases `extern char
   data_ov074_02122f34[]` and friends outside any block.
   src/func_ov074_02122634.cpp declares Model::LoadFile and
   MaterialChanger::Prepare as STATIC MEMBERS, which MSVC gives __cdecl under
   /Gd -- so the calling conventions agree and only the decoration differs, the
   ov058 Memory::Deallocate case -- and spells its four SharedFilePtr globals
   with `struct SharedFilePtr` types.

   Every LHS below is declared somewhere and DEFINED NOWHERE, so no alias here
   can be defeated by a real definition (the wave-5 R1/R2 lesson);
   port/tools/alternatename_guard.py fails the build post-link if that ever
   stops being true. */
#pragma comment(linker, "/alternatename:?data_ov074_02122f34@@3PADA=_data_ov074_02122f34")
#pragma comment(linker, "/alternatename:?data_ov074_02122f38@@3PADA=_data_ov074_02122f38")
#pragma comment(linker, "/alternatename:?data_ov074_02122f3c@@3PADA=_data_ov074_02122f3c")
#pragma comment(linker, "/alternatename:?LoadFile@Model@@SAXAAUSharedFilePtr@@@Z=__ZN5Model8LoadFileER13SharedFilePtr")
#pragma comment(linker, "/alternatename:?Prepare@MaterialChanger@@SAXAAUBMD_File@@AAUBMA_File@@@Z=__ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File")
#pragma comment(linker, "/alternatename:?data_ov074_0212292c@@3PAPAUSharedFilePtr@@A=_data_ov074_0212292c")
#pragma comment(linker, "/alternatename:?data_ov084_0213089c@@3USharedFilePtr@@A=_data_ov084_0213089c")
#pragma comment(linker, "/alternatename:?data_ov084_02130cc8@@3USharedFilePtr@@A=_data_ov084_02130cc8")
#pragma comment(linker, "/alternatename:?data_ov084_02130cf8@@3USharedFilePtr@@A=_data_ov084_02130cf8")
/* The same shape THE OTHER WAY ROUND, from the closure's second round. The
   slice's src/func_02011d20.c calls the C name __ZN5Sound13Func_02048ee4Ev,
   and the body that provides it -- src/_ZN5Sound13Func_02048ee4Ev.cpp, pulled
   in by that same closure -- declares it as a STATIC MEMBER of class Sound, so
   MSVC emits ?Func_02048ee4@Sound@@SAXXZ (confirmed with dumpbin /symbols on
   the object, not assumed). Static members are __cdecl under /Gd, so the
   calling conventions agree and only the decoration differs -- the ov058
   Memory::Deallocate case. */
#pragma comment(linker, "/alternatename:__ZN5Sound13Func_02048ee4Ev=?Func_02048ee4@Sound@@SAXXZ")

// ============================================================================
// THE LEVEL-FILE SEAT: TWENTY WORDS THAT POINT INTO ov053
// ============================================================================
//
// Twenty words in this mount hold an address in the LEVEL-OVERLAY window
// 0x021111a0..0x02112560 -- ov053's own span, and ov053 is level 45's overlay,
// the only level that loads ov074. ovdata.py's per-mount pointer pass cannot
// touch them (it only sees ov074's own symbols) and its --cross pass cannot
// either: a level overlay is mounted WHOLE, which deliberately gives the cross
// pass no per-symbol map to resolve into. The ov035/wave-17 measurement is
// what that costs -- two actors ticked 300 frames on a garbage CLPS without
// faulting, and a third divided by zero on an animation descriptor that read
// as zero frames. Left raw these twenty would point into the port's reserved
// DS pages, which ntr/io.cpp maps and ZEROES, so every read succeeds and hands
// the model code a zero file header.
//
// port_ov053_at() is the whole mount's own DS-to-host resolver and it is a
// pure function over a static array, so this seat has no ordering requirement
// beyond running after port_ov074_syms_patch() has laid the bytes down. The
// JRB STATIC_ROCK precedent (hal/level_boot.cpp:1781) is the same move at the
// same seam. Each word is VALIDATED against the ROM's own value first, so a
// wrong mount aborts loudly instead of seating a resolved pointer over the
// wrong slot.
//
//   data_ov074_02122e5c  seven words, stride 4 -- the BTA_File pointers
//        Goomboss::InitResources hands TextureTransformer::Prepare in a loop.
//   data_ov074_02122f38  thirteen words, stride 12 -- column 1 of the
//        thirteen-entry file table func_ov074_02121a4c indexes
//        (`*(void**)(data_ov074_02122f38 + idx*0xc)` straight into
//        TextureTransformer::SetFile). Column 0 is a SharedFilePtr* the mount
//        already patches and column 2 is an int flag.
namespace {
struct Ov074LevelWord {
    unsigned char *base;
    unsigned off;
    unsigned int rom;
};
const Ov074LevelWord g_ov074_level_words[] = {
    {data_ov074_02122e5c,   0, 0x02112404}, {data_ov074_02122e5c,   4, 0x0211244c},
    {data_ov074_02122e5c,   8, 0x02112494}, {data_ov074_02122e5c,  12, 0x021124c4},
    {data_ov074_02122e5c,  16, 0x02112464}, {data_ov074_02122e5c,  20, 0x02112434},
    {data_ov074_02122e5c,  24, 0x021124ac},
    {data_ov074_02122f38,   0, 0x02112404}, {data_ov074_02122f38,  12, 0x0211244c},
    {data_ov074_02122f38,  24, 0x0211244c}, {data_ov074_02122f38,  36, 0x02112464},
    {data_ov074_02122f38,  48, 0x021124c4}, {data_ov074_02122f38,  60, 0x02112434},
    {data_ov074_02122f38,  72, 0x021124ac}, {data_ov074_02122f38,  84, 0x021124ac},
    {data_ov074_02122f38,  96, 0x021124ac}, {data_ov074_02122f38, 108, 0x021124ac},
    {data_ov074_02122f38, 120, 0x021124ac}, {data_ov074_02122f38, 132, 0x02112404},
    {data_ov074_02122f38, 144, 0x021124ac},
};
DSSTATE_BEGIN
bool g_ov074_level_seated;
DSSTATE_END
}  /* namespace */

extern "C" void port_ov074_level_files_seat(void)
{
    if (g_ov074_level_seated)
        return;
    for (unsigned i = 0;
         i < sizeof g_ov074_level_words / sizeof g_ov074_level_words[0]; ++i) {
        const Ov074LevelWord &r = g_ov074_level_words[i];
        unsigned int *w = (unsigned int *)(r.base + r.off);
        if (*w != r.rom) {
            std::fprintf(stderr,
                "FATAL: ov074 level-file seat +%u: mounted word reads 0x%08x, "
                "the ROM says 0x%08x\n", r.off, *w, r.rom);
            std::abort();
        }
        void *h = port_ov053_at(r.rom);
        if (!h) {
            std::fprintf(stderr,
                "FATAL: ov074 level-file seat +%u: 0x%08x is outside ov053's "
                "mounted image\n", r.off, r.rom);
            std::abort();
        }
        *w = (unsigned int)(size_t)h;
    }
    g_ov074_level_seated = true;
}

// ============================================================================
// THE THREE MISSING BODIES
// ============================================================================
namespace {
unsigned ov74_id_of(void *c)
{ return c ? *(unsigned short *)((char *)c + 0xc) : 0u; }

void ov74_missing(void *c, const char *sym, const char *what, int *said)
{
    unsigned id = ov74_id_of(c);
    if (!*said) {
        *said = 1;
        std::fprintf(stderr,
                     "UNHOSTED: %s (%s) HAS NO MATCHED BODY -- no delink block "
                     "and no src file anywhere in the tree. Actor id %u %s "
                     "reached it. Three of ov074's 56 function symbols are in "
                     "this state; see port/slice_ov074.txt section 3.\n",
                     sym, what, id, port_actor_class_name(id));
    }
    { static char _m[160];
      std::snprintf(_m, sizeof _m, "unhosted ov074 body %s on id %u %s",
                    sym, id, port_actor_class_name(id));
      port_actor_slot_decline_for(c, _m); }
}
int g_said_021201f0, g_said_021204c0, g_said_02121380;
}  /* namespace */

extern "C" {
/* state 0's TICK -- reached only through the PMF table, so it is seated by
   ADDRESS below rather than under the ROM's own name. */
void port_ov074_state0_tick_face(void *c)
{ ov74_missing(c, "func_ov074_021201f0", "ov074 state 0 tick, 0x1f0 bytes",
               &g_said_021201f0); }

/* called from state 6's and state 7's ticks; both branch on its result. The
   ROM returns a flag, so this face returns 0, which is the branch both callers
   take when the effect did not complete. */
int func_ov074_021204c0(void *c)
{ ov74_missing(c, "func_ov074_021204c0",
               "ov074 particle-effect helper, 0x208 bytes", &g_said_021204c0);
  return 0; }

/* called unconditionally from Goomboss::Behavior. decl_common.h:2825 declares
   it `void (char *)`; this definition matches that. */
void func_ov074_02121380(char *c)
{ ov74_missing(c, "func_ov074_02121380",
               "ov074 Behavior step, 0x374 bytes", &g_said_02121380); }
}  /* extern "C" */

// ============================================================================
// THE STATE SEAT
// ============================================================================
namespace {
struct Ov074Seat {
    unsigned char *rec;     /* the mounted 8-byte {fn, 0} record */
    unsigned int rom;       /* what the ROM's own word 0 must read */
    void *host;             /* the host body that replaces it */
    const char *name;
};

/* Nine {enter, tick} cells, in the order src/__sinit_ov074_02122978.c copies
   them into data_ov074_021230f8[0..17]: index 2k is state k's ENTER and index
   2k+1 is its TICK. func_ov074_021203e4 dispatches the enter half and
   func_ov074_0212042c the tick half (both host copies in
   port/unmatched/Goomboss_StateDispatch.cpp). Every `rom` column below is the
   raw word read out of extracted/overlays/overlay_0074.bin at that record's
   own address. */
const Ov074Seat g_ov074_seats[] = {
    /* state 0 */
    {data_ov074_02122dbc, 0x021203e0, (void *)func_ov074_021203e0,          "s0.enter/02122dbc"},
    {data_ov074_02122de4, 0x021201f0, (void *)port_ov074_state0_tick_face,  "s0.tick /02122de4 MISSING"},
    /* state 1 */
    {data_ov074_02122e0c, 0x021201ec, (void *)func_ov074_021201ec,          "s1.enter/02122e0c"},
    {data_ov074_02122dc4, 0x0212018c, (void *)func_ov074_0212018c,          "s1.tick /02122dc4"},
    /* state 2 */
    {data_ov074_02122d8c, 0x0211ffac, (void *)func_ov074_0211ffac,          "s2.enter/02122d8c"},
    {data_ov074_02122dcc, 0x0211fd74, (void *)func_ov074_0211fd74,          "s2.tick /02122dcc"},
    /* state 3 */
    {data_ov074_02122dd4, 0x0212016c, (void *)func_ov074_0212016c,          "s3.enter/02122dd4"},
    {data_ov074_02122d94, 0x02120080, (void *)func_ov074_02120080,          "s3.tick /02122d94"},
    /* state 4 */
    {data_ov074_02122d9c, 0x0212007c, (void *)func_ov074_0212007c,          "s4.enter/02122d9c"},
    {data_ov074_02122da4, 0x0211ffcc, (void *)func_ov074_0211ffcc,          "s4.tick /02122da4"},
    /* state 5 */
    {data_ov074_02122d84, 0x0211fd48, (void *)func_ov074_0211fd48,          "s5.enter/02122d84"},
    {data_ov074_02122dac, 0x0211fc38, (void *)func_ov074_0211fc38,          "s5.tick /02122dac"},
    /* state 6 */
    {data_ov074_02122e1c, 0x0211fb84, (void *)func_ov074_0211fb84,          "s6.enter/02122e1c"},
    {data_ov074_02122e14, 0x0211fa74, (void *)func_ov074_0211fa74,          "s6.tick /02122e14"},
    /* state 7 */
    {data_ov074_02122db4, 0x0211fa08, (void *)func_ov074_0211fa08,          "s7.enter/02122db4"},
    {data_ov074_02122df4, 0x0211f5b8, (void *)func_ov074_0211f5b8,          "s7.tick /02122df4"},
    /* state 8 */
    {data_ov074_02122dec, 0x0211fc34, (void *)func_ov074_0211fc34,          "s8.enter/02122dec"},
    {data_ov074_02122ddc, 0x0211fbd0, (void *)func_ov074_0211fbd0,          "s8.tick /02122ddc"},
};
DSSTATE_BEGIN
bool g_ov074_seated;
DSSTATE_END
}  /* namespace */

extern "C" void port_ov074_states_seat(void)
{
    if (g_ov074_seated)
        return;
    for (unsigned i = 0; i < sizeof g_ov074_seats / sizeof g_ov074_seats[0]; ++i) {
        const Ov074Seat &s = g_ov074_seats[i];
        unsigned int *w = (unsigned int *)s.rec;
        if (w[0] != s.rom) {
            /* Either the mount is pointing at the wrong bytes or this record
               has already been seated. Both are worth stopping for: seating a
               second time over a host address would write a host pointer where
               the ROM check expects a DS one, and every later run of this
               function would then read as a wrong mount. */
            std::fprintf(stderr,
                "FATAL: ov074 state seat %s: mounted fn word reads 0x%08x, "
                "the ROM says 0x%08x\n", s.name, w[0], s.rom);
            std::abort();
        }
        if (w[1] != 0) {
            std::fprintf(stderr,
                "FATAL: ov074 state seat %s: delta word is 0x%08x, every ov074 "
                "PMF record's delta is 0 in the ROM\n", s.name, w[1]);
            std::abort();
        }
        w[0] = (unsigned int)(size_t)s.host;
    }
    g_ov074_seated = true;
}

// ============================================================================
// THE 31-SLOT TABLE
// ============================================================================
static void ov74_trap_report(void *self, int slot)
{
    unsigned id = self ? *(unsigned short *)((char *)self + 0xc) : 0u;
    std::fprintf(stderr,
                 "UNHOSTED: ov074 vtable slot %d is not hosted (actor id %u "
                 "%s)\n", slot, id, port_actor_class_name(id));
    { static char _m[128];
      std::snprintf(_m, sizeof _m, "unhosted ov074 vtable slot %d on id %u %s",
                    slot, id, port_actor_class_name(id));
      port_actor_slot_decline(_m); }
}
#define OV74_TRAP(n) \
    static int __fastcall ov74_trap##n(void *s, void *) \
    { ov74_trap_report(s, n); return 0; }
OV74_TRAP(13) OV74_TRAP(14) OV74_TRAP(30)
#undef OV74_TRAP

static int __fastcall ov74_binit(void *s, void *)
{ return _ZN5Actor19BeforeInitResourcesEv(s); }
static void __fastcall ov74_ainit(void *s, void *, unsigned a)
{ _ZN5Actor18AfterInitResourcesEj(s, a); }
static int __fastcall ov74_bclean(void *s, void *)
{ return ((Actor *)s)->Actor::BeforeCleanupResources(); }
static void __fastcall ov74_aclean(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterCleanupResources(a); }
static int __fastcall ov74_bbeh(void *s, void *)
{ return _ZN5Actor14BeforeBehaviorEv(s); }
static void __fastcall ov74_abeh(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterBehavior(a); }
static int __fastcall ov74_bren(void *s, void *)
{ return _ZN5Actor12BeforeRenderEv(s); }
static void __fastcall ov74_aren(void *s, void *, unsigned a)
{ ((ActorBase *)s)->ActorBase::AfterRender(a); }
/* slot 12 is 0x02043ac0 in the ROM table -- ActorBase's own body, not an
   override. */
static int __fastcall ov74_pdes(void *s, void *)
{ ((ActorBase *)s)->ActorBase::OnPendingDestroy(); return 0; }
static int __fastcall ov74_heap(void *s, void *)
{ return ((ActorBase *)s)->ActorBase::OnHeapCreated(); }
static int __fastcall ov74_yoshi(void *s, void *)
{ return _ZN5Actor13OnYoshiTryEatEv(s); }
static int __fastcall ov74_turn_egg(void *s, void *, void *p)
{ _ZN5Actor13OnTurnIntoEggER6Player(s, p); return 0; }
static int __fastcall ov74_v50(void *s, void *)
{ return _ZN5Actor9Virtual50Ev(s); }
static int __fastcall ov74_pounded(void *s, void *, void *o)
{ _ZN5Actor15OnGroundPoundedERS_(s, o); return 0; }
static int __fastcall ov74_atk1(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked1ERS_(s, o); return 0; }
static int __fastcall ov74_atk2(void *s, void *, void *o)
{ _ZN5Actor11OnAttacked2ERS_(s, o); return 0; }
static int __fastcall ov74_kicked(void *s, void *, void *o)
{ _ZN5Actor8OnKickedERS_(s, o); return 0; }
static int __fastcall ov74_pushed(void *s, void *, void *o)
{ _ZN5Actor8OnPushedERS_(s, o); return 0; }
static int __fastcall ov74_cannon(void *s, void *, void *o)
{ _ZN5Actor24OnHitByCannonBlastedCharERS_(s, o); return 0; }
static int __fastcall ov74_mega(void *s, void *, void *p)
{ _ZN5Actor15OnHitByMegaCharER6Player(s, p); return 0; }
static int __fastcall ov74_under(void *s, void *, void *o)
{ _ZN5Actor19OnHitFromUnderneathERS_(s, o); return 0; }
static int __fastcall ov74_egg(void *s, void *)
{ return _ZN5Actor16OnAimedAtWithEggEv(s); }

/* the class's own slots */
static int __fastcall gb_init(void *s, void *)
{ return _ZN8Goomboss13InitResourcesEv(s); }
static int __fastcall gb_clean(void *s, void *)
{ return _ZN8Goomboss16CleanupResourcesEv(s); }
static int __fastcall gb_behavior(void *s, void *)
{ return _ZN8Goomboss8BehaviorEv(s); }
static int __fastcall gb_render(void *s, void *)
{ port_actor_render_probe("GOOMBOSS", (char *)s + 0x210);
  return _ZN8Goomboss6RenderEv(s); }
static int __fastcall gb_d1(void *s, void *)
{ return _ZN8GoombossD1Ev(s); }
static int __fastcall gb_d0(void *s, void *)
{ return _ZN8GoombossD0Ev(s); }

/* ONE table and TWO registry rows (198 and 199) but ONE caller shape, so the
   shared half is written inline -- the gate-200 elided-stores bug needs two
   call sites with distinct extern-array arguments. The pointer is still
   volatile. THIRTY-ONE slots: there is no vt[31] to write, and writing one
   would run past the array.
   The mount bring-up (pack check, syms patch, seat, sinit) is
   hal/actor_overlays.cpp's, in the ROM's own order; the seat is called again
   here for the reason ov045's guard exists -- a fill is reachable from the
   registry install, and actor_overlays is not the only way in. It is
   idempotent. */
extern "C" void hal_fill_goomboss_vtable(void)
{
    port_ov074_states_seat();
    port_ov074_level_files_seat();
    void *volatile *vt = (void *volatile *)_ZTV8Goomboss;
    vt[0]  = (void *)gb_init;
    vt[1]  = (void *)ov74_binit;
    vt[2]  = (void *)ov74_ainit;
    vt[3]  = (void *)gb_clean;
    vt[4]  = (void *)ov74_bclean;
    vt[5]  = (void *)ov74_aclean;
    vt[6]  = (void *)gb_behavior;
    vt[7]  = (void *)ov74_bbeh;
    vt[8]  = (void *)ov74_abeh;
    vt[9]  = (void *)gb_render;
    vt[10] = (void *)ov74_bren;
    vt[11] = (void *)ov74_aren;
    vt[12] = (void *)ov74_pdes;
    vt[13] = (void *)ov74_trap13;
    vt[14] = (void *)ov74_trap14;
    vt[15] = (void *)ov74_heap;
    vt[16] = (void *)gb_d1;
    vt[17] = (void *)gb_d0;
    vt[18] = (void *)ov74_yoshi;
    vt[19] = (void *)ov74_turn_egg;
    vt[20] = (void *)ov74_v50;
    vt[21] = (void *)ov74_pounded;
    vt[22] = (void *)ov74_atk1;
    vt[23] = (void *)ov74_atk2;
    vt[24] = (void *)ov74_kicked;
    vt[25] = (void *)ov74_pushed;
    vt[26] = (void *)ov74_cannon;
    vt[27] = (void *)ov74_mega;
    vt[28] = (void *)ov74_under;
    vt[29] = (void *)ov74_egg;
    vt[30] = (void *)ov74_trap30;
}

// ---- method faces ----------------------------------------------------------
// TWO ov074 TUs define a REAL C++ METHOD against include/Goomboss.h
// (int Goomboss::Behavior() and int Goomboss::CleanupResources()), so MSVC
// mangles them ?Behavior@Goomboss@@QAEHXZ and the ROM's C names
// _ZN8Goomboss8BehaviorEv / _ZN8Goomboss16CleanupResourcesEv are not provided
// by those objects at all. Faced here, the ov036 ShipWing/DonutBlock recipe: a
// thin extern "C" wrapper that calls the qualified method.
//
// The third method the header declares, int Goomboss::Render(), is NOT faced:
// its src TU is held out of the slice and port/unmatched/Goomboss_HostSites.cpp
// defines _ZN8Goomboss6RenderEv directly (the ModelAnim slot-5 collision).
extern "C" {
int _ZN8Goomboss16CleanupResourcesEv(void *self)
{ return ((Goomboss *)self)->Goomboss::CleanupResources(); }
int _ZN8Goomboss8BehaviorEv(void *self)
{ return ((Goomboss *)self)->Goomboss::Behavior(); }
}
