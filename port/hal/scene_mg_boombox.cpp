// dScMgSound_c, the "Boom Box" minigame -- actor id 0x16f, scene 367.
// Run mg9, lane BOX. The TWELFTH ov006 minigame class the port seats and the
// FOURTH with a three-deep hierarchy.
//
// Read port/slice_box.txt for the identity derivation, the FIVE width checks
// and the closure; port/unmatched/MgSound_StateDispatch.cpp for the sixteen
// pointer-to-member host copies and the fifty-seven state addresses; and the
// three ABI host copies for the same disease in three disguises --
// MgSound_ShadowSlot35.cpp (a vtable word read with no receiver at all),
// MgShared4f38_ModelRender_020c29dc.cpp (a shadow class at the wrong host slot)
// and MgShared4f38_ShadowFader_020c2924.cpp (an object pushed where the host
// body reads ecx). This file is the seat: the two face arrays, the fill, the
// factory forwarder and the run census.
//
// THE FILE IS NAMED FOR THE MINIGAME, NOT FOR THE CLASS, and that is the one
// place this seat departs from the flower/memory2 naming. hal/scene_mg_sound.cpp
// already exists and is the FAMILY's ov005 sound-and-spawn-param seat; a
// second file named for dScMgSound_c would read as its sibling and is not.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// hal/scene_mg_flower.cpp section 1's ruling, unchanged and not re-argued.
// scene_mg.cpp is the FAMILY half -- the mounts, the thirty-five overlay
// constructors, dScMgBase_c's address-keyed faces and the gate that keeps the
// constructors off a level boot -- and all of it is reached through the one
// seam that file exports, port_scene_mg_fill_shared(). kMgBaseFaces is NOT
// copied here: a second copy would install a different set of host thunks over
// the same DS words in the same mounted table and split the per-slot witness
// in two.
//
// ---- 2. THE HIERARCHY IS THREE DEEP, AND THE ROM SAYS SO THREE WAYS -------
//
//     Scene -> dScMgBase_c         data_ov004_020bc0c0  36 slots
//           -> dScMgSingle3DBase_c data_ov006_0213e448  36 slots
//           -> dScMgSound_c        data_ov006_0213f844  36 slots
//
// The class name is the ROM's own RTTI and not a src spelling, which is the
// chain port/mg_fanout_costs.txt section 14 says every later lane should
// prefer: the word immediately BEFORE the vtable is the type_info pointer,
// data_ov006_0213f844[-1] = 0x0213f6e4, whose second word is 0x0213f708, which
// reads "12dScMgSound_c" in extracted/overlays/overlay_0006.bin. That record's
// THIRD word is 0x0213bc64, another type_info, whose name pointer reads
// "19dScMgSingle3DBase_c" -- so the intermediate base is named by the ROM
// rather than inferred from the override count.
//
// Both edges are also read off code, the way run mg6 lane MEM read 0x16b's:
//
//   src/MgBoomBox_Spawn.cpp (0x0211cb70) calls func_ov004_020b2adc(p) WITH its
//   argument, writes data_ov006_0213e448 into p[0] (ROM 0x0211cbc4), then
//   writes data_ov006_0213f844 over it (ROM 0x0211cbcc).
//   src/func_ov006_02119958.cpp (slot 17, D0) unwinds them in the OPPOSITE
//   order -- 0x0213f844 first, then 0x0213e448 -- and src/func_ov006_02119904
//   .cpp (slot 16, D2) does the same without the Deallocate.
//
// SIX OF THIS CLASS'S THIRTEEN OVERRIDES ARE THE MIDDLE BASE'S BODIES, slots
// 2, 5, 7, 10, 26 and 33, which is the SAME six dScMgMemory2_c inherits. Its
// own seven are slots 0, 6, 9, 16, 17, 18 and 20. A seat that filled one
// derived table would have left six raw DS words in a table the factory
// installs, and the vtable axis cannot see it: a row's ovr and mark columns
// are computed against dScMgBase_c, so an intermediate base shows up only as
// "more overrides".
//
// ---- 3. THE FILL IS ADDRESS-KEYED, SO TWO TABLES ARE CHEAP ----------------
//
// port_scene_mg_fill_rom and snd_apply both key on the ROM WORD a slot holds,
// not on the slot number, so the same face array can be applied to every table
// in the chain and can only land where the ROM parked that exact body.
//
//   kSingle3DFaces  the eight bodies dScMgSingle3DBase_c overrides
//                   (slots 2, 5, 7, 10, 16, 17, 26, 33)
//   kSoundFaces     the seven this class adds or replaces on top
//                   (slots 0, 6, 9, 16, 17, 18, 20)
//
// Slots 16 and 17 appear in both and hold DIFFERENT addresses -- 0x0210a4b0 /
// 0x0210a4e8 in the middle table, 0x02119904 / 0x02119958 in this one -- so
// both arrays can be applied to both tables and each writes only its own.
//
// THE MIDDLE TABLE IS SHARED WITH hal/scene_mg_flower.cpp AND hal/
// scene_mg_memory2.cpp, AND THIS SEAT IS THE THIRD CLASS UNDER IT. Section 3
// of scene_mg_memory2.cpp closes with "if a THIRD class under this base is
// ever seated, promoting kSingle3DFaces to an exported seam the way
// scene_mg.cpp exports port_scene_mg_fill_shared is the right move, and this
// paragraph is the argument for it."
//
//   THIS LANE DOES NOT TAKE THAT MOVE, deliberately, and the reason is the run
//   law rather than taste: promoting the array means editing two other lanes'
//   files, and run mg9 has ten lanes on ten branches merging in id order. A
//   third private copy is inert -- snd_apply keys on a DS address, both earlier
//   rows run first because this row is APPENDED LAST, and a slot already
//   holding a host thunk matches no key -- so this file's middle-table witness
//   reads 0 and the flower's keeps counting exactly what it counted before.
//   THE ARGUMENT IS NOW STRONGER, NOT WEAKER: three copies of eight rows is
//   where a seam stops being an improvement and starts being a defect, and the
//   next lane to seat a fourth (0x169 dScMgCup_c is one) should do it. The
//   measured cost of not doing it is printed by this seat every run, as the
//   middle-table line reading zero.
//
// ---- 4. SLOT 2 IS NOT src's BODY, AND IT IS NOT THIS LANE'S HOST COPY -----
//
// func_ov006_0210a6e4 (AfterInitResources) drops the framework's second
// argument: the ROM never writes r1 before its `bl 0x20b08f0`, so the flags
// ride through in r1, and src spells the call with one argument because that
// is the only way to spell an unnamed value in C. On the host the callee reads
// stack litter, and func_ov004_020b08f0's tail is Scene::AfterInitResources
// (this, flags) where flags == 1 marks the actor for destruction.
//
// port/unmatched/MgFlower_Slot2.cpp already carries the repair as
// port_mg_flower_after_init and its header ends "THE NEXT LANE TO SEAT 361 OR
// 363 SHOULD CALL THIS RATHER THAN WRITE A SECOND". This is 367 and it calls
// it too. src/func_ov006_0210a6e4.cpp stays out of port/slice_box.txt for the
// same reason it is out of port/slice_flw.txt and port/slice_mem.txt: listing
// it would be an LNK2005 against that host copy.
//
// ---- 5. THE WIDTH IS 36, CHECKED FOUR WAYS --------------------------------
//
// port/mg_fanout_costs.txt section 11 measured that twelve of twenty-nine
// widths in its own section 3 table were phantoms, and 0x16f is one of the
// twelve it corrected. All four checks were redone here from the image:
//
//   1. SPAN. config/arm9/overlays/ov006/symbols.txt puts the next symbol after
//      data_ov006_0213f844 at data_ov006_0213f8d4, exactly 0x90 = 36 words on,
//      so a 37th slot cannot exist inside the table. The middle table's next
//      symbol is data_ov006_0213e4d8, also exactly 0x90 on.
//   2. TERMINAL SLOT. Slot 35 of both tables holds 0x020ad660, the word every
//      dScMgBase_c-derived table holds there.
//   3. WHAT THE WORD PAST IT IS, and this one has a named victim. Index 36 of
//      0x0213f844 reads 0x0211f040 and index 37 reads 0x00000000 -- an mwcc
//      {code, 0} MEMBER POINTER, not a vtable slot. It is data_ov006_0213f8d4,
//      and __sinit_ov006_02132f68 copies it into data_ov006_02142ed8 slot 5.
//      THAT TABLE IS dScMgTeresa_c'S (actor id 0x183, scene 387), not this
//      class's: all twenty-seven of its code words lie in 0x0211cd24..
//      0x021200cc, which is 0x183's code block. So a 37-slot fill here writes
//      a host thunk over a state belonging to a DIFFERENT minigame -- section
//      11's hazard exactly, with the neighbour identified rather than guessed.
//   4. RELOCATION COUNT. ov006's relocs.txt has exactly 36 relocations whose
//      `from` lies inside 0x0213f844..0x0213f8d4, one per slot and none past
//      the end. Section 11 lists three checks; this is a fourth and it is the
//      only one of the four that can neither be inflated by unsymbolised
//      padding nor fooled by a word that looks like an address.
//
// port/tools/vtablerows.py --vtable 0x0213f844 --width 36 answers ovr 13,
// mark 11, nosrc 0, which is what section 3's row predicts. ALL ELEVEN MARKERS
// WERE DISASSEMBLED AND RULED REAL_DECOMP BY THIS LANE, with per-body evidence
// and per-body instruction arithmetic in port/tools/inferred_stub_adjudicated
// .txt. The two unmarked overrides are slot 16 (func_ov006_02119904) and slot
// 33 (func_ov006_0210a708).
//
// ---- 6. THE ROW GOES LAST, AND THE ORDER IS LOAD-BEARING ------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors ONCE PER PROCESS from the tail of the first
// minigame row's fill. Appending after every existing row therefore means the
// constructors have already run with clean ROM words when this fill starts,
// which is the latent-safe direction section 11 names -- and section 3 above
// depends on it for a second reason.
//
// ---- 7. WHAT THIS SEAT DOES NOT CLAIM -------------------------------------
//
// The state machine is proven to run by the census this file prints, not by
// this comment. A run that reports slot hits and zero routed dispatches has
// ticked an object without entering its state machine, and that reads as a
// success unless it is printed -- which is why the numbers below are printed
// whether they are zero or not.
//
// AND THIS IS A SOUND GAME, which bounds what a headless run can say. Slot 18
// ends on Sound::LoadAndSetMusic_Layer1(6) and the whole point of the minigame
// is a rhythm the player answers with the stylus. A boot with no input can
// show the machine ticking, the entities advancing and the countdowns
// counting; it cannot show that the right note played at the right frame. The
// census below says which of those it is measuring.
//
// ---- 8. WHAT A TAP DOES, AND THE FLOOR IT LANDS ON ------------------------
//
// Six pads, read off a run rather than off a picture: the ten-record array at
// +0x50e8 holds SIX live records at (64,32) (128,32) (192,32) (64,112)
// (128,112) (192,112), and all six sit in state 0 forever on an unattended
// boot. src/func_ov006_0211bf44.c IS state 0, and it is a STYLUS HIT TEST --
// data_020a0dea/deb against the record's own x/y in a +-0x18 box, and on a hit
// it writes state 1 into the record, zeroes the record's +0x50f0 and +0x50f7,
// increments the per-round tap counter at +0x5624 and plays
// Sound::PlayBank2_2D(0x201).
//
// STATE 1 IS func_ov006_0211bc8c, WHICH IS THIS CLASS'S ONE HARD FLOOR. So the
// class's zero floor asks on an unattended boot is one of the dishonest zeros
// port/mg_fanout_costs.txt section 16 lists, and this seat says so out loud
// rather than banking it: SM64DS_TOUCH_PROBE="100-104:64:32,160-164:128:32"
// moves two records to state 1, drives +0x5624 to its own early-out of 2, and
// turns 0 floor asks into 338 over the same 300 frames.
//
// The census below therefore prints the pad coordinates and the tap counter,
// so the next reader can drive it without re-deriving either.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);

