// THE LOCKSTEP DRIVER, transcribed. Run mg16, lane MP2.
// Why this is a transcription and not the linked TU is in comms_lockstep.h.
//
// EVERY MOVE BELOW CARRIES ITS ROM LINE. If a line here has no citation it is
// port plumbing and says so. The point of the citations is that a reviewer can
// diff this file against src/func_0203ea5c.c and src/func_0203df40.c without
// trusting a word of the prose.
//
// THE TWO BYTE LAYOUTS, both read off the ROM and both already written down in
// comms_seam.h's wire-format table:
//
//   THE 0x20 BLOCK          THE 0x24 RECORD (data_020a1040 local,
//   (staged at              data_020a1154 + i*0x24 per player)
//    data_020a1020)
//   +0x00 (2) flag    <-->  +0x0C (2) flag word, 0x8000 = live
//   +0x02 (4) frame   <-->  +0x00 (4) frame counter
//   +0x06 (2) key     <-->  +0x0E (2) key word
//   +0x08 (1) stylus x<-->  +0x04 (1) stylus x
//   +0x09 (1) stylus y<-->  +0x06 (1) stylus y
//   +0x0A (1) touch   <-->  +0x08 (1) touch
//   +0x0B (2) heading <-->  +0x10 (2) heading
//   +0x0D (1) players  ->   data_020a0f08, slot 0 only
//   +0x0E (1) reserved ->   data_020a0f00, slot 0 only
//   +0x0F (17) payload<-->  +0x12 (17) game payload
//
// The left column is src/func_0203ea5c.c:177-186 (staging, ten CpuCopy8 moves)
// and :277-290 (unpack, the mirror image). The right column's host symbols are
// the grouped .dsstate$camcomm / $camrec runs in hal/camera_bridges.cpp, which
// put the DS's named symbols back in ROM order precisely so a base-plus-offset
// walk like the one below is legal.

#include "comms_lockstep.h"

#include "comms_loopback.h"
#include "comms_seam.h"

#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {

// The local record, 0x020a1040..0x020a1064. Named symbols rather than one base
// plus offsets, because that is how src/func_0203ea5c.c:177-186 names them and
// a reviewer should be able to match them one for one.
extern unsigned char data_020a1040[];   // +0x00 (4)    frame counter
extern unsigned char data_020a1044[];   // +0x04 (2)    stylus x
extern unsigned char data_020a1046[];   // +0x06 (2)    stylus y
extern unsigned char data_020a1048[];   // +0x08 (4)    touch, and unkA
extern unsigned char data_020a104c[];   // +0x0C (2)    flag word
extern unsigned char data_020a104e[];   // +0x0E (2)    key word
extern unsigned char data_020a1050[];   // +0x10 (2)    heading (the camera's)
extern unsigned char data_020a1052[];   // +0x12 (0x12) game payload

// The four per-player records, stride 0x24. comms_seam.cpp's report already
// walks them exactly this way.
extern unsigned char data_020a1154[];

extern unsigned char data_020a0f04[];   // the ROLE byte (hal/stage_slot0.cpp)
extern unsigned char data_020a0f00[];   // slot 0's reserved byte (hal/auto_bss.cpp)
extern int data_020a0f10[];             // MY comms slot (hal/actor_vtables.cpp)
extern int data_020a0f1c[];             // the flag word (hal/player_bridges.cpp)

// The port's own input for this frame, read BEFORE the fan-out overwrites it.
extern unsigned char data_020a0de8[];   // TouchInfo[4], {now, chg, x, y}
extern unsigned short data_020a0e58[];  // PadData[4], {held, pressed}

// The eight seam faces, signatures exactly as hal/comms_seam.cpp defines them.
void func_020408b0(unsigned short mode);
void func_02040820(void);
void func_02040790(void);
void func_02040724(void);
int  func_02040714(void);
int  func_02040704(int ignored);
int  func_020406b4(const void *block, unsigned short *status);
const void *func_0204068c(unsigned short aid);

}  // extern "C"

