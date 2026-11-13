// COOP BOSSES. Run mp1, lane BOSSCORE1. See coop_boss.h for the design.
//
// PORT_HOST_ABI. Reads and writes live boss actors by raw offset (the established convention in port/hal); nothing in
// src/ changes. Every received value is range-checked before it is written.

#include "coop_boss.h"
#include "coop_target.h"
#include "coop_world.h"
#include "coop_spawn.h"
#include "coop_groups.h"
#include "coop_wire.h"
#include "coop_actors.h"
#include "comms_hub.h"

#include "dActor_c.h"
#include "Player.h"
#include "Message.h"
#include "daFPkn_c.h"

#include <windows.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
extern void *data_0209f394[];
extern unsigned char data_0209f250;
extern short data_02092144[];
extern dActor_c *STAR_MARKERS[12];      // 0x0209f40c: the actors showing a star glint on the minimap
void *_ZN8dActor_c4NextEPKS_(const void *prev);
int _ZN12daBombking_c19KingBobOmb_SetStateEPv(void *c, void *state);
void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned int);
extern void *data_0209f318;             // bowser2: the dCamera_c (+0x154 flags; bit 0x8 = the boss intro holds the camera)
int _ZN6Player7TryGrabER8dActor_c(void *self, void *actor);
int _ZN6Player9DropActorEv(void *self);
int func_ov002_020dacb4(char *p, char *actor);   // Player: take hold of an actor (the body of TryGrab once its checks pass)
// bossdesc1: Chief Chilly's state records (16 x 0x10 at ov073 0x02123320), his ROM transition, Wiggler's ROM transition, ModelAnim::SetAnim
int ChiefChilly_ChangeState(void *c, void *state);
void _ZN12daHanachan_c8SetStateEi(void *c, int state);
int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *anim, void *file, int a, int speed, unsigned int b);
extern char data_ov073_02123320[], data_ov073_02123330[], data_ov073_02123340[], data_ov073_02123350[], data_ov073_02123360[],
            data_ov073_02123370[], data_ov073_02123380[], data_ov073_02123390[], data_ov073_021233a0[], data_ov073_021233b0[],
            data_ov073_021233c0[], data_ov073_021233d0[], data_ov073_021233e0[], data_ov073_021233f0[], data_ov073_02123400[],
            data_ov073_02123410[];
// kbo2: the same sixteen records as src's C++ State objects (d_a_king_donketu.cpp), reached by their mangled names.
extern "C" char kbo2_cc_3320[], kbo2_cc_3330[], kbo2_cc_3340[], kbo2_cc_3350[], kbo2_cc_3360[], kbo2_cc_3370[], kbo2_cc_3380[], kbo2_cc_3390[], kbo2_cc_33a0[], kbo2_cc_33b0[], kbo2_cc_33c0[], kbo2_cc_33d0[], kbo2_cc_33e0[], kbo2_cc_33f0[], kbo2_cc_3400[], kbo2_cc_3410[];
#pragma comment(linker, "/alternatename:_kbo2_cc_3320=?data_ov073_02123320@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_3330=?data_ov073_02123330@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_3340=?data_ov073_02123340@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_3350=?data_ov073_02123350@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_3360=?data_ov073_02123360@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_3370=?data_ov073_02123370@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_3380=?data_ov073_02123380@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_3390=?data_ov073_02123390@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_33a0=?data_ov073_021233a0@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_33b0=?data_ov073_021233b0@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_33c0=?data_ov073_021233c0@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_33d0=?data_ov073_021233d0@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_33e0=?data_ov073_021233e0@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_33f0=?data_ov073_021233f0@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_3400=?data_ov073_02123400@@3UState@daKing_Donketu_c@@A")
#pragma comment(linker, "/alternatename:_kbo2_cc_3410=?data_ov073_02123410@@3UState@daKing_Donketu_c@@A")
// bossdesc2: Goomboss's ROM transition (state index at +0x5cc, runs the enter pair), its hit animation, Eyerok's transition + records + globals
void _ZN7fBase_c18MarkForDestructionEv(void *self);
// coopgoomboss2: a minion's home point (what the summon calls after the Spawn) and the Goomba's own removal at the end of its death
void func_ov084_021296b0(void *goomba, void *pos);
void func_ov084_02129498(void *goomba);
extern unsigned char data_ov074_02122d80[];     // Goomboss's set size by size index (the summon's spawnTarget)
void _ZN12daKuriKing_c19func_ov074_021203e4Ei(char *c, int state);
void func_ov074_02121a4c(char *c, int idx);
int _ZN6Eyerok19func_ov066_02119454EPv(void *c, void *state);
extern char data_ov066_0211afcc[], data_ov066_0211afdc[], data_ov066_0211afec[], data_ov066_0211affc[], data_ov066_0211b00c[],
            data_ov066_0211b01c[], data_ov066_0211b02c[], data_ov066_0211b03c[], data_ov066_0211b04c[], data_ov066_0211b05c[],
            data_ov066_0211b06c[], data_ov066_0211b07c[], data_ov066_0211b08c[], data_ov066_0211b09c[], data_ov066_0211b0ac[],
            data_ov066_0211b0bc[], data_ov066_0211b0cc[], data_ov066_0211b0dc[], data_ov066_0211b0ec[];
extern void *data_ov066_0211aea4[], *data_ov066_0211ae8c[];
extern unsigned char data_ov066_0211abe0, data_ov066_0211ae08;
extern int data_ov066_0211abe4;
extern char data_ov078_02126ffc[], data_ov078_0212700c[], data_ov078_0212701c[], data_ov078_0212702c[], data_ov078_0212703c[],
            data_ov078_0212704c[], data_ov078_0212705c[], data_ov078_0212706c[], data_ov078_0212707c[], data_ov078_0212708c[],
            data_ov078_0212709c[], data_ov078_021270ac[], data_ov078_021270bc[], data_ov078_021270cc[], data_ov078_021270dc[],
            data_ov078_021270ec[], data_ov078_021270fc[], data_ov078_0212710c[];
}

// COOPBOO5: daTrs_c::func_ov063_02116244 (src/game/actors/d_a_trs.cpp): with the count at 5 or more and nothing spawned yet, a Big
// Boo spawns his icon, the actor his star comes from. Declared bare, as in coop_world.cpp: only the member's name is needed.
class daTrs_c { public: void func_ov063_02116244(); };
extern "C" void coop_camshield_mark(void *a, const char *why);   // hal/coop_camshield.cpp (CANNON2): a friend's star never films here