/* the mount storage the fill writes into. All three are the mounts' own, so
   the fill replaces the mounted words rather than building a fresh host array
   -- the ov007 and curling treatment, for the same reason: a second host array
   of the same name is a duplicate symbol, and leaving the mounted table alone
   would leave live wild DS pointers in a table the factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,         36 */
extern unsigned char data_ov006_0213e448[];   /* dScMgSingle3DBase_c, 36 */
extern unsigned char data_ov006_0213f844[];   /* dScMgSound_c,        36 */
extern unsigned char MgBoomBox_SpawnInfo[];

/* dScMgSingle3DBase_c's eight overrides. Slot 2 is NOT src's body: see
   section 4 and port/unmatched/MgFlower_Slot2.cpp. */
int   port_mg_flower_after_init(void *c, unsigned f);   /* slot  2 */
void  func_ov006_0210a608(void *c, unsigned f);         /* slot  5 */
int   func_ov006_0210a698(void *c);                     /* slot  7 */
int   func_ov006_0210a664(void *c);                     /* slot 10 */
int   func_ov006_0210a4b0(char *c);                     /* slot 16 D2 */
int   func_ov006_0210a4e8(char *c);                     /* slot 17 D0 */
int   func_ov006_0210a600(void);                        /* slot 26 */
void  func_ov006_0210a708(char *c);                     /* slot 33 */