// ---------------------------------------------------------------------------
// data_020a0f08, THE PLAYER-COUNT BYTE, hosted here because nothing hosted it.
//
// src/func_0203ea5c.c:288 writes block+0x0D into it, and it is the only symbol
// this file needs that the tree did not already have. SIZED BY ROM SPAN
// (0x020a0f08 -> 0x020a0f0c = 4) and not by the one byte that reads it, which
// is the undersized-hosted-global trap this tree has been bitten by before.
//
// THE SECTION SUFFIX IS A MEASUREMENT, NOT A PREFERENCE, and it is MP1's
// reasoning applied one step further. port/tools/battery.py's header: some
// hosted DS data reaches the geometry stream as a POINTER VALUE, so an
// insertion INTERIOR to .dsstate shifts every global past it and changes the
// rendered frame while leaving the section base where it was. MP1 put its three
// new globals at $wcomms because that sorts after every family the tree uses
// and before the $zzz sentinel, so nothing existing moved.
//
// $xmp2 sorts after $wcomms ('x' > 'w') and still before $zzz. That is
// deliberately stricter than reusing $wcomms: appending to an existing group
// leaves the order WITHIN the group to the linker, and if this global landed
// ahead of MP1's three then MP1's three would move. A suffix of its own makes
// "not one existing hosted global moves" a property of the sort order rather
// than a hope about link order.
//
// RETIREMENT CONDITION, the same one MP1 wrote: if this ever has to be
// ROM-SPACED against data_020a0f00 or data_020a0f04 (its ROM neighbours), it
// moves into that band and every BMP baseline taken against this layout has to
// be retaken. Nothing needs that today -- no code reaches one of the three as
// an interior address of another.
#pragma section(".dsstate$wcomms", read, write)
extern "C" {
__declspec(allocate(".dsstate$wcomms")) __declspec(align(4))
unsigned char data_020a0f08[4] = {0};   // 0x020a0f08 .. 0x020a0f0c
}