namespace port {
namespace {
// kbo2: SM64DS_COOP_KBO_FIX=0 is the before control (1.0.3 King Bob-omb: target-seat authority to the end, the defeat talk skipped
// on every console but the authority's, bombs left in a copy's hand, a copy's own HP drop counted when its player is near; Chief Chilly
// state records matched by address only).
bool g_kbo_fix = true;

enum : int { kSeats = 4, kSlots = 512, kPayloadMax = kHubCoopMaxDatagram - kHubCoopHeaderBytes };

// ---------------------------------------------------------------------------
// The boss rows. A field is {offset, size, kind}; the kinds say how a value crosses the wire.
// ---------------------------------------------------------------------------
enum : uint16_t { kPlain = 0, kStatePtr = 1, kVol = 2,   // kVol: changes every frame, never a reason to publish by itself
                 // bossdesc1 kinds:
                 kChillyPtr = 4,    // Chief Chilly's state record pointer (+0x37c), mapped to a table id; set through ChiefChilly_ChangeState
                 kAlive = 8,        // a read-only "HP" for a boss without one: 1 while the int state at the field is not 4 (dead) or 5 (gone)
                 kMaxUp = 16,       // a counter that only goes up (0xff = "not used" is never touched)
                 kBullyState = 32,  // Big Bully's int state (+0x398): written with the ROM's animation for that state
                 kWigState = 64,    // Wiggler's state index (+0x8c4): set through _ZN12daHanachan_c8SetStateEi
                 // bossdesc2 kinds:
                 kS8 = 128,         // a signed byte (sign-extended on the wire, masked on a write): Eyerok's hand HP, Bowser's HP
                 kGlobal = 256,     // not a field of the actor: off = index of an overlay global (Eyerok: 0 = phase bits, 1 = hands down)
                 kZeroOnForce = 512,// a step counter: a forced state restarts at step 0, so the state's own set-up runs
                 kGoomState = 1024, // Goomboss's state index (+0x5cc): set through _ZN12daKuriKing_c19func_ov074_021203e4Ei (enter pair runs)
                 kEyeState = 2048,  // Eyerok's state record pointer (+0x48c), mapped to a table id; set through _ZN6Eyerok19func_ov066_02119454EPv
                 // coopgoomboss2:
                 kLocal = 4096 };   // sent for the log only, never written: each console keeps its own (Goomboss's summoned count +0x602)
enum : int { kLoOff = 0x100, kHiOff = 0x900 };
struct BField { uint16_t off; uint8_t size; uint16_t kind; };
struct BDesc {
    uint16_t cls;
    const char *name;
    uint8_t nf;
    BField f[kBossMaxFields];
    int8_t hp;        // index of the HP field (min wins), -1 none
    int8_t state;     // index of the state field, -1 none
    int8_t pos;       // index of the first of three position fields (x, y, z), -1 none
    uint16_t pl[2];   // offsets of player pointers the class's states dereference (a copy that is put into such a state must hold one)
    uint16_t hit_who; // 0, or the offset where the ROM keeps the pointer of the player that hit (the proof knob must set it too)
    uint16_t hit_off; // 0, or the offset of a byte the class's own handler turns into one HP (the proof knob sets it)
};

// Whomp King (daBtn_c, shared Whomp class, king flag +0x414): include/Whomp.h, src/func_ov079_02124b08.c / 02124ed4.c:
// mState +0x3b0, HP +0x401 (3 for the king), the hit flag +0x402, the sub-state +0x40c, the flat-on-the-ground timer
// +0x3fc (u16), the state frame counter +0x100 (u16), +0x407, and the animation cursor in the ModelAnim's Animation.
const BDesc kDescs[] = {
    { 165, "WHOMP_KING", 14,
      { { 0x3b0, 4, kPlain }, { 0x401, 1, kPlain }, { 0x40c, 1, kPlain }, { 0x407, 1, kPlain }, { 0x3fc, 2, kVol },
        { 0x100, 2, kVol }, { 0x324, 4, kVol }, { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol },
        { 0x98, 4, kVol }, { 0x9c, 4, kVol }, { 0xa0, 4, kVol }, { 0xa8, 4, kVol } },
      1, 0, 7, { 0x3ac, 0 }, 0x3ac, 0x402 },
    // King Bob-omb (daBombking_c): state record pointer +0x420 (mapped to a table id), HP mHealth +0x500 (3), mPhase +0x4a0,
    // the two timers +0x504 / +0x505, mAnimSpeed +0x4fc, +0x499.
    { 189, "KING_BOB_OMB", 14,
      { { 0x420, 4, kStatePtr }, { 0x500, 4, kPlain }, { 0x4a0, 4, kPlain }, { 0x504, 1, kVol }, { 0x505, 1, kVol },
        { 0x4fc, 4, kPlain }, { 0x499, 1, kPlain }, { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol },
        { 0x98, 4, kVol }, { 0x9c, 4, kVol }, { 0xa0, 4, kVol }, { 0xa8, 4, kVol } },
      1, 0, 7, { 0x430, 0 }, 0, 0 },
    // ---- bossdesc1 (run mp1, lane BOSSDESC1). Offsets from include/ChiefChilly.h, daOts_c.h / BigBully.h, daTrs_c.h, Wiggler.h and
    // the state functions read for each (see out/BOSSDESC1/notes). Console-local pointers are never fields: they sit in pl[].
    // Chief Chilly (class 218, daKing_Donketu_c): state record pointer +0x37c, knock-downs left mHitsRemaining +0x4cb (3 -> 0, taken
    // when he slides off the edge: func_ov073_02120c7c), sub-state +0x4c8, flags +0x4ca, waypoint cursor +0x4c4, turn target +0x4c6,
    // phase counter +0x4b4. +0x3e4 is the player he talks to (console-local). His reward is a KEY (class 282), spawned by each console.
    { 218, "CHIEF_CHILLY", 14,
      { { 0x37c, 4, kChillyPtr }, { 0x4cb, 1, kPlain }, { 0x4c8, 1, kPlain }, { 0x4ca, 1, kPlain }, { 0x4c4, 1, kPlain },
        { 0x4c6, 2, kVol }, { 0x4b4, 4, kVol }, { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol },
        { 0x98, 4, kVol }, { 0x9c, 4, kVol }, { 0xa0, 4, kVol }, { 0xa8, 4, kVol } },
      1, 0, 7, { 0x3e4, 0 }, 0, 0 },
    // Big Bully (class 216, daBDonketu_c on daOts_c): no HP. He is killed by the lava (state 4) and the star is spawned from there.
    // field 0 = the int state +0x398, field 1 = "alive" derived from it, then the bullies-killed counter +0x3fe (the three small
    // bullies he spawns die on each console, or by the death list: only goes up), the bounce count +0x3f9, and the death point
    // +0x3a8 / +0x3ac / +0x3b0 (the particles of the dying state read it).
    { 216, "BIG_BULLY", 14,
      { { 0x398, 4, kBullyState }, { 0x398, 4, kAlive }, { 0x3fe, 1, kMaxUp }, { 0x3f9, 1, kVol }, { 0x3a8, 4, kPlain },
        { 0x3ac, 4, kPlain }, { 0x3b0, 4, kPlain }, { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol },
        { 0x98, 4, kVol }, { 0x9c, 4, kVol }, { 0xa0, 4, kVol }, { 0xa8, 4, kVol } },
      1, 0, 7, { 0, 0 }, 0, 0 },
    // Big Boo (class 210, daTrs_c, types 12..15): state byte +0x5cc, hits left +0x5ca (3 -> 0, taken in state 3 when the timer is 0),
    // sub-state +0x5ce, +0x5c9, timer +0x5c2, +0x5d2, scale +0x584. +0x488 / +0x484 are the players he talks to (console-local).
    // NOT synced on purpose: +0x5c8 (opacity) and the flags word +0x5d4 (camera / area visibility): those key on the LOCAL camera.
    { 210, "BIG_BOO", 14,
      { { 0x5cc, 1, kPlain }, { 0x5ca, 1, kPlain }, { 0x5ce, 1, kPlain }, { 0x5c9, 1, kPlain }, { 0x5c2, 2, kVol },
        { 0x5d2, 1, kPlain }, { 0x584, 4, kPlain }, { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol },
        { 0x98, 4, kVol }, { 0x9c, 4, kVol }, { 0xa0, 4, kVol }, { 0xa8, 4, kVol } },
      1, 0, 7, { 0x488, 0x484 }, 0, 0 },
    // Wiggler (class 248): state index +0x8c4 (set through func_ov034_021125b8, whose enter half takes a health point in state 3),
    // hits left +0x8db (4 -> 1, taken by DAMAGE's enter code; state 7 DEAD ends it), angry +0x8de, falling +0x8df, dying +0x8e0, timer +0x8da, turn target
    // +0x8d8. +0x8c8 is the player he talks to; +0x8e1 (drive the camera) stays local. The body chain is rebuilt from the head.
    { 248, "WIGGLER", 14,
      { { 0x8c4, 4, kWigState }, { 0x8db, 1, kPlain }, { 0x8de, 1, kPlain }, { 0x8df, 1, kPlain }, { 0x8e0, 1, kPlain },
        { 0x8da, 1, kVol }, { 0x8d8, 2, kVol }, { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol },
        { 0x98, 4, kVol }, { 0x9c, 4, kVol }, { 0xa0, 4, kVol }, { 0xa8, 4, kVol } },
      1, 0, 7, { 0x8c8, 0 }, 0, 0 },
    // ---- bossdesc2 (run mp1, lane BOSSDESC2). Append-only rows. Offsets from include/Goomboss.h, Bowser.h, Eyerok.h and the state
    // code read for each (src/actors/Goomboss.cpp, src/func_ov060_*, src/actors/Eyerok.cpp + src/func_ov066_0211603c.c).
    // Goomboss (class 198, daKuriKing_c): state index +0x5cc (9 states, enter/tick pairs; set through func_ov074_021203e4), HP = the size
    // index +0x604 (each stomp's state 5 enter takes one, 0 = the defeat talk), sub-step +0x603, goombas out +0x602 (coopgoomboss2: the
    // goombas are ONE set, summoned by the authority and announced; the count is each console's own, raised by each copy built and
    // lowered by each copy's CleanupResources, so it is sent for the log only and never written), shrink counter +0x607, timer +0x5fc, direction +0x5f0, walk angle / speed
    // +0x5f4 / +0x5f6, scale +0x5e4. +0x5c8 is the player he talks to (console-local), +0x5dc a spawned actor's unique id (never sent).
    // His reward is a key (actor 0x11a): each console spawns its own in state 7 (COOPSPAWN1 announces keys only in courses 36/38/40/49).
    { 198, "GOOMBOSS", 14,
      { { 0x5cc, 4, kGoomState }, { 0x604, 1, kPlain }, { 0x603, 1, kPlain }, { 0x602, 1, kLocal | kVol }, { 0x607, 1, kPlain },
        { 0x5fc, 2, kVol }, { 0x5f0, 4, kPlain }, { 0x5f4, 2, kVol }, { 0x5f6, 2, kVol }, { 0x5e4, 4, kVol },
        { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol }, { 0x98, 4, kVol } },
      1, 0, 10, { 0x5c8, 0 }, 0, 0 },
    // Bowser (class 279, daKpa_c, all three arenas): main state +0x40c, HP = signed byte +0x41e (from a table by variant +0x414, a bomb
    // hit takes one and moves him to state 0xc or, at 0, to 4 = defeat), tail-grab sub-mode +0x410 (0 free, 1 swung by a player, 2
    // thrown, 3 landing), step inside a state +0x423 (restarts at 0 when a state is forced: the state sets its own animation there), +0x422,
    // timers +0x420 / +0x3fc. +0x3a4 is the player holding his tail, +0x3a8 / +0x3ac are unique ids (console-local, never sent).
    { 279, "BOWSER", 14,
      { { 0x40c, 4, kPlain }, { 0x41e, 1, kS8 }, { 0x410, 4, kPlain }, { 0x423, 1, kZeroOnForce | kVol }, { 0x422, 1, kPlain },
        { 0x420, 2, kVol }, { 0x3fc, 2, kVol }, { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol },
        { 0x98, 4, kVol }, { 0x9c, 4, kVol }, { 0xa0, 4, kVol }, { 0xa8, 4, kVol } },
      1, 0, 7, { 0, 0 }, 0, 0 },
    // Eyerok (class 176, daIwante_c): THREE actors of this class: part 0 (the head, a death-table slot) and the two hands (mPartIdx
    // +0x49c = 1 / 2, runtime spawns with slot -1: scan() gives them the record slots 510 / 511). State record pointer +0x48c (one of
    // 19 records), hand HP = signed byte +0x4d8 (3, a punch takes one; at 0 the ROM sets the overlay globals and sends the hand to
    // record 0211b07c), part 0's intro / talk steps +0x4a0 / +0x498 / +0x494, timers +0x4d0 / +0x4d2. The overlay globals are carried in
    // every record: field 10 = the hands-alive bits (a hand's death clears its bit: merged by AND), field 11 = hands down so far (merged by max).
    // +0x490 is the player the head talks to (console-local).
    { 176, "EYEROK", 14,
      { { 0x48c, 4, kEyeState }, { 0x4d8, 1, kS8 }, { 0x4a0, 4, kPlain }, { 0x498, 4, kPlain }, { 0x494, 4, kPlain },
        { 0x4d0, 2, kVol }, { 0x4d2, 2, kVol }, { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol },
        { 0, 1, kGlobal }, { 1, 1, kGlobal }, { 0x98, 4, kVol }, { 0xa8, 4, kVol } },
      1, 0, 7, { 0x490, 0 }, 0, 0 },
    // coopbully1: the small Bully (class 215, daDonketu_c on daOts_c): the Big Bully's row without his killed counter (+0x3fc..+0x3ff is
    // this class's Big Bully unique id, console-local). Placed ones, and the Big Bully's three runtime spawns on record slots 505..507.
    { 215, "BULLY", 13,
      { { 0x398, 4, kBullyState }, { 0x398, 4, kAlive }, { 0x3f9, 1, kVol }, { 0x3a8, 4, kPlain }, { 0x3ac, 4, kPlain },
        { 0x3b0, 4, kPlain }, { 0x5c, 4, kVol }, { 0x60, 4, kVol }, { 0x64, 4, kVol }, { 0x98, 4, kVol }, { 0x9c, 4, kVol },
        { 0xa0, 4, kVol }, { 0xa8, 4, kVol } },
      1, 0, 6, { 0, 0 }, 0, 0 },
};
const int kNumDescs = (int)(sizeof kDescs / sizeof kDescs[0]);

const BDesc *desc_of(unsigned cls) {
    for (int i = 0; i < kNumDescs; ++i) if (kDescs[i].cls == cls) return &kDescs[i];
    return nullptr;
}

// bossfix1 (item 11): the allocation size of each boss class (the literal the ROM passes to operator new; the same numbers as
// port/hal/coop_actor_sizes.inc in COOPMOVE1_FIX, port/l7-coopmove1 bbbaccb252). A descriptor field past its class would read or write
// another object's memory, and the generic bound (kHiOff) is above every boss class, so each class gets its own.
int class_size(unsigned cls) {
    switch (cls) {
    case 165: return 0x610; case 189: return 0x50c; case 176: return 0x874; case 198: return 0x610; case 210: return 0x5e0;
    case 248: return 0x8e8; case 279: return 0x454; case 216: return 0x400; case 218: return 0x504; case 215: return 0x400;
    default: return 0;
    }
}
bool field_in_class(unsigned cls, const BField &f) {
    if (f.kind & kGlobal) return true;
    const int sz = class_size(cls);
    return sz > 0 && (int)f.off + f.size <= sz;
}
bool g_desc_checked = false;
void check_descriptors() {
    if (g_desc_checked) return;
    g_desc_checked = true;
    int bad = 0;
    for (int i = 0; i < kNumDescs; ++i) {
        const BDesc &d = kDescs[i];
        for (int k = 0; k < d.nf; ++k)
            if (!field_in_class(d.cls, d.f[k])) {
                ++bad;
                std::fprintf(stderr, "[cbs] DESCRIPTOR %s field %d at 0x%x+%d is past the class size 0x%x: refused\n", d.name, k, (unsigned)d.f[k].off, (int)d.f[k].size, class_size(d.cls));
            }
        for (int k = 0; k < 2; ++k)
            if (d.pl[k] && (int)d.pl[k] + 4 > class_size(d.cls)) { ++bad; std::fprintf(stderr, "[cbs] DESCRIPTOR %s pointer field 0x%x is past the class size\n", d.name, (unsigned)d.pl[k]); }
        if (d.hit_who && (int)d.hit_who + 4 > class_size(d.cls)) { ++bad; std::fprintf(stderr, "[cbs] DESCRIPTOR %s hit pointer 0x%x is past the class size\n", d.name, (unsigned)d.hit_who); }
        if (d.hit_off && (int)d.hit_off + 1 > class_size(d.cls)) { ++bad; std::fprintf(stderr, "[cbs] DESCRIPTOR %s hit byte 0x%x is past the class size\n", d.name, (unsigned)d.hit_off); }
    }
    std::fprintf(stderr, "[cbs] descriptor class-size check: %d descriptors, %d problems\n", kNumDescs, bad);
}

bool field_ok(const BField &f) {
    if (f.kind & kGlobal) return f.off < 2 && f.size == 1;
    if (f.size != 1 && f.size != 2 && f.size != 4) return false;
    const bool low_ok = f.off == 0x5c || f.off == 0x60 || f.off == 0x64 || f.off == 0x98 || f.off == 0x9c || f.off == 0xa0 || f.off == 0xa8;
    if (!low_ok && (f.off < kLoOff || (int)f.off + f.size > kHiOff)) return false;
    return (f.off % f.size) == 0;
}

// King Bob-omb's state records, by ROM address (the host's symbols are the same records the ROM code names).
char *const *kbo_states(int *n) {
    static char *const k[] = {
        data_ov078_02126ffc, data_ov078_0212700c, data_ov078_0212701c, data_ov078_0212702c, data_ov078_0212703c, data_ov078_0212704c,
        data_ov078_0212705c, data_ov078_0212706c, data_ov078_0212707c, data_ov078_0212708c, data_ov078_0212709c, data_ov078_021270ac,
        data_ov078_021270bc, data_ov078_021270cc, data_ov078_021270dc, data_ov078_021270ec, data_ov078_021270fc, data_ov078_0212710c };
    *n = (int)(sizeof k / sizeof k[0]);
    return k;
}
uint32_t kbo_ptr_to_id(uint32_t ptr) {
    int n; char *const *k = kbo_states(&n);
    for (int i = 0; i < n; ++i) if ((uint32_t)(uintptr_t)k[i] == ptr) return (uint32_t)i;
    return 0xffu;
}
void *kbo_id_to_ptr(uint32_t id) {
    int n; char *const *k = kbo_states(&n);
    return id < (uint32_t)n ? (void *)k[id] : nullptr;
}

// Chief Chilly's sixteen state records (data_ov073_02123320 + 0x10 * i).
char *const *chilly_states(int *n) {
    static char *const k[] = {
        data_ov073_02123320, data_ov073_02123330, data_ov073_02123340, data_ov073_02123350, data_ov073_02123360, data_ov073_02123370,
        data_ov073_02123380, data_ov073_02123390, data_ov073_021233a0, data_ov073_021233b0, data_ov073_021233c0, data_ov073_021233d0,
        data_ov073_021233e0, data_ov073_021233f0, data_ov073_02123400, data_ov073_02123410 };
    *n = (int)(sizeof k / sizeof k[0]);
    return k;
}
uint32_t chilly_ptr_to_id(uint32_t ptr) {
    int n; char *const *k = chilly_states(&n);
    for (int i = 0; i < n; ++i) if ((uint32_t)(uintptr_t)k[i] == ptr) return (uint32_t)i;
    // kbo2 (bug 27): his own code (src/game/actors/d_a_king_donketu.cpp) holds its sixteen records as C++ State objects, a second
    // copy beside the flat ov073 tables above (port/unmatched/Ov073_State.cpp). States entered from that code had no id here (0xff)
    // and every record sent while he sat in one was refused (this lane's Chilly row: field 0 = 255, before the first hit). The
    // second copy has the same names and the same order, so its index is the same state.
    if (ptr && g_kbo_fix) {
        static char *const s[] = {
            kbo2_cc_3320, kbo2_cc_3330, kbo2_cc_3340, kbo2_cc_3350, kbo2_cc_3360, kbo2_cc_3370, kbo2_cc_3380, kbo2_cc_3390,
            kbo2_cc_33a0, kbo2_cc_33b0, kbo2_cc_33c0, kbo2_cc_33d0, kbo2_cc_33e0, kbo2_cc_33f0, kbo2_cc_3400, kbo2_cc_3410 };
        for (int i = 0; i < n && i < 16; ++i) if ((uint32_t)(uintptr_t)s[i] == ptr) return (uint32_t)i;
        static uint32_t s_logged = 0;
        if (s_logged != ptr) { s_logged = ptr; std::fprintf(stderr, "[cbs] chilly state record %08x is in neither table\n", (unsigned)ptr); }
    }
    return 0xffu;
}
void *chilly_id_to_ptr(uint32_t id) {
    int n; char *const *k = chilly_states(&n);
    return id < (uint32_t)n ? (void *)k[id] : nullptr;
}

// Eyerok's nineteen state records (.bss, 16 bytes each, installed at +0x48c by func_ov066_02119454).
char *const *eye_states(int *n) {
    static char *const k[] = {
        data_ov066_0211afcc, data_ov066_0211afdc, data_ov066_0211afec, data_ov066_0211affc, data_ov066_0211b00c, data_ov066_0211b01c,
        data_ov066_0211b02c, data_ov066_0211b03c, data_ov066_0211b04c, data_ov066_0211b05c, data_ov066_0211b06c, data_ov066_0211b07c,
        data_ov066_0211b08c, data_ov066_0211b09c, data_ov066_0211b0ac, data_ov066_0211b0bc, data_ov066_0211b0cc, data_ov066_0211b0dc,
        data_ov066_0211b0ec };
    *n = (int)(sizeof k / sizeof k[0]);
    return k;
}
uint32_t eye_ptr_to_id(uint32_t ptr) {
    int n; char *const *k = eye_states(&n);
    for (int i = 0; i < n; ++i) if ((uint32_t)(uintptr_t)k[i] == ptr) return (uint32_t)i;
    return 0xffu;
}
void *eye_id_to_ptr(uint32_t id) {
    int n; char *const *k = eye_states(&n);
    return id < (uint32_t)n ? (void *)k[id] : nullptr;
}
constexpr int kEyeDeadRecord = 11;      // data_ov066_0211b07c, the dead hand
// The dead hand's enter code (func_ov066_02116d14) starts the hand's death animation from a file pointer in the overlay's table; if that
// file is not there on this console the call faults (measured: hand 2, accessing 2), so the transition is refused and logged.
bool eye_enter_safe(char *a, uint32_t id) {
    if (id != (uint32_t)kEyeDeadRecord) return true;
    const int part = *(int *)(a + 0x49c);
    void *f = part == 2 ? data_ov066_0211aea4[1] : data_ov066_0211ae8c[1];
    if (!f) { std::fprintf(stderr, "[cbs] eyerok dead-hand animation file missing (part %d): enter refused\n", part); return false; }
    return true;
}
uint32_t read_global(int idx) { return idx == 0 ? (uint32_t)data_ov066_0211abe0 : (uint32_t)data_ov066_0211ae08; }

// Big Bully (daOts_c): the int state at +0x398 (0 wait, 1 run, 2 knocked back, 3 turned at a cliff, 4 dying, 5 gone). The ROM sets
// the animation at each transition (src/actors/daOts_c.cpp); a copy that is put into a state gets the same animation and a fresh
// state timer. Death is final: a copy that is dying or gone is never pulled back.
void bully_enter(char *a, uint32_t st) {
    if (st > 5) return;
    int32_t cur; std::memcpy(&cur, a + 0x398, 4);
    if (cur == (int32_t)st || cur == 4 || cur == 5) return;
    const int32_t v = (int32_t)st;
    std::memcpy(a + 0x398, &v, 4);
    const uint16_t zero = 0; std::memcpy(a + 0x100, &zero, 2);
    static const int kFile[5] = { 4, 4, 2, 3, 1 };
    static const int kSpeed[5] = { 0x1000, 0x2000, 0x1000, 0x1000, 0x1000 };
    if (st < 5) {
        char *cfg = nullptr; std::memcpy(&cfg, a + 0x330, sizeof cfg);
        if (cfg) {
            char *sf = nullptr; std::memcpy(&sf, cfg + 4 * kFile[st], sizeof sf);
            void *anim = nullptr; if (sf) std::memcpy(&anim, sf + 4, sizeof anim);
            if (anim) _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(a + 0x110, anim, 0, kSpeed[st], 0);
        }
    }
}

// ---------------------------------------------------------------------------
// State.
// ---------------------------------------------------------------------------
struct Rx { bool valid, fresh, first_done; unsigned t; uint16_t seq; uint8_t rflags; int from; uint32_t w[kBossMaxFields]; };
struct Boss {
    char *a;
    const BDesc *d;
    uint32_t seen;
    bool known;                    // first sight done
    int32_t synced_hp;             // HP as of the last publish / apply: a lower local HP is a hit by my player
    uint32_t last_w[kBossMaxFields]; bool lw_valid;
    unsigned pub_t;
    unsigned sample_t;
    int mism;                      // consecutive state mismatches against the authority's record
    int last_auth; unsigned auth_t;
    bool announced_dead;
    unsigned knob_ok_t;            // bossdesc2: wall ms of the last proof-knob hit made on this copy (a drop right after it is a hit from my player)
    bool retired;                  // bossdesc2: a defeated Bowser copy that was told to go
    uint32_t last_st;              // the state as of the last tick (bossdesc1: Chilly's hit shows as a state change, not an HP change)
    bool heard;                    // an authority record was ever applied here
    bool def_seen;                 // bossfix1: the boss was seen defeated (or its star was spawned) on this console
    bool mine_last;                // bossfix1: I was this boss's authority the last time I saw it
    bool bit_set;                  // bossfix1: the group was told this boss is dead
    bool killed_late;              // bossfix1: this copy was removed because the group said the boss is dead
    bool summon_held;              // coopgoomboss2: this Goomboss copy's summon is being held (log once per summon)
    bool kpa_shown;                // bowser2: this Bowser copy is shown held by a friend (kpa_show_held)
    int kpa_seat;                  // bowser2: the friend holding him
    unsigned kpa_t;                // bowser2: wall ms of the last record that said he is held
    int16_t kpa_yaw;               // bowser2: the holder body's facing last tick (its turn per tick is his swing speed)
    unsigned kpa_talk_t;           // bowser2: wall ms since which this copy's own intro talk kept a forced state away (0 = not)
    unsigned kpa_quiet_t;          // bowser2: when this copy was let go quietly (kpa_release quiet; 0 = not): hits ignored for 5 s
    unsigned kpa_air_t;            // bowser2 BFS: wall ms since which this copy has been far above the authority's Bowser (0 = not)
    unsigned wig_talk_t;           // bossfix2: wall ms since which this Wiggler copy's own talk kept a forced state away (0 = not)
    unsigned eye_talk_t;           // bossfix2: the same for Eyerok's head (its talk state with my player)
    unsigned wig3_ticks;           // bossfix2: ticks this Wiggler copy has stood in MOVE_START with no talk of its own
    unsigned wig_dead_t;           // bossfix3: wall ms since which this Wiggler copy has been in its own DEAD (0 = not)
    unsigned wig2_ticks;           // bossfix3: ticks this Wiggler copy has stood in MOVE_STOP with no talk of its own
    bool wig_music;                // bossfix3: the boss music was started here by the MOVE_STOP way out below (once)
    unsigned wig_stag_t;           // bossfix3: wall ms this Wiggler copy was last seen in its stagger (states 8 to 10; 0 = never)
    unsigned goom_talk_tries;      // bossfix2: StartTalk attempts for a Goomboss copy put into its defeat by a record
    uint8_t rule;                  // bossfix4: the owner-after-defeat rule for this boss, latched at its defeat (0 not yet, 1 on, 2 off)
    bool wig_call_seen;            // bossfix4: this Wiggler copy was already seen in DEMOCALL (1) on an earlier tick
    uint32_t bst; bool bst_valid;  // coopbully1: a bully's state at the end of the last tick (a change before the next one is the ROM's own)
    Rx rx;
};

CoopBossStats g_st = {};
bool g_env = false, g_on = true, g_log = false;
struct HitKnob { unsigned tick; bool done; bool armed; bool lethal; int hp0; int slot; int slot_tries; };
HitKnob g_hit[4]; int g_nhit = 0;
struct ForgeKnob { unsigned tick; char name[12]; bool done; };
ForgeKnob g_forge[8]; int g_nforge = 0;
unsigned g_stand_tick = 0, g_stand_every = 0; int g_stand_dist = 0; bool g_stand_done = true, g_god = false;
bool g_deadmark = true;   // SM64DS_COOP_BOSS_DEADMARK=0: must-fail control, a defeated boss is not marked
bool g_face = false;   // bossfix1: SM64DS_COOP_BOSS_FACE=1: while Bowser is in his defeat state the local player stands 300 units from him, facing him (step 2 of the ROM's defeat asks for exactly that)
bool g_bomb = false;   // bossfix1: SM64DS_COOP_BOSS_BOMB=1: Bowser's proof hit is a REAL bomb hit (the ROM's own detection)
unsigned g_behind_tick = 0, g_behind_every = 0; int g_behind_dist = 0; bool g_behind_done = true;   // bossdesc2: the local player behind the boss, facing his way (the grab proof)
// bowser2 proof knob SM64DS_COOP_BOWSER_SWING=F:S:N (off by default): from tick F my player stands at Bowser's tail and takes it the
// ROM's way (Player::TryGrab during a real punch: the row's pad presses B), turns S angle units a tick for N ticks, then lets go.
struct SwingKnob { unsigned from; int speed; unsigned dur; int phase; unsigned t0; unsigned tries; };
SwingKnob g_swing = { 0, 0, 0, -1, 0, 0 };
unsigned g_kpa_jump_every = 0, g_kpa_jump_t = 0;
struct StallKnob { unsigned at, ms; bool req; } g_stall[2] = {}; int g_nstall = 0; unsigned g_stall_until = 0; bool g_stall_on = false;   // bowser2 proof knob SM64DS_COOP_BOWSER_JUMP=N: the authority's idle Bowser jumps every N ticks
char *kpa_arena();
bool g_kpa_fix = true;   // bowser2: SM64DS_COOP_BOWSER_FIX=0 is the before control (no held copy, no intro guard, no camera give-back)
unsigned g_bad_logged = 0;   // kbo2: refused boss records logged so far (the first few per run, with the field and the value)
bool g_kbo_look = false;     // kbo2 proof log SM64DS_COOP_KBO_LOOK=1
bool g_regd = false;
int  g_my = -1;
bool g_minion_share = true;   // coopgoomboss2: SM64DS_COOP_GOOMBAS=0 is the must-fail control: every console summons its own goombas (the 1.0.2 behaviour)
struct MinionKnob { unsigned tick; bool done; };
MinionKnob g_mkill[8]; int g_nmkill = 0;     // SM64DS_COOP_GOOMBA_KILL=T[,T]: at boss tick T, on THIS console, the first live minion is stomped dead
CoopKey g_key;
unsigned g_ticks = 0, g_seq = 0;
Boss g_boss[kSlots];
uint16_t g_dead_cls[kSlots];    // slot -> class of a boss the group knows is dead (0 = none)
uint8_t  g_dead_tx[kSlots];     // sends still owed for it
unsigned g_dead_t[kSlots];
unsigned g_claim_until[kSeats];     // wall ms until which a seat's claim holds
// bossfix2 (H5): a claim of 0 is "none". The test was (int)(until - now) > 0 alone, which for until = 0 is true once the hub clock
// (milliseconds of PC uptime) passes 2^31, about 24.9 days: every seat then looked claimed and the lowest seat was every boss's
// authority. A stamp is never 0 ((now + 3000) | 1), and 0 is never live. SM64DS_COOP_BOSSFIX2=0 brings the old test back (control).
bool g_bf2 = true;
unsigned g_clock_add = 0;   // SM64DS_COOP_BOSS_CLOCK_ADD=ms (dev builds only): this file's clock pushed forward, to run a row "at 30 days uptime"
unsigned hub_now() { return comms_hub_now_ms() + g_clock_add; }
bool claim_live(unsigned until, unsigned now) { return (until != 0 || !g_bf2) && (int)(until - now) > 0; }
unsigned claim_stamp(unsigned now) { return g_bf2 ? ((now + 3000u) | 1u) : now + 3000u; }
// bossfix4: WHO RUNS THE OWNER-AFTER-DEFEAT RULE. bossfix2 hands a defeated Whomp King, Wiggler or Eyerok to the group owner and holds
// back every other console's star, but a 1.0.4 console does not know that rule: as the owner it never took the boss over and never
// made the star (REVIEW-AL mixed_k1_al: no star for anyone, the 1.0.4 friend's Wiggler stuck in his hurt talk). Every console that
// runs the rule says so once a second in a boss caps message (SYNW sub-type 0x53, kWorldBossDefeat: 1.0.4 routes it here too and
// drops it alone as a bad sub-type, nothing else in the batch is lost because world sub-types travel singly). The rule applies to a
// boss only when the owner is me or was heard sending caps in the last 5 s; otherwise that boss keeps 1.0.4's authority (claims, then
// the target seat) for its records, the rule the 1.0.4 console itself runs, and every console keeps its own star (star_is_mine;
// coop_spawn merges the copies into one). The answer is latched per boss at his defeat, so a late caps message in
// the middle of a long defeat talk cannot change who gives the star. King Bob-omb keeps kbo2's rule (1.0.4 runs it too).
enum : unsigned { kCapsEveryMs = 1000, kCapsFreshMs = 5000 };
enum : uint8_t { kBossCapsVer = 1, kBossCapOwnerRule = 0x01 };
#pragma pack(push, 1)
struct BossCaps { CoopKeyWire key; uint8_t ver; uint8_t caps; uint16_t reserved; };
#pragma pack(pop)
static_assert(sizeof(BossCaps) == 8, "boss caps are 8 bytes");
unsigned g_caps_t[kSeats];   // hub ms (|1) a seat's caps were last heard, 0 = never
uint8_t  g_caps[kSeats];     // that seat's caps bits
unsigned g_caps_tx_t = 0;
bool g_caps_send = true;     // SM64DS_COOP_BOSS_CAPS=0 (dev builds only): this console sends no caps, so friends treat it as a 1.0.4 one
// PIRANHA1 (the Big Fire Piranha group count, see pkn_census below for the whole story).
enum : unsigned { kPknCls = 251, kPknGroupSize = 5, kPknStarGraceMs = 3000, kPknJoinMs = 8000 };
enum : int { kPknStateWait = 1, kPknStateSpit = 2, kPknStateDefeated = 4 };
bool g_pkn_on = true;                  // SM64DS_COOP_PIRANHA=0 (dev builds only): the 1.0.4 control
unsigned g_pkn_first_t = 0;            // hub ms (|1) this console first saw the group in this visit
struct PknStar { bool pending, join; unsigned due; Vector3 pos; s8 area; unsigned param; int slot; };
PknStar g_pkn_star = {};
#ifndef PORT_ROM_CLEAN
// SM64DS_COOP_PKN_HIT=F:i.j.k[:dist]: from boss tick F, THIS console's player kills plants i, j, k (indices into the group's slots in
// slot order) one after the other: he is put dist units (default 250) beside the plant every 30 ticks, and once the plant is up at
// full size and spitting its mouth collider is given his hit, the way the ROM's collision pass fills it (otherOwner = the player,
// hitFlags = a damage kind), so CheckClsnHits takes the rest. A plant that is gone already (a friend's kill) is skipped.
struct PknKnob { bool on, listed, hit_logged; unsigned tick, next_place; int idx[8], n, at, slots[8], nslots, dist; };
PknKnob g_pknk = {};
#endif
bool seat_runs_owner_rule(int s, unsigned now) {
    if (s == g_my) return true;
    if (s < 0 || s >= kSeats || !g_caps_t[s]) return false;
    return coop_age_ms(now, g_caps_t[s]) < kCapsFreshMs && (g_caps[s] & kBossCapOwnerRule);
}
// coopbully1: A BULLY BELONGS TO THE PLAYER WHO KNOCKED IT. A bump exists only on the hitter's console (a friend's body is parked away
// between draws, coop_target.h), so the console whose own copy entered the knocked-back state by itself owns that bully, pose and state,
// while the knock or the lava death lasts and kBullyClaimMs after. Per slot (several bullies at once), unlike the per-seat claim above.
enum : unsigned { kBullyClaimMs = 1500 };
unsigned g_bclaim[kSlots][kSeats];  // hub ms until which a seat's claim on a bully slot holds
bool g_bully_on = true;             // SM64DS_COOP_BULLY=0: must-fail control, no bully claim (the 1.0.2 behaviour, plus the small Bully row)
struct BumpKnob { unsigned tick, every; int cls; int ang; bool on; int left; int speed; };
struct BStandKnob { unsigned tick, every; int cls, dist; bool on; };
BStandKnob g_bstand = {};          // SM64DS_COOP_BULLY_STAND=F:cls:dist[:every]: the local player stands dist units from the nearest bully of cls
BumpKnob g_bump = {};               // SM64DS_COOP_BULLY_BUMP=F:cls[:every[:count[:angle[:speed]]]]
bool is_bully(unsigned cls) { return cls == 215 || cls == 216; }
unsigned g_tok_t = 0; double g_tokens = 6.0;
unsigned g_stat_t = 0;

long long wall_ms() {
    FILETIME ft; GetSystemTimeAsFileTime(&ft);
    return (long long)((((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime) / 10000ull);
}
inline int32_t rd32(const char *a, int off) { int32_t v; std::memcpy(&v, a + off, 4); return v; }
inline int16_t rd16(const char *a, int off) { int16_t v; std::memcpy(&v, a + off, 2); return v; }

uint32_t read_field(const char *a, const BField &f) {
    if (f.kind & kGlobal) return read_global(f.off);
    uint32_t v = 0;
    std::memcpy(&v, a + f.off, f.size);
    if (f.kind & kS8) v = (uint32_t)(int32_t)(int8_t)(uint8_t)v;
    else if (f.kind & kEyeState) v = eye_ptr_to_id(v);
    if (f.kind & kStatePtr) v = kbo_ptr_to_id(v);
    else if (f.kind & kChillyPtr) v = chilly_ptr_to_id(v);
    else if (f.kind & kAlive) v = (v == 4u || v == 5u) ? 0u : 1u;
    return v;
}
void write_field(char *a, const BDesc &d, int idx, uint32_t v) {
    const BField &f = d.f[idx];
    if (!field_ok(f) || !field_in_class(d.cls, f)) return;
    if ((f.kind & kLocal) && g_minion_share) return;      // SM64DS_COOP_GOOMBAS=0 (control): written as in 1.0.2
    if (f.kind & kGlobal) {                  // the overlay's own words: a merge that only ever moves one way, so a replay is harmless
        if (f.off == 0) { if (v <= 3u) data_ov066_0211abe0 = (unsigned char)(data_ov066_0211abe0 & v); }
        else if (v <= 3u && v > data_ov066_0211ae08) data_ov066_0211ae08 = (unsigned char)v;
        return;
    }
    if (f.kind & kZeroOnForce) { a[f.off] = 0; return; }
    // bossfix3: Wiggler's mDefeated (+0x8e0) is set by DEAD's enter code and never cleared by the ROM. The authority's is still 0 while
    // the stomper reads his last talk, so writing it down after this copy's own DEAD entry let the next record run DEAD again (45 times
    // in 37 s, each one poof, sound and a star call). It only ever goes up on a copy.
    if (g_bf2 && d.cls == 248 && f.off == 0x8e0 && (v & 0xffu) <= (unsigned char)a[0x8e0]) return;
    // bossfix3: DAMAGE_STOP (8) and DAMAGE_START (10) change the body's animations segment by segment at their timer's 3, 6 and 9 and
    // end when every segment has its new one. A copy forced into them took the authority's timer (13 by then), skipped those steps and
    // stood in 8 for good (own_f8 row: 13 samples, until the authority's MOVE pulled it out). The enter code's timer stays.
    if (g_bf2 && d.cls == 248 && f.off == 0x8da && (rd32(a, 0x8c4) == 8 || rd32(a, 0x8c4) == 10)) return;
    if (f.kind & kGoomState) {
        if (v > 8u) return;
        if (v == 5u) func_ov074_02121a4c(a, 2);     // the stomp sets the hit animation before it enters state 5, whose tick waits for it to finish
        _ZN12daKuriKing_c19func_ov074_021203e4Ei(a, (int)v);
        return;
    }
    if (f.kind & kEyeState) {
        void *p = eye_id_to_ptr(v);
        if (!p) return;
        if (!eye_enter_safe(a, v)) return;
        _ZN6Eyerok19func_ov066_02119454EPv(a, p);          // the ROM's own transition (the record's enter pair runs)
        return;
    }
    if (f.kind & kStatePtr) {
        void *p = kbo_id_to_ptr(v);
        if (!p) return;
        _ZN12daBombking_c19KingBobOmb_SetStateEPv(a, p);          // the ROM's own transition, with its enter code
        return;
    }
    if (f.kind & kChillyPtr) {
        void *p = chilly_id_to_ptr(v);
        if (!p) return;
        ChiefChilly_ChangeState(a, p);      // the ROM's own transition (his enter code runs)
        return;
    }
    if (f.kind & kAlive) return;            // derived, never written: the state field carries it
    if (f.kind & kWigState) {
        if (!g_bf2) { if (v <= 8u) _ZN12daHanachan_c8SetStateEi(a, (int)v); return; }
        if (v > 10u) return;
        // bossfix2 (H1): DAMAGE (5) and DAMAGETALK (6) are the hurt talk of the player who stomped him, on that player's console
        // (HandlePlayerHits puts the stomper in +0x8c8 there). A copy put into them from the wire started that talk with whatever
        // body +0x8c8 held: a friend's body refuses StartTalk for ever (stuck in DAMAGE), my own body got a talk for a stomp it never
        // made. A copy goes straight to where the stomper's talk ends: MOVE_START (3) while health is left, DEAD (7) on the last
        // point (DAMAGETALK's own choice, StateDamageTalkMain). The HP is merged before the state, so it is the group's here.
        if (v == 5u || v == 6u) {
            const unsigned hp = (unsigned char)a[0x8db];
            if (hp <= 1u ? a[0x8e0] != 0 : (rd32(a, 0x8c4) == 3 || rd32(a, 0x8c4) == 4)) return;   // already where that talk ends
            const uint32_t nv = hp > 1u ? 3u : 7u;
            if (g_log) std::fprintf(stderr, "[cbs] wiggler copy: wire state %u is the stomper's hurt talk, entered as %u here (hp %u) w=%lld\n", v, nv, hp, wall_ms());
            v = nv;
        }
        if ((uint32_t)rd32(a, 0x8c4) == v) return;
        _ZN12daHanachan_c8SetStateEi(a, (int)v);
        return;
    }
    if (f.kind & kBullyState) { bully_enter(a, v); return; }
    if (f.kind & kMaxUp) {
        uint32_t cur = 0; std::memcpy(&cur, a + f.off, f.size);
        const uint32_t unused = f.size == 1 ? 0xffu : (f.size == 2 ? 0xffffu : 0xffffffffu);
        if (cur == unused || v == unused || v <= cur) return;
    }
    if (f.size == 1) v &= 0xffu; else if (f.size == 2) v &= 0xffffu;
    std::memcpy(a + f.off, &v, f.size);
}

// Sane values only: a state of Whomp King is a small index; HP is small.
bool value_ok(const BDesc &d, int idx, uint32_t v) {
    if (d.pos >= 0 && idx >= d.pos && idx < d.pos + 3) { const long long lim = 40000LL * 4096; return (int32_t)v > -lim && (int32_t)v < lim; }
    if (d.cls == 165) {
        if (idx == 0) return v < 16;                 // mState indexes the 12-cell handler table
        if (idx == 1) return v <= 8;                 // HP
        if (idx == 2) return v <= 32;                // sub-state
        return true;
    }
    if (d.cls == 189) {
        if (idx == 0) return v < 18;                 // a table id
        if (idx == 1) return (int32_t)v >= -4 && (int32_t)v <= 8;   // mHealth (coopkbo1: a landing past 0 reads -1; refusing it lost every record after)
        return true;
    }
    if (d.cls == 218) {
        if (idx == 0) return v < 16;                 // one of the sixteen state records
        // knock-downs left. kbo2 (bug 27): 0xff too. The hit-reaction state (func_ov073_02120c7c) takes one with no floor, so a copy whose
        // count the group had already brought to 0 wrapped to 0xff there; every record it sent after that was refused (PLAYLOGS1: 139 of
        // 1221 and 395 of 1273) and its own falls never reached 0 again. 0xff reads as 0 (apply_rx) and the copy is put back to 0 (process).
        if (idx == 1) return v <= 3 || v == 0xffu;
        if (idx == 2) return v <= 3;                 // mPhase, the chase's leg (0..3, func_ov073_02120ed0; it was <= 8)
        return true;
    }
    if (d.cls == 216) {
        if (idx == 0) return v <= 5;                 // int state
        if (idx == 1) return v <= 1;                 // alive
        if (idx == 2) return v <= 4 || v == 0xffu;   // bullies killed (0xff: none to kill)
        return true;
    }
    if (d.cls == 215) {
        if (idx == 0) return v <= 5;                 // int state
        if (idx == 1) return v <= 1;                 // alive
        return true;
    }
    if (d.cls == 210) {
        if (idx == 0) return v <= 8;                 // state byte
        if (idx == 1) return v <= 3;                 // hits left
        if (idx == 2) return v <= 16;                // sub-state
        return true;
    }
    if (d.cls == 248) {
        // state index: eleven states (DEMOWAIT 0 .. DAMAGE_START 10, d_a_hanachan.cpp). bossfix2 (H6): 9 / 10 are the stagger after he
        // hurts a player (DAMAGE_STOP 8 -> DAMAGE_WAIT 9 -> DAMAGE_START 10, about 2.5 s); refusing them stopped every record meanwhile.
        if (idx == 0) return v <= (g_bf2 ? 10u : 8u);
        if (idx == 1) return v <= 4;                 // hits left
        return true;
    }
    if (d.cls == 198) {
        if (idx == 0) return v <= 8;                 // state index (nine states)
        if (idx == 1) return v <= 5;                 // size index = HP (the ROM clamps it at 5)
        if (idx == 2) return v <= 8;                 // sub-step
        if (idx == 3) return true;                   // goombas out (kLocal, never written; a wrapped count must not refuse the record)
        if (idx == 4) return v <= 4;                 // shrink counter
        return true;
    }
    if (d.cls == 279) {
        if (idx == 0) return v <= 0x20;              // main state
        if (idx == 1) return (int32_t)v >= -4 && (int32_t)v <= 8;     // HP
        if (idx == 2) return v <= 3;                 // tail-grab sub-mode
        if (idx == 3) return v <= 0x20;              // step
        return true;
    }
    if (d.cls == 176) {
        if (idx == 0) return v < 19;                 // one of the nineteen state records
        if (idx == 1) return (int32_t)v >= -4 && (int32_t)v <= 3;     // hand HP
        if (idx == 10 || idx == 11) return v <= 3;   // overlay globals
        return true;
    }
    return true;
}

void read_env() {
    if (g_env) return;
    g_env = true;
    auto flag = [](const char *n, bool dflt) { const char *e = std::getenv(n); return (e && *e) ? *e != '0' : dflt; };
    g_on = flag("SM64DS_COOP_BOSS", true);
    g_log = flag("SM64DS_COOP_BOSS_LOG", false);
    if (const char *h = std::getenv("SM64DS_COOP_BOSS_HIT")) {
        const char *p = h;
        while (*p && g_nhit < 4) {
            unsigned fr = 0; int used = 0;
            if (std::sscanf(p, "%u%n", &fr, &used) < 1 || used <= 0) break;
            const bool lethal = std::strncmp(p + used, ":lethal", 7) == 0;
            if (lethal) used += 7;
            g_hit[g_nhit].lethal = lethal;
            g_hit[g_nhit].tick = fr; g_hit[g_nhit].done = false; g_hit[g_nhit].armed = false; g_hit[g_nhit].hp0 = 0; g_hit[g_nhit].slot = -1; ++g_nhit;
            p += used; while (*p == ',' || *p == ';' || *p == ' ') ++p;
        }
    }
    if (const char *st = std::getenv("SM64DS_COOP_BOSS_STAND")) {
        unsigned f = 0, ev = 0; int d = 0;
        const int n = std::sscanf(st, "%u:%i:%u", &f, &d, &ev);
        if (n >= 2) { g_stand_tick = f; g_stand_dist = d; g_stand_done = false; g_stand_every = n >= 3 ? ev : 0; }
    }
    if (const char *st = std::getenv("SM64DS_COOP_BOSS_BEHIND")) {
        unsigned f = 0, ev = 0; int d = 0;
        const int n = std::sscanf(st, "%u:%i:%u", &f, &d, &ev);
        if (n >= 2) { g_behind_tick = f; g_behind_dist = d; g_behind_done = false; g_behind_every = n >= 3 ? ev : 0; }
    }
    if (const char *sw = std::getenv("SM64DS_COOP_BOWSER_SWING")) {
        unsigned f = 0, n = 0; int sp = 0;
        if (std::sscanf(sw, "%u:%i:%u", &f, &sp, &n) == 3 && n > 0) g_swing = { f, sp, n, 0, 0, 0 };
    }
    g_kpa_fix = flag("SM64DS_COOP_BOWSER_FIX", true);
    g_kbo_fix = flag("SM64DS_COOP_KBO_FIX", true);
    g_kbo_look = flag("SM64DS_COOP_KBO_LOOK", false);
    if (const char *j = std::getenv("SM64DS_COOP_BOWSER_JUMP")) g_kpa_jump_every = (unsigned)std::atoi(j);
    if (const char *j = std::getenv("SM64DS_COOP_BOWSER_STALL")) {
        while (*j && g_nstall < 2) {
            unsigned at = 0, ms = 0; if (std::sscanf(j, "%u:%u", &at, &ms) != 2) break;
            g_stall[g_nstall].at = at; g_stall[g_nstall].ms = ms; g_stall[g_nstall].req = false; ++g_nstall;
            const char *c = std::strchr(j, ','); if (!c) break; j = c + 1;
        }
    }
    g_bf2 = flag("SM64DS_COOP_BOSSFIX2", true);
#ifndef PORT_ROM_CLEAN
    if (const char *ca = std::getenv("SM64DS_COOP_BOSS_CLOCK_ADD")) g_clock_add = (unsigned)std::strtoul(ca, nullptr, 0);
    g_caps_send = flag("SM64DS_COOP_BOSS_CAPS", true);
    g_pkn_on = flag("SM64DS_COOP_PIRANHA", true);           // PIRANHA1: 0 = the 1.0.4 control (a friend's kill is not counted here)
    if (const char *ph = std::getenv("SM64DS_COOP_PKN_HIT")) {
        unsigned f = 0; int used = 0;
        if (std::sscanf(ph, "%u:%n", &f, &used) >= 1 && used > 0) {
            PknKnob &k = g_pknk;
            k = PknKnob(); k.tick = f; k.dist = 250;
            const char *p = ph + used;
            while (*p >= '0' && *p <= '9' && k.n < 8) {
                k.idx[k.n++] = *p - '0'; ++p;
                if (*p == '.') ++p;
            }
            if (*p == ':') k.dist = std::atoi(p + 1);
            k.on = k.n > 0;
        }
    }
#endif
    g_god = flag("SM64DS_COOP_BOSS_GOD", false);
    g_bomb = flag("SM64DS_COOP_BOSS_BOMB", false);
    g_face = flag("SM64DS_COOP_BOSS_FACE", false);
    check_descriptors();
    g_deadmark = flag("SM64DS_COOP_BOSS_DEADMARK", true);
    g_minion_share = g_on && flag("SM64DS_COOP_GOOMBAS", true);   // INT: boss sync off (SM64DS_COOP_BOSS=0) keeps the goombas as in 1.0.2 too
    if (const char *mk = std::getenv("SM64DS_COOP_GOOMBA_KILL")) {
        const char *p = mk;
        while (*p && g_nmkill < 8) {
            unsigned fr = 0; int used = 0;
            if (std::sscanf(p, "%u%n", &fr, &used) < 1 || used <= 0) break;
            g_mkill[g_nmkill].tick = fr; g_mkill[g_nmkill].done = false; ++g_nmkill;
            p += used; while (*p == ',' || *p == ';' || *p == ' ') ++p;
        }
    }
    g_bully_on = flag("SM64DS_COOP_BULLY", true);
    if (const char *bs = std::getenv("SM64DS_COOP_BULLY_STAND")) {
        unsigned f = 0, ev = 0; int cls = 0, dist = 0;
        const int n = std::sscanf(bs, "%u:%i:%i:%u", &f, &cls, &dist, &ev);
        if (n >= 3 && (cls == 215 || cls == 216)) { g_bstand.tick = f; g_bstand.cls = cls; g_bstand.dist = dist; g_bstand.every = n >= 4 ? ev : 0; g_bstand.on = true; }
    }
    if (const char *bp = std::getenv("SM64DS_COOP_BULLY_BUMP")) {
        unsigned f = 0, ev = 0; int cls = 0, cnt = 1, ang = -1, spd = 40;
        const int n = std::sscanf(bp, "%u:%i:%u:%i:%i:%i", &f, &cls, &ev, &cnt, &ang, &spd);
        g_bump.speed = n >= 6 && spd > 0 && spd <= 200 ? spd : 40;
        if (n >= 2 && (cls == 215 || cls == 216)) { g_bump.tick = f; g_bump.cls = cls; g_bump.every = n >= 3 ? ev : 0; g_bump.left = n >= 4 && cnt > 0 ? cnt : 1; g_bump.ang = n >= 5 ? ang : -1; g_bump.on = true; }
    }
    if (const char *f = std::getenv("SM64DS_COOP_BOSS_FORGE")) {
        const char *p = f;
        while (*p && g_nforge < 8) {
            unsigned fr = 0; char nm[12] = {}; int used = 0;
            if (std::sscanf(p, "%u:%11[a-z]%n", &fr, nm, &used) < 2 || used <= 0) break;
            g_forge[g_nforge].tick = fr; std::memcpy(g_forge[g_nforge].name, nm, sizeof nm); g_forge[g_nforge].done = false;
            ++g_nforge; p += used; while (*p == ',' || *p == ';' || *p == ' ') ++p;
        }
    }
    std::fprintf(stderr, "[cbs] boss sync %s%s bossfix2=%d clock_add=%u caps=%d\n", g_on ? "on" : "OFF (control)", g_log ? " log" : "", (int)g_bf2, g_clock_add,
                 (int)(g_bf2 && g_caps_send));
}

bool is_player_ptr(const void *a) {
    for (int i = 0; i < 16; ++i) if (data_0209f394[i] == a) return true;
    return false;
}

// A state that was never entered on this copy finds its player pointers empty (Whomp King's defeat talk dereferences +0x3ac,
// King Bob-omb's +0x430): they are given the local player's body, so the dialogue and camera run for the player at this console.
void give_player_ptrs(Boss &b) {
    const int me = (int)data_0209f250;
    if (me < 0 || me >= 16 || !data_0209f394[me]) return;
    for (int k = 0; k < 2; ++k) {
        const uint16_t off = b.d->pl[k];
        if (!off) continue;
        void *cur = nullptr; std::memcpy(&cur, b.a + off, sizeof cur);
        if (!cur || !is_player_ptr(cur)) { void *mine = data_0209f394[me]; std::memcpy(b.a + off, &mine, sizeof mine); }
    }
}

CoopKeyWire wire_key() {
    CoopKeyWire w; w.level = (uint16_t)g_key.level; w.act = (uint8_t)g_key.act; w.reserved = 0;
    return w;
}

bool take_token(unsigned now) {
    if (g_tok_t == 0) g_tok_t = now;
    const unsigned dt = now - g_tok_t; g_tok_t = now;
    g_tokens = (std::min)(8.0, g_tokens + dt * 0.012);
    if (g_tokens < 1.0) return false;
    g_tokens -= 1.0;
    return true;
}

// coopbully1: the seat that holds a live claim on a bully slot (the lowest seat when two players knocked it at once), or -1.
int bully_claim_seat(int slot, unsigned wnow) {
    if (!g_bully_on || slot < 0 || slot >= kSlots) return -1;
    for (int s = 0; s < kSeats; ++s)
        if (g_bclaim[slot][s] && (int)(g_bclaim[slot][s] - wnow) > 0 && (s == g_my || ((coop_group_member_mask() >> s) & 1u))) return s;
    return -1;
}

int hp_of(const Boss &b);
// kbo2: ONE KING BOB-OMB FOR THE GROUP AFTER HIS LAST HIT. From the moment a console's copy is down (HP 0) its authority is the group
// owner, the one seat every console agrees on (OWNER1). The target seat moves with the players (and with a 1 s hold) while the
// consoles run the defeat talk at different times, so each console asked "who gives the star" at a different moment and could get a
// different answer: both suppressed it (Tango's bug 21, measured again in this lane's before row). The owner gives the star, every
// other console's arrives by announcement.
bool kbo_down(int slot) {
    if (!g_kbo_fix || slot < 0 || slot >= kSlots) return false;
    const Boss &b = g_boss[slot];
    return b.a && b.d && b.d->cls == 189 && (b.def_seen || hp_of(b) <= 0);
}

bool is_defeated(const Boss &b);
// bossfix2 (H4): the same rule for every boss whose star comes from his own defeat code on every console: Whomp King (165), Wiggler
// (248) and Eyerok (176). Once a console has seen him defeated, his authority is the group owner, so the one console that keeps
// its star is the same on every screen however the target seat moves during the consoles' defeat at slightly different moments.
bool boss_down(int slot, unsigned cls) {
    if (cls == 189) return kbo_down(slot);
    if (!g_bf2 || (cls != 165 && cls != 248 && cls != 176) || slot < 0 || slot >= kSlots) return false;
    Boss &b = g_boss[slot];
    if (!(b.d && b.d->cls == cls && (b.def_seen || (b.a && is_defeated(b))))) return false;
    // bossfix4: only when the owner runs the rule too (latched at the first ask after the defeat)
    if (!b.rule) {
        const int o = coop_group_owner();
        b.rule = (o < 0 || seat_runs_owner_rule(o, hub_now())) ? 1 : 2;
        if (g_log) std::fprintf(stderr, "[cbs] owner rule for cls=%u slot=%d: %s (owner seat %d) w=%lld\n", cls, slot,
                                b.rule == 1 ? "on, the owner gives the star" : "OFF, the owner sends no boss caps (1.0.4): 1.0.4 authority for records, the star kept on each console",
                                o, wall_ms());
    }
    return b.rule == 1;
}

// The seat whose copy of this boss is the right one.
int auth_for(int slot, unsigned wnow, unsigned cls = 0) {
    if (boss_down(slot, cls)) {
        const int o = coop_group_owner();
        if (o >= 0) return o;
    }
    if (is_bully(cls)) {
        const int c = bully_claim_seat(slot, wnow);
        if (c >= 0) return c;
        if (cls == 215) {                    // a small Bully has no target seat of its own: the console that steers its pose
            const int o = coop_move_owner(slot);
            return o >= 0 ? o : g_my;
        }
    }
    for (int s = 0; s < kSeats; ++s)
        if (claim_live(g_claim_until[s], wnow) && (s == g_my || ((coop_group_member_mask() >> s) & 1u))) return s;
    int s = coop_target_seat_of_slot(slot);
    if (s >= 0) return s;
    const int o = coop_group_owner();
    return o >= 0 ? o : g_my;
}

int rec_bytes(const BDesc &d) { return (int)sizeof(BossRec) + 4 * d.nf; }

int fill_rec(unsigned char *out, int slot, const Boss &b, uint8_t rflags) {
    BossRec r; std::memset(&r, 0, sizeof r);
    r.slot = (uint16_t)slot; r.cls = b.d->cls; r.nf = b.d->nf; r.rflags = rflags; r.seq = (uint16_t)++g_seq;
    std::memcpy(out, &r, sizeof r);
    for (int k = 0; k < b.d->nf; ++k) {
        const uint32_t v = read_field(b.a, b.d->f[k]);
        std::memcpy(out + sizeof r + 4 * k, &v, 4);
    }
    return rec_bytes(*b.d);
}

void send_batch(int sub, uint8_t flags, const unsigned char *recs, int nrecs, int bytes) {
    unsigned char buf[kPayloadMax];
    BossHead h; std::memset(&h, 0, sizeof h);
    h.key = wire_key(); h.count = (uint8_t)nrecs; h.flags = flags;
    std::memcpy(buf, &h, sizeof h);
    std::memcpy(buf + sizeof h, recs, (size_t)bytes);
    if (coop_send_world(sub, buf, (int)sizeof h + bytes)) {
        if (sub == kWorldBossHit) ++g_st.hits_sent; else { ++g_st.sent; g_st.recs_sent += (unsigned)nrecs; }
    }
}

void send_dead(int slot) {
    const BDesc *d = desc_of(g_dead_cls[slot]);
    if (!d) return;
    unsigned char rec[sizeof(BossRec) + 4 * kBossMaxFields];
    std::memset(rec, 0, sizeof rec);
    BossRec r; std::memset(&r, 0, sizeof r);
    r.slot = (uint16_t)slot; r.cls = d->cls; r.nf = d->nf; r.rflags = 4; r.seq = (uint16_t)++g_seq;
    std::memcpy(rec, &r, sizeof r);
    send_batch(kWorldBossState, 0, rec, 1, rec_bytes(*d));
}
void pump_dead(unsigned now) {
    for (int i = 0; i < kSlots; ++i)
        if (g_dead_tx[i] && g_dead_cls[i] && (unsigned)(now - g_dead_t[i]) >= 150u) { send_dead(i); --g_dead_tx[i]; g_dead_t[i] = now; }
}
// The owner answered a joiner's snapshot request: tell it which bosses are dead as well.
void on_snapshot_request(int /*seat*/, unsigned /*nonce*/) {
    for (int i = 0; i < kSlots; ++i) if (g_dead_cls[i] && g_dead_tx[i] < 2) g_dead_tx[i] = 2;
}

// ---------------------------------------------------------------------------
// Scan, publish, apply.
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// bossfix1: A DEFEATED BOSS STAYS DEAD. The ROM does not track a boss in the death table (re-entering a course fights him again), so a
// console that joined after the defeat got a live boss. When a boss that was seen defeated is gone from the actor list, its placed
// slot's bit goes up in the death table: coop_world announces it to the group and hands it to every later joiner in the snapshot, and
// BeforeInitResources then deletes the boss before it initialises.
// ---------------------------------------------------------------------------
unsigned g_recent_t = 0; bool g_recent_mine = true;     // the last time any boss was seen here, and whether I was its authority
// A dead boss must not come back for a console that joins later. The death table is NOT used for it: a boss deleted by a death-table bit
// never initialises, and its destructor then frees what its InitResources never took (measured: a late joiner's heap was corrupted and
// the minimap faulted). Instead the group remembers {slot, class} and tells a joiner, whose copy of the boss, once it has initialised,
// is removed the ordinary way (MarkForDestruction, as the ROM itself ends a boss).
void mark_dead_slot(int slot, Boss &b) {
    b.bit_set = true;
    if (!g_deadmark || slot < 0 || slot >= kSlots) return;
    g_dead_cls[slot] = b.d->cls; g_dead_tx[slot] = 3; g_dead_t[slot] = 0;
    ++g_st.dead_marked;
    if (g_log) std::fprintf(stderr, "[cbs] dead boss marked: cls=%u slot=%d (kept by the group; a joiner is told, not given the boss) w=%lld\n", (unsigned)b.d->cls, slot, wall_ms());
}

// coopbully1: THE BIG BULLY'S THREE BULLIES. daBDonketu_c::InitResources spawns them with no death-table slot (Spawn(0xd7, -1, ...),
// d_a_b_donketu.cpp), so neither this lane nor the movers saw them and each console fought its own three. They are spawned one after
// another, so their unique ids rise in spawn order on every console: the first, second and third get record slots 505, 506, 507. The
// order is taken once, while all three are alive, and kept (a dead one must not shift the others). Off when a placed actor of this level
// already uses one of those slots.
enum : int { kTripSlot0 = 505 };
struct Trip { uint32_t bb; uint32_t uid[3]; bool set; bool clash; };
Trip g_trip = {};
Trip g_trip_logged = {};                      // the last mapping logged (a key reset re-takes the same one: logged once per visit)
bool trip_log_new() {
    const bool same = g_trip_logged.set && g_trip_logged.bb == g_trip.bb && g_trip_logged.clash == g_trip.clash &&
                      std::memcmp(g_trip_logged.uid, g_trip.uid, sizeof g_trip.uid) == 0;
    g_trip_logged = g_trip;
    return !same;
}
int trip_slot_of(const char *a) {
    if (!g_on || !g_bully_on || !a || *(const unsigned short *)(a + 0x0c) != 215 || rd16(a, 0xce) >= 0) return -1;
    const uint32_t bb = (uint32_t)rd32(a, 0x3fc), uid = (uint32_t)rd32(a, 0x04);
    if (!bb) return -1;
    if (!g_trip.set || g_trip.bb != bb) {
        uint32_t u[8]; int n = 0; bool clash = false;
        for (char *x = (char *)_ZN8dActor_c4NextEPKS_(0); x; x = (char *)_ZN8dActor_c4NextEPKS_(x)) {
            const short sl = rd16(x, 0xce);
            if (sl >= kTripSlot0 && sl < kTripSlot0 + 3) clash = true;
            if (*(const unsigned short *)(x + 0x0c) == 215 && sl < 0 && (uint32_t)rd32(x, 0x3fc) == bb && n < 8) u[n++] = (uint32_t)rd32(x, 0x04);
        }
        if (n != 3) {                         // not the three of a fresh spawn: left alone for this visit (and not scanned again every frame)
            g_trip.bb = bb; g_trip.set = true; g_trip.clash = true; std::memset(g_trip.uid, 0, sizeof g_trip.uid);
            if (trip_log_new()) std::fprintf(stderr, "[cbs] bully triplet of big bully id %u: %d found, not 3: not synced\n", (unsigned)bb, n);
            return -1;
        }
        std::sort(u, u + 3);
        g_trip.bb = bb; g_trip.set = true; g_trip.clash = clash;
        for (int i = 0; i < 3; ++i) g_trip.uid[i] = u[i];
        if (trip_log_new()) std::fprintf(stderr, "[cbs] bully triplet of big bully id %u: record slots %d..%d by spawn order (ids %u %u %u)%s\n", (unsigned)bb, kTripSlot0,
                     kTripSlot0 + 2, (unsigned)u[0], (unsigned)u[1], (unsigned)u[2], clash ? ": OFF, a placed actor uses one of those slots" : "");
    }
    if (g_trip.clash) return -1;
    for (int i = 0; i < 3; ++i) if (g_trip.uid[i] == uid) return kTripSlot0 + i;
    return -1;
}

int g_nb = 0;
char *g_key_seen = nullptr; int g_keys_seen = 0;
void scan() {
    for (int i = 0; i < kSlots; ++i) g_boss[i].a = nullptr;
    g_nb = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        const unsigned id = *(const unsigned short *)(a + 0x0c);
        if (id == 282 && g_log && a != g_key_seen) {            // Chief Chilly's reward (a key): proof log only
            g_key_seen = a; ++g_keys_seen;
            std::fprintf(stderr, "[cbs] key actor seen at %d,%d,%d (#%d on this console) w=%lld\n", rd32(a, 0x5c) >> 12, rd32(a, 0x60) >> 12, rd32(a, 0x64) >> 12, g_keys_seen, wall_ms());
        }
        const BDesc *d = desc_of(id);
        if (!d) continue;
        if (id == 215 && !g_bully_on) continue;                    // coopbully1 control: the small Bully stays unsynced, as in 1.0.2
        short slot = rd16(a, 0xce);
        // bossdesc2: Eyerok's two hands are runtime spawns (slot -1); their record slot is 509 + the part number
        if (slot < 0 && id == 176) { const int part = rd32(a, 0x49c); if (part == 1 || part == 2) slot = (short)(509 + part); }
        if (slot < 0 && id == 215) slot = (short)trip_slot_of(a);     // coopbully1: the Big Bully's three
        if (slot < 0 || slot >= kSlots || is_player_ptr(a)) continue;
        if (id == 165 && *(const unsigned char *)(a + 0x414) == 0) continue;   // a plain Whomp that shares the class body
        Boss &b = g_boss[slot];
        if (b.a) continue;
        if (b.d != d) { b = Boss(); b.d = d; }
        b.a = a; ++b.seen; ++g_nb;
        // kbo2: a King that is down here is not ended by the group's dead mark: his defeat talk and the star run first
        if (g_deadmark && g_dead_cls[slot] == id && !b.killed_late && b.seen >= 3 && b.seen <= 900 && !(id == 189 && g_kbo_fix && rd32(a, 0x500) <= 0)) {
            b.killed_late = true;           // it has initialised (the loader ran its InitResources) and is new here (the record can arrive a little after the boss): end it the ordinary way
            // The star-marker table (the minimap's glints) holds the boss's pointer from its InitResources; the ROM's own end of a boss
            // (UntrackAndSpawnStar) releases the slot first. Without that the minimap reads a freed boss (measured: Whomp King).
            for (int m = 0; m < 12; ++m)
                if (STAR_MARKERS[m] == (dActor_c *)a) { signed char sl = (signed char)m; ((dActor_c *)a)->UntrackStar(sl); }
            _ZN7fBase_c18MarkForDestructionEv(a);
            // bossfix2: Eyerok's head spawned his two hands in its InitResources (mHandUniqueID1 / 2, +0x86c / +0x870): they go with it
            if (id == 176 && g_bf2 && rd32(a, 0x49c) == 0)
                for (int h = 0; h < 2; ++h)
                    if (char *hand = rd32(a, 0x86c + 4 * h) ? (char *)dActor_c::FindWithID((unsigned)rd32(a, 0x86c + 4 * h)) : nullptr) _ZN7fBase_c18MarkForDestructionEv(hand);
            ++g_st.dead_killed;
            if (g_log) std::fprintf(stderr, "[cbs] dead boss removed on arrival: cls=%u slot=%d w=%lld\n", id, (int)slot, wall_ms());
        }
    }
    for (int i = 0; i < kSlots; ++i) if (!g_boss[i].a && g_boss[i].d && g_boss[i].def_seen && !g_boss[i].bit_set) mark_dead_slot(i, g_boss[i]);
    for (int i = 0; i < kSlots; ++i) if (!g_boss[i].a) { g_boss[i].seen = 0; g_boss[i].known = false; g_boss[i].lw_valid = false; g_boss[i].heard = false; g_boss[i].rx.valid = false; g_boss[i].rx.first_done = false; }
}

int hp_of(const Boss &b) { return (int)read_field(b.a, b.d->f[b.d->hp]); }
uint32_t state_of(const Boss &b) { return read_field(b.a, b.d->f[b.d->state]); }

// COOPBOO5: the Big Boos that wait hidden for their small Boos (variants 0xc balcony, 0xd merry-go-round, 0xe lobby: d_a_trs.cpp
// func_ov063_021189f4, states 0..5). State 0 (func_ov063_02119074) keeps him hidden and out of reach until this console's count
// (+0x180) is 5 or more and the local player is within 742 units; only then does it make him visible, hittable, sized, at 3 HP.
// A state forced onto a copy skips that code: forced out of 0 he chased invisible and could not be hit, forced back into 0 he
// came out again at full health and was forced back, over and over. So state 0 is never forced in or out on these: a copy whose
// authority is out takes a count of 5 and comes out by the cartridge's own code once its player is near.
enum : int { kTrsCount = 0x180, kTrsVariant = 0x5cf, kTrsFlags = 0x5d4, kTrsDefeated = 0x80 };
bool hidden_big_boo(const Boss &b) {
    if (b.d->cls != 210) return false;
    const unsigned char v = (unsigned char)b.a[kTrsVariant];
    return v >= 0xc && v <= 0xe;
}
void big_boo_bring_out(Boss &b, int slot, const char *why) {
    int32_t cnt = rd32(b.a, kTrsCount);
    if (cnt >= 5) return;
    if (g_log) std::fprintf(stderr, "[cbs] big boo slot=%d count %d->5 here (%s): he comes out by his own code w=%lld\n", slot, (int)cnt, why, wall_ms());
    cnt = 5; std::memcpy(b.a + kTrsCount, &cnt, 4);
    reinterpret_cast<::daTrs_c *>(b.a)->func_ov063_02116244();   // his icon, as the fifth small Boo's talk would spawn it
}
// The group beat him while this copy was still hidden in state 0: it goes straight into his defeat (state 4, d_a_trs.cpp
// func_ov063_02118b98), the way his last hit ends (func_ov063_02118cd8: hidden, DEFEATED), so the icon gives the star where this
// console is the authority and the stairs open when a player comes near.
void big_boo_defeat_hidden(Boss &b, int slot, const char *why) {
    big_boo_bring_out(b, slot, why);
    b.a[0x5cc] = 4;
    uint16_t fl = (uint16_t)rd16(b.a, kTrsFlags); fl = (uint16_t)((fl & ~0x8u) | kTrsDefeated); std::memcpy(b.a + kTrsFlags, &fl, 2);
    if (g_log) std::fprintf(stderr, "[cbs] big boo slot=%d beaten by the group while hidden here: put in his defeat (%s) w=%lld\n", slot, why, wall_ms());
}

// The point of no return of each boss: after it the ROM only runs the defeat (and the reward). Latched once seen.
bool is_defeated(const Boss &b) {
    const int hp = hp_of(b);
    const uint32_t st = state_of(b);
    switch (b.d->cls) {
    case 165: return st == 8u || hp <= 0;                                           // Whomp King: the defeat state / HP 0
    case 189: return hp <= 0;                                                       // King Bob-omb
    case 218: return hp <= 0 && (st == 13u || st == 9u || st == 12u || st == 15u || st == 5u);   // Chilly: after the last blow (the revive happens before it)
    case 216: return hp == 0;                                                       // Big Bully: "alive" field 0 = dying or gone
    case 215: return hp == 0;                                                       // small Bully (coopbully1): the same
    case 210:                                                                       // Big Boo: no hits left and in his defeat
        // COOPBOO5: the hidden Big Boos (0xc..0xe) have states 0..5 and are beaten in state 4 (d_a_trs.cpp func_ov063_02118cd8 sets
        // it with the DEFEATED flag); state 6 and on is the Big Boo's Battle one (0xf)
        if (hidden_big_boo(b)) return hp <= 0 && (st == 4u || (rd16(b.a, kTrsFlags) & kTrsDefeated));
        return hp <= 0 && st >= 6u;
    case 248: return st == 7u;                                                      // Wiggler: DEAD
    case 198: return hp <= 0;                                                       // Goomboss: size index 0 = the defeat talk
    case 279: return hp <= 0 && st == 4u;                                           // Bowser: the defeat state
    case 176:                                                                       // bossfix2: Eyerok's head once both hands are gone (the hands are not covered)
        return g_bf2 && rd32(b.a, 0x49c) == 0 && read_global(0) == 0u;
    default: return false;
    }
}

// coopgoomboss2: the live minions (class 200, Goomba King minion type) that follow this Goomboss (target unique id +0x43c).
int count_minions(const char *boss) {
    int n = 0;
    const uint32_t uid = (uint32_t)rd32(boss, 0x04);
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (*(const unsigned short *)(a + 0x0c) == 200 && (uint32_t)rd32(a, 0x43c) == uid && !a[0x0f] && a[0x0e] != 2) ++n;
    return n;
}

char *local_body();
void log_sample(int slot, const Boss &b, int auth) {
    char line[768]; int n = std::snprintf(line, sizeof line, "[cbs] w=%lld t=%u slot=%d cls=%u pos=%d,%d,%d auth=%d me=%d hp=%d st=%u",
                                          wall_ms(), g_ticks, slot, (unsigned)b.d->cls, rd32(b.a, 0x5c) >> 12, rd32(b.a, 0x60) >> 12, rd32(b.a, 0x64) >> 12,
                                          auth, g_my, hp_of(b), state_of(b));
    for (int k = 2; k < b.d->nf && n < (int)sizeof line - 20; ++k) n += std::snprintf(line + n, sizeof line - n, " f%d=%u", k, read_field(b.a, b.d->f[k]));
    if (b.d->cls == 198) n += std::snprintf(line + n, sizeof line - n, " goombas_live=%d goombas_cnt=%u", count_minions(b.a), (unsigned)(unsigned char)b.a[0x602]);
    if (b.d->cls == 210)   // COOPBOO5: the Big Boo's count, VISIBLE flag, collider off bit (dCc_c flags +0x18 of mdCcAcPos_c +0x184)
        n += std::snprintf(line + n, sizeof line - n, " var=%x cnt=%d vis=%d cc_off=%d", (unsigned)(unsigned char)b.a[0x5cf], (int)rd32(b.a, 0x180),
                           (rd16(b.a, 0x5d4) & 0x8) ? 1 : 0, (int)(rd32(b.a, 0x19c) & 1));
    if (b.d->cls == 279) {
        n += std::snprintf(line + n, sizeof line - n, " talk=%d cnt=%d", (int)(signed char)b.a[0x424], (int)rd16(b.a, 0x3fe));
        // bowser2: hold mode, the intro camera's step and the camera itself (flags; 0x8 = held by the intro), so a frozen camera shows
        const char *cam = (const char *)data_0209f318;
        { const char *ar = (unsigned char)b.a[0x414] == 1 ? kpa_arena() : nullptr;
          n += std::snprintf(line + n, sizeof line - n, " y=%d gy=%d vs=%d g=%d disc=%d,%d", rd32(b.a, 0x60) >> 12, rd32(b.a, 0x3cc) >> 12, rd32(b.a, 0xa8) >> 12, rd32(b.a, 0x9c),
                             ar ? rd16(ar, 0x8c) : 0, ar ? rd16(ar, 0x90) : 0); }
        n += std::snprintf(line + n, sizeof line - n, " hold=%d grab=%s shown=%d ang=%d cs=%d cam=0x%x campos=%d,%d,%d",
                           rd32(b.a, 0x410), rd32(b.a, 0x3a4) ? "y" : "n", (int)b.kpa_shown, (int)(uint16_t)rd16(b.a, 0x8e), (int)(unsigned char)b.a[0x444],
                           cam ? (unsigned)rd32(cam, 0x154) : 0u, cam ? rd32(cam, 0x8c) >> 12 : 0, cam ? rd32(cam, 0x90) >> 12 : 0, cam ? rd32(cam, 0x94) >> 12 : 0);
    }
    if (b.d->cls == 189) n += std::snprintf(line + n, sizeof line - n, " yaw=%04x", (unsigned)(uint16_t)rd16(b.a, 0x8e));   // kbo2
    if (b.d->cls == 248 || b.d->cls == 176 || b.d->cls == 198) {   // bossfix2: who the boss talks to, my player's talk and the camera hold
        const int toff = b.d->cls == 248 ? 0x8c8 : b.d->cls == 176 ? 0x490 : 0x5c8;
        char *tp = nullptr; std::memcpy(&tp, b.a + toff, sizeof tp);
        char *lb = local_body();
        void *partner = nullptr; if (lb) std::memcpy(&partner, lb + 0x368, sizeof partner);
        const char *cam = (const char *)data_0209f318;
        n += std::snprintf(line + n, sizeof line - n, " talker=%s mytalk=%s talkst=%d cam=0x%x", !tp ? "none" : tp == lb ? "me" : "friend",
                           partner == (void *)b.a ? "boss" : partner ? "other" : "none", lb ? ((Player *)lb)->GetTalkState() : -99,
                           cam ? (unsigned)rd32(cam, 0x154) : 0u);
        if (b.d->cls == 248) n += std::snprintf(line + n, sizeof line - n, " drive=%d", (int)b.a[0x8e1]);
        if (b.d->cls == 176) n += std::snprintf(line + n, sizeof line - n, " part=%d sub=%d work1=%d hands=%u", rd32(b.a, 0x49c), rd32(b.a, 0x4a0), rd32(b.a, 0x498), read_global(0));
        if (b.d->cls == 198) n += std::snprintf(line + n, sizeof line - n, " sub=%d", (int)(unsigned char)b.a[0x603]);
    }
    n += std::snprintf(line + n, sizeof line - n, " tgt=%d", coop_target_seat_of_slot(slot));
    if (is_bully(b.d->cls)) n += std::snprintf(line + n, sizeof line - n, " claim=%d mover=%d", bully_claim_seat(slot, hub_now()), coop_move_owner(slot));
    std::fprintf(stderr, "%s\n", line);
}

// After a state was forced onto a copy (bossdesc1). Big Boo's hit state takes its health point only on the first frame of the state
// (timer +0x100 == 0, src/unnamed/ov063/func_ov063_021177b0.c); the authority has taken it already and the merged HP is the truth, so a
// forced copy skips that frame.
void post_force(Boss &b) {
    if (b.d->cls == 279) {                 // a forced state is never a held one: no stale held-player pointer on this copy
        const int32_t sub = rd32(b.a, 0x410);
        if (sub != 1 && sub != 2) { void *z = nullptr; std::memcpy(b.a + 0x3a4, &z, sizeof z); }
    }
    if (b.d->cls == 210) {
        uint16_t t = 0; std::memcpy(&t, b.a + 0x100, 2);
        if (t == 0) { t = 1; std::memcpy(b.a + 0x100, &t, 2); }
    }
}

// ---------------------------------------------------------------------------
// coopkbo1: King Bob-omb and Whomp King on a console that is NOT the boss's authority (Tango's 4-player field report, 11-10).
// State ids are kbo_states() order: 0 hurt (6ffc, enter takes a health point), 1 intro talk (700c, its tick reloads the boss music
// when the talk is over), 5 bomb throw (704c, enter spawns a bob-omb into his hand), 6 defeat talk (705c, tick spawns the star),
// 8 carried by a player (707c), 9 thrown (708c, the landing takes a health point), 17 idle (710c).
// ---------------------------------------------------------------------------
enum : uint32_t { kKboHurt = 0, kKboTalk = 1, kKboThrowBomb = 5, kKboDefeat = 6, kKboHeld = 8, kKboThrown = 9, kKboIdle = 17 };

// A copy taken out of the bomb throw before its throw frame keeps the bob-omb pinned to his hand (func_ov078_02125de0 re-pins every
// unthrown one each frame). Let go of it the ROM's own way (func_ov078_02123864, what a hit does) and mark it thrown.
void kbo_release_bombs(char *a) {
    for (int i = 0; i < 2; ++i) {
        const uint32_t id = (uint32_t)rd32(a, 0x424 + 4 * i);
        if (!id || a[0x42c + i]) continue;
        char *bmb = (char *)dActor_c::FindWithID(id);
        if (!bmb) continue;
        const int32_t zero = 0; std::memcpy(bmb + 0x3e0, &zero, 4); bmb[0x3f6] = 1;
        void *nul = nullptr; std::memcpy(bmb + 0xc8, &nul, sizeof nul);
        a[0x42c + i] = 1;
        if (g_log) std::fprintf(stderr, "[cbs] kbo bomb released (copy left the throw before the throw frame) w=%lld\n", wall_ms());
    }
}

// Carried on another console: without this the copy stays in its walk state while the pose lifts it off the floor, and its own
// Behavior (d_a_bombking.cpp: not on the ground in 703c) sends it into the JUMP-back-to-the-arena state. The copy is put into the
// carried state with the carrier's body on this console as the holder and the flags the carrying code would set (0x4000 held, 0x100
// so the carried tick waits instead of throwing). The throw itself arrives as state 9 from the carrier.
bool kbo_show_held(Boss &b, int seat) {
    char *body = (char *)coop_target_body_for_seat(seat);
    if (!body || seat == g_my) return false;
    if (g_kbo_fix) kbo_release_bombs(b.a);    // kbo2: carried, he holds no bomb (the carried state pins every unthrown one to his hand)
    _ZN12daBombking_c19KingBobOmb_SetStateEPv(b.a, kbo_id_to_ptr(kKboHeld));
    std::memcpy(b.a + 0x494, &body, sizeof body);
    uint32_t fl = (uint32_t)rd32(b.a, 0xb0);
    fl = (fl | 0x4000u | 0x100u) & ~(0x400u | 0x2000u);
    std::memcpy(b.a + 0xb0, &fl, 4);
    return true;
}
void kbo_clear_held(Boss &b) {
    char *holder = nullptr; std::memcpy(&holder, b.a + 0x494, sizeof holder);
    const int me = (int)data_0209f250;
    if (!holder || (me >= 0 && me < 16 && holder == (char *)data_0209f394[me])) return;
    void *nul = nullptr; std::memcpy(b.a + 0x494, &nul, sizeof nul);
    uint32_t fl = (uint32_t)rd32(b.a, 0xb0); fl &= ~(0x4000u | 0x100u | 0x400u | 0x2000u); std::memcpy(b.a + 0xb0, &fl, 4);
}

// The defeat talk belongs to the authority's player only. On every other console it froze the player mid-jump with the boss's
// last words. A copy that reaches HP 0 goes straight to its end: King Bob-omb into the defeat state, whose tick finds no talk on
// this console and ends (the star is suppressed here, the authority's arrives by announcement); Whomp King to step 3 of his
// defeat (the burst). Only before this console's own copy started a talk: a talk that was started is left to finish.
// kbo2: THE DEFEAT TALK ON EVERY SCREEN (Tango: the defeat always plays the cartridge talk and drops the star on every screen). A copy
// whose King is down runs the cartridge's own ending with its OWN player: the hurt state's tick on the ground turns him to the player
// (ClosestPlayer answers the local player for a downed King, coop_boss_prefer_local), StartTalk / ShowMessage the defeat line with
// mTalkingPlayer (+0x430), the defeat state (705c) waits for that talk to end, then UntrackAndSpawnStar (only the owner's star is real,
// star_is_mine) and his end. 1.0.3 skipped the talk on every console but the authority's and left +0x430 on a friend's body, whose talk
// never runs here: one console ended him at once with no talk, the other stood in the defeat state for ever (bug 21).
char *local_body() {
    const int me = (int)data_0209f250;
    return (me >= 0 && me < 16) ? (char *)data_0209f394[me] : nullptr;
}
void kbo_talker_local(Boss &b) {
    char *mine = local_body();
    if (!mine) return;
    char *cur = nullptr; std::memcpy(&cur, b.a + 0x430, sizeof cur);
    if (cur != mine) std::memcpy(b.a + 0x430, &mine, sizeof mine);
}
// Every tick for a King that is down on this console (mine or not). States 0 (hurt), 9 (thrown) and 6 (the defeat talk) end in the
// defeat by their own code; anything else (a copy that was walking when the last hit's HP arrived, a copy shown carried) is put into
// the hurt state, whose entry takes a point (put back at 0).
void kbo_defeat_tick(Boss &b) {
    if (b.d->cls != 189 || hp_of(b) > 0) return;
    const uint32_t st = state_of(b);
    if (st == kKboDefeat) return;          // the talk is running (or ending) with the player it was started with
    kbo_talker_local(b);
    if (st == kKboHurt || st == kKboThrown) return;
    if (st == 7u || st == kKboHeld) {      // held by MY player: his throw (state 9, the landing) ends it the cartridge's way
        char *holder = nullptr; std::memcpy(&holder, b.a + 0x494, sizeof holder);
        if (holder && holder == local_body()) return;
    }
    kbo_release_bombs(b.a);
    kbo_clear_held(b);
    _ZN12daBombking_c19KingBobOmb_SetStateEPv(b.a, kbo_id_to_ptr(kKboHurt));
    write_field(b.a, *b.d, b.d->hp, 0u);
    b.synced_hp = 0; b.lw_valid = false;
    if (g_log) std::fprintf(stderr, "[cbs] kbo down here in st=%u: hurt state, the defeat talk runs with my player w=%lld\n", st, wall_ms());
}

void skip_defeat_talk(Boss &b, bool hp_just_written) {
    if (b.d->cls == 189 && g_kbo_fix) { kbo_defeat_tick(b); return; }
    if (b.d->cls == 189) {
        if (hp_of(b) > 0) return;
        const uint32_t st = state_of(b);
        if (st == kKboDefeat) return;
        // states 0 and 9 start the talk from their own tick once he is on the ground: only safe when the HP was set to 0 just now
        if (!hp_just_written && (st == kKboHurt || st == kKboThrown)) return;
        kbo_release_bombs(b.a);
        kbo_clear_held(b);
        give_player_ptrs(b);
        _ZN12daBombking_c19KingBobOmb_SetStateEPv(b.a, kbo_id_to_ptr(kKboDefeat));
        if (g_log) std::fprintf(stderr, "[cbs] kbo defeat talk skipped on this console (not the authority) from st=%u w=%lld\n", st, wall_ms());
    } else if (b.d->cls == 165) {
        if (state_of(b) == 8u && (unsigned char)b.a[0x40c] == 0) {
            b.a[0x40c] = 3;
            if (g_log) std::fprintf(stderr, "[cbs] whomp king defeat talk skipped on this console (not the authority) w=%lld\n", wall_ms());
        }
    }
}

// ---------------------------------------------------------------------------
// bowser2: Bowser on a console that is NOT the authority (Tango's 4-player field report, 11-11).
// Bowser fields: +0x410 hold mode (0 free, 1 swung by the player in +0x3a4, 2 let go now, 3 let go after the animation), +0x425 the
// held handler's step, +0x3a8 his tail's unique id, +0x427 bounce on landing, +0x444 the intro camera's step (4 = done), +0x424 the
// intro talk's step. The tail (daKpaTail_c): +0x110 state (0 free, 1 cooldown, 2 holding), +0x10c the player holding it.
// ---------------------------------------------------------------------------
char *kpa_tail(Boss &b) {
    const uint32_t id = (uint32_t)rd32(b.a, 0x3a8);
    return id ? (char *)dActor_c::FindWithID(id) : nullptr;
}

// Held by a friend: the record says hold mode 1 and the carrier's console is the authority (the grab is its claim). Without this the
// copy kept its own walk, stood in its normal animation and only the position snap moved it. The copy is put into the ROM's own held
// mode with the friend's body on this console as the holder: the held handler (d_a_kpa.cpp func_ov060_02112724) plays the grab and
// swing animations and pins him 160 units out from the holder's hands every frame. His tail is parked holding nobody, so it never
// switches his body collider back on, never drops the friend's body and cannot be taken by my player at the same time.
bool kpa_show_held(Boss &b, int seat, unsigned now) {
    if (seat == g_my || rd32(b.a, 0x410) != 0) return false;
    char *body = (char *)coop_target_body_for_seat(seat);
    if (!body) return false;
    const int32_t one = 1; std::memcpy(b.a + 0x410, &one, 4);
    b.a[0x425] = 0;
    std::memcpy(b.a + 0x3a4, &body, sizeof body);
    if (char *t = kpa_tail(b)) {
        const int32_t holding = 2; std::memcpy(t + 0x110, &holding, 4);
        void *nul = nullptr; std::memcpy(t + 0x10c, &nul, sizeof nul);
    }
    b.kpa_quiet_t = 0;
    b.kpa_shown = true; b.kpa_seat = seat; b.kpa_t = now; b.kpa_yaw = rd16(body, 0x8e);
    const int16_t zero = 0; std::memcpy(body + 0x69c, &zero, 2);
    return true;
}

// The record says the friend let go (or records stopped): the ROM's own release on this copy. Mode 2 throws at once, mode 3 waits for
// the let-go animation (the friend dropped him); either way the release (func_ov060_021125f0) launches him from the swing speed and
// angle he has, puts him in the thrown state and sends the tail to its cooldown. The holder is forgotten the way the tail does it
// (func_ov060_021123c8: bounce on landing, no grabbed player). The position snap of the records then keeps the flight honest.
// quiet: the friend never let go as far as this console knows (records stopped, the body went, the claim ran out): his swing speed and
// tilt are cleared first, so the release launches him with no speed (he drops where he hangs, no throw from the local swing), and a
// hit on this copy in the next 5 s is not reported (a throw that never happened must not cost him a point).
void kpa_release(Boss &b, int mode, const char *why, unsigned now = 0, bool quiet = false) {
    b.kpa_shown = false;
    if (rd32(b.a, 0x410) != 1) return;
    if (quiet) {
        const int32_t z = 0; const int16_t z16 = 0;
        std::memcpy(b.a + 0x3f0, &z, 4); std::memcpy(b.a + 0x8c, &z16, 2);
        b.kpa_quiet_t = now ? now : 1u;
    }
    const int32_t m = mode; std::memcpy(b.a + 0x410, &m, 4);
    void *nul = nullptr; std::memcpy(b.a + 0x3a4, &nul, sizeof nul);
    b.a[0x427] = 1;
    if (g_log) std::fprintf(stderr, "[cbs] bowser let go on this copy (%s, mode %d%s, swing %d) pos=%d,%d,%d w=%lld\n", why, mode, quiet ? ", quiet: no launch" : "", rd32(b.a, 0x3f0),
                            rd32(b.a, 0x5c) >> 12, rd32(b.a, 0x60) >> 12, rd32(b.a, 0x64) >> 12, wall_ms());
}

// Every tick while shown held: the holder body's turn since the last tick is its swing speed (a friend's body on this console has no
// swing of its own: its Behavior is skipped and the pose comes from the wire), which the held handler reads for his tilt and the
// throw. Records that stop for 1.5 s, a lost body or this console becoming the authority end it.
void kpa_tick(Boss &b, bool mine, unsigned now) {
    if (!b.kpa_shown) return;
    if (rd32(b.a, 0x410) != 1) { b.kpa_shown = false; return; }
    if (mine) { kpa_release(b, 2, "this console became the authority", now, true); return; }
    if ((unsigned)(now - b.kpa_t) > 3000u) { kpa_release(b, 2, "no held record for 3 s", now, true); return; }
    char *body = (char *)coop_target_body_for_seat(b.kpa_seat);
    if (!body) { kpa_release(b, 2, "the holder's body is gone", now, true); return; }
    std::memcpy(b.a + 0x3a4, &body, sizeof body);
    const int16_t yaw = rd16(body, 0x8e);
    const int16_t turn = (int16_t)(yaw - b.kpa_yaw);
    b.kpa_yaw = yaw;
    std::memcpy(body + 0x69c, &turn, 2);
    if (g_log && (g_ticks % 15) == 0)
        std::fprintf(stderr, "[cbs] bowser held by seat %d on this copy: step=%d swing=%d tilt=%d ang=%d holder=%d,%d,%d bowser=%d,%d,%d w=%lld\n",
                     b.kpa_seat, (int)(unsigned char)b.a[0x425], (int)turn, (int)rd16(b.a, 0x8c), (int)(uint16_t)rd16(b.a, 0x8e),
                     rd32(body, 0x5c) >> 12, rd32(body, 0x60) >> 12, rd32(body, 0x64) >> 12, rd32(b.a, 0x5c) >> 12, rd32(b.a, 0x60) >> 12, rd32(b.a, 0x64) >> 12, wall_ms());
}

// The intro camera (func_ov060_02111f08) takes the camera at step 0 (dCamera flag 0x8) on the first frame of the intro wait and gives
// it back only at step 4, which the end of the intro talk runs. A copy forced out of the intro by the authority's record (a late
// arrival: the authority's intro was over) never got there and the camera stayed held. Step 4 is run here for any copy outside the
// two intro states whose intro camera started and did not finish.
void kpa_intro_camera_done(Boss &b) {
    const uint32_t st = state_of(b);
    const unsigned cs = (unsigned char)b.a[0x444];
    if (st == 5u || st == 6u || cs == 0u || cs >= 4u) return;
    b.a[0x444] = 4;
    char *cam = (char *)data_0209f318;
    const uint32_t before = cam ? (uint32_t)rd32(cam, 0x154) : 0u;
    if (cam) { const uint32_t f = before & ~8u; std::memcpy(cam + 0x154, &f, 4); }
    if (g_log) std::fprintf(stderr, "[cbs] bowser intro camera given back on this copy (st=%u intro camera step %u, cam flags 0x%x -> 0x%x) w=%lld\n",
                            st, cs, before, cam ? (unsigned)rd32(cam, 0x154) : 0u, wall_ms());
}

// ---------------------------------------------------------------------------
// bowser2, Bowser in the Fire Sea (variant 1). His landing from a jump or from the recovery leap puts him in the arena-tilt state
// (0x13), which writes the tilting disc's (KOOPA2BG, actor 0xa6) angular speeds every frame for 186 frames and zeroes its speeds and
// angles at the end (d_a_kpa.cpp func_ov060_02112bfc). The disc turns by its own speeds every frame (d_a_kpa2_bg.cpp Behavior) and
// is not synced: each console's disc is driven by its own Bowser copy. Three ways a copy went wrong (Tango 11-11: on one console
// Bowser hung in the air after the platform rocked while a friend grabbed him off the floor):
//  - a copy forced OUT of the tilt state by the authority's record before its own tilt ended never zeroed the disc: it kept turning
//    or stayed tipped, and his ground snap (states that may not leave the floor go back to the last floor point every frame they are
//    off it, d_a_kpa.cpp func_ov060_021128c0) held him at a floor point that was no longer there, in the air;
//  - a copy forced INTO the tilt state never found the disc (+0x3ac is only recorded by its own landing) and left it at once.
//  - the records carry his gravity (+0x9c): a value taken during the authority's recovery leap (0 there) written onto a copy in any
//    other state left that copy with no gravity. Outside the recovery (2) and the defeat (4) the ROM's gravity is always -0x2000.
// ---------------------------------------------------------------------------
char *kpa_arena() {
    for (char *x = (char *)_ZN8dActor_c4NextEPKS_(0); x; x = (char *)_ZN8dActor_c4NextEPKS_(x))
        if (*(const unsigned short *)(x + 0x0c) == 0xa6) return x;
    return nullptr;
}

void kpa_bfs_guard(Boss &b, bool mine, unsigned now) {
    const uint32_t st = state_of(b);
    // the disc is left level and still whenever this copy is not tilting it (the ROM's own end of the tilt)
    if ((unsigned char)b.a[0x414] == 1 && st != 0x13u) {
        if (char *ar = kpa_arena()) {
            if (rd16(ar, 0x31e) || rd16(ar, 0x320) || rd16(ar, 0x322) || rd16(ar, 0x8c) || rd16(ar, 0x90)) {
                if (g_log) std::fprintf(stderr, "[cbs] bowser BFS disc left tilted on this console (st=%u speeds %d,%d,%d angles %d,%d): levelled w=%lld\n",
                                        st, rd16(ar, 0x31e), rd16(ar, 0x320), rd16(ar, 0x322), rd16(ar, 0x8c), rd16(ar, 0x90), wall_ms());
                const int16_t z = 0;
                std::memcpy(ar + 0x31e, &z, 2); std::memcpy(ar + 0x320, &z, 2); std::memcpy(ar + 0x322, &z, 2);
                std::memcpy(ar + 0x8c, &z, 2); std::memcpy(ar + 0x90, &z, 2);
            }
        }
    }
    if (mine) { b.kpa_air_t = 0; return; }
    // gravity outside the recovery and the defeat
    if (st != 2u && st != 4u && (rd32(b.a, 0x9c) != -0x2000 || rd32(b.a, 0xa0) != -0x3c000)) {
        if (g_log) std::fprintf(stderr, "[cbs] bowser copy gravity %d / %d in st=%u: the ROM's -0x2000 / -0x3c000 put back w=%lld\n", rd32(b.a, 0x9c), rd32(b.a, 0xa0), st, wall_ms());
        const int32_t g = -0x2000, tv = -0x3c000;
        std::memcpy(b.a + 0x9c, &g, 4); std::memcpy(b.a + 0xa0, &tv, 4);
    }
    // the watchdog: far above the authority's Bowser for longer than any jump of his (a jump is about 1.5 s) while neither is in the
    // recovery, held or defeated: put down where the authority has him, his floor point with him, falling normally
    const BDesc &d = *b.d;
    const Rx &x = b.rx;
    // nor in the intro (5, 6: the intro talk guard owns the state there) nor the arena tilt (0x13: his rocking sits 150 to 230 units
    // above the authority's for about 1.5 s as a matter of course), on this copy or on the record
    if (!x.valid || d.pos < 0 || b.kpa_shown || st == 2u || st == 4u || st == 5u || st == 6u || st == 0x13u || rd32(b.a, 0x410) != 0) { b.kpa_air_t = 0; return; }
    const uint32_t rs = x.w[d.state];
    if (rs == 2u || rs == 4u || rs == 1u || rs == 5u || rs == 6u || rs == 0x13u || x.w[2] != 0u) { b.kpa_air_t = 0; return; }
    const int32_t ry = (int32_t)x.w[d.pos + 1];
    const int dy = (rd32(b.a, 0x60) - ry) >> 12;
    if (dy < 150) { b.kpa_air_t = 0; return; }
    if (!b.kpa_air_t) { b.kpa_air_t = now ? now : 1u; return; }
    if ((unsigned)(now - b.kpa_air_t) < 3000u) return;
    b.kpa_air_t = 0;
    for (int k = 0; k < 3; ++k) { const int32_t v = (int32_t)x.w[d.pos + k]; std::memcpy(b.a + 0x5c + 4 * k, &v, 4); std::memcpy(b.a + 0x3c8 + 4 * k, &v, 4); }
    const int32_t zero = 0, g = -0x2000;
    std::memcpy(b.a + 0xa8, &zero, 4); std::memcpy(b.a + 0x98, &zero, 4); std::memcpy(b.a + 0x9c, &g, 4);
    if (rs != st && rs <= 0x13u) {
        const int32_t ns = (int32_t)rs; std::memcpy(b.a + 0x40c, &ns, 4); b.a[0x423] = 0;
    }
    if (g_log) std::fprintf(stderr, "[cbs] bowser airborne watchdog: this copy was %d units above the authority's for 3 s (st=%u, authority st=%u): put down at %d,%d,%d w=%lld\n",
                            dy, st, rs, rd32(b.a, 0x5c) >> 12, rd32(b.a, 0x60) >> 12, rd32(b.a, 0x64) >> 12, wall_ms());
}

// ---------------------------------------------------------------------------
// bossfix2: talks a forced state must not cut. Wiggler: his intro (DEMOCALL 1, MOVE_STOP 2) and a hurt talk (DAMAGE 5, DAMAGETALK 6),
// both with MY player in +0x8c8, and MOVE_START (3) while he still drives the camera (+0x8e1: it waits there for the talk to end and
// then gives the camera back, d_a_hanachan.cpp StateMoveStartMain). Eyerok's head: its talk state (record b0ac, id 14) once it took
// MY player (+0x490) and started (+0x4a0 = 1): the player is held (SetNoControlState) and the camera bit 8 set until its end.
// A forced state there left my player frozen in the talk with the camera held (AUDIT-BOSSES H1 / H2).
// ---------------------------------------------------------------------------
enum : uint32_t { kEyeTalk = 14 };
bool wig_talk_here(const Boss &b) {
    if (b.d->cls != 248) return false;
    char *pl = nullptr; std::memcpy(&pl, b.a + 0x8c8, sizeof pl);
    if (!pl || pl != local_body()) return false;
    const uint32_t st = (uint32_t)rd32(b.a, 0x8c4);
    // bossfix3: MOVE_STOP (2) and DAMAGETALK (6) count only when this copy started the talk here (it drives the camera, +0x8e1): a copy
    // put into 2 by its first record skipped DEMOCALL's message, waited for a talk that never existed and was held 60 s
    return st == 1u || st == 5u || ((st == 2u || st == 3u || st == 6u) && b.a[0x8e1]);
}
// bossfix4: the cartridge's own intro check (StateDemoWaitMain): a player more than 300 units (0x12c000) above Wiggler. Asked of MY
// body here; the ROM asks ClosestPlayer, which in a room is the target seat's body, a friend's on most consoles.
bool wig_intro_due(const Boss &b, const char *me) {
    return me && rd32(me, 0x60) > rd32(b.a, 0x60) + 0x12c000;
}
bool eye_talk_here(const Boss &b) {
    if (b.d->cls != 176 || rd32(b.a, 0x49c) != 0 || state_of(b) != kEyeTalk || rd32(b.a, 0x4a0) == 0) return false;
    char *pl = nullptr; std::memcpy(&pl, b.a + 0x490, sizeof pl);
    return pl && pl == local_body();
}
// The camera a talk on this copy took is given back when a state is forced on it anyway (a hold past its time).
void give_camera_back(Boss &b, const char *why) {
    char *cam = (char *)data_0209f318;
    const uint32_t before = cam ? (uint32_t)rd32(cam, 0x154) : 0u;
    if (cam && (before & 8u)) { const uint32_t f = before & ~8u; std::memcpy(cam + 0x154, &f, 4); }
    if (b.d->cls == 248) b.a[0x8e1] = 0;
    if (g_log) std::fprintf(stderr, "[cbs] %s copy: camera given back (%s, cam flags 0x%x) w=%lld\n", b.d->name, why, before, wall_ms());
}

void apply_rx(int slot, Boss &b, unsigned now, bool mine) {
    Rx &x = b.rx;
    if (!x.valid || !x.fresh) return;
    x.fresh = false;
    const BDesc &d = *b.d;
    ++g_st.recs_rx;
    // coopbully1: the console that owns a bully (its player knocked it) is never pulled back by a record that was sent before it took it
    if (g_bully_on && is_bully(d.cls) && mine) return;
    // HP only goes down.
    const int lhp = hp_of(b);
    int rhp = (int)x.w[d.hp];
    if (d.cls == 218 && rhp == 0xff) rhp = 0;                            // kbo2: Chilly's wrapped count is "none left"
    if (((d.f[d.hp].kind & kS8) || d.cls == 189) && rhp < 0) rhp = 0;   // a signed byte HP (and King Bob-omb's, coopkbo1): anything at or below 0 is "down"; do not chase a double decrement
    // Chief Chilly's knock-down count also goes UP once: after the last fall he waits for a hit and, if none comes, his state function
    // sets it back to 1 (src/func_ov073_02120910.c). Both copies do that by themselves; a record from the other state would put the
    // revived copy back to 0, so HP is only compared when both are in the same state, and 0 -> 1 is accepted in the same state.
    const bool hold = d.cls == 218 && state_of(b) != x.w[d.state];
    if (!hold && rhp >= 0 && rhp < lhp) {
        write_field(b.a, d, d.hp, (uint32_t)rhp);
        b.synced_hp = rhp;
        ++g_st.applied_hp;
        if (g_log) std::fprintf(stderr, "[cbs] apply-hp slot=%d hp %d->%d from seat %d w=%lld\n", slot, lhp, rhp, x.from, wall_ms());
        if (!mine && rhp <= 0) skip_defeat_talk(b, true);
    } else if (!hold && d.cls == 218 && lhp == 0 && rhp == 1) {
        write_field(b.a, d, d.hp, 1u);
        b.synced_hp = 1;
        ++g_st.applied_hp;
        if (g_log) std::fprintf(stderr, "[cbs] apply-hp slot=%d hp 0->1 (revive) from seat %d w=%lld\n", slot, x.from, wall_ms());
    } else if (!hold && b.synced_hp > rhp && rhp >= 0) {
        b.synced_hp = rhp;
    }
    // counters that only go up (Big Bully's bullies killed): write_field refuses anything that is not larger
    for (int k = 0; k < d.nf; ++k)
        if ((d.f[k].kind & (kMaxUp | kGlobal)) && value_ok(d, k, x.w[k])) write_field(b.a, d, k, x.w[k]);
    // The state: forced on the first record this console ever takes, otherwise after two in a row that disagree.
    const uint32_t ls = state_of(b), rs = x.w[d.state];
    bool force = false;
    // King Bob-omb's two states in which HE holds a body (706c, 707c): the held pointer is a console-local one, so those states
    // are never forced onto a copy; the pose still follows through the mover path.
    // Bowser's tail grab is the same hand-off (sub-mode +0x410 = 1 swung, 2 thrown: the held pointer +0x3a4 is a console-local one).
    const bool carry_state = (d.cls == 189 && (rs == 7 || rs == 8)) || (d.cls == 279 && (x.w[2] == 1u || x.w[2] == 2u));
    if (ls != rs) { ++b.mism; ++g_st.state_mismatch; } else b.mism = 0;
    if (!b.heard) force = ls != rs;
    else if (b.mism >= 2) force = true;
    if (carry_state) force = false;
    // bowser2: held by a friend: shown attached to his hands (kpa_show_held); let go when the record stops saying so
    if (d.cls == 279 && !mine && g_kpa_fix) {
        // not while this copy's own intro talk runs (states 5 / 6, talk started): the talk ends first, the next held record shows it
        const bool own_talk = (ls == 5u || ls == 6u) && (unsigned char)b.a[0x424] >= 1u && (unsigned char)b.a[0x424] <= 2u;
        if (x.w[2] == 1u && hp_of(b) > 0 && ls != 4u && (b.kpa_shown || !own_talk)) {
            if (b.kpa_shown) b.kpa_t = now;
            else if (kpa_show_held(b, x.from, now)) {
                ++g_st.applied_state;
                if (g_log) std::fprintf(stderr, "[cbs] bowser held by seat %d shown on this copy (was st=%u) w=%lld\n", x.from, ls, wall_ms());
            }
        } else if (b.kpa_shown) {
            kpa_release(b, x.w[2] == 3u ? 3 : 2, "the holder let go");
        }
        if (b.kpa_shown) force = false;
    }
    // bowser2: this copy's own intro talk is under way (states 5 / 6 with the talk started): a forced state would leave my player inside
    // a talk that never ends and the camera held. The talk is left to finish (its end goes to the idle state and gives the camera back);
    // the next records force the fight's state then. 20 s at most.
    if (d.cls == 279 && !mine && g_kpa_fix && (ls == 5u || ls == 6u) && rs != 5u && rs != 6u) {
        const unsigned ts = (unsigned char)b.a[0x424];
        if (ts >= 1u && ts <= 2u) {
            if (!b.kpa_talk_t) {
                b.kpa_talk_t = now ? now : 1u;
                if (g_log) std::fprintf(stderr, "[cbs] bowser intro talk running on this copy (talk step %u): the authority's st=%u waits for its end w=%lld\n", ts, rs, wall_ms());
            }
            if ((unsigned)(now - b.kpa_talk_t) < 20000u) force = false;
        }
    }
    // bossfix2: a Wiggler or Eyerok talk with my player on this copy is left to finish (60 s at most); the next records force then
    // bossfix3: the stamp is cleared whenever no talk runs here (it was cleared only on a record that disagreed, so a talk that began
    // while the states agreed kept the last talk's stamp, looked 60 s old and was cut at once)
    if (g_bf2 && (d.cls == 248 || d.cls == 176)) {
        unsigned &t = d.cls == 248 ? b.wig_talk_t : b.eye_talk_t;
        if (mine || !(d.cls == 248 ? wig_talk_here(b) : eye_talk_here(b))) t = 0;
        else if (ls != rs) {
            if (!t) {
                t = now ? now : 1u;
                if (g_log) std::fprintf(stderr, "[cbs] %s talk with my player running on this copy (st=%u): the authority's st=%u waits for its end w=%lld\n", d.name, ls, rs, wall_ms());
            }
            if ((unsigned)(now - t) < 60000u) force = false;
            else if (force) give_camera_back(b, "talk held past 60 s");
        }
    }
    // bossfix2 (H1): the authority's hurt talk (5 / 6) is entered here as where it ends (write_field): a copy that is already there or past
    // it (MOVE_START / MOVE with health left, or DEAD already run: mDefeated +0x8e0) is not forced again, record after record
    if (g_bf2 && d.cls == 248 && !mine && (rs == 5u || rs == 6u)) {
        const bool last = (unsigned char)b.a[0x8db] <= 1u;
        if (last ? b.a[0x8e0] != 0 : (ls == 3u || ls == 4u)) { force = false; b.mism = 0; }
    }
    // bossfix3: a copy that has run DEAD (mDefeated set) is not sent through it again by the authority's own DEAD (7) that follows
    // the stomper's talk: its star call and poof ran once here already.
    if (g_bf2 && d.cls == 248 && !mine && rs == 7u && b.a[0x8e0] != 0 && ls != 7u) { force = false; b.mism = 0; }
    // bossfix4: a copy still waiting in DEMOWAIT (0) while MY player is up where the cartridge starts the intro is not pushed past the
    // intro by the authority's DEMOCALL / MOVE_STOP: process() starts it here with my player (the player next to Wiggler got no intro
    // when a friend's console started it first, REVIEW-AL own_al)
    if (g_bf2 && d.cls == 248 && !mine && ls == 0u && (rs == 1u || rs == 2u) && wig_intro_due(b, local_body())) { force = false; b.mism = 0; }
    // bossfix4: a copy that went on to MOVE_STOP with no intro of its own (not my player's) is not put back into the authority's
    // DEMOCALL (1): it bounced 2 -> 1 -> 2 once while the friend's message opened (low_g1 row)
    if (g_bf2 && d.cls == 248 && !mine && ls == 2u && rs == 1u && !b.a[0x8e1]) { force = false; b.mism = 0; }
    // bossfix3: a copy running its own DEAD (the stomper's, at the end of his talk) finishes it (about a second: the poofs, then its
    // star call, which is suppressed here when the star came by announcement) instead of being cut to the authority's MOVE at once.
    // 4 s at most.
    if (g_bf2 && d.cls == 248 && !mine) {
        if (ls != 7u) b.wig_dead_t = 0;
        else if (rs != 7u) {
            if (!b.wig_dead_t) b.wig_dead_t = now ? now : 1u;
            if ((unsigned)(now - b.wig_dead_t) < 4000u) { force = false; b.mism = 0; }
        }
    }
    // bossfix3: the stagger after he hurts a player (DAMAGE_STOP 8 -> DAMAGE_WAIT 9 -> DAMAGE_START 10 -> MOVE) runs on its own
    // timers. A copy already in it, or out of it less than 3 s ago, finishes it itself: records two behind put it back to 8 over and
    // over (4->8, 8->9, 4->8, 8->10 within 6 s).
    if (g_bf2 && d.cls == 248 && !mine && rs >= 8u && rs <= 10u && ls != rs &&
        ((ls >= 8u && ls <= 10u) || (b.wig_stag_t && (unsigned)(now - b.wig_stag_t) < 3000u))) { force = false; b.mism = 0; }
    // bossfix2 (H2): Eyerok's pre-fight talk is never forced onto a copy (its player would be frozen in a talk his own player did not
    // start); the copy is moved on into the fight by the next records. His after-fight talk is forced, and starts from its first step
    // with my player (coop_boss_prefer_local): that talk ends in his star (only the owner's is real) and his own removal.
    if (g_bf2 && d.cls == 176 && rd32(b.a, 0x49c) == 0 && rs == kEyeTalk && x.w[10] != 0u) force = false;
    // coopbully1: a knock by the owner's player and a death show at once on every copy (two records late, the knock was half over),
    // with the ROM's animation for the state (bully_enter); the pose follows through the mover pin.
    if (g_bully_on && is_bully(d.cls) && !mine && ls != rs && (rs == 2u || rs == 4u || rs == 5u) && ((x.rflags & 2) || rs != 2u)) force = true;
    // coopkbo1: carried by a friend: the copy shows the carried state at once (see kbo_show_held), not the jump back to the arena.
    if (d.cls == 189 && !mine && rs == kKboHeld && ls != kKboHeld && ls != kKboDefeat && hp_of(b) > 0 && kbo_show_held(b, x.from)) {
        ++g_st.applied_state;
        if (g_log) std::fprintf(stderr, "[cbs] kbo held by seat %d shown on this copy (was st=%u) w=%lld\n", x.from, ls, wall_ms());
    }
    // coopkbo1: the intro talk is never forced onto a copy. Its tick sees no talk running on this console, reloads the boss music and
    // moves on, the authority's next records force it back, and the music started over and over for the whole dialogue.
    if (d.cls == 189 && rs == kKboTalk) force = false;
    // kbo2 (REVIEW-T kbo_after): once the group's King is past his intro, this copy's intro is spent too. His idle tick starts the
    // intro talk with the nearest player while mIntroTalked (+0x508) is 0, so a friend's copy that never talked ran a second full
    // intro about 4 s after the owner's talk ended. Not during the talk itself (a set flag would send this copy into the fight early).
    if (d.cls == 189 && g_kbo_fix && rs != kKboIdle && rs != kKboTalk && rs < 18u && b.a[0x508] == 0) {
        b.a[0x508] = 1;
        if (g_log) std::fprintf(stderr, "[cbs] kbo intro spent on this copy (the group's King is in st=%u) w=%lld\n", rs, wall_ms());
    }
    // King Bob-omb's defeat talk on a copy: skip_defeat_talk owns it
    if (d.cls == 189 && !mine && ls == kKboDefeat) force = false;
    // kbo2: a King that is down here runs his own ending (kbo_defeat_tick): no record moves him out of it, the owner's included
    if (d.cls == 189 && g_kbo_fix && hp_of(b) <= 0) force = false;
    // COOPBOO5: a hidden Big Boo is never forced into or out of his hidden state 0 (see hidden_big_boo)
    if (hidden_big_boo(b) && (ls == 0u || rs == 0u)) {
        static unsigned s_logged = 0;
        if (force && g_log && (unsigned)(now - s_logged) >= 1000u) {
            s_logged = now;
            std::fprintf(stderr, "[cbs] big boo slot=%d st %u->%u not forced (hidden state) from seat %d w=%lld\n", slot, ls, rs, x.from, wall_ms());
        }
        force = false; b.mism = 0;
        if (ls == 0u && rs != 0u) big_boo_bring_out(b, slot, "the authority's is out");
    }
    // Bowser's defeat talk: the target seat's body is a parked friend on every console but the authority's; coop_target now answers the
    // LOCAL player for a Bowser in state 4 (coop_boss_prefer_local), so every console runs the whole talk with its own player.
    if (force) {
        // everything except the HP (already min-merged); the state field goes first so its enter code runs on the old fields
        // The state's enter code can have side effects of its own (King Bob-omb's hurt states take a health point): the HP
        // stays what the min-merge above made it.
        const int hp_keep = hp_of(b);
        const bool kbo_music_was = d.cls == 189 && b.a[0x506] != 0;
        if (d.cls == 189) {
            if (ls == kKboThrowBomb && rs != kKboThrowBomb) kbo_release_bombs(b.a);
            if (ls == kKboHeld) kbo_clear_held(b);
        }
        give_player_ptrs(b);
        for (int k = 0; k < d.nf; ++k) if (k != d.hp && value_ok(d, k, x.w[k])) write_field(b.a, d, k, x.w[k]);
        write_field(b.a, d, d.hp, (uint32_t)hp_keep);
        post_force(b);
        if (g_bf2 && d.cls == 176 && rd32(b.a, 0x49c) == 0 && rs == kEyeTalk) {      // bossfix2: the copy's own talk from its first step
            const int32_t z = 0; std::memcpy(b.a + 0x4a0, &z, 4); std::memcpy(b.a + 0x498, &z, 4); std::memcpy(b.a + 0x494, &z, 4);
            void *nul = nullptr; std::memcpy(b.a + 0x490, &nul, sizeof nul);
            if (g_log) std::fprintf(stderr, "[cbs] eyerok head forced into his after-fight talk: it starts here with my player w=%lld\n", wall_ms());
        }
        // bowser2 BFS: a copy forced into the arena-tilt state is given its disc (only its own landing records it), so it rocks here too
        if (d.cls == 279 && g_kpa_fix && rs == 0x13u) { if (char *ar = kpa_arena()) std::memcpy(b.a + 0x3ac, ar + 4, 4); }
        // coopkbo1: the music starts once on a copy that joins the fight by a forced state (the intro talk that starts it is not forced)
        if (d.cls == 189 && !kbo_music_was && rs != kKboIdle && rs != kKboTalk && rs != kKboDefeat) {
            _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
            b.a[0x506] = 1;
            if (g_log) std::fprintf(stderr, "[cbs] kbo boss music started once on this copy (forced into st=%u) w=%lld\n", rs, wall_ms());
        }
        if (g_bf2 && d.cls == 248 && b.a[0x8e1] && rs != 3u) give_camera_back(b, "forced out of a talk state");
        // coopkbo1: Whomp King forced into his defeat by the authority: this copy never started the talk, so it goes to the burst
        if (d.cls == 165 && !mine && ls != 8u && rs == 8u) { b.a[0x40c] = 3; if (g_log) std::fprintf(stderr, "[cbs] whomp king forced defeat: talk skipped w=%lld\n", wall_ms()); }
        b.synced_hp = hp_keep;
        b.mism = 0;
        ++g_st.applied_state;
        if (g_log) std::fprintf(stderr, "[cbs] apply-state slot=%d st %u->%u hp=%d from seat %d heard=%d w=%lld\n", slot, ls, rs, hp_of(b), x.from, (int)b.heard, wall_ms());
    }
    // The pose: the mover path carries it, but its heartbeat is 1.5 s and a copy that falls or slides in between (Whomp King's
    // slam, a forced state) is far off by then. A record this fresh that disagrees by more than 80 units puts the copy back,
    // with the speeds so it does not drift again.
    if (d.pos >= 0 && (unsigned)(now - x.t) < 300u && !(x.rflags & 1) && !b.kpa_shown) {   // bowser2: a held copy is pinned to the holder's hands
        double dd = 0;
        for (int k = 0; k < 3; ++k) { const double e = ((double)rd32(b.a, d.f[d.pos + k].off) - (double)(int32_t)x.w[d.pos + k]) / 4096.0; dd += e * e; }
        if (std::sqrt(dd) > 80.0) {
            for (int k = d.pos; k < d.nf; ++k) if (value_ok(d, k, x.w[k])) write_field(b.a, d, k, x.w[k]);
            ++g_st.applied_fields;
            if (g_log) std::fprintf(stderr, "[cbs] apply-pos slot=%d off by %.0f units from seat %d w=%lld\n", slot, std::sqrt(dd), x.from, wall_ms());
        }
    }
    b.heard = true;
    b.auth_t = now;
}

void process(unsigned now) {
    const unsigned wnow = hub_now();
    unsigned char recs[kPayloadMax]; int nrec = 0, bytes = 0; bool urgent = false;
    for (int slot = 0; slot < kSlots; ++slot) {
        Boss &b = g_boss[slot];
        if (!b.a) continue;
        const BDesc &d = *b.d;
        if (!b.known) { b.known = true; b.synced_hp = hp_of(b); b.last_st = state_of(b); }
        if (!b.def_seen && is_defeated(b)) {
            b.def_seen = true;
            if (g_log) std::fprintf(stderr, "[cbs] defeat seen slot=%d cls=%u hp=%d st=%u w=%lld\n", slot, (unsigned)d.cls, hp_of(b), state_of(b), wall_ms());
        }
        // King Bob-omb carried by MY player (state 707c with my body as the holder): the console that carries him is the one that is right.
        bool claiming = false;
        if (d.cls == 189) {
            const uint32_t st = state_of(b);
            char *holder = nullptr; std::memcpy(&holder, b.a + 0x494, sizeof holder);
            const int me = (int)data_0209f250;
            if ((st == 7 || st == 8) && holder && me >= 0 && me < 16 && holder == (char *)data_0209f394[me]) {
                if (!claim_live(g_claim_until[g_my], wnow)) {
                    ++g_st.claims;
                    if (g_log) std::fprintf(stderr, "[cbs] claim slot=%d st=%u holder=me w=%lld\n", slot, st, wall_ms());
                }
                g_claim_until[g_my] = claim_stamp(wnow);
            }
            if (g_kbo_fix && me >= 0 && me < 16 && data_0209f394[me] && hp_of(b) > 0) {
                char *talker = nullptr; std::memcpy(&talker, b.a + 0x430, sizeof talker);
                // kbo2 (bug 16): my copy is in his pre-fight talk with MY player: this console drives him (state, and the pose through
                // coop_boss_driver_seat) from the talk's first frame, so every screen shows him turning to the player he talks to.
                // Never over a friend's live claim: a copy that started a second intro of its own (the talkend row: Luigi's copy
                // talked while Mario's talk ran, before NPCTALK1's lock) must not take the King from the player already talking.
                bool friend_claim = false;
                for (int q = 0; q < kSeats; ++q) if (q != g_my && claim_live(g_claim_until[q], wnow)) friend_claim = true;
                const bool talk = st == kKboTalk && talker == (char *)data_0209f394[me] && !friend_claim;
                // kbo2: my player threw him (my claim is live): the claim lasts until he lands, the landing's health point is mine.
                const bool thrown = st == kKboThrown && claim_live(g_claim_until[g_my], wnow);
                if (talk || thrown) {
                    if (!claim_live(g_claim_until[g_my], wnow)) {
                        ++g_st.claims;
                        if (g_log) std::fprintf(stderr, "[cbs] claim slot=%d st=%u (%s) w=%lld\n", slot, st, talk ? "my player is in his talk" : "my player threw him", wall_ms());
                    }
                    g_claim_until[g_my] = claim_stamp(wnow);
                }
            }
            claiming = claim_live(g_claim_until[g_my], wnow);
        }
        if (d.cls == 279) {      // Bowser swung by MY player's hands (his tail): the grabbing console is the authority while the grab lasts
            const int32_t sub = rd32(b.a, 0x410);
            char *holder = nullptr; std::memcpy(&holder, b.a + 0x3a4, sizeof holder);
            const int me = (int)data_0209f250;
            if ((sub == 1 || sub == 2) && holder && me >= 0 && me < 16 && holder == (char *)data_0209f394[me]) {
                // a quiet let-go's hit window is over once MY player has him: his throw from my hands is real
                if (b.kpa_quiet_t) {
                    b.kpa_quiet_t = 0;
                    if (g_log) std::fprintf(stderr, "[cbs] bowser quiet window ended: my player has him (sub=%d) w=%lld\n", (int)sub, wall_ms());
                }
                if (!claim_live(g_claim_until[g_my], wnow)) {
                    ++g_st.claims;
                    if (g_log) std::fprintf(stderr, "[cbs] claim slot=%d sub=%d holder=me w=%lld\n", slot, (int)sub, wall_ms());
                }
                g_claim_until[g_my] = claim_stamp(wnow);
            }
            claiming = claim_live(g_claim_until[g_my], wnow);
        }
        bool bully_claim_new = false;
        if (g_bully_on && g_on && is_bully(d.cls) && g_my >= 0 && g_my < kSeats) {
            // coopbully1: my copy went into the knocked-back state between two ticks: only my player can have done that here (a friend's
            // body is parked away; a state forced by a record is taken into bst below, after apply_rx)
            const uint32_t st = state_of(b);
            const bool live = g_bclaim[slot][g_my] && (int)(g_bclaim[slot][g_my] - wnow) > 0;
            if (b.bst_valid && st != b.bst && st == 2u) {
                bully_claim_new = !live;
                const int was = auth_for(slot, wnow, d.cls);
                g_bclaim[slot][g_my] = (wnow + kBullyClaimMs) | 1u;
                b.lw_valid = false;
                ++g_st.claims;
                if (g_log) std::fprintf(stderr, "[cbs] bully claim slot=%d cls=%u st %u->2 by my player (was auth %d) pos=%d,%d,%d spd=%d ang=%d w=%lld\n", slot, (unsigned)d.cls,
                                        b.bst, was, rd32(b.a, 0x5c) >> 12, rd32(b.a, 0x60) >> 12, rd32(b.a, 0x64) >> 12, rd32(b.a, 0x98) >> 12,
                                        (int)rd16(b.a, 0x94), wall_ms());
            } else if (live && (st == 2u || st == 4u || st == 5u) && auth_for(slot, wnow, d.cls) == g_my) {   // a claim lost to a lower seat's is not renewed
                g_bclaim[slot][g_my] = (wnow + kBullyClaimMs) | 1u;     // held while the knock or the death lasts
            }
            claiming = g_bclaim[slot][g_my] && (int)(g_bclaim[slot][g_my] - wnow) > 0;
        }
        const int auth = auth_for(slot, wnow, d.cls);
        const bool mine = auth == g_my;
        if (d.cls != 215) { g_recent_t = wnow ? wnow : 1u; g_recent_mine = mine; }    // (a small Bully gives no reward)
        if (bully_claim_new && mine) urgent = true;
        b.mine_last = mine;
        // a hit that landed on my copy: HP below what was last synced
        if (d.cls == 218 && g_kbo_fix && hp_of(b) == 0xff) {             // kbo2 (bug 27): the reaction state took one more from 0: none left is 0
            write_field(b.a, d, d.hp, 0u);
            if (g_log) std::fprintf(stderr, "[cbs] chilly slot=%d knock-downs wrapped to 255 here: put back to 0 (st=%u) w=%lld\n", slot, state_of(b), wall_ms());
        }
        const int hp = hp_of(b);
        if (hp > b.synced_hp && d.cls == 218) b.synced_hp = hp;    // Chilly's revive raises it by himself
        // COOPBOO5: a hidden Big Boo coming out sets 3 HP (d_a_trs.cpp func_ov063_02119074); the group's fight already took some
        if (hp > b.synced_hp && hidden_big_boo(b)) {
            write_field(b.a, d, d.hp, (uint32_t)b.synced_hp);
            if (g_log) std::fprintf(stderr, "[cbs] big boo slot=%d came out here at hp %d, kept at the group's %d w=%lld\n", slot, hp, b.synced_hp, wall_ms());
        }
        if (hidden_big_boo(b) && state_of(b) == 0u && hp_of(b) <= 0) big_boo_defeat_hidden(b, slot, mine ? "I am the authority" : "a copy");
        // A console that is NOT the authority only reports a drop in its own copy's HP as a hit when the hit can have come from ITS player:
        // the proof knob made it, or its player is within reach of the boss. Anything else is the copy's own ROM chain walking into a hit
        // state after a state was forced onto it (it sees a friend's placed body as the attacker): REVBOSSCORE1 item 4, King Bob-omb lost a
        // second point about 10 s after one hit. That drop is undone and never sent.
        if (hp < b.synced_hp && !mine && g_on && (unsigned)(wnow - b.knob_ok_t) >= 1500u) {
            const int me = (int)data_0209f250;
            bool near_me = false;
            if (me >= 0 && me < 16 && data_0209f394[me]) {
                const char *pl = (const char *)data_0209f394[me];
                const double dx = ((double)rd32(pl, 0x5c) - rd32(b.a, 0x5c)) / 4096.0, dy = ((double)rd32(pl, 0x60) - rd32(b.a, 0x60)) / 4096.0, dz = ((double)rd32(pl, 0x64) - rd32(b.a, 0x64)) / 4096.0;
                near_me = std::sqrt(dx * dx + dy * dy + dz * dz) < 900.0;
            }
            // coopkbo1: King Bob-omb is only hurt by a throw, and the console whose player threw him holds the claim and so IS the
            // authority. A non-authority drop is its forced copy landing from the throw (state 9's tick takes a point after the
            // thrower's point was already merged: one throw cost two). Only his hurt state's own entry (a hit on this copy) counts.
            if (d.cls == 189 && state_of(b) != kKboHurt) near_me = false;
            // kbo2 (bugs 18 / 20): never, on a copy. His bombs are each console's own (spawned by each copy's throw state), so one
            // exploding on this screen only (a bomb left in a copy's hand, a throw at a different moment) hurt this copy alone and was
            // sent to everyone as a hit. The real hits (his landing after a throw) are the thrower's, who holds the claim and so is the
            // authority; the proof knob's hits pass above through knob_ok_t.
            if (d.cls == 189 && g_kbo_fix) near_me = false;
            if (!near_me) {
                ++g_st.hit_spurious;
                if (g_log) std::fprintf(stderr, "[cbs] drop ignored slot=%d hp %d->%d (not from my player; auth %d me %d) w=%lld\n", slot, b.synced_hp, hp, auth, g_my, wall_ms());
                write_field(b.a, d, d.hp, (uint32_t)b.synced_hp);
            }
        }
        // bowser2: a copy let go quietly (its holder's records stopped mid-swing) reports no hit for 5 s, authority or not
        if (d.cls == 279 && b.kpa_quiet_t && hp_of(b) < b.synced_hp) {
            if ((unsigned)(now - b.kpa_quiet_t) < 5000u) {
                ++g_st.hit_spurious;
                if (g_log) std::fprintf(stderr, "[cbs] bowser drop ignored slot=%d hp %d->%d (this copy was let go quietly %u ms ago) w=%lld\n", slot, b.synced_hp, hp_of(b), now - b.kpa_quiet_t, wall_ms());
                write_field(b.a, d, d.hp, (uint32_t)b.synced_hp);
            } else b.kpa_quiet_t = 0;
        }
        if (hp_of(b) < b.synced_hp) {
            ++g_st.hit_local;
            if (g_log) std::fprintf(stderr, "[cbs] hit slot=%d hp %d->%d (auth %d me %d) w=%lld\n", slot, b.synced_hp, hp_of(b), auth, g_my, wall_ms());
            b.synced_hp = hp_of(b);
            if (!mine && g_on) {
                unsigned char one[64];
                const int n = fill_rec(one, slot, b, 0);
                send_batch(kWorldBossHit, 1, one, 1, n);
            }
            b.lw_valid = false;                 // publish at once when I am the authority
        }
        // Chief Chilly's last blow is a player hit in his waiting window after the last fall: his HP is already 0, so it shows only as
        // the ROM's own move into state 33f0 (id 13). A console that is not the authority and saw it tells the authority like an HP hit.
        if (d.cls == 218) {
            const uint32_t stn = state_of(b);
            if (stn == 13u && b.last_st != 13u && !mine && g_on) {
                ++g_st.hit_local;
                if (g_log) std::fprintf(stderr, "[cbs] hit slot=%d state %u->13 (auth %d me %d) w=%lld\n", slot, b.last_st, auth, g_my, wall_ms());
                unsigned char one[64];
                const int n = fill_rec(one, slot, b, 0);
                send_batch(kWorldBossHit, 1, one, 1, n);
            }
            b.last_st = stn;
        }
        // bossfix3: the talk hold's stamp is cleared every tick no talk of my player runs on this copy (or I am its authority). Cleared only
        // when a record came in, it kept the intro talk's stamp through a stretch as authority (no records taken), and the stomper's
        // last talk, held from its first record, looked 60 s old and was cut at once (own_f9 row: "talk held past 60 s" 22 s in).
        if (g_bf2 && d.cls == 248 && (mine || !wig_talk_here(b))) b.wig_talk_t = 0;
        if (g_bf2 && d.cls == 176 && (mine || !eye_talk_here(b))) b.eye_talk_t = 0;
        if (g_on) apply_rx(slot, b, now, mine);
        if (g_on && d.cls == 279) { kpa_tick(b, mine, now); if (g_kpa_fix) { kpa_intro_camera_done(b); kpa_bfs_guard(b, mine, now); } }
        // bowser2 proof knob: the authority's idle Bowser jumps (variant 1 lands in the arena-tilt state: the rocking slam)
        if (g_kpa_jump_every && d.cls == 279 && mine && state_of(b) == 0u && rd32(b.a, 0x410) == 0 && g_ticks - g_kpa_jump_t >= g_kpa_jump_every) {
            g_kpa_jump_t = g_ticks;
            const int32_t j = 0xd; std::memcpy(b.a + 0x40c, &j, 4); b.a[0x423] = 0; const uint16_t z = 0; std::memcpy(b.a + 0x3fc, &z, 2);
            std::fprintf(stderr, "[cbs-knob] jump: the authority's Bowser sent to his jump w=%lld\n", wall_ms());
        }
        if (g_on && !mine) skip_defeat_talk(b, false);
        if (g_on && g_kbo_fix && d.cls == 189) {
            kbo_defeat_tick(b);                   // kbo2: down here, mine or not: the cartridge's ending with my player
            // kbo2 (bug 18): on a copy a bomb is in his hand only during his throw. A copy that left the throw by a record (or was hurt
            // in it) kept it pinned to his hand through every later state, the bounce included, until it went off there.
            if (!mine && state_of(b) != kKboThrowBomb) kbo_release_bombs(b.a);
        }
        // coopgoomboss2: Goomboss's summon (state 2, func_ov074_0211fd74) runs only on his authority. A copy holds the summon's frame
        // timer (+0x5fc) above 1, so its tick returns before the Spawn; its goombas are the authority's, built here as they are
        // announced (coop_boss_spawn_minion), and the authority's records move the copy on to state 3 when the set is out.
        if (g_on && !mine && d.cls == 198 && g_minion_share && state_of(b) == 2u) {
            uint16_t t = 0; std::memcpy(&t, b.a + 0x5fc, 2);
            if (t < 2) { t = 2; std::memcpy(b.a + 0x5fc, &t, 2); }
            if (!b.summon_held) {
                b.summon_held = true;
                if (g_log) std::fprintf(stderr, "[cbs] goomboss summon held on this copy (auth %d me %d): its goombas come from the authority w=%lld\n", auth, g_my, wall_ms());
            }
        } else if (d.cls == 198) b.summon_held = false;
        // coopgoomboss2: a console that BECOMES the authority while its copy still waits in the summon with the whole set already out
        // (built from the old authority's announcements) would summon one more: the ROM's tick spawns before it tests the count. The
        // ROM itself never sits in state 2 with a full set (its last spawn enters state 3), so the copy goes on to state 3 as the ROM's
        // last spawn would. The set size is the summon's own (func_ov074_0211fd74): the table by size index, one more at size 1 unless
        // the closest player is Yoshi (param1 3).
        if (g_on && mine && d.cls == 198 && g_minion_share && state_of(b) == 2u && b.heard) {
            const unsigned sz = (unsigned char)b.a[0x604];
            if (sz <= 3u) {
                unsigned target = data_ov074_02122d80[sz];
                if (sz == 1u) {
                    const char *pl = (const char *)((dActor_c *)b.a)->ClosestPlayer();
                    if (pl && (int)rd32(pl, 0x08) != 3) ++target;
                }
                const unsigned cnt = (unsigned char)b.a[0x602];
                if (target && cnt >= target) {
                    _ZN12daKuriKing_c19func_ov074_021203e4Ei(b.a, 3);
                    b.lw_valid = false;
                    if (g_log) std::fprintf(stderr, "[cbs] goomboss authority found the set already out (%u of %u) in the summon: on to state 3, no extra goomba w=%lld\n", cnt, target, wall_ms());
                }
            }
        }
        // bossfix2 (H1): MOVE_START (3) waits for its player's HasFinishedTalking, true only right after a talk. A copy that came into
        // it with no talk of its own (the stomper's talk entered as 3 here, or the ROM's own way back from DEAD) stood there for good,
        // the authority's copy included (measured: 60 s in state 3 on the console that did not stomp). With no talk of its own (it
        // drives no camera) and its walk animation set (the state timer stops at 13), it ends the way the state ends: the hop by the
        // health left, then MOVE.
        if (g_on && g_bf2 && d.cls == 248) {
            { const uint32_t st = state_of(b); if (st >= 8u && st <= 10u) b.wig_stag_t = now ? now : 1u; }
            // bossfix4: the intro is shown the cartridge's way, to MY player when he is where its check starts it (more than 300 units
            // above Wiggler, wig_intro_due), and to nobody here otherwise. REVIEW-AL: with bossfix3 every player in the area got it,
            // even one 2500 units off, while the player next to Wiggler could miss it when a friend's console started it first.
            // In DEMOWAIT (0) my own player is checked as well as ClosestPlayer's answer (a friend's body on most consoles).
            char *me = local_body();
            if (state_of(b) == 0u && me && wig_intro_due(b, me)) {
                const bool auth_past = !mine && b.rx.valid && b.rx.w[d.state] > 2u;
                if (!auth_past) {
                    _ZN12daHanachan_c8SetStateEi(b.a, 1);
                    if (g_log) std::fprintf(stderr, "[cbs] wiggler DEMOWAIT: my player is above him, the intro starts here w=%lld\n", wall_ms());
                }
            }
            if (state_of(b) == 1u) {
                char *pl = nullptr; std::memcpy(&pl, b.a + 0x8c8, sizeof pl);
                const bool first = !b.wig_call_seen;
                b.wig_call_seen = true;
                // Not my intro: on the tick DEMOCALL is first seen (this hook runs after the actor phase that entered it, so no message was
                // asked of my player yet), or while it holds a friend's body (whose message never shows), the copy goes on to MOVE_STOP
                // with no message, where it stops and waits like the authority's (the MOVE_STOP way out below moves it on).
                if (me && !b.a[0x8e1] && !wig_intro_due(b, me) && (first || pl != me)) {
                    _ZN12daHanachan_c8SetStateEi(b.a, 2);
                    if (g_log) std::fprintf(stderr, "[cbs] wiggler DEMOCALL: my player is not above him, no intro on this screen (dy %d) w=%lld\n",
                                            (rd32(me, 0x60) - rd32(b.a, 0x60)) >> 12, wall_ms());
                } else if (me && pl != me) {
                    // bossfix3: DEMOCALL (1) shows the intro to the player its entry asked ClosestPlayer for. Entered from DEMOWAIT's tick,
                    // that second ask in the same frame gets the frame's cached answer (the target seat, coop_target), not my own player
                    // (coop_boss_prefer_local is never asked): on a console whose target is a friend the message never showed, and
                    // Wiggler kept walking at his wander speed, off his mountain top and out of the world (far_f7b row).
                    std::memcpy(b.a + 0x8c8, &me, sizeof me);
                    if (g_log) std::fprintf(stderr, "[cbs] wiggler DEMOCALL had a friend's player: the intro is my own player's here w=%lld\n", wall_ms());
                }
            } else b.wig_call_seen = false;
            // bossfix3: MOVE_STOP (2) ends only when its player's intro talk ends. A copy put into it by its first record skipped
            // DEMOCALL's message, so no talk exists and it stood there for good, deaf to stomps (MOVE_STOP never checks hits); when that
            // console then took the authority, its records froze every friend's copy in 2 as well (far_f5 row: 35 s on both screens).
            // With no talk of its own (it drives no camera) it ends the way the state ends: the boss music, then MOVE_START (and the
            // MOVE_START way out below), the authority's copy included. Not on a copy while the authority's own Wiggler is in 2 (its
            // player is reading the intro): the next record would put it back, and it bounced 2 -> 3 -> 2 every second (far_f6 first try).
            const bool auth_in_2 = !mine && b.rx.valid && (b.rx.w[d.state] == 2u || b.rx.w[d.state] == 1u);   // bossfix4: or still in 1
            if (state_of(b) == 2u && !b.a[0x8e1] && (unsigned char)b.a[0x8da] >= 13u && !auth_in_2) {
                if (++b.wig2_ticks >= 20u) {
                    if (!b.wig_music) { _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d); b.wig_music = true; }
                    _ZN12daHanachan_c8SetStateEi(b.a, 3);
                    b.wig2_ticks = 0; b.lw_valid = false;
                    if (g_log) std::fprintf(stderr, "[cbs] wiggler copy in MOVE_STOP with no talk of its own: on to MOVE_START w=%lld\n", wall_ms());
                }
            } else b.wig2_ticks = 0;
            if (state_of(b) == 3u && !b.a[0x8e1] && (unsigned char)b.a[0x8da] >= 13u) {
                if (++b.wig3_ticks >= 20u) {
                    const unsigned hp = (unsigned char)b.a[0x8db];
                    if (hp < 4u && hp > 1u) {
                        const int32_t vs = 0x41000 - (((int)(hp - 2) * 10) << 12), va = -0x4000;
                        std::memcpy(b.a + 0xa8, &vs, 4); std::memcpy(b.a + 0x9c, &va, 4);
                    }
                    _ZN12daHanachan_c8SetStateEi(b.a, 4);
                    b.wig3_ticks = 0; b.lw_valid = false;
                    if (g_log) std::fprintf(stderr, "[cbs] wiggler copy in MOVE_START with no talk of its own: on to MOVE (hp %u) w=%lld\n", hp, wall_ms());
                }
            } else b.wig3_ticks = 0;
        }
        // bossfix2 (H7): Goomboss's defeat talk (state 7, d_a_kuri_king.cpp func_ov074_0211f5b8) shows its message to mTalkPlayer and
        // waits on it, but the talk itself is started by state 5's tick (func_ov074_0211fc38: StartTalk, then state 7). A copy put into
        // state 7 by a record never ran that, so it waited at step 0 for ever (no message, no key, never removed). The copy starts the
        // talk with my player the cartridge's way, so his last words, the key and his end run on every screen (one key: coop_spawn).
        if (g_on && g_bf2 && d.cls == 198 && state_of(b) == 7u && b.a[0x603] == 0) {
            char *me = local_body();
            void *partner = nullptr; if (me) std::memcpy(&partner, me + 0x368, sizeof partner);
            if (me && partner != (void *)b.a) {
                ++b.goom_talk_tries;
                if (((Player *)me)->StartTalk(*(fBase_c *)b.a, true)) {
                    Message::PrepareTalk();
                    std::memcpy(b.a + 0x5c8, &me, sizeof me);
                    if (g_log) std::fprintf(stderr, "[cbs] goomboss defeat talk started on this copy with my player (try %u) w=%lld\n", b.goom_talk_tries, wall_ms());
                } else if (g_log && (b.goom_talk_tries == 1 || b.goom_talk_tries % 120 == 0)) {
                    std::fprintf(stderr, "[cbs] goomboss in his defeat here without a talk: StartTalk refused (try %u) w=%lld\n", b.goom_talk_tries, wall_ms());
                }
            }
        }
        if (is_bully(d.cls)) { b.bst = state_of(b); b.bst_valid = true; }
        // coopkbo1: the authority's King Bob-omb at HP 0 outside the states that end in his defeat (his last point arrived by merge)
        // never runs the defeat, so nobody got the star. Put him in the hurt state, whose tick on the ground runs the defeat talk and
        // the star; its entry takes a point, the HP is put back.
        if (g_on && mine && d.cls == 189 && hp_of(b) <= 0) {
            const uint32_t st = state_of(b);
            if (st != kKboHurt && st != kKboThrown && st != kKboDefeat && st != kKboHeld && st != 7u) {
                give_player_ptrs(b);
                _ZN12daBombking_c19KingBobOmb_SetStateEPv(b.a, kbo_id_to_ptr(kKboHurt));
                write_field(b.a, d, d.hp, 0u);
                b.synced_hp = 0; b.lw_valid = false;
                if (g_log) std::fprintf(stderr, "[cbs] kbo authority at hp 0 in st=%u: sent to the hurt state for the defeat w=%lld\n", st, wall_ms());
            }
        }
        if (g_log && (unsigned)(now - b.sample_t) >= 500u) { b.sample_t = now; log_sample(slot, b, auth); }
        // kbo2 proof log SM64DS_COOP_KBO_LOOK=1 (off by default): King Bob-omb's facing and who drives him every 2 ticks before the fight
        if (g_kbo_look && d.cls == 189 && (g_ticks & 1u) == 0) {
            const uint32_t st = state_of(b);
            if (st == kKboIdle || st == kKboTalk || st == 2u) {
                char *talker = nullptr; std::memcpy(&talker, b.a + 0x430, sizeof talker);
                std::fprintf(stderr, "[kbolook] t=%u st=%u yaw=%04x auth=%d mover=%d tgt=%d talker=%s w=%lld\n", g_ticks, st, (unsigned)(uint16_t)rd16(b.a, 0x8e), auth,
                             coop_move_owner(slot), coop_target_seat_of_slot(slot), !talker ? "none" : talker == local_body() ? "me" : "friend", wall_ms());
            }
        }
        if (!b.announced_dead && hp_of(b) == 0 && g_log) {
            b.announced_dead = true;
            std::fprintf(stderr, "[cbs] defeated slot=%d cls=%u hp=0 st=%u w=%lld\n", slot, (unsigned)d.cls, state_of(b), wall_ms());
        }
        if (b.last_auth != auth) {
            if (g_log) std::fprintf(stderr, "[cbs] authority slot=%d %d->%d w=%lld\n", slot, b.last_auth, auth, wall_ms());
            b.last_auth = auth; b.auth_t = now; b.mism = 0;
        }
        if (!mine || !g_on) continue;
        if (d.cls == 279 && g_nstall) {
            for (int i = 0; i < g_nstall; ++i) if (g_stall[i].req) {
                g_stall[i].req = false; g_stall_until = now + g_stall[i].ms; g_stall_on = true;
                std::fprintf(stderr, "[cbs-knob] stall: no Bowser record from this console for %u ms (hold=%d) w=%lld\n", g_stall[i].ms, rd32(b.a, 0x410), wall_ms());
            }
            if (g_stall_on) {
                if ((int)(g_stall_until - now) > 0) continue;
                g_stall_on = false;
                std::fprintf(stderr, "[cbs-knob] stall: Bowser records from this console again (hold=%d) w=%lld\n", rd32(b.a, 0x410), wall_ms());
            }
        }
        // I am the authority: publish when anything changed, and as a heartbeat
        uint32_t w[kBossMaxFields]; bool changed = !b.lw_valid;
        // a hit is never held back by the rate limit: the boss can be gone (a dead Eyerok hand destroys itself) before the next heartbeat
        if (b.lw_valid && (int32_t)read_field(b.a, d.f[d.hp]) < (int32_t)b.last_w[d.hp]) urgent = true;
        for (int k = 0; k < d.nf; ++k) { w[k] = read_field(b.a, d.f[k]); if (b.lw_valid && w[k] != b.last_w[k] && !(d.f[k].kind & kVol)) changed = true; }
        if (changed || (unsigned)(now - b.pub_t) >= (unsigned)kBossHeartbeatMs) {
            if (bytes + rec_bytes(d) <= kPayloadMax - (int)sizeof(BossHead) && nrec < kBossMaxRecs) {
                bytes += fill_rec(recs + bytes, slot, b, claiming ? 2 : 0); ++nrec;
                std::memcpy(b.last_w, w, sizeof w); b.lw_valid = true; b.pub_t = now;
            }
        }
    }
    if (nrec > 0 && (urgent || take_token(now))) send_batch(kWorldBossState, 0, recs, nrec, bytes);
}

// SM64DS_COOP_BOSS_HIT=F[,F]: from boss tick F, on THIS console, hit the first boss the ROM's own way. Where the class has a
// hit flag (Whomp King) the flag is set every tick until the boss's own handler has taken it (it only does in its vulnerable
// state), so the HP drop is the game's; where it has none the HP is lowered directly (the sync path is what is under test).
bool bossdesc1_knob(Boss &b, HitKnob &k);
bool bossdesc2_knob(Boss &b, HitKnob &k);
void do_hit_knob(HitKnob &k) {
    if (!k.armed) {
        for (int slot = 0; slot < kSlots; ++slot) {
            Boss &b = g_boss[slot];
            if (!b.a || hp_of(b) <= 0) continue;
            if (k.lethal && b.d->hit_off && hp_of(b) > 1) { write_field(b.a, *b.d, b.d->hp, 1u); b.synced_hp = 1; }   // the next hit is the last one, the ROM's own way
            k.armed = true; k.slot = slot; k.hp0 = hp_of(b);
            std::fprintf(stderr, "[cbs-knob] hit armed slot=%d cls=%u hp=%d w=%lld\n", slot, (unsigned)b.d->cls, k.hp0, wall_ms());
            break;
        }
        if (!k.armed) return;
    }
    Boss &b = g_boss[k.slot];
    if (!b.a) { k.done = true; return; }
    b.knob_ok_t = hub_now();
    if (hp_of(b) < k.hp0) {
        k.done = true;
        std::fprintf(stderr, "[cbs-knob] hit taken slot=%d hp %d->%d st=%u w=%lld\n", k.slot, k.hp0, hp_of(b), state_of(b), wall_ms());
        return;
    }
    if (b.d->hit_off) {
        if (b.d->hit_who && data_0209f394[data_0209f250 & 15]) { void *me = data_0209f394[data_0209f250 & 15]; std::memcpy(b.a + b.d->hit_who, &me, sizeof me); }
        b.a[b.d->hit_off] = 1;
    } else if (b.d->cls == 189) {
        // King Bob-omb takes a hit by entering state 02126ffc: its enter code (func_ov078_021243c0) is the health point.
        if (k.lethal && hp_of(b) > 1) { write_field(b.a, *b.d, b.d->hp, 1u); k.hp0 = 1; }
        give_player_ptrs(b);          // the hit state reads the player it was hit by (+0x430)
        _ZN12daBombking_c19KingBobOmb_SetStateEPv(b.a, kbo_id_to_ptr(0));
    } else if (bossdesc1_knob(b, k)) {
    } else if (bossdesc2_knob(b, k)) {
    } else { write_field(b.a, *b.d, b.d->hp, (uint32_t)(k.hp0 - 1)); }
}

// bossdesc1: the proof's "a hit from this console" for the four families, each the ROM's own way as far as it can be made without
// a player pushing: Chilly goes through the knock-down code of func_ov073_02120c7c (one fewer, then the hit or the defeat state);
// Big Bully falls into the lava (the dying state with the death point that func_ov064_02116220 sets); Big Boo enters its hit state
// with the timer at 0 so the ROM's own handler takes the health point; Wiggler enters the hit state (whose enter half takes it) or,
// on the last point, the death state. Returns true when the class is one of the four.
bool bossdesc1_knob(Boss &b, HitKnob &k) {
    char *a = b.a;
    const unsigned cls = b.d->cls;
    if (cls != 218 && cls != 216 && cls != 210 && cls != 248) return false;
    give_player_ptrs(b);
    if (cls == 218) {
        if (k.hp0 == -2) {
            // stage 2 of the lethal hit: the player's hit in his waiting window (state 33f0 is where func_ov073_02120910 sends him on a hit)
            ChiefChilly_ChangeState(a, chilly_id_to_ptr(13u));
        } else {
            uint8_t hp = (uint8_t)a[0x4cb];
            if (k.lethal && hp > 1) { a[0x4cb] = 1; hp = 1; }
            a[0x4cb] = (char)(hp - 1);
            ChiefChilly_ChangeState(a, chilly_id_to_ptr(hp - 1 ? 10u : 11u));
            if (k.lethal) {
                k.hp0 = -2; k.tick = g_ticks + 30;     // the waiting window is 90 frames
                std::fprintf(stderr, "[cbs-knob] fall made slot=%d hp=%u st=%u, the hit follows in 30 ticks w=%lld\n", k.slot, (unsigned)hp_of(b), state_of(b), wall_ms());
                return true;
            }
        }
    } else if (cls == 216) {
        int32_t x = rd32(a, 0x5c), y = rd32(a, 0x60) + 0x5000, z = rd32(a, 0x64);
        std::memcpy(a + 0x3a8, &x, 4); std::memcpy(a + 0x3ac, &y, 4); std::memcpy(a + 0x3b0, &z, 4);
        bully_enter(a, 4u);
    } else if (cls == 210) {
        if (k.lethal && (uint8_t)a[0x5ca] > 1) a[0x5ca] = 1;
        a[0x5cc] = 3; const uint16_t zero = 0; std::memcpy(a + 0x100, &zero, 2);
    } else {
        // His stomp (func_ov034_02112688) sets the stomper into +0x8c8 (give_player_ptrs did) and enters DAMAGE (5): the enter half takes the
        // health point (floor 1), the tick talks, DAMAGETALK (6) then ends in MOVE_START (3) or, on the last point, DEAD (7).
        if (k.lethal && (uint8_t)a[0x8db] > 1) a[0x8db] = 1;
        _ZN12daHanachan_c8SetStateEi(a, 5);
    }
    k.done = true;
    std::fprintf(stderr, "[cbs-knob] hit made slot=%d cls=%u hp=%u st=%u w=%lld\n", k.slot, cls, (unsigned)hp_of(b), state_of(b), wall_ms());
    return true;
}

// bossdesc2: the proof's "a hit from this console" for Goomboss, Bowser and an Eyerok hand, each the ROM's own transition as far as it
// can be made without a player's body: Goomboss goes through the stomp's own tail (turn, hit animation, state 5 whose enter code takes
// the size point); Bowser takes the bomb hit's own tail (func_ov060_02113b5c: one HP, state 0xc or at 0 state 4); a hand takes the punch
// (func_ov066_0211603c's tail: HP, and at 0 the overlay globals and record 0211b07c).
bool bossdesc2_knob(Boss &b, HitKnob &k) {
    char *a = b.a;
    const unsigned cls = b.d->cls;
    if (cls != 198 && cls != 279 && cls != 176) return false;
    if (cls == 198) {
        if (k.lethal && (uint8_t)a[0x604] > 1) a[0x604] = 1;
        if ((uint8_t)a[0x604] == 0) { k.done = true; return true; }
        const int16_t sp = (int16_t)(rd32(a, 0x5f0) * -0x190);
        std::memcpy(a + 0x5f6, &sp, 2);
        func_ov074_02121a4c(a, 2);
        _ZN12daKuriKing_c19func_ov074_021203e4Ei(a, 5);
    } else if (cls == 279 && g_bomb) {
        // The ROM's own bomb hit: Bowser is put into his thrown state (aed4[1] = func_ov060_02113b5c) on the bomb's point, and that state's
        // own check (func_ov060_02113d20 -> 02118544) finds the bomb, takes the HP point and picks state 0xc or, at 0, the defeat state 4.
        // Retried every 4 ticks (the state needs a frame to run) until the HP has dropped.
        char *bomb = nullptr;
        for (char *x = (char *)_ZN8dActor_c4NextEPKS_(0); x; x = (char *)_ZN8dActor_c4NextEPKS_(x))
            if (*(const unsigned short *)(x + 0x0c) == 284 && rd32(x, 0x170) == 0) { bomb = x; break; }
        if (!bomb) { std::fprintf(stderr, "[cbs-knob] bomb: no live bomb actor (class 284) in this arena w=%lld\n", wall_ms()); k.done = true; return true; }
        if (k.lethal && (int8_t)a[0x41e] > 1) { a[0x41e] = 1; k.hp0 = 1; }
        // The bombs hang in the air on a ring beyond the arena's floor, so Bowser put on one falls into the pit (measured: the level ended). The
        // bomb is brought to him instead: its point and ring radius are set to where he stands on the floor, and the ROM's own test
        // (func_ov060_02113d20 -> 02118544: ring radius +-300 and distance < the sphere) then finds it.
        const int32_t bx = rd32(a, 0x5c), by = rd32(a, 0x60), bz = rd32(a, 0x64);
        const int32_t hlen = (int32_t)std::sqrt((double)bx * bx + (double)bz * bz);
        std::memcpy(bomb + 0x174, &bx, 4); std::memcpy(bomb + 0x178, &by, 4); std::memcpy(bomb + 0x17c, &bz, 4);
        std::memcpy(bomb + 0x180, &hlen, 4);
        const int32_t rad = rd32(bomb, 0x184);
        const int32_t px = bx, py = by, pz = bz;
        const int32_t one = 1, zero = 0;
        std::memcpy(a + 0x40c, &one, 4); std::memcpy(a + 0x410, &zero, 4); a[0x423] = 0;
        k.tick = g_ticks + 4;
        if (++k.slot_tries >= 60) { k.done = true; std::fprintf(stderr, "[cbs-knob] bomb: gave up after 60 tries hp=%d w=%lld\n", hp_of(b), wall_ms()); }
        else if (k.slot_tries == 1) std::fprintf(stderr, "[cbs-knob] bomb: the bomb brought to Bowser at %d,%d,%d, he is in his thrown state (sphere radius %d, ring %d), the ROM decides hp=%d st=%u w=%lld\n", px >> 12, py >> 12, pz >> 12, rad >> 12, rd32(bomb, 0x180) >> 12, hp_of(b), state_of(b), wall_ms());
        return true;
    } else if (cls == 279) {
        if (k.lethal && (int8_t)a[0x41e] > 1) a[0x41e] = 1;
        a[0x41e] = (char)((int8_t)a[0x41e] - 1);
        const int32_t st = (int8_t)a[0x41e] <= 0 ? 4 : 0xc;
        std::memcpy(a + 0x40c, &st, 4);
        a[0x423] = 0;
    } else {
        int8_t hp = (int8_t)a[0x4d8];
        if (k.lethal && hp > 1) hp = 1;
        hp = (int8_t)(hp - 1);
        a[0x4d8] = (char)hp;
        if (hp <= 0) {
            const int side = rd32(a, 0x49c);
            data_ov066_0211abe0 = (unsigned char)(data_ov066_0211abe0 ^ side);
            data_ov066_0211ae08 = (unsigned char)(data_ov066_0211ae08 + 1);
            data_ov066_0211abe4 = -3;
            std::fprintf(stderr, "[cbs-knob] eyerok part %d death: files ae8c=%p aea4=%p\n", side, data_ov066_0211ae8c[1], data_ov066_0211aea4[1]);
            if (eye_enter_safe(a, (uint32_t)kEyeDeadRecord)) _ZN6Eyerok19func_ov066_02119454EPv(a, eye_id_to_ptr((uint32_t)kEyeDeadRecord));
        }
    }
    k.done = true;
    std::fprintf(stderr, "[cbs-knob] hit made slot=%d cls=%u hp=%d st=%u w=%lld\n", k.slot, cls, hp_of(b), state_of(b), wall_ms());
    return true;
}

// SM64DS_COOP_GOOMBA_KILL: the first live minion of the Goomboss on this console ends the way a stomped Goomba ends (its own removal,
// func_ov084_02129498; its CleanupResources lowers the boss's count). Waits (retried each tick) until a minion is out.
void do_minion_kill(MinionKnob &k) {
    for (int slot = 0; slot < kSlots; ++slot) {
        Boss &b = g_boss[slot];
        if (!b.a || b.d->cls != 198) continue;
        const uint32_t uid = (uint32_t)rd32(b.a, 0x04);
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
            if (*(const unsigned short *)(a + 0x0c) != 200 || (uint32_t)rd32(a, 0x43c) != uid || a[0x0f] || a[0x0e] == 2) continue;
            const int before = count_minions(b.a);
            func_ov084_02129498(a);
            k.done = true;
            std::fprintf(stderr, "[cbs-knob] goomba stomped here: minion uid=%u index=%u at %d,%d,%d (live before %d, boss count %u) w=%lld\n", (unsigned)rd32(a, 0x04),
                         (unsigned)(unsigned char)a[0x474], rd32(a, 0x5c) >> 12, rd32(a, 0x60) >> 12, rd32(a, 0x64) >> 12, before, (unsigned)(unsigned char)b.a[0x602], wall_ms());
            return;
        }
    }
}

// coopbully1 proof knob SM64DS_COOP_BULLY_BUMP=F:cls[:every[:count[:angle]]]: from boss tick F, on THIS console, the nearest live bully of
// class cls (215 or 216) is knocked back as the ROM's hit reaction does it for a punch (daOts_c::func_ov064_02116754: heading, 40 units a
// frame, the hit animation, STATE_KNOCKED_BACK, hops cleared), with the local player put 150 units behind it, facing the push. The
// push goes away from where the player stood, or along `angle` (0..65535, -1 = away) when given, at `speed` units a frame (default 40).
// Repeated every `every` ticks, `count` times.
void do_bump() {
    const int me = (int)data_0209f250;
    if (me < 0 || me >= 16 || !data_0209f394[me]) return;
    char *pl = (char *)data_0209f394[me];
    int best = -1; double bd = 1e18;
    for (int slot = 0; slot < kSlots; ++slot) {
        const Boss &b = g_boss[slot];
        if (!b.a || b.d->cls != (unsigned)g_bump.cls) continue;
        const uint32_t st = state_of(b);
        if (st == 2u || st >= 4u) continue;
        if (b.d->cls == 216 && (unsigned char)b.a[0x3fe] < 4u) continue;   // still waiting for its three bullies: its Behavior does not run yet
        const double dx = ((double)rd32(pl, 0x5c) - rd32(b.a, 0x5c)) / 4096.0, dz = ((double)rd32(pl, 0x64) - rd32(b.a, 0x64)) / 4096.0;
        const double dd = dx * dx + dz * dz;
        if (dd < bd) { bd = dd; best = slot; }
    }
    g_bump.tick = g_ticks + (g_bump.every ? g_bump.every : 30u);
    if (best < 0) return;
    Boss &b = g_boss[best];
    char *a = b.a;
    int16_t ang;
    if (g_bump.ang >= 0) ang = (int16_t)(uint16_t)g_bump.ang;
    else {
        const double dx = (double)rd32(a, 0x5c) - rd32(pl, 0x5c), dz = (double)rd32(a, 0x64) - rd32(pl, 0x64);
        ang = (int16_t)(std::atan2(dx, dz) * 32768.0 / 3.14159265358979323846);
    }
    const double rad = (double)(uint16_t)ang * 6.283185307179586 / 65536.0;
    const int32_t px = rd32(a, 0x5c) - (int32_t)(std::sin(rad) * 150.0 * 4096.0), py = rd32(a, 0x60) + 0x2000, pz = rd32(a, 0x64) - (int32_t)(std::cos(rad) * 150.0 * 4096.0);
    std::memcpy(pl + 0x5c, &px, 4); std::memcpy(pl + 0x60, &py, 4); std::memcpy(pl + 0x64, &pz, 4);
    std::memcpy(pl + 0x8e, &ang, 2);
    std::memcpy(a + 0x94, &ang, 2);                     // mPrevAngleY = the player's mAngleY
    const int32_t spd = g_bump.speed << 12; std::memcpy(a + 0x98, &spd, 4);   // 40 = the ROM's punch; more only to reach the lava in a row
    a[0x3f9] = 0;                                       // mHopCount
    const uint32_t was = state_of(b);
    bully_enter(a, 2u);
    --g_bump.left;
    std::fprintf(stderr, "[cbs-knob] bully bump slot=%d cls=%u st %u->%u at %d,%d,%d heading %d speed %d, my player put behind it (%d left) w=%lld\n", best, (unsigned)b.d->cls,
                 was, state_of(b), rd32(a, 0x5c) >> 12, rd32(a, 0x60) >> 12, rd32(a, 0x64) >> 12, (int)(uint16_t)ang, g_bump.speed, g_bump.left, wall_ms());
}

// coopbully1 proof knob SM64DS_COOP_BULLY_STAND: the local player stands `dist` units beside the nearest live bully of class cls (so the
// shared target, and with it the Big Bully's authority, can be put on this console in a row).
void do_bstand() {
    const int me = (int)data_0209f250;
    g_bstand.tick = g_ticks + (g_bstand.every ? g_bstand.every : 1u);
    if (!g_bstand.every) g_bstand.on = false;
    if (me < 0 || me >= 16 || !data_0209f394[me]) return;
    char *pl = (char *)data_0209f394[me];
    for (int slot = 0; slot < kSlots; ++slot) {
        const Boss &b = g_boss[slot];
        if (!b.a || b.d->cls != (unsigned)g_bstand.cls || state_of(b) >= 4u) continue;
        if (b.d->cls == 216 && (unsigned char)b.a[0x3fe] < 4u) continue;
        const int32_t x = rd32(b.a, 0x5c) + (g_bstand.dist << 12), y = rd32(b.a, 0x60) + 0x8000, z = rd32(b.a, 0x64);
        std::memcpy(pl + 0x5c, &x, 4); std::memcpy(pl + 0x60, &y, 4); std::memcpy(pl + 0x64, &z, 4);
        if (g_log) std::fprintf(stderr, "[cbs-knob] bully stand: local player %d units from slot=%d cls=%u w=%lld\n", g_bstand.dist, slot, (unsigned)b.d->cls, wall_ms());
        return;
    }
    g_bstand.on = true;                     // none yet: try again next tick
}

void do_forge(const char *name) {
    int slot = -1;
    for (int i = 0; i < kSlots; ++i) if (g_boss[i].a) { slot = i; break; }
    if (slot < 0) { std::fprintf(stderr, "[cbs-knob] forge %s: no boss\n", name); return; }
    const Boss &b = g_boss[slot];
    unsigned char buf[kPayloadMax]; std::memset(buf, 0, sizeof buf);
    BossHead h; std::memset(&h, 0, sizeof h); h.key = wire_key(); h.count = 1;
    unsigned char rec[64]; int rn = fill_rec(rec, slot, b, 0);
    BossRec r; std::memcpy(&r, rec, sizeof r);
    int sub = kWorldBossState;
    int len;
    if (!std::strcmp(name, "short")) { len = (int)sizeof h + rn - 5; }
    else if (!std::strcmp(name, "long")) { len = (int)sizeof h + rn + 3; }
    else {
        if (!std::strcmp(name, "count")) h.count = 9;
        else if (!std::strcmp(name, "cls")) r.cls = 200;
        else if (!std::strcmp(name, "slot")) r.slot = 600;
        else if (!std::strcmp(name, "nf")) r.nf = (uint8_t)(r.nf + 1);
        else if (!std::strcmp(name, "key")) h.key.act = (uint8_t)((g_key.act + 1) & 7);
        else if (!std::strcmp(name, "flags")) h.flags = 1;
        else if (!std::strcmp(name, "rflags")) r.rflags = 0x80;
        else if (!std::strcmp(name, "hp")) { const uint32_t v = 200; std::memcpy(rec + sizeof r + 4 * b.d->hp, &v, 4); }
        else if (!std::strcmp(name, "state")) { const uint32_t v = 99; std::memcpy(rec + sizeof r + 4 * b.d->state, &v, 4); }
        else if (!std::strcmp(name, "hit")) { sub = kWorldBossHit; h.flags = 1; }
        else if (!std::strcmp(name, "notboss")) { r.cls = 164; }
        else return;
        std::memcpy(rec, &r, sizeof r);
        len = (int)sizeof h + rn;
    }
    std::fprintf(stderr, "[cbs-knob] forge %s\n", name);
    std::memcpy(buf, &h, sizeof h); std::memcpy(buf + sizeof h, rec, (size_t)rn);
    if (len < 1) len = 1;
    coop_send_world(sub, buf, len);
}

// ---------------------------------------------------------------------------
// ONE STAR FOR ALL. Every console runs the boss's defeat code and each would spawn the star (dActor_c::UntrackAndSpawnStar,
// src/_ZN8dActor_c19UntrackAndSpawnStarERajRK7Vector3h.cpp: UntrackStar, then Spawn(0xb2, ...)); COOPSPAWN1 announces each one and
// the others spawn it too, so a console ended with two. The replacement keeps the ROM's two lines for everything except a boss
// whose authority is another console: that copy retires its star marker and spawns nothing, the authority's star arrives
// through COOPSPAWN1's announcement.
// ---------------------------------------------------------------------------
bool boss_class_for_star(unsigned cls) {
    if (desc_of(cls)) return true;
    return cls == 176 || cls == 198 || cls == 210 || cls == 216 || cls == 218 || cls == 248 || cls == 279;
}

bool star_is_mine(dActor_c *self) {
    if (!g_on || !coop_world_active() || !coop_group_established()) return true;
    const unsigned cls = *(const unsigned short *)((const char *)self + 0x0c);
    if (!boss_class_for_star(cls)) return true;
    const short slot = rd16((const char *)self, 0xce);
    if (slot < 0 || slot >= kSlots) return true;
    if (g_my < 0) g_my = comms_hub_my_seat();
    // bossfix4: the owner sends no boss caps (a 1.0.4 owner): the star is kept here. 1.0.4's own answer (the target seat at each
    // console's star call) can differ between consoles, and both held it back (wk104_g2 row: Whomp King, no star for anyone). A second
    // copy from a friend is merged by coop_spawn's star dup rule (the lower seat's stays, 1.0.4 runs it too), a taken one is not rebuilt.
    if (g_bf2 && (cls == 165 || cls == 248 || cls == 176) && !boss_down(slot, cls) && g_boss[slot].rule == 2) {
        if (g_log) std::fprintf(stderr, "[cbs] star kept here: cls=%u, the owner runs no owner rule (1.0.4) w=%lld\n", cls, wall_ms());
        return true;
    }
    const int auth = desc_of(cls) ? auth_for(slot, hub_now(), cls) : coop_target_seat_of_slot(slot);
    if (auth < 0) return true;
    return auth == g_my;
}

dActor_c *__fastcall star_replacement(dActor_c *self, void * /*edx*/, s8 *trackStarID, u32 starID, const Vector3 *spawnPos, u8 how) {
    self->UntrackStar(*trackStarID);
    { const short sl = rd16((const char *)self, 0xce); if (sl >= 0 && sl < kSlots && g_boss[sl].d) g_boss[sl].def_seen = true; }
    if (!star_is_mine(self)) {
        ++g_st.stars_suppressed;
        if (g_log) std::fprintf(stderr, "[cbs] star suppressed: this console is not the authority of cls=%u (the authority's star comes by announcement) w=%lld\n",
                                (unsigned)*(const unsigned short *)((const char *)self + 0x0c), wall_ms());
        return nullptr;
    }
    ++g_st.stars_spawned;
    return self->Spawn(0xb2, starID | ((u32)how << 4), *spawnPos, 0, ((const s8 *)self)[0xcc], -1);
}

bool g_star_patched = false;
void install_star_patch() {
    if (g_star_patched) return;
    g_star_patched = true;
    dActor_c *(dActor_c::*pmf)(s8 &, u32, const Vector3 &, u8) = &dActor_c::UntrackAndSpawnStar;
    void *from = nullptr; std::memcpy(&from, &pmf, sizeof from);
    DWORD old = 0;
    if (!VirtualProtect(from, 8, PAGE_EXECUTE_READWRITE, &old)) { std::fprintf(stderr, "[cbs] cannot patch UntrackAndSpawnStar\n"); return; }
    unsigned char b[5]; b[0] = 0xE9;
    const int rel = (int)((unsigned char *)&star_replacement - (unsigned char *)from - 5);
    std::memcpy(b + 1, &rel, 4);
    std::memcpy(from, b, 5);
    VirtualProtect(from, 8, old, &old);
    FlushInstructionCache(GetCurrentProcess(), from, 8);
    std::fprintf(stderr, "[cbs] UntrackAndSpawnStar replaced at %p\n", from);
}

// bowser2 proof knob (see g_swing). Phase 0: every tick my player stands 40 units behind Bowser's tail, facing his way, and the ROM's
// Player::TryGrab is asked for the tail (it only agrees during a real punch, which the row's scripted B presses make); on success the
// tail's own grab (d_a_kpa.cpp func_ov060_02115d68) is done with its exact writes. Phase 1: my player turns S a tick (facing and
// turn speed, what the swing does with a spun pad), N ticks long. Then the tail's let-go bit (0x400, what a throw sets) and the
// player's DropActor: the ROM's release path throws him.
void do_swing_knob() {
    const int me = (int)data_0209f250;
    if (me < 0 || me >= 16 || !data_0209f394[me]) return;
    char *pl = (char *)data_0209f394[me];
    Boss *bp = nullptr;
    for (int slot = 0; slot < kSlots; ++slot) if (g_boss[slot].a && g_boss[slot].d->cls == 279) { bp = &g_boss[slot]; break; }
    if (!bp) return;
    Boss &b = *bp;
    char *tail = kpa_tail(b);
    if (!tail) return;
    const uint32_t st = state_of(b);
    if (g_swing.phase == 0) {
        if (st == 4u || st == 5u || st == 6u || rd32(b.a, 0x410) != 0 || rd32(tail, 0x110) != 0) return;
        const double rad = (double)(uint16_t)rd16(b.a, 0x8e) * 6.283185307179586 / 65536.0;
        const int32_t x = rd32(tail, 0x5c) - (int32_t)(std::sin(rad) * 40.0 * 4096.0), y = rd32(b.a, 0x60), z = rd32(tail, 0x64) - (int32_t)(std::cos(rad) * 40.0 * 4096.0);
        std::memcpy(pl + 0x5c, &x, 4); std::memcpy(pl + 0x60, &y, 4); std::memcpy(pl + 0x64, &z, 4);
        const int16_t face = rd16(b.a, 0x8e); std::memcpy(pl + 0x8e, &face, 2);
        const int32_t zero = 0; std::memcpy(pl + 0x98, &zero, 4);
        ++g_swing.tries;
        if (!_ZN6Player7TryGrabER8dActor_c(pl, tail)) {
            // no punch on this tick: after 30 tries the hold is taken the way TryGrab takes it once its checks pass
            if (g_swing.tries < 30 || rd32(pl, 0x358) != 0) return;
            func_ov002_020dacb4(pl, tail);
            std::fprintf(stderr, "[cbs-knob] swing: TryGrab refused %u times (no punch), the player takes the hold directly w=%lld\n", g_swing.tries, wall_ms());
        }
        const int16_t *ip = (const int16_t *)(b.a + 0x8c);
        std::memcpy(pl + 0x8c, ip, 6);
        std::memcpy(pl + 0x92, ip, 6);    // mPrevAngleX/Y/Z
        const int32_t holding = 2; std::memcpy(tail + 0x110, &holding, 4);
        std::memcpy(tail + 0x10c, &pl, sizeof pl);
        uint32_t cf = (uint32_t)rd32(b.a, 0x378); cf |= 1u; std::memcpy(b.a + 0x378, &cf, 4);   // func_ov060_021123a0(0): body collider off
        const int32_t one = 1; std::memcpy(b.a + 0x410, &one, 4);
        std::memcpy(b.a + 0x3a4, &pl, sizeof pl);
        const uint16_t cd = 0x96; std::memcpy(tail + 0x116, &cd, 2);
        g_swing.phase = 1; g_swing.t0 = g_ticks;
        std::fprintf(stderr, "[cbs-knob] swing: my player took Bowser's tail (try %u, bowser st=%u) at %d,%d,%d w=%lld\n", g_swing.tries, st,
                     x >> 12, y >> 12, z >> 12, wall_ms());
        return;
    }
    if (rd32(b.a, 0x410) != 1) {
        std::fprintf(stderr, "[cbs-knob] swing: Bowser left the hold by himself (hold=%d st=%u) w=%lld\n", rd32(b.a, 0x410), st, wall_ms());
        g_swing.phase = 2; return;
    }
    if (g_ticks - g_swing.t0 < g_swing.dur) {
        for (int i = 0; i < g_nstall; ++i) if (g_ticks - g_swing.t0 == g_stall[i].at) g_stall[i].req = true;
        const int16_t sp = (int16_t)g_swing.speed;
        int16_t yaw = rd16(pl, 0x8e); yaw = (int16_t)(yaw + sp);
        std::memcpy(pl + 0x8e, &yaw, 2); std::memcpy(pl + 0x69c, &sp, 2);
        if (((g_ticks - g_swing.t0) % 30) == 0)
            std::fprintf(stderr, "[cbs-knob] swing: turning, my angle %d, Bowser at %d,%d,%d step=%d w=%lld\n", (int)(uint16_t)yaw,
                         rd32(b.a, 0x5c) >> 12, rd32(b.a, 0x60) >> 12, rd32(b.a, 0x64) >> 12, (int)(unsigned char)b.a[0x425], wall_ms());
        return;
    }
    uint32_t tf = (uint32_t)rd32(tail, 0xb0); tf |= 0x400u; std::memcpy(tail + 0xb0, &tf, 4);
    _ZN6Player9DropActorEv(pl);
    g_swing.phase = 2;
    std::fprintf(stderr, "[cbs-knob] swing: let go of the tail after %u ticks (swing %d) w=%lld\n", g_swing.dur, rd32(b.a, 0x3f0), wall_ms());
}

// ---------------------------------------------------------------------------
// PIRANHA1 (AUDIT-BOSSES H3): THE BIG FIRE PIRANHA GROUP COUNTS EVERY PLAYER'S KILLS.
// The five FIREPAKUN (251) on Tiny-Huge Island's huge side (level 24, every act, star 1) are one group: the first to run StateInit is
// the leader (d_a_f_pkn.cpp:665-685) and holds mGroupDefeatedCount; a kill made here adds one (StateWait, :561-593) and the fifth spawns
// the star. Each kill also leaves a death bit (KillAndTrackInDeathTable, or TrackInDeathTable for the leader, which stays in
// STATE_DEFEATED), and the room already shares death bits: a friend's kill removed my copy with no count, so kills split between
// players reached 5 on no console (no star), and a friend's leader kill took my leader away (KillAndTrackInDeathTable), after which
// my plants never came up again (StateWait returns when FindWithID(mGroupLeaderID) is null).
// Now coop_world's reconcile asks coop_boss_pkn_remote_kill before it removes a copy a friend's bit names: the kill is added to my
// leader the way StateWait adds one (the alive count down if my copy was up), my leader copy is kept in its defeated state instead of
// removed, and the group's fifth kill, when it is a friend's, takes my leader away as the cartridge does. The killer's own console
// spawns the star (his count is 5 too) and coop_spawn announces it. A 1.0.4 friend sends the same death bits but counts nobody else's
// kills, so a fifth kill of his can make no star: if no star of that number stands here kPknStarGraceMs after my count reached 5
// this console spawns it, the cartridge's own call; coop_spawn's dup and taken rules (1.0.4 runs the dup rule too) keep it one star.
// No new message: the death bits are what 1.0.4 already sends, so his kills count here too.
// SM64DS_COOP_PIRANHA=0 (dev builds only) is the 1.0.4 control; SM64DS_COOP_PKN_HIT is the proof's hit (below).
// ---------------------------------------------------------------------------
bool pkn_live(const char *a) { return a && !a[0x0f] && a[0x0e] != 2; }

void pkn_census(unsigned now) {
    if (g_pkn_first_t || (g_ticks % 15u) != 0) return;
    int n = 0;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != kPknCls || !pkn_live(a)) continue;
        const daFPkn_c *p = reinterpret_cast<const daFPkn_c *>(a);
        if (p->mState == 0) return;                    // StateInit has not picked the leader yet
        if (!n++) g_pkn_first_t = now | 1u;
        std::fprintf(stderr, "[cpk] group here: slot=%d %s state=%d count=%u alive=%u star=%u at %d,%d,%d w=%lld\n", (int)rd16(a, 0xce),
                     p->mRespawnMode == 1 ? "leader" : "member", (int)p->mState, (unsigned)p->mGroupDefeatedCount, (unsigned)p->mGroupAliveCount,
                     (unsigned)p->mStarID, p->mPosX >> 12, p->mPosY >> 12, p->mPosZ >> 12, wall_ms());
    }
}

