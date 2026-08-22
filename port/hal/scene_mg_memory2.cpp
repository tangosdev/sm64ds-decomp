// dScMgMemory2_c, the "Memory Master" minigame -- actor id 0x16b, scene 363.
// Run mg6, lane MEM.  The seventh ov006 minigame class the port seats and the
// SECOND with a three-deep hierarchy.
//
// Read port/slice_mem.txt for the identity derivation, the three width checks
// and the closure.  Read port/mg_fanout_costs.txt section 3's "0x16b IS DERIVED
// IN FULL AND NOT SEATED" block for what run link60 lane MGB paid before this
// lane existed.  This file is the seat: the two new vtables' faces, the fill,
// the factory forwarder and the run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The reason hal/scene_mg_flower.cpp gives, unchanged.  scene_mg.cpp is the
// FAMILY half -- the mounts, the thirty-five overlay constructors,
// dScMgBase_c's twenty-eight address-keyed faces and the gate that keeps the
// constructors off a level boot -- and all of it is reached through the one
// seam that file exports, port_scene_mg_fill_shared().  Nothing of it is
// re-derived here and kMgBaseFaces is NOT copied: a second copy would install a
// different set of host thunks over the same DS addresses in the same mounted
// tables and split the per-slot witness in two, which is the failure that
// file's header names.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM SAYS SO TWICE ------------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgMemory2_c      data_ov006_0213d4d4  36 slots
//
// THE HANDOVER DID NOT KNOW THIS AND IT IS THIS LANE'S FIRST FINDING.
// port/mg_fanout_costs.txt section 3 lists 0x16b as "vtable 0x0213d4d4, 36
// slots, 14 overrides" and says nothing about an intermediate base, so a lane
// that filled one derived table would have left six of its own slots holding
// raw DS words.  Both edges are read off code, not inferred from names:
//
//   src/MgMemoryMaster_Spawn.c (the factory, 0x020f75d4) calls
//   func_ov004_020b2adc(p) -- dScMgBase_c's constructor -- then writes
//   data_ov006_0213e448 into p[0], constructs the Particle::SysTracker at
//   +0x471c, then writes data_ov006_0213d4d4 over it.
//
//   src/func_ov006_020f55b8.cpp (slot 17, D0) unwinds them in the OPPOSITE
//   order: 0x0213d4d4 first, then 0x0213e448, then
//   Particle::SysTracker::~SysTracker(+0x471c) and func_ov004_020b29c0.
//   src/func_ov006_020f5564.cpp (slot 16, D2) does the same without the
//   Deallocate.
//
// A hierarchy written by the constructor and unwritten by the destructor in
// reverse is not a guess.  It is the same middle table dScMgFlower_c (0x186)
// and dScMgCup_c (0x169) sit under, and port/slice_flw.txt derived it
// independently and agrees word for word.
//
// ---- 3. THE FILL IS ADDRESS-KEYED, SO THREE TABLES ARE CHEAP --------------
//
// port_scene_fill_rom and mem_apply both key on the ROM WORD a slot holds, not
// on the slot number, so the same face array can be applied to every table in
// the chain and can only land where the ROM parked that exact body.
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kMemory2Faces   the eight this class adds or replaces on top
//                   (slots 0, 6, 9, 16, 17, 18, 19, 21)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x0210a4b0 /
// 0x0210a4e8 in the middle table, 0x020f5564 / 0x020f55b8 in this one -- so
// both arrays can be applied to both tables and each writes only its own.  Six
// of kSingle3DFaces' eight are what fills slots 2, 5, 7, 10, 26 and 33 of the
// DERIVED table, because this class does not override them.
//
// THE MIDDLE TABLE IS SHARED WITH hal/scene_mg_flower.cpp AND THAT IS SAFE,
// MEASURED RATHER THAN ASSUMED.  Both files define their own eight-row array
// over the same eight DS words, and both fills run on every boot.  The one that
// runs FIRST claims the middle table; the second finds no DS word left there
// and writes nothing, because mem_apply keys on a DS address and a slot already
// holding a host thunk matches none.  Row order decides which, and this class's
// row is APPENDED AFTER the flower row, so on any tree carrying both:
//
//   - data_ov006_0213e448 keeps hal/scene_mg_flower.cpp's thunks, unchanged
//     from the baseline, and that file's dScMgSingle3DBase_c witness keeps
//     counting exactly what it counted before this seat existed;
//   - data_ov006_0213d4d4 gets THIS file's thunks in its six inherited slots,
//     which is the only table this seat needs to own.
//
// Both sets forward to the identical ROM bodies -- including the shared
// port_mg_flower_after_init host copy for slot 2, section 4 -- so the only
// observable difference between them is which file's counter a middle-table
// dispatch lands in.  This is deliberately NOT the kMgBaseFaces case section 1
// refuses: that array is the FAMILY's and is reached through one exported seam
// precisely so it is never duplicated.  kSingle3DFaces belongs to a class two
// files happen to derive from, it has no exported seam today, and inventing one
// means editing another lane's file.  If a third class under this base is ever
// seated, promoting it to a seam is the right move and this paragraph is the
// argument for it.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND IT IS NOT THIS LANE'S HOST COPY -----
//
// func_ov006_0210a6e4 (AfterInitResources) drops the framework's second
// argument: the ROM never writes r1 before its `bl 0x20b08f0`, so the flags
// ride through in r1, and src spells the call with one argument because that is
// the only way to spell an unnamed value in C.  On the host the callee reads
// stack litter, and func_ov004_020b08f0's tail is Scene::AfterInitResources
// (this, flags) where flags == 1 marks the actor for destruction -- so the
// wrong value is a coin flip on whether the scene survives frame 0.
//
// port/unmatched/MgFlower_Slot2.cpp already carries the repair as
// port_mg_flower_after_init, and its header ends "THE NEXT LANE TO SEAT 361 OR
// 363 SHOULD CALL THIS RATHER THAN WRITE A SECOND."  This is 363, and it calls
// it.  src/func_ov006_0210a6e4.cpp stays out of port/slice_mem.txt for the same
// reason it is out of port/slice_flw.txt: listing it would be an LNK2005
// against that host copy.
//
// ---- 5. THE WIDTH IS 36 ON BOTH TABLES, CHECKED THREE WAYS ----------------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own table were phantoms and that a 37-slot fill writes a host
// thunk over live ROM data belonging to another class.  Both tables went
// through all three of its checks before a word was written:
//
//   table       span to next symbol    slot 35     word past the end
//   ---------   --------------------   ---------   -------------------------
//   0213d4d4    0213d564, 0x90 = 36    020ad660    0x00000001, the first word
//                                                  of data_ov006_0213d564
//   0213e448    0213e4d8, 0x90 = 36    020ad660    0x050a0a0f, the first word
//                                                  of data_ov006_0213e4d8
//
// Section 11 names 0x16b's own word-past-the-end as 0x00000001 and this lane
// re-read it out of extracted/overlays/overlay_0006.bin rather than citing it.
//
// ---- 6. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first minigame
// row's fill.  Appending after every existing row therefore means the
// constructors have already run with clean ROM words when this fill starts,
// which is the latent-safe direction section 11 names -- and section 3 above
// depends on it for a second reason.
//
// ---- 7. THE WALL IS THIS CLASS'S AND IT IS SEVEN TUs ----------------------
//
// port/unmatched/MgMemory2_StateDispatch.cpp carries the four TABLE
// dispatchers and the twenty-nine state addresses; port/unmatched/
// MgMemory2_FieldPmf.cpp carries the three that dispatch a member pointer held
// in an object FIELD, one of which -- func_ov004_020b52fc -- no previous lane
// had found.  Both headers are the derivation.  Nothing in this file dispatches
// a member pointer.
//
// ---- 8. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
// The state machine is proven to run by the census this file prints, not by
// this comment.  A run that reports slot hits and zero routed dispatches has
// ticked an object without entering its state machine, and that reads as a
// success unless it is printed -- which is why the numbers below are printed
// whether they are zero or not.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);