namespace port {
namespace {

bool g_opened      = false;
bool g_ever_round  = false;
bool g_said_solo   = false;

unsigned long long g_ticks = 0, g_rounds = 0, g_timeouts = 0;
unsigned long long g_spins = 0, g_peer_updates = 0;

// SM64DS_COMMS_INJECT="key=<hex>[,x=<n>][,y=<n>][,touch=<n>]"
//
// A scripted local record, for the proof runs. It exists because the harness's
// own scripted input -- SM64DS_PAD_TEST and SM64DS_CLICK_TEST -- is READ AS
// UNSET UNDER SM64DS_WINDOW_SELFTEST (walk_window.cpp:3388 and :3480 both gate
// on g_selftest), and the headless two-instance proofs are selftest runs. So
// there was no way to put a known value on the wire in the configuration the
// ladder actually measures. This writes the same four fields func_0203df40
// writes, from a string instead of from the pad.
bool     g_inject_on = false;
unsigned g_inj_key = 0, g_inj_x = 0, g_inj_y = 0, g_inj_touch = 0;

void inject_parse() {
    static bool done = false;
    if (done) return;
    done = true;
    const char *s = std::getenv("SM64DS_COMMS_INJECT");
    if (!s || !*s) return;
    g_inject_on = true;
    for (const char *p = s; *p; ) {
        while (*p == ',' || *p == ' ') ++p;
        if (!*p) break;
        const char *eq = std::strchr(p, '=');
        if (!eq) break;
        const unsigned v = (unsigned)std::strtoul(eq + 1, 0, 0);
        if      (std::strncmp(p, "key",   3) == 0) g_inj_key   = v;
        else if (std::strncmp(p, "touch", 5) == 0) g_inj_touch = v;
        else if (std::strncmp(p, "x",     1) == 0) g_inj_x     = v;
        else if (std::strncmp(p, "y",     1) == 0) g_inj_y     = v;
        const char *c = std::strchr(eq, ',');
        if (!c) break;
        p = c + 1;
    }
    std::fprintf(stderr, "[comms:lockstep] injecting key=0x%04x stylus={%u,%u} "
                 "touch=%u into the local record\n",
                 g_inj_key, g_inj_x, g_inj_y, g_inj_touch);
}

inline void put16(unsigned char *p, unsigned v) {
    const unsigned short h = (unsigned short)v;
    std::memcpy(p, &h, 2);
}
inline unsigned get16(const unsigned char *p) {
    unsigned short h = 0;
    std::memcpy(&h, p, 2);
    return h;
}

// ---------------------------------------------------------------------------
// FILL THE LOCAL RECORD -- src/func_0203df40.c:31-39 and :59.
//
// MP1's own commit message named this as MP2's wiring: "nothing fills the local
// comms record there, because the port writes TouchInfo and PadData directly
// instead. Filling it -- the ROM does it in func_0203df40 -- is MP2's."
//
// THE ONE DIVERGENCE, and it is forced. The ROM reads the pad at :31 as
// ((*(u16*)0x4000130 | *(u16*)0x27fffa8) ^ 0x2fff) & 0x2fff. THE PORT NEVER
// WRITES EITHER REGISTER -- both KEYINPUT and the 0x027FFFA8 shared word are
// listed as unwritten gaps in port/ppu_gap_audit.txt:122 -- so reading them
// here would put zeros on the wire and the crossing proof would measure
// nothing. The port's equivalent is PadData[0].held at data_020a0e58, which
// hal/input_probe.cpp:304 states is already in DS KEYINPUT bit order and
// already active-high, i.e. it is the value the ROM's expression EVALUATES TO
// rather than the raw register it evaluates it from. Masked with the same
// 0x2fff so the wire byte is identical either way.
//
// Read here, at the top of the tick, and that ordering is load-bearing: with
// SM64DS_COMMS_FANOUT=1 the ROM's steps 0x16 and 0x17 OVERWRITE data_020a0de8
// and data_020a0e58 from the four records later in the same frame. Reading
// them after the fan-out would feed the record back into itself.
void fill_local_record() {
    inject_parse();

    unsigned key = 0, sx = 0, sy = 0, touch = 0;
    if (g_inject_on) {
        key = g_inj_key; sx = g_inj_x; sy = g_inj_y; touch = g_inj_touch;
    } else {
        key   = data_020a0e58[0];          // PadData[0].held
        touch = data_020a0de8[0];          // TouchInfo[0].now
        sx    = data_020a0de8[2];          // TouchInfo[0].x
        sy    = data_020a0de8[3];          // TouchInfo[0].y
    }

    put16(data_020a104e, key & 0x2fff);    // :31, the key word
    put16(data_020a1044, sx);              // :35
    put16(data_020a1046, sy);              // :36
    data_020a1048[0] = (unsigned char)touch;   // :37
    data_020a1048[2] = 0;                      // :38, unkA

    // :39 -- I am live. The 0x8000 bit is what gates the whole unpack on the
    // far side (:278), so without it a peer's record is never written.
    put16(data_020a104c, get16(data_020a104c) | 0x8000);

    // :59 -- the frame counter. The parent's copy of this is the session clock.
    int f = 0;
    std::memcpy(&f, data_020a1040, 4);
    ++f;
    std::memcpy(data_020a1040, &f, 4);

    // data_020a1050, the heading, is NOT written here on purpose: Camera::
    // Behavior publishes it and walk_window's rig echoes it in, exactly as on
    // the DS. It stages straight through to block+0x0B below.
}

// ---------------------------------------------------------------------------
// STAGE -- src/func_0203ea5c.c:177-186, the ten CpuCopy8 moves, in order.
//
// The ROM stages into data_020a1020. Nothing else in this tree references that
// symbol, so hosting it would add a .dsstate member with no reader; the block
// is a local here and the citation is what carries the fidelity.
void stage_block(unsigned char *b) {
    std::memcpy(b + 0x00, data_020a104c, 2);      // :177  flag
    std::memcpy(b + 0x02, data_020a1040, 4);      // :178  frame
    std::memcpy(b + 0x06, data_020a104e, 2);      // :179  key
    std::memcpy(b + 0x08, data_020a1044, 1);      // :180  stylus x
    std::memcpy(b + 0x09, data_020a1046, 1);      // :181  stylus y
    std::memcpy(b + 0x0A, data_020a1048, 1);      // :182  touch
    std::memcpy(b + 0x0B, data_020a1050, 2);      // :183  heading
    std::memcpy(b + 0x0D, data_020a0f08, 1);      // :184  player count
    std::memcpy(b + 0x0E, data_020a0f00, 1);      // :185  reserved
    std::memcpy(b + 0x0F, data_020a1052, 0x11);   // :186  game payload
}

// ---------------------------------------------------------------------------
// UNPACK one peer -- src/func_0203ea5c.c:277-290, the mirror image.
void unpack_slot(int i, const unsigned char *b) {
    unsigned char *rec = data_020a1154 + i * 0x24;

    std::memcpy(rec + 0x0C, b + 0x00, 2);         // :277  flag, ALWAYS
    if ((get16(rec + 0x0C) & 0x8000) == 0)        // :278  the live gate
        return;

    std::memcpy(rec + 0x00, b + 0x02, 4);         // :280  frame
    std::memcpy(rec + 0x0E, b + 0x06, 2);         // :281  key
    std::memcpy(rec + 0x04, b + 0x08, 1);         // :282  stylus x
    std::memcpy(rec + 0x06, b + 0x09, 1);         // :283  stylus y
    std::memcpy(rec + 0x08, b + 0x0A, 1);         // :284  touch
    std::memcpy(rec + 0x10, b + 0x0B, 2);         // :285  heading
    std::memcpy(rec + 0x12, b + 0x0F, 0x11);      // :286  game payload
    ++g_peer_updates;

    // :287-289 -- the player count and the reserved byte are honoured ONLY
    // from slot 0, and only by a console that is not itself slot 0.
    if (data_020a0f10[0] != 0 && i == 0) {
        std::memcpy(data_020a0f08, b + 0x0D, 1);  // :288
        std::memcpy(data_020a0f00, b + 0x0E, 1);  // :289
    }
}

}  // namespace

// ===========================================================================
// THE TICK
// ===========================================================================

bool comms_lockstep_tick() {
    if (!comms_transport()) return false;      // no transport: the role-0 arm

    // --- BRING-UP, :138. Gated one-shot the way the ROM gates it on
    //     data_02099e1c, and the ROM passes 2.
    if (!g_opened) {
        g_opened = true;
        func_020408b0(2);
    }

    // --- :189, the top of every loop turn: ask the link what it is.
    int st = func_02040714();

    // --- :193 / :196, case 0 of the state switch. The ROM picks the arm off
    //     the role byte; here the transport was pre-configured by env with the
    //     role, so we ask for the one it bound and let it refuse a mismatch
    //     (see PROPOSED CONTRACT CHANGE 3 in comms_loopback.cpp).
    if (st == kCommsIdle) {
        const CommsLoopbackStats ls = comms_loopback_stats();
        if (ls.role == 1) func_02040820();     // become PARENT
        else              func_02040790();     // become CHILD
        st = func_02040714();
    }

    if (st != kCommsParentConnected && st != kCommsChildConnected) {
        // ------------------------------------------------------------------
        // A DELIBERATE, DOCUMENTED DIVERGENCE, and the only one in the tick.
        //
        // The ROM spins HERE too, inside one call, against the 0x4B0 bound at
        // :142-146, and drops to solo when it runs out. On the DS that is
        // right: both consoles are already sitting in the multiplayer menu
        // with the radio warming up, so the wait is milliseconds.
        //
        // Two PC processes boot independently and a boot is seconds, not
        // milliseconds -- the second instance is still loading assets while
        // the first is already ticking frames. Spinning the ROM's bound here
        // would freeze the first window for 1200 turns and then drop a session
        // that was about to work.
        //
        // So the CONNECT phase is per-frame and non-blocking: return false,
        // the caller runs func_0203e0ac() for this frame exactly as it does
        // today, and we ask again next frame. The ROUND phase below is NOT
        // relaxed -- it spins the ROM's bound, because by then both sides are
        // in the session and a stall is the peer's problem to answer.
        //
        // PROPOSE: the frozen contract should say that connecting is the
        // TRANSPORT's business and may take as long as it likes, and that the
        // caller is entitled to run the solo arm while state() is 0 or 2. The
        // ROM's own bound then applies to the ROUND, which is what it is
        // actually protecting.
        // ------------------------------------------------------------------
        return false;
    }

    ++g_ticks;

    // The role byte, so the rest of the game and comms_report() see the truth.
    // src/func_0203df40.c switches on it; nothing else in the port reads it.
    data_020a0f04[0] = (st == kCommsParentConnected) ? kCommsRoleParent
                                                     : kCommsRoleChild;

    fill_local_record();

    unsigned char block[kCommsBlockBytes];
    stage_block(block);

    // --- THE WAIT, :157 `while ((sp8 == 0) && (sp4 != 0))`, with the bound
    //     selected at :142-146: 0x12C once the session is up, 0x4B0 before.
    int bound = g_ever_round ? 0x12C : 0x4B0;
    unsigned short status = 0;
    int done = 0;
    while (done == 0 && bound > 0) {
        --bound;                                  // :414
        ++g_spins;
        const int s = func_02040714();            // :189, every turn
        if (s != kCommsParentConnected && s != kCommsChildConnected) break;
        done = func_020406b4(block, &status);     // :208 / :221
        if (done == 0) {
            // The DS sleeps here through func_02042778 -> OS_SleepThread on
            // the per-VBlank queue. The port has one game context and
            // hal/os_thread.h's pump is not installed on this path, so a
            // sleep would return immediately and spin the CPU flat. One
            // millisecond per turn keeps the ROM's TURN COUNT as the bound
            // while leaving the core idle; on loopback a round completes in
            // the first turn or two, so this is a ceiling and not a cost.
            ::Sleep(1);
        }
    }

    if (done == 0) {
        ++g_timeouts;
        // --- :487, the ROM's own drop back to solo, both writes.
        data_020a0f04[0] = kCommsRoleSolo;
        data_020a0f1c[0] |= 1;
        if (!g_said_solo) {
            g_said_solo = true;
            std::fprintf(stderr, "[comms:lockstep] the wait bound expired; "
                         "dropping the session to solo the way "
                         "src/func_0203ea5c.c:487 drops it. NOTE the transport "
                         "is not told (PROPOSED CONTRACT CHANGE 5).\n");
        }
        return false;
    }

    ++g_rounds;
    g_ever_round = true;

    // --- :237. Only once a round is in, and the argument is the masked flag
    //     the ROM passes, not a slot number -- which is why the seam's face
    //     names its parameter `ignored`.
    data_020a0f10[0] = func_02040704(data_020a0f1c[0] & 0x4000);

    // --- :240 then :274, all four slots.
    for (int i = 0; i < kCommsMaxPlayers; ++i) {
        const unsigned char *b =
            (const unsigned char *)func_0204068c((unsigned short)i);
        if (!b) continue;
        unpack_slot(i, b);
    }
    return true;
}

CommsLockstepStats comms_lockstep_stats() {
    CommsLockstepStats s;
    s.ticks        = g_ticks;
    s.rounds       = g_rounds;
    s.timeouts     = g_timeouts;
    s.spins        = g_spins;
    s.peer_updates = g_peer_updates;
    return s;
}

void comms_lockstep_report(const char *tag) {
    const CommsLockstepStats s = comms_lockstep_stats();
    std::fprintf(stderr,
        "[lockstep:%s] ticks=%llu rounds=%llu timeouts=%llu spins=%llu "
        "peer_updates=%llu\n",
        tag ? tag : "-", s.ticks, s.rounds, s.timeouts, s.spins,
        s.peer_updates);
}

}  // namespace port