void pkn_star_tick(unsigned now) {
    PknStar &s = g_pkn_star;
    if (!s.pending || (int)(now - s.due) < 0) return;
    s.pending = false;
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a)) {
        if (*(const unsigned short *)(a + 0x0c) != 178 || !pkn_live(a) || (rd32(a, 0x08) & 0xff) != (s.param & 0xff)) continue;
        std::fprintf(stderr, "[cpk] star %u is here already (the killer's, announced): none made here w=%lld\n", s.param & 0xff, wall_ms());
        return;
    }
    if (s.join) {
        std::fprintf(stderr, "[cpk] the group was finished before this console watched it (joined): no star made here w=%lld\n", wall_ms());
        return;
    }
    // Spawned AND announced: left to coop_spawn's scan, a star that has already linked itself to the placed marker of its number is a
    // "linked" star every console makes on its own (ctl_p2 row), and the 1.0.4 friend whose kill ended the group would see none.
    const int pos[3] = { s.pos.x, s.pos.y, s.pos.z };
    void *st = coop_spawn_and_announce(178, s.param, pos, s.area);
    if (st) coop_camshield_mark(st, "a Big Fire Piranha star from a friend's last kill");
    std::fprintf(stderr, "[cpk] no star %u came in %u ms (the last kill's console made none, a 1.0.4 one): made here at %d,%d,%d (%s) w=%lld\n",
                 s.param & 0xff, (unsigned)kPknStarGraceMs, s.pos.x >> 12, s.pos.y >> 12, s.pos.z >> 12, st ? "spawned" : "SPAWN FAILED", wall_ms());
}