/* the mount storage the fill writes into.  All three are the mounts' own, so
   the fill replaces the mounted words rather than building a fresh host array
   -- the ov007 and curling treatment, for the same reason: a second host array
   of the same name is a duplicate symbol, and leaving the mounted table alone
   leaves live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_0213d4d4[];   /* dScMgMemory2_c,      36 */
extern unsigned char MgMemoryMaster_SpawnInfo[];

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

/* dScMgMemory2_c's own eight.  func_ov006_020f7458 is the HOST COPY in
   unmatched/MgMemory2_StateDispatch.cpp, not the src TU: it is the
   pointer-to-member dispatcher and the port cannot compile the src. */
int   func_ov006_020f74b4(char *self);        /* slot  0 InitResources */
int   func_ov006_020f7458(void *self);        /* slot  6 Behavior, host copy */
int   func_ov006_020f73f4(char *c);           /* slot  9 Render */
void *func_ov006_020f5564(char *c);           /* slot 16 D2 */
void *func_ov006_020f55b8(char *c);           /* slot 17 D0 */
void  func_ov006_020f7394(char *c);           /* slot 18 state reset */
int   func_ov006_020f7320(char *c);           /* slot 19 */
int   func_ov006_020f730c(char *p);           /* slot 21 */

/* the factory */
int  *MgMemoryMaster_Spawn(void);