/* dScMgSound_c's own seven. func_ov006_0211c720 is the src TU and NOT a host
   copy: slot 6 is a plain switch on the int at +0x5608 with a compiler jump
   table (ROM 0x0211c734 `addls pc,pc,r2,lsl #2`), so the "read the slot-6 src
   before wiring by name" rule came back clean. The two dispatchers it CALLS
   are the host copies. */
int   func_ov006_0211c984(void *self);         /* slot  0 InitResources */
int   func_ov006_0211c720(char *c);            /* slot  6 Behavior      */
int   func_ov006_0211c6c4(char *c);            /* slot  9 Render        */
void *func_ov006_02119904(char *c);            /* slot 16 D2            */
void *func_ov006_02119958(char *c);            /* slot 17 D0            */
void  func_ov006_0211c5d0(void *self, int r1); /* slot 18 state reset   */
void  func_ov006_0211c5b8(char *c);            /* slot 20               */

/* the factory */
void *MgBoomBox_Spawn(void);

/* the dispatch file's witness */
void port_mg_sound_counts(unsigned *hits, unsigned *floor, unsigned *unknown,
                          unsigned *field_calls, unsigned *field_routed,
                          unsigned *field_unknown);
/* the framework's, from unmatched/MgBase_StateDispatch.cpp */
void port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the persistent minigame record and the two framework bodies that address it,
   named so the census can print the record rather than leave it inferred.
   Nothing new is defined here: the storage is hal/level_boot.cpp's
   .dsstate$savblk0004 and the index function is src/func_ov004_020adc3c.c. */