#ifndef PORT_ROM_CLEAN
void pkn_knob_tick() {
    PknKnob &k = g_pknk;
    if (!k.on || k.at >= k.n) return;
    if (!k.listed) {                                   // listed at the first sight (before a friend's kill can take one away)
        k.nslots = 0;
        for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a && k.nslots < 8; a = (char *)_ZN8dActor_c4NextEPKS_(a))
            if (*(const unsigned short *)(a + 0x0c) == kPknCls) k.slots[k.nslots++] = rd16(a, 0xce);
        if (!k.nslots) return;
        std::sort(k.slots, k.slots + k.nslots);
        k.listed = true;
        std::fprintf(stderr, "[cpk-knob] %d plants, slots %d %d %d %d %d (by index)\n", k.nslots, k.slots[0], k.nslots > 1 ? k.slots[1] : -1,
                     k.nslots > 2 ? k.slots[2] : -1, k.nslots > 3 ? k.slots[3] : -1, k.nslots > 4 ? k.slots[4] : -1);
    }
    if (g_ticks < k.tick) return;
    const int want = k.idx[k.at];
    char *a = nullptr;
    if (want >= 0 && want < k.nslots)
        for (char *x = (char *)_ZN8dActor_c4NextEPKS_(0); x; x = (char *)_ZN8dActor_c4NextEPKS_(x))
            if (*(const unsigned short *)(x + 0x0c) == kPknCls && rd16(x, 0xce) == k.slots[want] && pkn_live(x)) { a = x; break; }
    daFPkn_c *p = reinterpret_cast<daFPkn_c *>(a);
    if (!p || p->mState == kPknStateDefeated) {
        std::fprintf(stderr, "[cpk-knob] plant %d (slot %d) is gone here already: next w=%lld\n", want, want >= 0 && want < k.nslots ? k.slots[want] : -1, wall_ms());
        ++k.at; k.hit_logged = false; return;
    }
    if (p->mSpinCount != 0 || p->mSuppressDeathReward == 0) {
        std::fprintf(stderr, "[cpk-knob] plant %d (slot %d) took the hit (spin %u): next w=%lld\n", want, k.slots[want], (unsigned)p->mSpinCount, wall_ms());
        ++k.at; k.hit_logged = false; k.next_place = 0; return;
    }
    const int me = (int)data_0209f250 & 15;
    char *pl = (char *)data_0209f394[me];
    if (!pl) return;
    if (g_ticks >= k.next_place) {
        k.next_place = g_ticks + 30;
        const int32_t x = p->mPosX + (k.dist << 12), y = p->mPosY + 0x8000, z = p->mPosZ, zero = 0;
        const int16_t ang = (int16_t)0xc000;           // facing -x, toward the plant
        std::memcpy(pl + 0x5c, &x, 4); std::memcpy(pl + 0x60, &y, 4); std::memcpy(pl + 0x64, &z, 4);
        std::memcpy(pl + 0x98, &zero, 4); std::memcpy(pl + 0x8e, &ang, 2);
    }
    if (p->mEmerged && p->mScale == p->mMaxScale && p->mState == kPknStateSpit) {
        p->mdCcAcPos_c.otherOwner = reinterpret_cast<dActor_c *>(pl)->uniqueID;
        p->mdCcAcPos_c.hitFlags = 0x20;
        if (!k.hit_logged) {
            k.hit_logged = true;
            std::fprintf(stderr, "[cpk-knob] plant %d (slot %d) is up: my player's hit given to its mouth collider w=%lld\n", want, k.slots[want], wall_ms());
        }
    }
}
#endif

