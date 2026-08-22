// dScMg3DEsp_c, the "Psyche Out!" minigame -- actor id 0x185, scene 389.
// Run mg9, lane PSY.  The twelfth ov006 minigame class the port seats and the
// THIRD under dScMgSingle3DBase_c.
//
// Read port/slice_psy.txt for the identity derivation, the four width checks
// and the closure, and port/unmatched/Mg3DEsp_StateDispatch.cpp for the state
// machine.  This file is the seat: the two new vtables' faces, the fill, the
// factory forwarder and the run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The reason hal/scene_mg_flower.cpp and hal/scene_mg_memory2.cpp give,
// unchanged.  scene_mg.cpp is the FAMILY half -- the mounts, the thirty-five
// overlay constructors, dScMgBase_c's address-keyed faces and the gate that
// keeps the constructors off a level boot -- and all of it is reached through
// the one seam that file exports, port_scene_mg_fill_shared().  Nothing of it
// is re-derived here and kMgBaseFaces is NOT copied: a second copy would
// install a different set of host thunks over the same DS addresses in the
// same mounted tables and split the per-slot witness in two.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM SAYS SO THREE TIMES ------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMg3DEsp_c        data_ov006_0213c8c4  36 slots
//
// port/mg_fanout_costs.txt section 3 lists 0x185 as "vtable 0x0213c8c4, 36
// slots, 14 overrides" and says nothing about an intermediate base, which is
// exactly the gap section 13's CORRECTION 1 predicts will fire again.  Three
// independent witnesses, none of them a name:
//
//   THE type_info CHAIN.  ov006/relocs.txt from:0x0213c8c0 to:0x0213c7c8 is the
//   word before the vtable pointing at this class's type_info; its +4 points at
//   0x0213c7d4, which reads "12dScMg3DEsp_c", and its +8 points at 0x0213bc64,
//   whose name at 0x0213bd00 reads "19dScMgSingle3DBase_c".
//
//   THE FACTORY.  src/MgPsycheOut_Spawn.cpp calls func_ov004_020b2adc(o),
//   writes data_ov006_0213e448 into o[0], constructs the Particle::SysTracker
//   at +0x471c, then writes data_ov006_0213c8c4 over it.
//
//   THE DESTRUCTORS.  src/func_ov006_020e76e4.c (slot 17, D0) unwinds in the
//   opposite order -- 0x0213c8c4, the four sub-objects, 0x0213e448,
//   ~SysTracker, func_ov004_020b29c0, Memory::Deallocate -- and
//   src/func_ov006_020e7660.cpp (slot 16, D2) does the same without the
//   Deallocate.
//
// Six of the fourteen overrides (slots 2, 5, 7, 10, 26 and 33) are therefore
// the MIDDLE base's bodies, the same six as 0x16b's, and a seat that filled one
// derived table would have left six raw DS words in a table the factory
// installs.
//
// ---- 3. THIS IS THE THIRD CLASS UNDER THAT TABLE, AND THE SEAM IS NOT -----
//         PROMOTED
//
// hal/scene_mg_memory2.cpp section 3 ends: "If a THIRD class under this base is
// ever seated, promoting kSingle3DFaces to an exported seam the way
// scene_mg.cpp exports port_scene_mg_fill_shared is the right move, and this
// paragraph is the argument for it."  This is the third class, and this lane
// does NOT take the move.  The reason is scheduling rather than disagreement:
// promoting the array means editing hal/scene_mg_flower.cpp and
// hal/scene_mg_memory2.cpp, and run mg9 has ten lanes cutting in parallel off
// one base -- one of them (CUP, 0x169) sits under this same middle table.  A
// lane that rewrote two other seats' files would hand the merge a three-way
// conflict in exchange for deleting sixteen lines.
//
// THE PER-FILE COPY IS MEASURED SAFE AND THE MEASUREMENT IS REPEATED HERE
// RATHER THAN CITED.  psy_apply keys on the ROM WORD a slot holds, so the fill
// that runs FIRST claims the middle table and the ones after it find no DS word
// left there and write nothing.  Row order decides which, and this class's row
// is APPENDED AFTER both the flower's and memory2's, so on any tree carrying
// all three:
//
//   - data_ov006_0213e448 keeps hal/scene_mg_flower.cpp's thunks and that
//     file's witness keeps counting exactly what it counted before this seat
//     existed;
//   - data_ov006_0213c8c4 gets THIS file's thunks in its six inherited slots,
//     which is the only table this seat needs to own;
//   - this seat's own dScMgSingle3DBase_c counter reads its DERIVED table's
//     six and zero from the middle table, and the census below prints both so
//     the split is visible rather than inferred.
//
// ---- 4. SLOT 2 IS NOT src's BODY -----------------------------------------
//
// func_ov006_0210a6e4 (AfterInitResources) drops the framework's second
// argument: the ROM never writes r1 before its `bl 0x20b08f0`, so the flags
// ride through in r1, and src spells the call with one argument because that is
// the only way to spell an unnamed value in C.  port/unmatched/MgFlower_Slot2.
// cpp already carries the repair as port_mg_flower_after_init and its header
// says the next lane to seat a class under this base should call it rather than
// write a second.  This seat calls it.
//
// ---- 5. SLOT 18 READS ITS ARGUMENT, AND EVERY PRECEDENT DROPS IT ----------
//
// This is the one signature in the seat that is NOT the shape the other eleven
// minigame files use, and getting it wrong would have been silent.
//
// Run mg5 lane BASESET scanned vtable offset 0x48 out of both overlay images
// and found 22 sites, ONE argument at every one of them, and the four slot-18
// thunks in the tree gained a `int /*ridethrough*/` parameter that exists only
// so __fastcall cleans four bytes.  Every seated class's slot-18 body ignores
// its r1.
//
// THIS ONE DOES NOT.  src/func_ov006_020e9c20.c takes (char *c, int a) and the
// ROM says so at 0x020e9c2c:
//
//     020e9c20  push {r4,r5,lr} / sub sp,sp,#0xc
//     020e9c28  mov r4,r0
//     020e9c2c  mov r5,r1              <- the argument is KEPT
//     ...
//     020e9c40  cmp r5,#0 / bne        <- a == 0:    ++[+0xbc], capped 0x270e
//     020e9c6c  cmp r5,#0x12 / bne     <- a == 0x12: [+0xb4] = 0, [+0xbc] = 0
//
// so the thunk must PASS the ride-through, not merely pop it.  A dropping
// thunk hands the body stack litter, and both compares would then answer on
// whatever was there -- with a == 0x12 zeroing the live dScMgBase_c's +0xb4,
// which is the field func_ov004_020adb1c draws as the star badge and the field
// section 17 of port/mg_fanout_costs.txt traces to the save record.
//
// AND ITS OWN InitResources IS ONE OF THE 22 SITES.  src/func_ov006_020e9e70.
// cpp ends with `((Obj*)c)->v48(-1);` through a shadow class over the SELF
// object, and the ROM at 0x020ea110 is `ldr r2,[r0] / ldr r2,[r2,#0x48] /
// blx r2` with `mvn r1,#0` before it.  -1 is neither 0 nor 0x12, so both
// branches are skipped -- which means a dropping thunk would have looked FINE
// on the init path and gone wrong only when the state machine reset.
//
// ---- 6. THE TWO SHADOW-CLASS MODEL DISPATCHES ARE SAFE, AND THAT IS -------
//         MEASURED RATHER THAN ASSUMED
//
// src/func_ov006_020e9d1c.cpp (slot 9, Render) draws through
// `((Obj *)(c + 0x4f38))->vcall(0)` and the same at +0x4f88, where Obj is a
// local six-virtual shadow -- the construct hal/scene_mg_memory2.cpp section
// "THE SHADOW-CLASS TEST" rules on.  That ruling is: a shadow over the MOUNTED
// ROM table is correct, a shadow over a HOST C++ vtable is wrong.  Both of
// these are host objects, so the test says wrong -- and they are still safe,
// for a reason that is one line of hal/cxxname_bridge.cpp:
//
//   WHICH CLASS EACH OBJECT IS comes from the CONSTRUCTOR, not the call.
//   src/MgPsycheOut_Spawn.cpp runs _ZN5ModelC1Ev(o + 0x4f38) and
//   _ZN5ModelC1Ev(o + 0x4f88), so both are plain Model.  slot 17 agrees:
//   it destroys both with _ZN5ModelD1Ev.
//
//   hal_fill_model_vtable() writes _ZTV5Model[4] = mv_render AND
//   _ZTV5Model[5] = mv_render, with the comment "TUs that dispatch through
//   LOCAL shadow classes count in ROM/Itanium numbering (two dtor slots), which
//   lands Render at 5."  Byte +0x14 -- what the ROM loads -- is the sixth word,
//   index 5, and it holds mv_render.
//
// So this class does NOT need the MgMemory2_ModelRender_020c1804 repair.  It
// would have needed it for a BlendModelAnim or a ModelAnim2, where slot 5 is
// Virtual18 and the dual fill deliberately does not exist.  The check is which
// class the constructor built, and it was made before slot 9 was wired.
//
// ---- 7. THE WALL IS THIS CLASS'S AND IT IS SEVEN TUs, TWO OF THEM SILENT --
//
// port/unmatched/Mg3DEsp_StateDispatch.cpp carries all seven table dispatchers
// and the twenty-two state addresses.  Nothing in this file dispatches a
// member pointer.  Two of the seven are the open-coded-int third shape that
// neither a link nor a `::*` sweep can see, and one of those two is reachable
// only through the other, so a clean link and a clean sweep would have shipped
// both.
//
// ---- 8. WHAT THIS SEAT DOES NOT CLAIM ------------------------------------
//
// The state machine is proven to run by the census this file prints, not by
// this comment.  A run that reports slot hits and zero routed dispatches has
// ticked an object without entering its state machine, and that reads as a
// success unless it is printed -- which is why the numbers below are printed
// whether they are zero or not, per table as well as in total.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* the mount storage the fill writes into.  All three are the mounts' own, so
   the fill replaces the mounted words rather than building a fresh host array:
   a second host array of the same name is a duplicate symbol, and leaving the
   mounted table alone leaves live wild DS pointers in a table the factory
   installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_0213c8c4[];   /* dScMg3DEsp_c,        36 */