extern void         *data_ov004_020beb68;     /* the live dScMgBase_c `this` */
extern unsigned char data_0209caf4[];         /* 36 records x 20 bytes       */
int  func_ov004_020adc3c(void *c);            /* (self->field_8 >> 8) & 0xff */

/* THE STYLUS RECORD, the four names src/func_ov006_0211bf44.c reads. Four DS
   symbols over one 16-byte block, hal/auto_bss.cpp carries the layout and its
   evidence; nothing is defined here. */
extern unsigned char data_020a0e40;           /* which player's record       */
extern unsigned char data_020a0de8[];         /* held                        */
extern unsigned char data_020a0de9[];         /* the touch-XOR-previous edge */
extern unsigned char data_020a0dea[];         /* x                           */
extern unsigned char data_020a0deb[];         /* y                           */

}  /* extern "C" */

/* The record row this class's object is keyed to, or -1 with no live base.
   func_ov004_020ad878 -- the writer of +0xb4's seed -- goes through it. */
static int port_snd_record_index(void)
{
    return data_ov004_020beb68 ? func_ov004_020adc3c(data_ov004_020beb68) : -1;
}

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot on each of the two tables this file owns
// faces for. The framework slots dScMgBase_c keeps are counted by
// scene_mg.cpp's own array.
static unsigned g_snd_hits[36];
static unsigned g_snd_mid_hits[36];   /* the same slots on the MIDDLE table */

#define SND(n)   (++g_snd_hits[(n)])
#define S3D(n)   (++g_snd_mid_hits[(n)])