void on_frame(unsigned now) {
    read_env();
    now += g_clock_add;                       // bossfix2: the proof clock (0 unless SM64DS_COOP_BOSS_CLOCK_ADD is set in a dev build)
    if (!coop_world_active()) return;
    if (!g_regd) { g_regd = true; g_my = comms_hub_my_seat(); }
    install_star_patch();
    int lv = -1, ac = 0;
    if (!coop_group_established() || !coop_group_key(&lv, &ac)) return;
    const CoopKey nk(lv, ac);
    if (nk != g_key) {
        for (int i = 0; i < kSlots; ++i) g_boss[i] = Boss();
        for (int s = 0; s < kSeats; ++s) g_claim_until[s] = 0;
        g_recent_t = 0; g_recent_mine = true;
        std::memset(g_dead_cls, 0, sizeof g_dead_cls); std::memset(g_dead_tx, 0, sizeof g_dead_tx);
        std::memset(g_bclaim, 0, sizeof g_bclaim); g_trip = Trip();
        g_pkn_first_t = 0; g_pkn_star = PknStar();     // PIRANHA1: a new visit, a new group
        g_key = nk;
    }
    ++g_ticks; ++g_st.ticks;
    scan();
    if (g_pkn_on) { pkn_census(now); pkn_star_tick(now); }   // PIRANHA1
#ifndef PORT_ROM_CLEAN
    pkn_knob_tick();
#endif
    g_st.bosses = (unsigned long long)g_nb;
    if (g_god) { const int me = (int)data_0209f250 & 15; data_02092144[me] = (short)((data_02092144[me] & 0xff) | (8 << 8)); }
    if (!g_stand_done && g_ticks >= g_stand_tick && g_nb > 0) {
        for (int slot = 0; slot < kSlots; ++slot) {
            Boss &b = g_boss[slot];
            const int me = (int)data_0209f250;
            if (!b.a || me < 0 || me >= 16 || !data_0209f394[me]) continue;
            char *pl = (char *)data_0209f394[me];
            int32_t x = rd32(b.a, 0x5c) + (g_stand_dist << 12), y = rd32(b.a, 0x60) + 0x10000, z = rd32(b.a, 0x64);
            std::memcpy(pl + 0x5c, &x, 4); std::memcpy(pl + 0x60, &y, 4); std::memcpy(pl + 0x64, &z, 4);
            const int32_t zero = 0; std::memcpy(pl + 0x98, &zero, 4);
            if (g_stand_every) g_stand_tick = g_ticks + g_stand_every; else g_stand_done = true;
            std::fprintf(stderr, "[cbs-knob] stand local player %d units from boss slot=%d cls=%u at %d,%d,%d w=%lld\n", g_stand_dist, slot, (unsigned)b.d->cls, x >> 12, y >> 12, z >> 12, wall_ms());
            break;
        }
    }
    if (!g_behind_done && g_ticks >= g_behind_tick && g_nb > 0) {
        for (int slot = 0; slot < kSlots; ++slot) {
            Boss &b = g_boss[slot];
            const int me = (int)data_0209f250;
            if (!b.a || me < 0 || me >= 16 || !data_0209f394[me]) continue;
            char *pl = (char *)data_0209f394[me];
            const int16_t ang = rd16(b.a, 0x8e);
            const double rad = (double)(uint16_t)ang * 6.283185307179586 / 65536.0;
            int32_t x = rd32(b.a, 0x5c) - (int32_t)(std::sin(rad) * g_behind_dist * 4096.0), y = rd32(b.a, 0x60) + 0x10000, z = rd32(b.a, 0x64) - (int32_t)(std::cos(rad) * g_behind_dist * 4096.0);
            std::memcpy(pl + 0x5c, &x, 4); std::memcpy(pl + 0x60, &y, 4); std::memcpy(pl + 0x64, &z, 4);
            const int32_t zero = 0; std::memcpy(pl + 0x98, &zero, 4);
            std::memcpy(pl + 0x8e, &ang, 2);
            if (g_behind_every) g_behind_tick = g_ticks + g_behind_every; else g_behind_done = true;
            std::fprintf(stderr, "[cbs-knob] behind local player %d units behind boss slot=%d cls=%u (boss angle %d) at %d,%d,%d w=%lld\n", g_behind_dist, slot, (unsigned)b.d->cls, (int)ang, x >> 12, y >> 12, z >> 12, wall_ms());
            break;
        }
    }
    pump_dead(now);
    if (g_on && g_bf2 && g_caps_send && (!g_caps_tx_t || coop_age_ms(now, g_caps_tx_t) >= kCapsEveryMs)) {   // bossfix4: my boss caps
        g_caps_tx_t = now | 1u;
        BossCaps c; std::memset(&c, 0, sizeof c);
        c.key = wire_key(); c.ver = kBossCapsVer; c.caps = kBossCapOwnerRule;
        coop_send_world(kWorldBossDefeat, &c, (int)sizeof c);
    }
    if (g_swing.phase >= 0 && g_swing.phase < 2 && g_ticks >= g_swing.from && g_nb > 0) do_swing_knob();
    if (g_face && g_nb > 0 && (g_ticks % 12) == 0) {
        for (int slot = 0; slot < kSlots; ++slot) {
            Boss &b = g_boss[slot];
            const int me = (int)data_0209f250;
            if (!b.a || b.d->cls != 279 || state_of(b) != 4u || me < 0 || me >= 16 || !data_0209f394[me]) continue;
            char *pl = (char *)data_0209f394[me];
            const int32_t bx = rd32(b.a, 0x5c), by = rd32(b.a, 0x60), bz = rd32(b.a, 0x64);
            const int32_t px = bx - (300 << 12), pz = bz;
            const int16_t ang = (int16_t)(std::atan2((double)(bx - px), (double)(bz - pz)) * 32768.0 / 3.14159265358979323846);
            const int32_t py = by + 0x4000, zero = 0;
            std::memcpy(pl + 0x5c, &px, 4); std::memcpy(pl + 0x60, &py, 4); std::memcpy(pl + 0x64, &pz, 4);
            std::memcpy(pl + 0x98, &zero, 4); std::memcpy(pl + 0x8e, &ang, 2);
            if (g_log && (g_ticks % 120) == 0) std::fprintf(stderr, "[cbs-knob] face: local player put 300 units from Bowser, facing him (step %d) w=%lld\n", (int)(unsigned char)b.a[0x423], wall_ms());
            break;
        }
    }
    if (g_nb > 0) {
        process(now);
        for (int i = 0; i < g_nhit; ++i)
            if (!g_hit[i].done && g_ticks >= g_hit[i].tick) do_hit_knob(g_hit[i]);
        for (int i = 0; i < g_nforge; ++i)
            if (!g_forge[i].done && g_ticks >= g_forge[i].tick) { g_forge[i].done = true; do_forge(g_forge[i].name); }
        for (int i = 0; i < g_nmkill; ++i)
            if (!g_mkill[i].done && g_ticks >= g_mkill[i].tick) do_minion_kill(g_mkill[i]);
        if (g_bump.on && g_bump.left > 0 && g_ticks >= g_bump.tick) do_bump();
        if (g_bstand.on && g_ticks >= g_bstand.tick) do_bstand();
    }
    if (g_log && g_nb > 0 && (unsigned)(now - g_stat_t) >= 4000u) {
        g_stat_t = now;
        const CoopBossStats &s = g_st;
        std::fprintf(stderr, "[cbsstat] seat=%d bosses=%llu sent=%llu recs=%llu hits_sent=%llu rx=%llu rrx=%llu hits_rx=%llu app st%llu hp%llu mism%llu hit_local=%llu "
                     "hit_adopted=%llu drop bad%llu grp%llu mem%llu auth%llu cls%llu off%llu seat%llu\n",
                     g_my, s.bosses, s.sent, s.recs_sent, s.hits_sent, s.rx, s.recs_rx, s.hits_rx, s.applied_state, s.applied_hp, s.state_mismatch,
                     s.hit_local, s.hit_adopted, s.drop_bad, s.drop_group, s.drop_member, s.drop_notauth, s.drop_cls, s.drop_off, s.drop_seat);
    }
}