extern unsigned char MgPsycheOut_SpawnInfo[];

/* dScMgSingle3DBase_c's eight overrides.  Slot 2 is NOT src's body: see
   section 4 and port/unmatched/MgFlower_Slot2.cpp. */
int   port_mg_flower_after_init(void *c, unsigned f);   /* slot  2 */
void  func_ov006_0210a608(void *c, unsigned f);         /* slot  5 */
int   func_ov006_0210a698(void *c);                     /* slot  7 */
int   func_ov006_0210a664(void *c);                     /* slot 10 */
int   func_ov006_0210a4b0(char *c);                     /* slot 16 D2 */
int   func_ov006_0210a4e8(char *c);                     /* slot 17 D0 */
int   func_ov006_0210a600(void);                        /* slot 26 */
void  func_ov006_0210a708(char *c);                     /* slot 33 */

/* dScMg3DEsp_c's own eight.  func_ov006_020e9e00 is the HOST COPY in
   unmatched/Mg3DEsp_StateDispatch.cpp, not the src TU: it is the
   pointer-to-member Behavior and the port cannot compile the src. */
int   func_ov006_020e9e70(char *self);        /* slot  0 InitResources */
int   func_ov006_020e9cec(void);              /* slot  3 CleanupResources */
int   func_ov006_020e9e00(void *self);        /* slot  6 Behavior, host copy */
int   func_ov006_020e9d1c(char *c);           /* slot  9 Render */
void *func_ov006_020e7660(int c);             /* slot 16 D2 */
void *func_ov006_020e76e4(char *c);           /* slot 17 D0 */
void  func_ov006_020e9c20(char *c, int a);    /* slot 18 state reset, TAKES a */
int   func_ov006_020e9c10(void);              /* slot 20 */