/* the two dispatch files' witnesses */
unsigned port_mg_memory2_state_hits(void);
unsigned port_mg_memory2_floor_hits(void);
unsigned port_mg_memory2_nonmatching(void);
void     port_mg_memory2_field_counts(unsigned *calls, unsigned *hits);
/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void     port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

}  /* extern "C" */

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the two tables this file owns
// faces for.  The instrument every scene seat in this port carries, for the
// reason hal/scene_mg.cpp gives: an object that EXISTS and an object that RUNS
// look identical from outside.  The framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array.
static unsigned g_mem_hits[36];
static unsigned g_mem_base_hits[36];   /* the same slots on the MIDDLE table */

#define MEM(n)   (++g_mem_hits[(n)])
#define M3D(n)   (++g_mem_base_hits[(n)])

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3_ainit(void *s, void *, unsigned f)
{ M3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3_aclean(void *s, void *, unsigned f)
{ M3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3_bbeh(void *s, void *)
{ M3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3_bren(void *s, void *)
{ M3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3_d2(void *s, void *)
{ M3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3_d0(void *s, void *)
{ M3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3_v26(void *, void *)
{ M3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3_v33(void *s, void *)
{ M3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgMemory2_c's eight --------------------------------------------- */
static int  __fastcall mem_init(void *s, void *)
{ MEM(0);  const int r = func_ov006_020f74b4((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall mem_beh(void *s, void *)
{ MEM(6);  const int r = func_ov006_020f7458(s); hal_gapless_splice(); return r; }
static int  __fastcall mem_render(void *s, void *)
{ MEM(9);  return func_ov006_020f73f4((char *)s); }
static void *__fastcall mem_d2(void *s, void *)
{ MEM(16); return func_ov006_020f5564((char *)s); }
static void *__fastcall mem_d0(void *s, void *)
{ MEM(17); return func_ov006_020f55b8((char *)s); }
/* SLOTS 18 AND 19 TAKE ONE STACK ARGUMENT AND THE THUNK MUST POP IT.  Run mg5
   lane BASESET scanned both offsets out of the two overlay images word by word
   (runs/mg5/out/baseset/slot18_19_scan.txt): offset 0x48 is 22 sites and offset
   0x4c is 14, and the argument count is ONE at every one of them.  A __fastcall
   thunk declared (void*, void*) compiles to a bare `ret`, leaks the caller's
   four bytes, and a later `ret` takes a garbage return address at a fault that
   MOVES WITH THE BUILD.  The parameter exists so __fastcall cleans four bytes;
   neither ROM body reads its r1 and neither is called with it. */
static int  __fastcall mem_reset(void *s, void *, int /*ridethrough*/)
{ MEM(18); func_ov006_020f7394((char *)s); return 1; }
static int  __fastcall mem_v19(void *s, void *, int /*ridethrough*/)
{ MEM(19); return func_ov006_020f7320((char *)s); }
static int  __fastcall mem_v21(void *s, void *)
{ MEM(21); return func_ov006_020f730c((char *)s); }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a no-op
   as the real body having run. */
static unsigned g_mem_init_skipped, g_mem_render_skipped;
static int __fastcall mem_init_noop(void *, void *)
{ ++g_mem_init_skipped; return 1; }
static int __fastcall mem_render_noop(void *, void *)
{ ++g_mem_render_skipped; return 1; }

struct MemFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so the array is order-independent and
   cannot land on a slot the ROM did not park that body in. */
static const MemFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

static const MemFace kMemory2Faces[] = {
    {0x020f74b4u, (void *)mem_init},  {0x020f7458u, (void *)mem_beh},
    {0x020f73f4u, (void *)mem_render},{0x020f5564u, (void *)mem_d2},
    {0x020f55b8u, (void *)mem_d0},    {0x020f7394u, (void *)mem_reset},
    {0x020f7320u, (void *)mem_v19},   {0x020f730cu, (void *)mem_v21},
};

static unsigned mem_apply(void **vt, unsigned n, const MemFace *f, unsigned nf)
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
static unsigned mem_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_memory2_hits(void);

extern "C" void port_scene_fill_memory2(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213d4d4;

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
       arrays here is safe next to hal/scene_mg_flower.cpp's. */
    port_scene_mg_fill_shared(mid, 36);
    mem_apply(mid, 36, kSingle3DFaces,
              sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    mem_apply(mid, 36, kMemory2Faces,
              sizeof kMemory2Faces / sizeof kMemory2Faces[0]);

    /* THE DERIVED TABLE.  Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own eight.  Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order. */
    port_scene_mg_fill_shared(vt, 36);
    mem_apply(vt, 36, kSingle3DFaces,
              sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    mem_apply(vt, 36, kMemory2Faces,
              sizeof kMemory2Faces / sizeof kMemory2Faces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)mem_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)mem_render_noop;
    }

    {
        const unsigned lb = mem_raw_left(base, 36);
        const unsigned lm = mem_raw_left(mid, 36);
        const unsigned lv = mem_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] MEMORY2 FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgMemory2_c %u. A dispatch "
                         "of any of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it.  Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 363) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_memory2_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory; the forwarder is kept for the reason title_spawn and
   port_mg_curling_spawn are, so the seat has one place to observe the object
   without the registry table growing a second column.

   THE FACTORY NEEDS NO DISPLACEMENT RULING, and that is worth recording because
   0x169's did.  src/MgMemoryMaster_Spawn.c calls func_ov004_020b2adc(p) WITH
   its argument, where src/func_ov006_020e0574.cpp (0x169's factory) calls the
   same base constructor with none and rides r0 through -- and that callee
   dereferences on its first statement and then writes three vtable words
   through the pointer, so the difference is a wild write versus a correct one.
   This class's factory is on the correct side of it and is linked from the
   slice rather than host-copied. */
static char *g_mem_self;

extern "C" void *port_mg_memory2_spawn(void)
{
    void *p = (void *)MgMemoryMaster_Spawn();
    g_mem_self = (char *)p;
    return p;
}

extern "C" void port_scene_memory2_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) { total += g_mem_hits[i]; mtotal += g_mem_base_hits[i]; }

    std::printf("[scene] dScMgMemory2_c slot hits: init %u, behavior %u, "
                "render %u, D2 %u, D0 %u, state-reset %u%s\n",
                g_mem_hits[0], g_mem_hits[6], g_mem_hits[9], g_mem_hits[16],
                g_mem_hits[17], g_mem_hits[18],
                g_mem_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_mem_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_mem_init_skipped);

    std::printf("[scene] dScMgMemory2_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_mem_hits[i]) std::printf(" %d(x%u)", i, g_mem_hits[i]);
    std::printf("   (%u total)\n", total);

    std::printf("[scene] dScMgSingle3DBase_c slots entered (this seat's copy):");
    for (int i = 0; i < 36; ++i)
        if (g_mem_base_hits[i]) std::printf(" %d(x%u)", i, g_mem_base_hits[i]);
    std::printf("   (%u total)\n", mtotal);

    /* THE STATE MACHINE'S OWN WITNESS.  Printed whether or not it fired: a run
       with slot hits and zero routed dispatches has ticked the object without
       entering its state machine, and that reads as a success unless it is
       said out loud. */
    {
        unsigned fcalls = 0, fhits = 0, calls = 0, unknown = 0;
        port_mg_memory2_field_counts(&fcalls, &fhits);
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] dScMgMemory2_c state dispatch: %u routed to one of "
                    "the class's 26 table states (of which %u ran the "
                    "NONMATCHING 0x020f6904), %u bodiless want(s); FIELD "
                    "dispatch %u call(s), %u routed; %u framework call(s), "
                    "%u UNHANDLED address(es)\n",
                    port_mg_memory2_state_hits(),
                    port_mg_memory2_nonmatching(),
                    port_mg_memory2_floor_hits(),
                    fcalls, fhits, calls, unknown);
    }

    /* THE FLOOR THAT USED TO BE HERE. func_ov006_020f5b98 is the sixth call
       vtable slot 9 (Render) makes, and it is the only code in this class that
       draws a card. It has a config symbol, still no delink block, and now a
       src TU: src/func_ov006_020f5b98.c, carried by port/slice_mem.txt. The
       line stays so a reader of an old log and a reader of a new one are
       looking at the same place. */
    std::printf("[scene] dScMgMemory2_c floor: the Render callee 0x020f5b98 is "
                "DECOMPILED (src/func_ov006_020f5b98.c) and no longer trapped\n");

    /* The two state indexes the ROM's own dispatchers read, at the offsets
       disassembled in unmatched/MgMemory2_StateDispatch.cpp section 2. +0x53d4
       is slot 6's and is the one slot 18 zeroes, which cross-confirms both. */
    if (g_mem_self)
        std::printf("[scene] dScMgMemory2_c object at %p, state index +0x53d4 "
                    "= %d, +0x53d8 = %d\n",
                    (void *)g_mem_self,
                    *(int *)(g_mem_self + 0x53d4),
                    *(int *)(g_mem_self + 0x53d8));

    /* THE CARD RECORDS, because a run that renders no cards and a run that has
       no cards to render read the same on every other line.
       func_ov006_020f5b98 walks TWENTY records at +0x51a8 with stride 0x18 and
       draws one sprite per record whose +0x12 (i.e. +0x51ba) byte is nonzero.
       The fields are read off the ROM: +0x00/+0x04 are the 20.12 screen x/y
       the draw shifts down by 12, +0x10 is the card identity (the row of the
       eleven-by-five halfword table at 0x0213d45c), +0x15 is the flip frame
       (its column) and +0x14 is the per-card state func_ov006_020f6088 sets to
       1 when a dealt card reaches its slot.  src/func_ov006_020f6c90.c is what
       populates them: 16, 18 or 20 records depending on +0x540a, each with a
       type in 1..8/9/10 twice over, x = 128.0 and y = -128.0 -- off the top of
       the screen, which is where the deal animates them in from.  So a count
       here is the difference between "the deal never happened" and "the deal
       happened and nothing drew it". */
    if (g_mem_self) {
        int live = 0, on_screen = 0, placed = 0;
        int minx = 0x7fffffff, maxx = -0x7fffffff;
        int miny = 0x7fffffff, maxy = -0x7fffffff;
        for (int i = 0; i < 20; ++i) {
            const char *r = g_mem_self + 0x51a8 + i * 0x18;
            if (*(const unsigned char *)(r + 0x12) == 0) continue;
            ++live;
            int x = *(const int *)(r + 0x00) >> 12;
            int y = *(const int *)(r + 0x04) >> 12;
            if (x < minx) minx = x;
            if (x > maxx) maxx = x;
            if (y < miny) miny = y;
            if (y > maxy) maxy = y;
            if (x > -32 && x < 288 && y > -44 && y < 236) ++on_screen;
            if (*(const unsigned char *)(r + 0x14) == 1) ++placed;
        }
        std::printf("[scene] dScMgMemory2_c cards: %d of 20 record(s) live, "
                    "%d inside the screen box, %d settled on a slot", live,
                    on_screen, placed);
        if (live)
            std::printf(", x %d..%d y %d..%d", minx, maxx, miny, maxy);
        std::printf("  (difficulty byte +0x540a = %u)\n",
                    *(const unsigned char *)(g_mem_self + 0x540a));
        /* type.frame.state per live card, and the two bytes the TOUCH path
           turns on. +0x13 is the in-play gate func_ov006_020f5c40's per-card
           dispatch reads before it dispatches at all, +0x14 is the per-card
           state it dispatches ON: state 2 is func_ov006_020f5f0c, the hit test
           that compares the stylus in data_020a0dea/deb against this record's
           x/y with a +-0x10 by +-0x16 box. +0x5406 is how many cards the
           player has turned over and is the hit test's own early-out at 2. */
        int tappable = 0;
        std::printf("[scene] dScMgMemory2_c card type.frame.state:");
        for (int i = 0; i < 20; ++i) {
            const char *r = g_mem_self + 0x51a8 + i * 0x18;
            if (*(const unsigned char *)(r + 0x12) == 0) continue;
            std::printf(" %u.%u.%u", *(const unsigned char *)(r + 0x10),
                        *(const unsigned char *)(r + 0x15),
                        *(const unsigned char *)(r + 0x14));
            if (*(const unsigned char *)(r + 0x13) != 0 &&
                *(const unsigned char *)(r + 0x14) == 2)
                ++tappable;
        }
        std::printf("\n[scene] dScMgMemory2_c touch: %d card(s) in the hit-test "
                    "state, %u turned over (+0x5406), picks +0x53f0/f1 = %u/%u\n",
                    tappable, *(const unsigned char *)(g_mem_self + 0x5406),
                    *(const unsigned char *)(g_mem_self + 0x53f0),
                    *(const unsigned char *)(g_mem_self + 0x53f1));
    }
    std::fflush(stdout);
}