struct Registrar {
    Registrar() { coop_on_frame(on_frame); coop_on_snapshot_request(on_snapshot_request); }
} g_registrar;

}  // namespace

// ---------------------------------------------------------------------------
// Public.
// ---------------------------------------------------------------------------
CoopBossStats coop_boss_stats() { return g_st; }

// bossfix1 hooks for coop_target and coop_spawn.
bool coop_boss_prefer_local(const char *actor) {
    if (!g_on || !actor) return false;
    const unsigned short cls = *(const unsigned short *)(actor + 0x0c);
    if (cls == 279) return rd32(actor, 0x40c) == 4;
    // bossfix2: Eyerok's head in his talk state (record b0ac, its first step asks ClosestPlayer for the player it holds and talks to) and
    // Wiggler's DEMOCALL (its entry asks ClosestPlayer for the player of his intro talk): my own player, so the talk runs here
    if (g_bf2 && cls == 176 && rd32(actor, 0x49c) == 0 && eye_ptr_to_id((uint32_t)rd32(actor, 0x48c)) == kEyeTalk) return true;
    if (g_bf2 && cls == 248 && rd32(actor, 0x8c4) == 1) return true;
    // kbo2: King Bob-omb down (HP 0): his hurt state's tick turns to ClosestPlayer before the defeat talk, which runs with my player
    return cls == 189 && g_kbo_fix && rd32(actor, 0x500) <= 0;
}