/* the factory */
void *MgPsycheOut_Spawn(void);

/* the dispatch file's witnesses */
unsigned port_mg_esp3d_state_hits(void);
unsigned port_mg_esp3d_floor_hits(void);
unsigned port_mg_esp3d_nonmatching(void);
void     port_mg_esp3d_table_counts(unsigned *out7);
/* the FIELD dispatcher's, from unmatched/Mg3DEsp_FieldPmf.cpp -- a separate
   pair because it is a separate defect: a member pointer held in an object
   field rather than in a table, on the sub-object at this+0x4fd8. */
void     port_mg_esp3d_field_counts(unsigned *calls, unsigned *routed);
/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the persistent minigame record, for the reason hal/scene_mg_memory2.cpp
   names: nothing new is defined here, the storage is hal/level_boot.cpp's
   .dsstate$savblk0004 and the index function is src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

}  /* extern "C" */

static int port_mg_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the two tables this file owns
// faces for.  The instrument every scene seat in this port carries, for the
// reason hal/scene_mg.cpp gives: an object that EXISTS and an object that RUNS
// look identical from outside.  The framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array.
static unsigned g_psy_hits[36];
static unsigned g_psy_base_hits[36];   /* the same slots on the MIDDLE table */

#define PSY(n)   (++g_psy_hits[(n)])
#define P3D(n)   (++g_psy_base_hits[(n)])

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3_ainit(void *s, void *, unsigned f)
{ P3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3_aclean(void *s, void *, unsigned f)
{ P3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3_bbeh(void *s, void *)
{ P3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3_bren(void *s, void *)
{ P3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3_d2(void *s, void *)
{ P3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3_d0(void *s, void *)
{ P3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3_v26(void *, void *)
{ P3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3_v33(void *s, void *)
{ P3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMg3DEsp_c's own eight ------------------------------------------- */
static int  __fastcall psy_init(void *s, void *)
{ PSY(0);  const int r = func_ov006_020e9e70((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall psy_clean(void *, void *)
{ PSY(3);  return func_ov006_020e9cec(); }
static int  __fastcall psy_beh(void *s, void *)
{ PSY(6);  const int r = func_ov006_020e9e00(s); hal_gapless_splice(); return r; }
static int  __fastcall psy_render(void *s, void *)
{ PSY(9);  return func_ov006_020e9d1c((char *)s); }
static void *__fastcall psy_d2(void *s, void *)
{ PSY(16); return func_ov006_020e7660((int)(size_t)s); }
static void *__fastcall psy_d0(void *s, void *)
{ PSY(17); return func_ov006_020e76e4((char *)s); }
/* SLOT 18 TAKES ONE STACK ARGUMENT AND THIS CLASS READS IT.  Section 5 has the
   disassembly; the parameter is FORWARDED here where every other seated seat
   drops it, because this body branches on it. */
static int  __fastcall psy_reset(void *s, void *, int a)
{ PSY(18); func_ov006_020e9c20((char *)s, a); return 1; }
static int  __fastcall psy_v20(void *, void *)
{ PSY(20); return func_ov006_020e9c10(); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a no-op
   as the real body having run. */
static unsigned g_psy_init_skipped, g_psy_render_skipped;
static int __fastcall psy_init_noop(void *, void *)
{ ++g_psy_init_skipped; return 1; }
static int __fastcall psy_render_noop(void *, void *)
{ ++g_psy_render_skipped; return 1; }

struct PsyFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so the array is order-independent and
   cannot land on a slot the ROM did not park that body in. */
static const PsyFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

static const PsyFace kEsp3DFaces[] = {
    {0x020e9e70u, (void *)psy_init},  {0x020e9cecu, (void *)psy_clean},
    {0x020e9e00u, (void *)psy_beh},   {0x020e9d1cu, (void *)psy_render},
    {0x020e7660u, (void *)psy_d2},    {0x020e76e4u, (void *)psy_d0},
    {0x020e9c20u, (void *)psy_reset}, {0x020e9c10u, (void *)psy_v20},
};

static unsigned psy_apply(void **vt, unsigned n, const PsyFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address.  A minigame table is 36 slots and every one
   of them is dispatched by something, so a nonzero answer is a wild call
   waiting to happen and the seat says so out loud. */
static unsigned psy_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_esp3d_hits(void);

/* how many words THIS fill claimed on each table, kept so the census can state
   section 3's split as a measurement rather than as an expectation */
static unsigned g_psy_mid_claimed, g_psy_vt_claimed_mid, g_psy_vt_claimed_own;

extern "C" void port_scene_fill_esp3d(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213c8c4;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY.  Earlier rows'
       fills already did it and run first, so on a tree carrying them this is a
       second pass over words that are already host pointers and finds nothing.
       It is here so this class does not depend on another class's row existing:
       the factory's first act is func_ov004_020b2adc, which writes
       data_ov004_020bc0c0 into the object's first word before either derived
       table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE.  Live storage in its own right -- the destructor chain
       writes it into the object on the way down, so a slot dispatched between
       the two vptr writes reads it.  Section 3 is why applying this file's own
       arrays here is safe next to the flower's and memory2's, and why the count
       this claims is expected to be ZERO on a tree that carries either. */
    port_scene_mg_fill_shared(mid, 36);
    g_psy_mid_claimed =
        psy_apply(mid, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]) +
        psy_apply(mid, 36, kEsp3DFaces,
                  sizeof kEsp3DFaces / sizeof kEsp3DFaces[0]);

    /* THE DERIVED TABLE.  Shared arm9 and dScMgBase_c words first, then the
       intermediate base's six that this class does not override, then this
       class's own eight.  Order does not matter -- the key sets are disjoint,
       because a word is one address -- but it reads in inheritance order. */
    port_scene_mg_fill_shared(vt, 36);
    g_psy_vt_claimed_mid =
        psy_apply(vt, 36, kSingle3DFaces,
                  sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    g_psy_vt_claimed_own =
        psy_apply(vt, 36, kEsp3DFaces,
                  sizeof kEsp3DFaces / sizeof kEsp3DFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)psy_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)psy_render_noop;
    }

    {
        const unsigned lb = psy_raw_left(base, 36);
        const unsigned lm = psy_raw_left(mid, 36);
        const unsigned lv = psy_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] ESP3D FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMg3DEsp_c %u. A dispatch "
                         "of any of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it.  Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 389) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_esp3d_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory; the forwarder is kept for the reason title_spawn and
   port_mg_curling_spawn are, so the seat has one place to observe the object
   without the registry table growing a second column.

   THE FACTORY NEEDS NO DISPLACEMENT RULING, and it is worth recording because
   0x169's did.  src/MgPsycheOut_Spawn.cpp calls func_ov004_020b2adc(o) WITH its
   argument, where src/func_ov006_020e0574.cpp (0x169's factory) calls the same
   base constructor with none and rides r0 through -- and that callee
   dereferences on its first statement and then writes vtable words through the
   pointer.  This class's factory is on the correct side of it and is linked
   from the slice rather than host-copied. */
static char *g_psy_self;

extern "C" void *port_mg_esp3d_spawn(void)
{
    void *p = MgPsycheOut_Spawn();
    g_psy_self = (char *)p;
    return p;
}

extern "C" void port_scene_esp3d_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) { total += g_psy_hits[i]; mtotal += g_psy_base_hits[i]; }

    std::printf("[scene] dScMg3DEsp_c slot hits: init %u, cleanup %u, "
                "behavior %u, render %u, D2 %u, D0 %u, state-reset %u%s\n",
                g_psy_hits[0], g_psy_hits[3], g_psy_hits[6], g_psy_hits[9],
                g_psy_hits[16], g_psy_hits[17], g_psy_hits[18],
                g_psy_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_psy_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_psy_init_skipped);

    std::printf("[scene] dScMg3DEsp_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_psy_hits[i]) std::printf(" %d(x%u)", i, g_psy_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_psy_base_hits[i]) std::printf(" %d(x%u)", i, g_psy_base_hits[i]);
    std::printf("   (%u total)\n", mtotal);

    /* SECTION 3's SPLIT, MEASURED.  On a tree carrying the flower's row and
       memory2's, the middle table is claimed before this fill runs, so this
       seat expects 0 there and 6 + 8 on its own table.  Printed rather than
       asserted: a different number is a fact about row order, not a fault, and
       the reader should see which. */
    std::printf("[scene] dScMg3DEsp_c fill claimed: middle table %u word(s), "
                "derived table %u from the middle base + %u own\n",
                g_psy_mid_claimed, g_psy_vt_claimed_mid, g_psy_vt_claimed_own);

    /* THE STATE MACHINE'S OWN WITNESS.  Printed whether or not it fired: a run
       with slot hits and zero routed dispatches has ticked the object without
       entering its state machine, and that reads as a success unless it is
       said out loud.  PER TABLE as well as in total, because this machine is
       three levels deep and a single number cannot tell a class sitting in the
       Behavior's slot 0 apart from one working its way down. */
    {
        unsigned t[7] = {0,0,0,0,0,0,0};
        unsigned calls = 0, unknown = 0;
        port_mg_esp3d_table_counts(t);
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] dScMg3DEsp_c state dispatch: %u routed to one of "
                    "the class's 22 table states, %u bodiless want(s), %u into "
                    "the NONMATCHING 0x020e8830; %u framework call(s), %u "
                    "UNHANDLED address(es)\n",
                    port_mg_esp3d_state_hits(), port_mg_esp3d_floor_hits(),
                    port_mg_esp3d_nonmatching(), calls, unknown);
        std::printf("[scene] dScMg3DEsp_c per table: f2c(slot6) %u, "
                    "fac %u, f5c %u, f74 %u, f1c %u, f8c %u, f44 %u\n",
                    t[0], t[1], t[2], t[3], t[4], t[5], t[6]);
        unsigned fc = 0, fr = 0;
        port_mg_esp3d_field_counts(&fc, &fr);
        std::printf("[scene] dScMg3DEsp_c FIELD dispatch (sub-object +0x4fd8, "
                    "member pointer at its +0x210): %u call(s), %u routed\n",
                    fc, fr);
    }

    std::printf("[scene] dScMg3DEsp_c floors: NONE. All twenty-two state code "
                "words reach a real src TU, and all seven tables have a "
                "dispatcher (two relocations each: the constructor's and the "
                "dispatcher's literal pool)\n");

    /* The three state indexes the ROM's own dispatchers read, at the offsets
       disassembled in unmatched/Mg3DEsp_StateDispatch.cpp section 3.  +0x553c
       is slot 6's and is the one slot 18 zeroes, which cross-confirms both. */
    if (g_psy_self)
        std::printf("[scene] dScMg3DEsp_c object at %p, state index +0x553c "
                    "= %d, sub-state +0x5540 = %d, f44 index +0x5552 = %u\n",
                    (void *)g_psy_self,
                    *(int *)(g_psy_self + 0x553c),
                    *(int *)(g_psy_self + 0x5540),
                    *(const unsigned char *)(g_psy_self + 0x5552));

    /* THE THREE RECORD ARRAYS THE ARITY-1 TABLES WALK, because a run that
       dispatches nothing and a run whose records are all switched off read the
       same on every other line.  The bounds, strides and byte offsets are the
       ROM's own, read off the three dispatchers' disassembly rather than off
       src:

         020e9374   5 records at +0x5218, stride 0x18, gate +0x00, state +0x01
         020e8d08   3 records at +0x5290, stride 0x14, gate +0x00, state +0x01
         020e8a44  20 records at +0x52d4, stride 0x20, gate +0x00, state +0x05
                   and the sub-state 020e8830 indexes f8c with at +0x06 */
    if (g_psy_self) {
        int live5 = 0, live3 = 0, live20 = 0;
        std::printf("[scene] dScMg3DEsp_c records A (5 @ +0x5218, x0x18) "
                    "gate.state:");
        for (int i = 0; i < 5; ++i) {
            const unsigned char *r =
                (const unsigned char *)(g_psy_self + 0x5218 + i * 0x18);
            if (r[0]) ++live5;
            std::printf(" %u.%u", r[0], r[1]);
        }
        std::printf("   (%d live)\n", live5);

        std::printf("[scene] dScMg3DEsp_c records B (3 @ +0x5290, x0x14) "
                    "gate.state:");
        for (int i = 0; i < 3; ++i) {
            const unsigned char *r =
                (const unsigned char *)(g_psy_self + 0x5290 + i * 0x14);
            if (r[0]) ++live3;
            std::printf(" %u.%u", r[0], r[1]);
        }
        std::printf("   (%d live)\n", live3);

        std::printf("[scene] dScMg3DEsp_c records C (20 @ +0x52d4, x0x20) "
                    "gate.state.sub:");
        for (int i = 0; i < 20; ++i) {
            const unsigned char *r =
                (const unsigned char *)(g_psy_self + 0x52d4 + i * 0x20);
            if (r[0]) ++live20;
            std::printf(" %u.%u.%u", r[0], r[5], r[6]);
        }
        std::printf("   (%d live)\n", live20);
    }

    /* THE PARAM THE ROW WAS SEATED WITH.  hal/scene_mg_sound.cpp's
       port_mg_scene_spawn_param finds scene 389 at ov005 row 18 -- the ONLY row
       for this id, so none of section 17's six-duplicate caveat applies -- and
       that row's +0x04 word is 0x00121200: record 18, name text 18, and the
       slot-35 low byte 0.

       +0xb4 IS NOT THE SAVED LEVEL FOR THIS CLASS, and that is worth stating
       next to the record because Memory Master's is.  This class's
       InitResources ends `*(int*)(c + 0xb4) = 0` -- the ROM writes sb, and
       0x020ea024 is `mov sb,#0` -- so nothing seeds it from data_0209caf4.  The
       record is printed anyway: slot 18 WRITES +0xb4 (to zero) when its
       argument is 0x12, and +0xbc is the counter it increments when the
       argument is 0. */
    if (g_psy_self) {
        const int idx = port_mg_record_index();
        std::printf("[scene] dScMg3DEsp_c +0xa4 = %d, +0xa8 score = %d, "
                    "+0xb4 = %d (written 0 by InitResources, not seeded), "
                    "+0xbc counter = %d\n",
                    *(const int *)(g_psy_self + 0xa4),
                    *(const int *)(g_psy_self + 0xa8),
                    *(const int *)(g_psy_self + 0xb4),
                    *(const int *)(g_psy_self + 0xbc));
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMg3DEsp_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }  (ov005 row 18 says record "
                        "18, text 18)\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMg3DEsp_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }
    std::fflush(stdout);
}