/* ---- dScMgSingle3DBase_c's eight ---------------------------------------- */
static void *__fastcall s3_ainit(void *s, void *, unsigned f)
{ S3D(2);  return (void *)(size_t)port_mg_flower_after_init(s, f); }
static void __fastcall s3_aclean(void *s, void *, unsigned f)
{ S3D(5);  func_ov006_0210a608(s, f); }
static int  __fastcall s3_bbeh(void *s, void *)
{ S3D(7);  return func_ov006_0210a698(s); }
static int  __fastcall s3_bren(void *s, void *)
{ S3D(10); return func_ov006_0210a664(s); }
static void *__fastcall s3_d2(void *s, void *)
{ S3D(16); return (void *)(size_t)func_ov006_0210a4b0((char *)s); }
static void *__fastcall s3_d0(void *s, void *)
{ S3D(17); return (void *)(size_t)func_ov006_0210a4e8((char *)s); }
static int  __fastcall s3_v26(void *, void *)
{ S3D(26); return func_ov006_0210a600(); }
static int  __fastcall s3_v33(void *s, void *)
{ S3D(33); func_ov006_0210a708((char *)s); return 0; }

/* ---- dScMgSound_c's own seven ------------------------------------------- */
static int  __fastcall snd_init(void *s, void *)
{ SND(0);  const int r = func_ov006_0211c984(s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall snd_beh(void *s, void *)
{ SND(6);  const int r = func_ov006_0211c720((char *)s); hal_gapless_splice();
  return r; }
static int  __fastcall snd_render(void *s, void *)
{ SND(9);  return func_ov006_0211c6c4((char *)s); }
static void *__fastcall snd_d2(void *s, void *)
{ SND(16); return func_ov006_02119904((char *)s); }
static void *__fastcall snd_d0(void *s, void *)
{ SND(17); return func_ov006_02119958((char *)s); }
/* SLOT 18 TAKES ONE STACK ARGUMENT AND THIS CLASS READS IT, which is the
   difference between this thunk and hal/scene_mg_memory2.cpp's. Run mg5 lane
   BASESET scanned both offsets out of the two overlay images word by word:
   offset 0x48 is 22 sites, offset 0x4c is 14, and the argument count is ONE at
   every one of them, so the parameter has to exist on every slot-18 thunk or
   __fastcall leaves four bytes on the stack. Most seats then DISCARD it
   because their ROM body takes only `this`. Not this one: the ROM at
   0x0211c5e8 is `cmp r1,#0 / bne`, and the two arms differ in whether +0xbc is
   incremented under a 0x270e clamp or zeroed and the save record's +0xb4 reset
   with it. Passing it through is the whole of the difference between "one more
   round" and "start again". hal/scene_mg.cpp's mp_reset is the precedent. */
static int  __fastcall snd_reset(void *s, void *, int flag)
{ SND(18); func_ov006_0211c5d0(s, flag); return 1; }
static int  __fastcall snd_v20(void *s, void *)
{ SND(20); func_ov006_0211c5b8((char *)s); return 0; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics every scene
   seat in this port carries, counted separately so a run can never read a
   no-op as the real body having run. */
static unsigned g_snd_init_skipped, g_snd_render_skipped;
static int __fastcall snd_init_noop(void *, void *)
{ ++g_snd_init_skipped; return 1; }
static int __fastcall snd_render_noop(void *, void *)
{ ++g_snd_render_skipped; return 1; }

struct SndFace { unsigned ds; void *host; };

/* Keyed on the ROM WORD each slot holds, so the array is order-independent and
   cannot land on a slot the ROM did not park that body in. */
static const SndFace kSingle3DFaces[] = {
    {0x0210a6e4u, (void *)s3_ainit},  {0x0210a608u, (void *)s3_aclean},
    {0x0210a698u, (void *)s3_bbeh},   {0x0210a664u, (void *)s3_bren},
    {0x0210a4b0u, (void *)s3_d2},     {0x0210a4e8u, (void *)s3_d0},
    {0x0210a600u, (void *)s3_v26},    {0x0210a708u, (void *)s3_v33},
};

static const SndFace kSoundFaces[] = {
    {0x0211c984u, (void *)snd_init},  {0x0211c720u, (void *)snd_beh},
    {0x0211c6c4u, (void *)snd_render},{0x02119904u, (void *)snd_d2},
    {0x02119958u, (void *)snd_d0},    {0x0211c5d0u, (void *)snd_reset},
    {0x0211c5b8u, (void *)snd_v20},
};

static unsigned snd_apply(void **vt, unsigned n, const SndFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address. A minigame table is 36 slots and every one
   of them is dispatched by something, so a nonzero answer is a wild call
   waiting to happen and the seat says so out loud. */
static unsigned snd_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

/* How many slots each array claimed, so the middle-table sharing argument in
   section 3 is a MEASUREMENT in every run report rather than a paragraph. */
static unsigned g_snd_mid_claimed, g_snd_own_claimed;

extern "C" void port_scene_boombox_hits(void);

extern "C" void port_scene_fill_boombox(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **mid  = (void **)data_ov006_0213e448;
    void **vt   = (void **)data_ov006_0213f844;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY. Earlier rows'
       fills already did it and run first, so on this tree this is a second
       pass over words that are already host pointers and finds nothing. It is
       here so this class does not depend on another class's row existing: the
       factory's first act is func_ov004_020b2adc, which writes
       data_ov004_020bc0c0 into the object's first word before either derived
       table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE MIDDLE TABLE. Live storage in its own right -- the destructor chain
       writes it into the object on the way down, so a slot dispatched between
       the two vptr writes reads it. Section 3 is why applying this file's own
       arrays here is safe next to the flower's and memory2's. */
    port_scene_mg_fill_shared(mid, 36);
    g_snd_mid_claimed  = snd_apply(mid, 36, kSingle3DFaces,
                                   sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    g_snd_mid_claimed += snd_apply(mid, 36, kSoundFaces,
                                   sizeof kSoundFaces / sizeof kSoundFaces[0]);

    /* THE DERIVED TABLE. Shared arm9 and dScMgBase_c words first, then the
       intermediate base's eight, then this class's own seven. Order does not
       matter -- the three key sets are disjoint, because a word is one address
       -- but it reads in inheritance order. */
    port_scene_mg_fill_shared(vt, 36);
    g_snd_own_claimed  = snd_apply(vt, 36, kSingle3DFaces,
                                   sizeof kSingle3DFaces / sizeof kSingle3DFaces[0]);
    g_snd_own_claimed += snd_apply(vt, 36, kSoundFaces,
                                   sizeof kSoundFaces / sizeof kSoundFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)snd_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)snd_render_noop;
    }

    {
        const unsigned lb = snd_raw_left(base, 36);
        const unsigned lm = snd_raw_left(mid, 36);
        const unsigned lv = snd_raw_left(vt, 36);
        if (lb || lm || lv) {
            std::fprintf(stderr, "  [scene] BOOMBOX FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgSingle3DBase_c %u, dScMgSound_c %u. A dispatch "
                         "of any of them jumps to a DS address as a host one.\n",
                         lb, lm, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as every other
       minigame fill does it. Idempotent behind scene_mg.cpp's own
       once-per-process guard. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 367) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_boombox_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory; the forwarder is kept for the reason title_spawn and
   port_mg_curling_spawn are, so the seat has one place to observe the object
   without the registry table growing a second column.

   THE FACTORY NEEDS NO DISPLACEMENT RULING. src/MgBoomBox_Spawn.cpp calls
   func_ov004_020b2adc(p) WITH its argument -- ROM 0x0211cb84 `bl 0x20b2adc`
   with r0 carrying the allocation from 0x0211cb78 -- where src/
   func_ov006_020e0574.cpp (0x169's factory) calls the same base constructor
   with none and rides r0 through. That callee dereferences on its first
   statement and then writes vtable words through the pointer, so the
   difference is a wild write versus a correct one. This class is on the
   correct side of it and is linked from the slice rather than host-copied. */
static char *g_snd_self;

extern "C" void *port_mg_boombox_spawn(void)
{
    void *p = MgBoomBox_Spawn();
    g_snd_self = (char *)p;
    return p;
}

extern "C" void port_scene_boombox_hits(void)
{
    unsigned total = 0, mtotal = 0;
    for (int i = 0; i < 36; ++i) { total += g_snd_hits[i]; mtotal += g_snd_mid_hits[i]; }

    std::printf("[scene] dScMgSound_c slot hits: init %u, behavior %u, "
                "render %u, D2 %u, D0 %u, state-reset %u, slot20 %u%s\n",
                g_snd_hits[0], g_snd_hits[6], g_snd_hits[9], g_snd_hits[16],
                g_snd_hits[17], g_snd_hits[18], g_snd_hits[20],
                g_snd_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_snd_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_snd_init_skipped);

    std::printf("[scene] dScMgSound_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_snd_hits[i]) std::printf(" %d(x%u)", i, g_snd_hits[i]);
    std::printf("   (%u total)\n", total);

    /* THE MIDDLE BASE'S BODIES, COUNTED WHERE THIS SEAT OWNS THEM. Read this
       line as "entries into a dScMgSingle3DBase_c BODY through a thunk this
       file installed", not as "entries into the middle TABLE": the two are
       different and the claimed counts below say which is which.

       kSingle3DFaces is applied to BOTH tables and lands in only one of them.
       This class's registry row is appended after the flower's and memory2's,
       so whichever of those fills first claims data_ov006_0213e448 and this
       copy finds no DS word left there -- the middle-table claim reads 0 by
       design, and a NONZERO one would mean the row order changed, which is the
       fact section 3's sharing argument rests on. The DERIVED table is a
       different matter: six of its thirteen overrides ARE the middle base's
       bodies and nobody else fills that table, so this seat's own claim is
       13 = 6 inherited + 7 of its own, and the hits below are those six. */
    std::printf("[scene] dScMgSingle3DBase_c bodies entered (this seat's thunks):");
    for (int i = 0; i < 36; ++i)
        if (g_snd_mid_hits[i]) std::printf(" %d(x%u)", i, g_snd_mid_hits[i]);
    std::printf("   (%u total; this fill claimed %u middle-table slot(s) and "
                "%u of its own)\n", mtotal, g_snd_mid_claimed, g_snd_own_claimed);

    /* THE STATE MACHINE'S OWN WITNESS. Printed whether or not it fired: a run
       with slot hits and zero routed dispatches has ticked the object without
       entering its state machine, and that reads as a success unless it is
       said out loud. */
    {
        unsigned hits = 0, floor = 0, unknown = 0, fc = 0, fr = 0, fu = 0;
        unsigned calls = 0, fwunk = 0;
        port_mg_sound_counts(&hits, &floor, &unknown, &fc, &fr, &fu);
        port_mg_dispatch_counts(&calls, &fwunk);
        std::printf("[scene] dScMgSound_c state dispatch: %u routed to one of "
                    "the class's 56 reachable table states across 15 tables, "
                    "%u ask(s) for the floor 0x0211bc8c, %u UNHANDLED by this "
                    "class's switch; sub-object FIELD dispatch %u call(s), %u "
                    "routed to one of its six states, %u UNHANDLED; %u "
                    "framework call(s), %u UNHANDLED address(es)\n",
                    hits, floor, unknown, fc, fr, fu, calls, fwunk);
    }

    /* THE OBJECT, AND THE FIELDS THE ROM'S OWN SLOT 6 READS. Every offset here
       is out of the disassembly of func_ov006_0211c720 (the Behavior's plain
       switch) and func_ov006_0211c5d0 (slot 18), not out of a struct:
         +0x5608  the top-level state the switch reads (0 init, 1 intro,
                  2 result, 3 retry)
         +0x5618  the intro countdown, a u16 slot 0 and slot 18 both seed 0x20
         +0x5616  the result/retry countdown, seeded 0x20 on the retry arm
         +0x5626  the result byte: nonzero takes the finish arm, 3 also calls
                  func_ov004_020b67f8
         +0x5627  the index into the two spacing tables 0x0211ba88 reads
         +0xb4    the persistent level, clamped at 9999 on a win
         +0xb8    its high-water mark */
    if (g_snd_self)
        std::printf("[scene] dScMgSound_c object at %p, state +0x5608 = %d, "
                    "intro +0x5618 = %u, result +0x5616 = %u, result byte "
                    "+0x5626 = %u, spacing index +0x5627 = %u\n",
                    (void *)g_snd_self,
                    *(int *)(g_snd_self + 0x5608),
                    *(unsigned short *)(g_snd_self + 0x5618),
                    *(unsigned short *)(g_snd_self + 0x5616),
                    *(unsigned char *)(g_snd_self + 0x5626),
                    *(unsigned char *)(g_snd_self + 0x5627));

    /* THE TWO ENTITY ARRAYS, because a run that dispatches nothing and a run
       with nothing to dispatch read the same on every other line. Both loops
       and both offsets come from the two open-coded dispatchers:
         func_ov006_0211b954  TEN records, stride 0x14, base +0x50e8:
                              +0x00 a 20.12 position it walks down by 16.0 a
                              tick, +0x0c the state index, +0x0d the in-play
                              gate it tests before dispatching at all
         func_ov006_0211b5e0  THIRTY records, stride 0x24, base +0x51cc:
                              +0x00 the in-play gate, +0x04 the level-1 state
                              index, +0x05 the level-2 index every one of the
                              thirteen sub-dispatchers reads */
    if (g_snd_self) {
        int live10 = 0;
        /* x and y BOTH, and which word is which comes off the state-0 body
           rather than off a guess: src/func_ov006_0211bf44.c compares
           data_020a0dea[player*4] against (+0xe8 >> 12) and
           data_020a0deb[player*4] against (+0xec >> 12), so +0x50e8 is the
           stylus X and +0x50ec is the stylus Y, in 20.12. A lane that wants to
           TAP a pad reads its coordinates off this line. */
        std::printf("[scene] dScMgSound_c 10-array (idx:state@x,y):");
        for (int i = 0; i < 10; ++i) {
            const char *r = g_snd_self + 0x50e8 + i * 0x14;
            if (*(const unsigned char *)(r + 0x0d) == 0) continue;
            ++live10;
            std::printf(" %d:%u@%d,%d", i, *(const unsigned char *)(r + 0x0c),
                        *(const int *)(r + 0x00) >> 12,
                        *(const int *)(r + 0x04) >> 12);
        }
        std::printf("   (%d of 10 in play)\n", live10);
        /* THE HIT-TEST GATE, printed because a run that never leaves state 0
           and a run with no pads to hit read the same on every other line.
           +0x5624 is the per-round tap counter state 0 increments and its own
           early-out at 2; the three bytes after it are the stylus record the
           hit test reads. */
        std::printf("[scene] dScMgSound_c touch: taps this round +0x5624 = %u "
                    "(state 0 stops answering at 2), stylus record "
                    "de8=%u de9=%u dea=%u deb=%u\n",
                    *(const unsigned char *)(g_snd_self + 0x5624),
                    data_020a0de8[data_020a0e40 * 4],
                    data_020a0de9[data_020a0e40 * 4],
                    data_020a0dea[data_020a0e40 * 4],
                    data_020a0deb[data_020a0e40 * 4]);

        int live30 = 0;
        std::printf("[scene] dScMgSound_c 30-array (idx:lvl1.lvl2):");
        for (int i = 0; i < 30; ++i) {
            const char *r = g_snd_self + 0x51cc + i * 0x24;
            if (*(const unsigned char *)(r + 0x00) == 0) continue;
            ++live30;
            std::printf(" %d:%u.%u", i, *(const unsigned char *)(r + 0x04),
                        *(const unsigned char *)(r + 0x05));
        }
        std::printf("   (%d of 30 in play)\n", live30);
    }

    /* THE PERSISTENT RECORD. +0xb4 is seeded by InitResources from
       data_0209caf4[minigame index][1] and written back by slot 6's win arm
       and slot 18's reset arm, so this line is what says which of them moved.
       Scene 367 resolves to ov005 row 19, param 0x00070700 = record 7, name
       text 7, slot-35 low byte 0. */
    if (g_snd_self) {
        const int idx = port_snd_record_index();
        std::printf("[scene] dScMgSound_c level: +0xb4 = %d, +0xb8 best = %d, "
                    "+0xbc = %d\n",
                    *(const int *)(g_snd_self + 0xb4),
                    *(const int *)(g_snd_self + 0xb8),
                    *(const int *)(g_snd_self + 0xbc));
        if (idx >= 0 && idx < 36) {
            const int *rec = (const int *)(data_0209caf4 + idx * 20);
            std::printf("[scene] dScMgSound_c save record data_0209caf4[%d] "
                        "= { %d, %d, %d, %d, %d }\n", idx,
                        rec[0], rec[1], rec[2], rec[3], rec[4]);
        } else {
            std::printf("[scene] dScMgSound_c save record: NO minigame index "
                        "(data_ov004_020beb68 = %p)\n", data_ov004_020beb68);
        }
    }

    /* WHAT A HEADLESS RUN CANNOT SAY, printed rather than left to a document,
       because this is the line that stops a green census being read as a
       finished minigame. */
    std::printf("[scene] dScMgSound_c NOT MEASURED HERE: this is a rhythm game "
                "-- slot 18 ends on Sound::LoadAndSetMusic_Layer1(6) and the "
                "round is answered with the stylus. An unattended boot can "
                "show the machine ticking; it cannot show a note landing on "
                "the right frame, and nobody has held this next to a DS.\n");
    std::fflush(stdout);
}