// kbo2: who drives King Bob-omb's pose (coop_actors' MOVERS9 rule asks): the boss's authority, so the pose and the state come from one
// console. Not while he is carried on this console (states 7 / 8): the carry keeps the MOVERS9 rule it was proven with. -1 = no answer.
int coop_boss_driver_seat(int slot) {
    read_env();
    if (!g_on || !g_kbo_fix || slot < 0 || slot >= kSlots || g_my < 0) return -1;
    const Boss &b = g_boss[slot];
    if (!b.a || !b.d || b.d->cls != 189) return -1;
    const uint32_t st = state_of(b);
    if (st == 7u || st == kKboHeld) return -1;
    return auth_for(slot, hub_now(), 189);
}
// coopgoomboss2 hooks for coop_spawn.
namespace {
char *find_goomboss() {
    for (char *a = (char *)_ZN8dActor_c4NextEPKS_(0); a; a = (char *)_ZN8dActor_c4NextEPKS_(a))
        if (*(const unsigned short *)(a + 0x0c) == 198 && !a[0x0f] && a[0x0e] != 2 && !is_player_ptr(a)) return a;
    return nullptr;
}
}  // namespace

bool coop_boss_minion_mine(const char *minion) {
    if (!minion || *(const unsigned short *)(minion + 0x0c) != 200) return false;
    const uint32_t tid = (uint32_t)rd32(minion, 0x43c);
    if (!tid) return false;
    const char *boss = (const char *)dActor_c::FindWithID(tid);
    if (!boss || *(const unsigned short *)(boss + 0x0c) != 198) return false;
    read_env();
    if (!g_minion_share) return false;                       // control: nothing is announced, each console keeps its own set
    if (!g_on) return false;                                 // INT (REVIEW-A note 1): boss sync off, no shared set and nothing announced
    if (!coop_world_active() || !coop_group_established()) return true;
    const short slot = rd16(boss, 0xce);
    if (slot < 0 || slot >= kSlots) return true;
    if (g_my < 0) g_my = comms_hub_my_seat();
    return auth_for(slot, hub_now()) == g_my;
}

bool coop_boss_minion_ready() {
    read_env();
    return g_minion_share && find_goomboss() != nullptr;
}

char *coop_boss_spawn_minion(const int pos[3], unsigned param, int area, unsigned index) {
    char *boss = find_goomboss();
    if (!boss || !pos) return nullptr;
    Vector3 p; std::memcpy((void *)&p, pos, 12);         // three Fix12 words
    ((dActor_c *)boss)->DisappearPoofDustAt(p);             // the summon's puff
    char *m = (char *)dActor_c::Spawn(0xc8, param, p, (const Vector3_16 *)(boss + 0x8c), (s8)area, -1);
    if (!m) return nullptr;
    std::memcpy(m + 0x43c, boss + 0x04, 4);                 // mTargetUniqueID = the boss
    int32_t home[3]; std::memcpy(home, boss + 0x5d0, sizeof home);   // the boss's mSpawnPos
    func_ov084_021296b0(m, home);
    m[0x474] = (char)(unsigned char)index;                  // mMinionIndex
    boss[0x602] = (char)(unsigned char)((unsigned char)boss[0x602] + 1);   // mSpawnedCount, as the summon raises it
    if (g_log) std::fprintf(stderr, "[cbs] goomba copy built: index=%u at %d,%d,%d uid=%u boss count now %u w=%lld\n", index & 0xffu, pos[0] >> 12, pos[1] >> 12, pos[2] >> 12,
                            (unsigned)rd32(m, 0x04), (unsigned)(unsigned char)boss[0x602], wall_ms());
    return m;
}

bool coop_boss_reward_not_mine() {
    static const bool off = [] { const char *e = std::getenv("SM64DS_COOP_REWARD"); return e && *e == '0'; }();     // must-fail control: no suppression
    if (off || !g_on || !coop_world_active() || !coop_group_established() || !g_recent_t) {
        if (g_log) std::fprintf(stderr, "[cbs] reward decision: no (off=%d on=%d world=%d est=%d recent=%u)\n", (int)off, (int)g_on, (int)coop_world_active(), (int)coop_group_established(), g_recent_t);
        return false;
    }
    const bool r = (int)(hub_now() - g_recent_t) < 4000 && !g_recent_mine;
    if (g_log) std::fprintf(stderr, "[cbs] reward decision: not mine=%d (boss seen %u ms ago, mine=%d)\n", (int)r, (unsigned)(hub_now() - g_recent_t), (int)g_recent_mine);
    return r;
}

int coop_boss_bully_claim_seat(int slot) {
    if (!g_on || !coop_world_active()) return -1;
    return bully_claim_seat(slot, hub_now());
}
int coop_bully_virtual_slot(const void *actor) {
    return trip_slot_of(static_cast<const char *>(actor));
}

bool coop_boss_validate(const void *payload, int len) {
    if (!payload || len < (int)sizeof(BossHead) + (int)sizeof(BossRec)) return false;
    const unsigned char *p = static_cast<const unsigned char *>(payload);
    BossHead h; std::memcpy(&h, p, sizeof h);
    if (!coop_key_ok(h.key) || h.flags > 1 || h.reserved != 0) return false;
    if (h.count < 1 || h.count > kBossMaxRecs) return false;
    int off = (int)sizeof h;
    for (int i = 0; i < (int)h.count; ++i) {
        if (off + (int)sizeof(BossRec) > len) return false;
        BossRec r; std::memcpy(&r, p + off, sizeof r);
        if (r.slot >= kSlots || (r.rflags & ~0x07) != 0) return false;
        const BDesc *d = desc_of(r.cls);
        if (!d || r.nf != d->nf) return false;
        off += (int)sizeof r + 4 * r.nf;
        if (off > len) return false;
    }
    return off == len;
}

void coop_boss_on_world(int seat, int sub, const unsigned char *p, int len) {
    read_env();
    if (!g_on) { ++g_st.drop_off; return; }
    if (seat < 0 || seat >= kSeats || seat == g_my) { ++g_st.drop_seat; return; }
    if (sub == kWorldBossDefeat) {                // bossfix4: a friend's boss caps
        BossCaps c;
        if (len != (int)sizeof c) { ++g_st.drop_bad; return; }
        std::memcpy(&c, p, sizeof c);
        if (!coop_key_ok(c.key) || c.ver < kBossCapsVer) { ++g_st.drop_bad; return; }
        if (!g_caps_t[seat] && g_log) std::fprintf(stderr, "[cbs] boss caps from seat %d: 0x%x (owner rule %s) w=%lld\n", seat, (unsigned)c.caps,
                                                   (c.caps & kBossCapOwnerRule) ? "yes" : "no", wall_ms());
        g_caps[seat] = c.caps; g_caps_t[seat] = hub_now() | 1u;
        return;
    }
    if (sub != kWorldBossState && sub != kWorldBossHit) { ++g_st.drop_bad; return; }
    if (!coop_boss_validate(p, len)) { ++g_st.drop_bad; return; }
    BossHead h; std::memcpy(&h, p, sizeof h);
    if ((sub == kWorldBossHit) != (h.flags == 1) || (sub == kWorldBossHit && h.count != 1)) { ++g_st.drop_bad; return; }
    ++g_st.rx;
    if (!g_key.valid() || h.key.level != (uint16_t)g_key.level || h.key.act != (uint8_t)g_key.act) { ++g_st.drop_group; return; }
    if (!((coop_group_member_mask() >> seat) & 1u)) { ++g_st.drop_member; return; }
    const unsigned now = hub_now();
    int off = (int)sizeof h;
    for (int i = 0; i < (int)h.count; ++i) {
        BossRec r; std::memcpy(&r, p + off, sizeof r);
        const BDesc *d = desc_of(r.cls);
        uint32_t w[kBossMaxFields]; std::memset(w, 0, sizeof w);
        std::memcpy(w, p + off + sizeof r, 4u * r.nf);
        off += (int)sizeof r + 4 * r.nf;
        if (r.rflags & 4) {                       // "this boss is dead" (sent after its defeat, and to a joiner)
            if (sub == kWorldBossState && h.flags == 0 && d && r.slot < kSlots && g_dead_cls[r.slot] != r.cls) {
                g_dead_cls[r.slot] = r.cls; ++g_st.dead_marked;
                if (g_log) std::fprintf(stderr, "[cbs] dead boss noted: cls=%u slot=%d from seat %d w=%lld\n", (unsigned)r.cls, (int)r.slot, seat, wall_ms());
            }
            continue;
        }
        Boss &b = g_boss[r.slot];
        if (!b.a || b.d != d) { ++g_st.drop_cls; continue; }
        bool ok = true;
        for (int k = 0; k < d->nf; ++k) if (!value_ok(*d, k, w[k])) ok = false;
        if (!ok) {
            ++g_st.drop_bad;
            if (g_bad_logged < 12) {        // kbo2: which field and value a rejected record carried (bug 27 had no trace of it)
                ++g_bad_logged;
                for (int k = 0; k < d->nf; ++k)
                    if (!value_ok(*d, k, w[k])) std::fprintf(stderr, "[cbs] record refused: cls=%u slot=%d field %d (+0x%x) = %u (0x%x) from seat %d w=%lld\n", (unsigned)d->cls, (int)r.slot, k, (unsigned)d->f[k].off, w[k], w[k], seat, wall_ms());
            }
            continue;
        }
        const int auth = auth_for(r.slot, now, d->cls);
        if (sub == kWorldBossHit) {
            ++g_st.hits_rx;
            // the hitter's copy was hit: take its HP at once everywhere; the authority takes its state whole
            const int lhp = (int)read_field(b.a, d->f[d->hp]);
            if ((int)w[d->hp] < lhp) {
                write_field(b.a, *d, d->hp, w[d->hp]);
                b.synced_hp = (int)w[d->hp];
                ++g_st.applied_hp;
                if (g_log) std::fprintf(stderr, "[cbs] hit-rx slot=%d hp %d->%u from seat %d (auth %d) w=%lld\n", (int)r.slot, lhp, w[d->hp], seat, auth, wall_ms());
                if (auth != g_my && (int32_t)w[d->hp] <= 0) skip_defeat_talk(b, true);
            }
            // COOPBOO5: my hidden Big Boo is not forced out of (or into) state 0 by a friend's hit: he comes out by his own code
            const uint32_t hst = state_of(b), rst = w[d->state];
            if (hidden_big_boo(b) && hst == 0u && rst != 0u) big_boo_bring_out(b, (int)r.slot, "a friend hit his");
            if (auth == g_my && hidden_big_boo(b) && (hst == 0u || rst == 0u)) continue;
            // kbo2: the last hit's record is the hitter's King already in its defeat: the owner takes the HP (above) and runs the
            // cartridge's ending itself (kbo_defeat_tick), with the talk, instead of jumping into the defeat state with no talk
            if (auth == g_my && d->cls == 189 && g_kbo_fix && (int32_t)read_field(b.a, d->f[d->hp]) <= 0) {
                kbo_defeat_tick(b);
                continue;
            }
            if (auth == g_my) {
                const int hp_keep = (int)read_field(b.a, d->f[d->hp]);
                if (d->cls == 189 && g_kbo_fix && w[d->state] != kKboThrowBomb) kbo_release_bombs(b.a);   // kbo2: no bomb left pinned to his hand
                give_player_ptrs(b);
                for (int k = 0; k < d->nf; ++k) if (k != d->hp) write_field(b.a, *d, k, w[k]);
                write_field(b.a, *d, d->hp, (uint32_t)hp_keep);
                post_force(b);
                b.synced_hp = hp_keep;
                ++g_st.hit_adopted;
                b.lw_valid = false;
            }
            continue;
        }
        if ((r.rflags & 2) && is_bully(d->cls)) {     // coopbully1: that seat's player knocked this bully: it owns this slot for a while
            if (!g_bully_on) { ++g_st.drop_notauth; continue; }
            if (g_log && !(g_bclaim[r.slot][seat] && (int)(g_bclaim[r.slot][seat] - now) > 0))
                std::fprintf(stderr, "[cbs] bully claim-rx slot=%d cls=%u from seat %d (its player knocked it; was auth %d) st=%u w=%lld\n", (int)r.slot, (unsigned)d->cls, seat,
                             auth, w[d->state], wall_ms());
            g_bclaim[r.slot][seat] = (now + kBullyClaimMs) | 1u; ++g_st.claims;
            if (auth_for(r.slot, now, d->cls) != seat) { ++g_st.drop_notauth; continue; }   // two claims at once: the lower seat's copy is the one
        } else if (r.rflags & 2) {               // a claim: that seat's copy is carried by its player
            if (g_log && !claim_live(g_claim_until[seat], now)) std::fprintf(stderr, "[cbs] claim-rx slot=%d from seat %d (that console's player holds the boss) w=%lld\n", (int)r.slot, seat, wall_ms());
            g_claim_until[seat] = claim_stamp(now); ++g_st.claims;
        } else if (seat != auth) {
            // a late record from the authority before this one, within two seconds, still counts
            if (!(b.last_auth == seat && (unsigned)(now - b.auth_t) < 2000u)) {
                // two consoles can briefly disagree about the authority (the target seat changes with a 1 s hold): a lower HP is a hit and
                // counts for everybody whoever sent it, so the hit is never lost to the disagreement
                int rh = (int)w[d->hp]; if (((d->f[d->hp].kind & kS8) || d->cls == 189) && rh < 0) rh = 0;
                const int lh = (int)read_field(b.a, d->f[d->hp]);
                if (d->cls != 218 && rh >= 0 && rh < lh) {
                    write_field(b.a, *d, d->hp, (uint32_t)rh); b.synced_hp = rh; ++g_st.applied_hp;
                    if (g_log) std::fprintf(stderr, "[cbs] hp-from-nonauth slot=%d hp %d->%d from seat %d (auth %d) w=%lld\n", (int)r.slot, lh, rh, seat, auth, wall_ms());
                    if (auth != g_my && rh <= 0) skip_defeat_talk(b, true);
                }
                ++g_st.drop_notauth; continue;
            }
        }
        b.rx.valid = true; b.rx.fresh = true; b.rx.t = now; b.rx.seq = r.seq; b.rx.rflags = r.rflags; b.rx.from = seat;
        std::memcpy(b.rx.w, w, sizeof w);
    }
}

// PIRANHA1: a friend's death bit names my copy of a Big Fire Piranha. Count his kill on my leader the way StateWait counts mine
// (d_a_f_pkn.cpp:561-593: the alive count down when the plant was up here, the defeated count up, the fifth ends the group), then
// tell reconcile whether to remove the copy. The leader's own end on the killer's console is TrackInDeathTable + STATE_DEFEATED, so
// my leader copy goes the same way (hidden, its colliders off) and stays: the other plants here still find their leader. The bit
// stays in the room's remote set, so reconcile asks again later: a copy already in STATE_DEFEATED is left alone.
bool coop_boss_pkn_remote_kill(char *actor) {
    read_env();
    if (!g_on || !g_pkn_on || !actor || *(const unsigned short *)(actor + 0x0c) != kPknCls) return true;
    daFPkn_c *p = reinterpret_cast<daFPkn_c *>(actor);
    if (p->mState == kPknStateDefeated) return false;              // my leader copy, already counted and kept
    if (p->mState == 0 || p->mGroupLeaderID == 0) return false;     // StateInit has not set the group up yet: the next pass
    daFPkn_c *leader = reinterpret_cast<daFPkn_c *>(dActor_c::FindWithID(p->mGroupLeaderID));
    if (!leader || !pkn_live(reinterpret_cast<char *>(leader))) {
        std::fprintf(stderr, "[cpk] friend's kill of slot=%d: no group leader here, removed uncounted w=%lld\n", (int)rd16(actor, 0xce), wall_ms());
        return true;
    }
    const unsigned now = hub_now();
    if (!g_pkn_first_t) g_pkn_first_t = now | 1u;
    const bool was_up = p->mEmerged != 0;
    if (was_up) { p->mEmerged = 0; if (leader->mGroupAliveCount) --leader->mGroupAliveCount; }
    ++leader->mGroupDefeatedCount;
    const bool is_leader = p == leader;
    std::fprintf(stderr, "[cpk] friend's kill counted here: slot=%d (%s%s), group count %u/%u, up %u w=%lld\n", (int)rd16(actor, 0xce),
                 is_leader ? "the leader" : "a member", was_up ? ", up on this screen" : "", (unsigned)leader->mGroupDefeatedCount,
                 (unsigned)kPknGroupSize, (unsigned)leader->mGroupAliveCount, wall_ms());
    if (leader->mGroupDefeatedCount == kPknGroupSize) {
        PknStar &s = g_pkn_star;
        s = PknStar();
        s.pending = true; s.due = (now + kPknStarGraceMs) | 1u; s.join = coop_age_ms(now, g_pkn_first_t) < kPknJoinMs;
        s.pos.x = p->mPosX; s.pos.y = p->mPosY; s.pos.z = p->mPosZ; s.area = p->mAreaId;
        s.param = (unsigned)p->mStarID | 0x40u; s.slot = rd16(actor, 0xce);
        std::fprintf(stderr, "[cpk] group done by a friend's kill: the killer's console makes star %u%s w=%lld\n", s.param & 0xff,
                     s.join ? " (finished before I watched)" : "; made here if none comes", wall_ms());
        if (!is_leader) leader->KillAndTrackInDeathTable();         // the cartridge's group end: leader, then the fifth plant
        return true;
    }
    if (is_leader) {
        p->mState = kPknStateDefeated; p->mScale = 0; p->mSpinCount = 0;
        p->mdCcAc_c.flags |= 1; p->mdCcAcPos_c.flags |= 1; p->mFlags &= ~0x10000000u;
        return false;
    }
    return true;
}

}  // namespace port
