// THE RADIO SEAM, hosted. Run mg15, lane MP1.
// The contract, the wire format and the role semantics are all in
// comms_seam.h. This is the boundary itself and its honest solo answers.
//
// PORT_HOST_ABI: the eight ROM addresses below reach the NITRO WM SDK and,
// under it, arm7.bin -- which this repo does not decompile and a PC port does
// not want. They are hosted rather than linked, which is the whole point of a
// seam: everything ABOVE them stays the ROM's, verbatim.

#include "comms_seam.h"

#include "os_thread.h"

#include <cstdio>
#include <cstring>

// The ROM globals the seam reads. All four are hosted elsewhere and asserted
// at bring-up; see each site.
extern "C" {
extern unsigned char data_020a0f04[];   // the ROLE byte, hal/stage_slot0.cpp
extern unsigned char data_020a1040[];   // the local record, hal/camera_bridges.cpp
extern unsigned char data_020a1154[];   // the four records, ditto (0x24 stride)
extern unsigned char data_020a0de8[];   // TouchInfo[4], hal/auto_bss.cpp
extern int data_020a0e58[];             // PadData[4], hal/auto_bss.cpp

// THE TWO WORDS RUNG W0 GAVE BACK TO THE ROM (run link100, lane WM1).
// src/func_02040714.c reads the first and src/func_02040704.c reads the second,
// and both are LINKED now. data_020a0f94 is hosted by hal/comms_conductor.cpp
// (".dsstate$ymp3s0007"); data_020a0f24 is hosted at the foot of THIS file,
// which is why it is declared here and defined there.
extern unsigned char data_020a0f94[];   // the link state, comms_conductor.cpp
extern unsigned char data_020a0f24[];   // my slot (aid), the $ywmb block below

// The ROM's fan-out, steps 0x16 and 0x17 of src/func_020197b8.c.
void func_0203bb60(void);
void func_0203bc7c(void);

// THE HOST ARM7'S WIRELESS ARM, in hal/boot2_ipc.cpp. Run link100, lane WM2,
// rung W1: it is what opens the installed transport when the ROM claims channel
// 0xa, and port::comms_arm7_turn below is what gives it the moment to look. Not
// declared for the seam probe, which links this file without hal/boot2_ipc.cpp.
#ifndef PORT_COMMS_SEAM_PROBE_FACE_020408B0
void port_arm7_wireless_tick(void);
#endif
}

namespace port {
namespace {

const CommsTransport *g_transport = nullptr;
uint16_t g_boot_indicator = 0;      // 0 = cartridge boot. See comms_seam.h.
uint64_t g_exchanges = 0;
uint64_t g_rounds = 0;
uint64_t g_wire_activity = 0;       // bumped per accepted session datagram;
                                    // read by the conductor's pump. See
                                    // comms_note_wire_activity in the header.

// The seam's own state when no transport is installed. These mirror the ROM
// globals func_02040704 and func_02040714 return (data_020a0f24 and
// data_020a0f94), which nothing in the linked build reads except the ROM
// bodies of the seam functions themselves -- and those are hosted here.
int g_solo_state = kCommsIdle;
int g_solo_slot = 0;

}  // namespace

bool comms_set_transport(const CommsTransport *t) {
    if (!t) { g_transport = nullptr; return true; }
    if (!t->open || !t->close || !t->become_parent || !t->become_child ||
        !t->state || !t->slot || !t->player_count || !t->exchange ||
        !t->peer_block || !t->poll || !t->abandon) {
        /* v2's send_aux/recv_aux are deliberately NOT in this list: they are
           the optional half of the extension, and a v1 transport that predates
           them leaves them null and must still install. Everything above is a
           v1 entry and stays required. */
        std::fprintf(stderr, "[comms] transport '%s' REFUSED: an entry is null. "
                     "Every entry of CommsTransport is required.\n",
                     t->name ? t->name : "(unnamed)");
        return false;
    }
    g_transport = t;
    return true;
}

const CommsTransport *comms_transport() { return g_transport; }

void comms_note_wire_activity() { ++g_wire_activity; }
uint64_t comms_wire_activity() { return g_wire_activity; }

// ===========================================================================
// THE TWO WORDS, PUBLISHED. Run link100, lane WM1, rung W0.
//
// src/func_02040714.c and src/func_02040704.c are linked and their host faces
// are gone, so the game now reads data_020a0f94 and data_020a0f24 rather than
// asking the transport. On the DS the wireless thread writes both words as the
// radio progresses. Here the seam writes them, and the whole question this
// function has to answer is WHEN.
//
// WHERE IT IS CALLED, and each is a moment the transport can have changed its
// mind about the session:
//
//   func_020408b0   open       -- and it writes the same two zeroes the ROM's
//                                 own body writes (src/func_020408b0.c:25-27)
//   func_02040820   parent     -- the request is in; whatever the transport
//   func_02040790   child         answers is the new state
//   func_02040724   leave
//   func_020406b4   exchange   -- ONCE PER TURN of the ROM's own wait loop in
//                                 states 3 and 4, which is what keeps a live
//                                 session's words from going stale, and what
//                                 makes src/func_0203ea5c.c:252 read a slot
//                                 that was published in the same invocation
//                                 (:223 runs before :252)
//   hal/comms_conductor.cpp, comms_wait_for_session -- the one read that
//                                 happens BEFORE any lockstep round could have
//                                 published anything. See the hunk there.
//
// AND THE ONE WINDOW THE FACES CANNOT COVER, WHICH IS WHY THERE IS A PUMP.
//
// src/func_0203ea5c.c's case 2 -- kCommsConnecting -- is a bare `sp8 = 0`. It
// calls NOTHING, so no seam face runs, so a word that says 2 when the ROM
// reaches that arm would stay 2 for the rest of the ROM's wait bound and then
// drop the session to solo. And 2 is not a theoretical value: the loopback
// carrier reports it after become_parent/become_child and again whenever the
// live mask falls back to one (hal/comms_loopback.cpp:1929, :2219, :3280,
// :3295), which is a peer leaving mid-session -- a state it has a documented
// path OUT of, since a child re-JOINs while connecting (:2893). Before this
// rung func_02040714 read the transport live and that recovery worked; a mirror
// that freezes at 2 would silently take it away. That is exactly the failure
// lane DF40's note predicted for this closure, and it is the one thing here
// that had to be built rather than argued.
//
// So the seam chains a pump. hal/os_thread.h's pump runs once per turn of the
// ROM's own OS_SleepThread wait -- the same hook hal/comms_conductor.cpp's
// conductor_pump uses to call poll() -- and mirror_pump publishes after it.
// Every turn of case 2 is a pump turn, so the recovery is seen on the turn the
// carrier sees it.
//
// IT INSTALLS ITSELF LAST, ON PURPOSE. conductor_pump is written to hand its
// vote to a pump that was already there ("A PREVIOUSLY INSTALLED PUMP KEEPS ITS
// VOTE"), and that vote is what paces a connected wait turn at a DS VBlank --
// the 2026-08-28 field collapse this port paid for once. If this pump were
// installed FIRST it would become conductor_pump's g_prev_pump and conductor
// would return this function's vote instead of its own, undoing that pacing.
// So the install is lazy and refuses to run until there is already a pump to
// chain: it happens on the first publish, which is a seam face, which is long
// after hal/comms_loopback.cpp has installed the conductor's. This one is
// always the OUTER pump and never anybody's g_prev_pump.
//
// SOLO IS UNTOUCHED. With no transport this writes kCommsIdle and 0, which is
// what both words already hold -- and in a solo boot the role byte is 0, so
// src/func_0203df40.c takes its solo arm and not one of these faces is ever
// called at all. The only other writer of data_020a0f94 in the whole link is
// src/func_0203fd64.c, and its `= 1` arm is dead here because the port's
// func_0206259c always answers 2 (hal/comms_conductor.cpp's "2 or hang").
// ===========================================================================
// ===========================================================================
// THE HOST ARM7 GETS A TURN WHENEVER THE ARM9 TOUCHES THE RADIO.
//
// Run link100, lane WM2, rung W1. src/func_020408b0.c is the ROM's own body
// now, and the port's only t->open(mode) call went with the face it lived in.
// Its home is hal/boot2_ipc.cpp, the host ARM7, which opens the transport when
// it sees the ROM claim channel 0xa. Something has to give that model a moment
// to look, because the claim is a plain store into the shared block
// (src/func_0205ba64.c writes 0x027FFC00+0x388) and not an I/O access anything
// traps.
//
// THE RULE, stated rather than implied: the host ARM7 runs at the points the
// ARM9 yields to it. That is not an invention for this rung -- ntr/ipc.h says
// it about the IPCSYNC handshake in as many words ("one iteration per ARM9
// write to IPCSYNC ... the single-threaded host's version of the other core got
// there"). The radio's yield points are the seam faces themselves, so each of
// the three that remain calls this first, and the ARM7's turn therefore lands
// BEFORE the request the ARM9 is about to make. That ordering is load-bearing:
// the frozen contract refuses a become_parent()/become_child() that arrives
// before open() and leaves the state idle for good, and the conductor calls
// func_020408b0(2) and func_02040820() back to back
// (hal/comms_conductor.cpp:1133-1136).
//
// SOLO IS UNTOUCHED. With no transport installed there is nothing to open, the
// ARM7 arm returns after one mask test, and none of these faces is called at
// all in a solo boot anyway (the role byte never leaves 0, so
// src/func_0203df40.c takes its solo arm).
//
// NOT IN THE PROBE. tests/mp_comms_seam.cpp links hal/comms_seam.cpp without
// hal/boot2_ipc.cpp, so the symbol would not resolve there; the probe keeps the
// pre-W1 seam whole under the same define that keeps its open face.
// ===========================================================================
// (We are already inside `namespace port` here; the declaration of the ARM7
// entry is with the other extern "C" names at the head of this file.)
void comms_arm7_turn() {
#ifndef PORT_COMMS_SEAM_PROBE_FACE_020408B0
    port_arm7_wireless_tick();
#endif
}

namespace {

void publish_now() {
    const CommsTransport *t = g_transport;
    const int st   = t ? t->state() : g_solo_state;
    const int slot = t ? t->slot()  : g_solo_slot;
    // data_020a0f94 is a word (src/func_02040714.c reads it as int);
    // data_020a0f24 is the low halfword of its own 4-byte ROM span (every ROM
    // writer -- src/func_020408b0.c:27, src/func_02040014.c, src/func_020402a0.c,
    // src/func_0203fec4.c:23 -- writes it as u16).
    *reinterpret_cast<volatile int *>(data_020a0f94) = st;
    *reinterpret_cast<volatile unsigned short *>(data_020a0f24) =
        (unsigned short)slot;
}

ThreadPump g_pump_prev = nullptr;
bool       g_pump_installed = false;

// One turn of the ROM's own wait: whatever pump was already there decides
// whether the wait continues, and then the two words are refreshed. The vote is
// NOT this function's to make -- see the banner above.
bool mirror_pump(unsigned spin) {
    const bool go = g_pump_prev ? g_pump_prev(spin) : false;
    publish_now();
    return go;
}

}  // namespace

void comms_publish_link_words() {
    // Only with a transport (there is nothing to mirror without one, and a solo
    // boot must not grow a pump it never had), and only when there is already a
    // pump to chain, so this can never end up as conductor_pump's g_prev_pump.
    if (!g_pump_installed && g_transport) {
        const ThreadPump had = thread_pump();
        if (had) {
            g_pump_installed = true;
            g_pump_prev = had;
            thread_set_pump(mirror_pump);
        }
    }
    publish_now();
}

void comms_set_boot_indicator(uint16_t value) {
    g_boot_indicator = value;
    // The DS firmware leaves this halfword in the shared system block, and
    // src/func_0203db64.c and src/func_020408b0.c read it from there rather
    // than through any accessor. ntr maps 0x027ff000 (io.cpp's kRegions), so
    // the write lands where the ROM will look.
    *reinterpret_cast<volatile uint16_t *>(0x027FFC40u) = value;
}

uint16_t comms_boot_indicator() { return g_boot_indicator; }

CommsReadout comms_readout() {
    CommsReadout r;
    r.transport = g_transport ? (g_transport->name ? g_transport->name
                                                   : "(unnamed transport)")
                              : "solo (no transport)";
    r.link_state = g_transport ? g_transport->state() : g_solo_state;
    r.slot = g_transport ? g_transport->slot() : g_solo_slot;
    r.players = g_transport ? g_transport->player_count() : 1;
    r.role = data_020a0f04[0];
    r.connected = (r.link_state == kCommsParentConnected ||
                   r.link_state == kCommsChildConnected);
    r.boot_indicator = g_boot_indicator;
    r.exchanges = g_exchanges;
    r.rounds = g_rounds;
    return r;
}

void comms_fanout() {
    func_0203bb60();
    func_0203bc7c();
}

void comms_report(const char *tag) {
    // The records, read exactly the way the game reads them: 0x24 stride from
    // data_020a1154, and the local record at data_020a1040. Both are hosted
    // contiguously by hal/camera_bridges.cpp, which asserts the ROM offsets at
    // bring-up (hal_camera_check_layout).
    const CommsReadout r = comms_readout();
    std::fprintf(stderr,
        "[comms:%s] transport=%s link=%d connected=%s slot=%d players=%d "
        "role=%d boot=0x%04x exchanges=%llu rounds=%llu\n",
        tag ? tag : "-", r.transport, r.link_state, r.connected ? "yes" : "no",
        r.slot, r.players, r.role, (unsigned)r.boot_indicator,
        (unsigned long long)r.exchanges, (unsigned long long)r.rounds);

    const unsigned char *loc = data_020a1040;
    std::fprintf(stderr,
        "[comms:%s]   local  frame=%d stylus={%u,%u,%u} flag=%04x key=%04x "
        "head=%04x\n",
        tag ? tag : "-", *(const int *)(loc + 0x00),
        loc[0x04], loc[0x06], loc[0x08],
        *(const unsigned short *)(loc + 0x0C),
        *(const unsigned short *)(loc + 0x0E),
        *(const unsigned short *)(loc + 0x10));

    for (int i = 0; i < kCommsMaxPlayers; ++i) {
        const unsigned char *p = data_020a1154 + i * 0x24;
        const unsigned char *t = data_020a0de8 + i * 4;
        std::fprintf(stderr,
            "[comms:%s]   slot%d frame=%d stylus={%u,%u,%u} flag=%04x key=%04x "
            "head=%04x | touch{now=%u chg=%u x=%u y=%u} "
            "pad{held=%04x pressed=%04x}\n",
            tag ? tag : "-", i, *(const int *)(p + 0x00),
            p[0x04], p[0x06], p[0x08],
            *(const unsigned short *)(p + 0x0C),
            *(const unsigned short *)(p + 0x0E),
            *(const unsigned short *)(p + 0x10),
            t[0], t[1], t[2], t[3],
            (unsigned)(data_020a0e58[i] & 0xFFFF),
            (unsigned)((data_020a0e58[i] >> 16) & 0xFFFF));
    }
}

}  // namespace port

// ===========================================================================
// THE EIGHT SEAM FACES.
//
// Each one names the ROM TU it stands in for and what that TU does on the DS,
// so the day someone links the real WM SDK the comparison is one file away.
// ===========================================================================

extern "C" {

// src/func_020408b0.c IS LINKED NOW. Run link100, lane WM2, rung W1.
//
// The face that stood here allocated nothing, read no boot indicator and
// started no SDK: it called t->open(mode) and published. The ROM's own body
// does all three, and every one of them now runs -- Memory::Allocate takes the
// eight buffers off the game heap, the boot indicator picks the branch, and
// func_020616e8 (WM_Init) lays out the 0x1300 work buffer and registers the
// ARM9's channel-0xa reply handler. Twelve matched TUs came with it; the list
// and the argument are in port/slice_wm2.txt.
//
// WHERE THE OPEN WENT. Retiring this face deleted the port's ONLY
// t->open(mode) call, and its honest home is the host ARM7: hal/boot2_ipc.cpp
// opens the installed transport when it sees the ROM claim channel 0xa, with
// data_020a0f14 as the mode word. The ROM's own body writes that word FIRST
// (src/func_020408b0.c:25, `data_020a0f14 = arg`) and registers the channel
// LAST (func_020616e8's closing func_0205ba64(0xa, func_02061188)), so the mode
// is already there when the claim becomes visible. port_arm7_wireless_tick()
// below is where the ARM7 is given its turn to look.
//
// WHERE THE RECORD CLEAR WENT. hal/comms_conductor.cpp's
// comms_seat_session_request, which is the one point every path to this body
// goes through and has exactly the lifecycle the clear needs. See the hunk
// there.
//
// THE PROBE KEEPS THE FACE, and only the probe. tests/mp_comms_seam.cpp calls
// func_020408b0(2) at :147 to check the seam's SOLO answers; it links neither a
// heap nor Memory::Allocate, so handing it the ROM body would be a link error
// followed by a null dereference. It gets the stand-in under its own define,
// exactly the way mp_sleepwake keeps the retired OS_SleepThread pair alive
// under PORT_OS_THREAD_HOST_PAIR: a retired host body stays reachable where it
// is MEASURED and nowhere else. No shipped target compiles the block below.
#ifdef PORT_COMMS_SEAM_PROBE_FACE_020408B0
void func_020408b0(unsigned short mode) {
    std::memset(data_020a1154 + 4 * 0x24, 0, 12 * 0x24);
    const port::CommsTransport *t = port::comms_transport();
    if (t) {
        t->open(mode);
        port::comms_publish_link_words();
        return;
    }
    port::g_solo_state = port::kCommsIdle;
    port::g_solo_slot = 0;
    port::comms_publish_link_words();
}
#endif

// src/func_02040820.c: the ROM's become-parent state machine over
// data_020a0f94/data_020a0f5c, with Wireless_Reset on a role flip.
// PORT_HOST_ABI: hosted WM/radio seam face; ROM body is the become-parent state machine over the NITRO WM SDK / arm7.bin radio, which this repo does not decompile.
void func_02040820(void) {
    port::comms_arm7_turn();
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->become_parent(); port::comms_publish_link_words(); return; }
    // No radio: the request is recorded and refused the way an unanswered
    // radio refuses it -- the state never leaves idle, so the ROM's own wait
    // bound expires and src/func_0203ea5c.c:487 drops the session to solo.
    port::g_solo_state = port::kCommsIdle;
    port::comms_publish_link_words();
}

// src/func_02040790.c, the child half of the same machine.
// PORT_HOST_ABI: hosted WM/radio seam face; the become-child half of the same WM SDK / arm7.bin state machine this repo does not decompile.
void func_02040790(void) {
    port::comms_arm7_turn();
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->become_child(); port::comms_publish_link_words(); return; }
    port::g_solo_state = port::kCommsIdle;
    port::comms_publish_link_words();
}

// src/func_02040724.c: leave.
// PORT_HOST_ABI: hosted WM/radio seam face; the ROM body closes the NITRO WM SDK session over arm7.bin, which this repo does not decompile.
void func_02040724(void) {
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->close(); port::comms_publish_link_words(); return; }
    port::g_solo_state = port::kCommsIdle;
    port::g_solo_slot = 0;
    port::comms_publish_link_words();
}

// ===========================================================================
// THE NEXT TWO WERE CLOSABLE, AND RUNG W0 CLOSED THEM.
//
// Run link100, lane WM1. The note below is lane DF40's, kept because it is the
// argument this rung was taken on and because it named the one cost that had to
// be paid -- and the answer to that cost is in comms_publish_link_words above.
// Both host bodies are GONE. src/func_02040714.c and src/func_02040704.c are
// linked by port/slice_wm1.txt, data_020a0f24 is hosted at the foot of this
// file, and the two words are published by the seam at every point a transport
// can have changed its mind. DF40's ruling that "the decision belongs to the
// conductor lane" is what WM1 was spawned to make.
//
// DF40 named the exact hazard -- "a mirror that misses an update expires the
// ROM's wait bound and drops a live session to solo" -- and it is real: the
// ROM's case-2 arm calls nothing at all. The answer is three-layered and all of
// it is in comms_publish_link_words' banner: the faces publish, the exchange
// face publishes once per turn of a live session's wait, and a chained pump
// publishes once per turn of ANY wait, which is what covers case 2.
//
// ---- DF40's note, verbatim ------------------------------------------------
// (run link100, lane DF40; the census that found them is in
// port/slice_gate221.txt.)
//
// func_02040714 is `return data_020a0f94` and func_02040704 is
// `return data_020a0f24`. They are the only two of this file's twelve faces
// whose ROM bodies touch no radio at all -- two pure global reads, no callees,
// nothing arm7.bin owns. The PORT_HOST_ABI tags below are therefore about the
// STATE, not about the instruction: what is hosted here is not a hardware
// access, it is the decision that the transport ANSWERS the link state rather
// than KEEPING it in the ROM's own word. That is a real decision and it is
// stated rather than dressed as an ABI fact.
//
// EVERYTHING A CLOSURE NEEDS IS ALREADY IN THE TREE:
//   * comms_seam.h's CommsLinkState enum IS the ROM's data_020a0f94 encoding
//     and says so -- kCommsIdle 0, kCommsConnecting 2, kCommsParentConnected
//     3, kCommsChildConnected 4, which are exactly the cases
//     src/func_02040724.c switches on. So `data_020a0f94 = t->state()` would
//     be an identity, not an approximation.
//   * data_020a0f94 is ALREADY hosted (hal/comms_conductor.cpp's MP3_BSS row,
//     ".dsstate$ymp3s0007"), and a linked matched TU already writes it:
//     src/func_0203fd64.c sets it to 1 on its refusal path.
//   * src/func_0203ea5c.c calls func_02040714 exactly once per invocation
//     (:204) and func_02040704 exactly once (:252). Neither sits in a tight
//     loop, so a word refreshed at poll time cannot be spun on.
//
// WHAT IT COSTS, AND WHY IT IS NOT THIS LANE'S HUNK. data_020a0f24 is hosted
// nowhere yet, and more to the point the seam would stop ANSWERING the state
// and start MIRRORING it: the lifecycle faces writing the ROM's word on
// open/become/close, and conductor_pump writing it on every poll. That mirror
// is the DS's own shape -- on hardware the wireless thread is what writes that
// word -- but it moves ownership inside a contract this file's banner declares
// FROZEN, and a mirror that misses an update expires the ROM's wait bound and
// drops a live session to solo. Two matched TUs, one contract decision, a
// 2-player and a 4-player proof; the decision belongs to the conductor lane.
// ---- end of DF40's note ---------------------------------------------------
//
// AND THE ARGUMENT MISMATCH IS SAFE, which is the other half of the seat.
// src/func_02040704.c is `int func_02040704(void)`. Two callers pass it one
// argument anyway -- src/func_0203ea5c.c:252 (`func_02040704(temp_r0_5)`, a
// dead r0 write the DS callee overwrites) and hal/comms_conductor.cpp:1176
// (`func_02040704(0)`, written against the retired host face). This binary is
// 32-bit x86 (PE32, machine 0x014c) and both are extern "C" __cdecl: the map
// names the symbol _func_02040704, a leading underscore with NO @N suffix,
// which is __cdecl and not __stdcall. Under __cdecl the CALLER pops what it
// pushed, so a zero-parameter callee reached through a one-parameter
// declaration leaves the stack exactly where it found it. It is not the
// __fastcall arity hazard this port has been bitten by (that one smashes the
// frame because the receiver rides in a register); and had this been __stdcall
// the same mismatch would have been a LOUD link error, @0 against @4, rather
// than a silent leak. So no call site had to change.
// ===========================================================================

// src/func_020406b4.c: hands the staged block to func_02062df0 (the WM send)
// and reports back through data_020a0f80. Returns 1 when the round is in.
// PORT_HOST_ABI: hosted WM/radio seam face; the ROM body hands the staged block to the NITRO WM send over arm7.bin, which this repo does not decompile.
int func_020406b4(const void *block, unsigned short *status) {
    port::comms_arm7_turn();
    ++port::g_exchanges;
    const port::CommsTransport *t = port::comms_transport();
    if (!t) {
        if (status) *status = 0;
        return 0;               // no partners: the round never completes
    }
    const int done = t->exchange(block, status);
    if (done) ++port::g_rounds;
    // W0. This is the one seam face the ROM calls on EVERY turn of its own wait
    // loop while the link is up (src/func_0203ea5c.c:223 and :236), so it is
    // what keeps data_020a0f94 and data_020a0f24 from going stale during a live
    // session -- and :223 runs before :252, so the slot the ROM seats into
    // data_020a0f10 was published in the same invocation that read it.
    port::comms_publish_link_words();
    return done;
}

/* port/rollback: the seam's two counters are host bookkeeping outside the
   snapshot regions, and the divergence detector stamps `rounds` on every
   frame's hash line. A rewind puts them back with the world so a replayed
   frame prints the same stamp it printed the first time. */
extern "C" void port_comms_counters_get(unsigned long long *exchanges,
                                        unsigned long long *rounds)
{
    *exchanges = port::g_exchanges;
    *rounds = port::g_rounds;
}
extern "C" void port_comms_counters_set(unsigned long long exchanges,
                                        unsigned long long rounds)
{
    port::g_exchanges = exchanges;
    port::g_rounds = rounds;
}

// src/func_0204068c.c: player `aid`'s received block, or 0.
// PORT_HOST_ABI: hosted WM/radio seam face; returns a peer's received block out of the NITRO WM buffers over arm7.bin, which this repo does not decompile.
const void *func_0204068c(unsigned short aid) {
    const port::CommsTransport *t = port::comms_transport();
    if (!t) return nullptr;
    if ((int)aid >= port::kCommsMaxPlayers) return nullptr;
    return t->peer_block((int)aid);
}

// src/func_02040c34.c: starts the DS's wireless THREAD with two callbacks,
// which are the ROM's own src/func_0203f644.c and src/func_0203f604.c. A host
// transport has no thread to start -- it is polled from the seam's own pump,
// which EXISTS as of run mg16 lane MP3: hal/comms_conductor.cpp installs it on
// hal/os_thread.h's hook and the ROM's own wait sleeps through it, so poll()
// runs once per turn of src/func_0203ea5c.c's real wait loop. HOLE 1 is CLOSED
// in the frozen contract at the top of comms_seam.h. The annotation that stood
// here said no such pump existed; that was true when it was written and is not
// now. So this records the request and returns. The callbacks are deliberately NOT
// invoked: on the DS they run in the wireless thread's context and mutate the
// same state the poll path does, so calling them from here would double-apply
// it. A transport that wants them driven does it from poll().
// PORT_HOST_ABI: hosted WM/radio seam face; the ROM body starts the DS wireless thread in the NITRO WM SDK over arm7.bin, which this repo does not decompile; the host is polled from the seam pump instead.
void func_02040c34(int role, int b, void *cb_recv, void *cb_send, int e) {
    (void)role; (void)b; (void)cb_recv; (void)cb_send; (void)e;
}

// THE STATUS-WORD PAIR IS RETIRED (run link100, lane DF40).
//
// src/func_02040a5c.c / src/func_02040a84.c stood here as two host bodies over
// a file-static `g_wm_status`, on the stated ground that "there is no WM work
// buffer here". The ground was wrong, and measuring it is what retired them.
//
// The pair is a read-modify-write of ONE word inside data_020a3fc0, and
// data_020a3fc0 IS NOT A WM SDK OBJECT AS FAR AS THIS BINARY IS CONCERNED. A
// sweep of every relocation in config/arm9/relocs.txt whose target lands in
// its ROM span [0x020a3fc0, 0x020a4b40) returns exactly two ROM functions:
// func_02040a5c and func_02040a84. Nothing else in the cartridge names one
// byte of it by relocation. So hosting it is not "hosting the WM work buffer";
// it is hosting 0xB80 bytes of DS BSS that two accessors own outright, which
// is what this port does with DS BSS everywhere else.
//
// AND THE OBSERVABLE IS PROVABLY UNCHANGED. src/func_0203ea5c.c:386 (and the
// wide copy's :475) is `func_02040a5c(func_02040a84())` -- read it, clear
// bit 0, write it back. Storage starts zero; a84 answers 0; a5c writes
// 0 & ~1 == 0. That was the loop invariant under g_wm_status and it is the
// loop invariant under the ROM's own storage, with no third reader anywhere
// in the link to tell the two apart. What changes is who computes it: the
// ROM's own two bodies, over the ROM's own word, instead of a stand-in.
//
// A DECOMP DISCREPANCY THE MOVE SURFACED, recorded rather than papered over:
// src/func_02040a5c.c writes offset 0xB50 (its struct is `char pad[0xb50];
// u32 field_b50`) while src/func_02040a84.c reads `data_020a3fc0[726]`, which
// is offset 0xB58. Two words, not one, for what the call site plainly means as
// one. Both TUs are byte-matched individually, so the disagreement is in the
// recovered C types and not in the ROM; it is invisible here because nothing
// reads either word, and it is invisible on the DS because the ROM's own
// bl-pair passes r0 straight through. Whichever way it resolves, this file is
// not the place it resolves -- flagged for the arm9 side.
//
// The storage. ROM span 0x020a4b40 - 0x020a3fc0 = 0xB80 = 2944 bytes, sized by
// span and not by the 0xB5c the two accessors happen to reach (the undersized
// hosted-global trap; hal/auto_bss.cpp's data_0209d3c4 note is the precedent).
// It is DS BSS, so it is inside the save-state bracket -- but in a section of
// its own for the reason the $wcomms block at the foot of this file spells
// out: an insertion INTERIOR to .dsstate shifts every hosted global past it,
// and 2944 bytes is not a small shift. ".dsstate$ywm" sorts after every suffix
// the tree uses ($ymp3* included: "ymp3" < "ywm" at the second character) and
// before the $zzz high sentinel, so the captured span grows at its tail and
// NOT ONE existing hosted global moves.
#pragma section(".dsstate$ywm", read, write)
__declspec(allocate(".dsstate$ywm")) __declspec(align(4))
unsigned char data_020a3fc0[0xB80] = {0};

// THE LINK STATE, rung W0's first global. Run link100, lane WM1.
//
// src/func_02040714.c is `return data_020a0f94` and it is LINKED now. The word
// lived in hal/comms_conductor.cpp's MP3_BSS run until this rung. It moved here
// because this file is what publishes it, and because the mp_comms_seam probe
// links this file WITHOUT linking the conductor -- a word the seam now
// references unconditionally has to live where the seam lives, and that is what
// turned the move from tidy into necessary.
//
// THE MOVE IS ADDRESS-NEUTRAL BY CONSTRUCTION: same section name, same size,
// same align(1), and ".dsstate$ymp3s0007" has exactly one contribution either
// way, so the linker puts it at the byte it put it at before. That matters more
// here than it usually would -- hosted DS globals reach the geometry stream as
// pointer VALUES, so a shifted global is a changed pixel, and the battery's 51
// level and 36 scene BMP rows are what checks it.
#pragma section(".dsstate$ymp3s0007", read, write)
__declspec(allocate(".dsstate$ymp3s0007")) __declspec(align(1))
unsigned char data_020a0f94[4] = {0};

// MY SLOT (the aid), rung W0's other global. Run link100, lane WM1.
//
// src/func_02040704.c is `return data_020a0f24` and it is LINKED now, so the
// word needs a home. It is DS BSS like everything else in this bracket, and it
// gets its OWN suffix for the same reason data_020a3fc0 above got one: ".dsstate$ywmb"
// sorts after "$ywm" (fourth character, "" < "b") and before the "$zzz" high
// sentinel, so the captured span grows AT ITS TAIL and neither data_020a3fc0
// nor one single global that came before it moves one byte. That is what keeps
// every BMP baseline in this tree comparable across this commit -- some hosted
// DS data reaches the geometry stream as a POINTER VALUE, so a shifted global
// is a changed pixel (port/tools/battery.py's own header measures it).
//
// SIZED BY ROM SPAN, four bytes: config/arm9/symbols.txt puts data_020a0f24 at
// 0x020a0f24 and the next symbol, data_020a0f28, at 0x020a0f28. Every ROM
// writer touches only the low halfword (src/func_020408b0.c:27,
// src/func_02040014.c, src/func_020402a0.c, src/func_0203fec4.c:23) and
// src/func_02040704.c reads it as one -- but the span is what gets hosted, not
// the width of the first field that reads it. That is the undersized-global
// trap this file's data_020a3fc0 note and hal/auto_bss.cpp's data_0209d3c4 note
// both name.
#pragma section(".dsstate$ywmb", read, write)
__declspec(allocate(".dsstate$ywmb")) __declspec(align(4))
unsigned char data_020a0f24[4] = {0};

// ===========================================================================
// RUNG W1'S STORAGE: NINETEEN DS GLOBALS THE WM CHAIN NAMES AND NOTHING HOSTED.
//
// Run link100, lane WM2. src/func_020408b0.c, src/func_020616e8.c (WM_Init),
// src/func_02061188.c (the channel-0xa reply dispatcher) and the nine TUs behind
// them are LINKED now, and between them they name twenty-three DS bss symbols.
// Four were already hosted -- data_020a0f28 and data_020a0f2c in
// hal/comms_conductor.cpp's MP3_BSS run, data_020a0f94 and data_020a0f24 at the
// foot of this file from rung W0 -- and they stay exactly where they are. The
// other nineteen are here.
//
// NINETEEN, NOT SIXTEEN. The rung's brief named sixteen. The closure names
// three more, and they are not optional: src/func_02061188.c reaches
// data_020a89c4 (the ten-entry command-buffer ring func_02058894 indexes),
// data_020a89ec (the synthesised 0x82 port-recv message) and data_020a8a00
// (that message's own +0x14 field, see the pair note below). Measured with
// port/tools/closure.py over the twelve TUs before anything was written:
// nineteen unresolved externals, exactly this list.
//
// EVERY SPAN IS THE ROM'S. Sizes are the distance to the next symbol in
// config/arm9/symbols.txt, never the width of the first field that reads one --
// that is the undersized-hosted-global trap this file's data_020a3fc0 note and
// hal/auto_bss.cpp's data_0209d3c4 note both name, and hal/boot2_ipc.cpp's
// data_020a8138 note names the interior-address half of it.
//
//   data_020a0f14  4     the wireless MODE the ROM was opened with; :25 of
//                        src/func_020408b0.c writes it FIRST, which is what
//                        lets hal/boot2_ipc.cpp read it at the claim
//   data_020a0f44  4     0x40 scratch, Memory::Allocate
//   data_020a0f48  4     0x20 scratch
//   data_020a0f4c  4     THE 0x1300 WM WORK BUFFER, handed to WM_Init
//   data_020a0f54  4     0xc0 multiboot parameter block (func_020672d0's thiz)
//   data_020a0f58  4     0x220, the size beside data_020a0f60
//   data_020a0f60  4     0x220 buffer
//   data_020a0f64  4     0x480, the size beside data_020a0f68
//   data_020a0f68  4     0x480 buffer
//   data_020a0f74  4     0x420 buffer (the send side W2 wants)
//   data_020a0f80  4     0x100 MP RECEIVE buffer (the one W2/W3 fill)
//   data_020a1064  64    0x020a1064..0x020a10a4; :47 writes the halfword at +0x30
//   data_020a89a8  4     WM_Init's "already initialised" guard, func_0206152c
//   data_020a89ac  4     the work-buffer POINTER; WM_GetSystemWork returns it
//   data_020a89b0  20    the command-buffer queue head (func_02058940's struct)
//   data_020a89c4  40    that queue's ten int slots; count 0xa x 4 = 0x28 exactly
//   data_020a89ec  20    the synthesised port-recv message
//   data_020a8a00  64    its +0x14 field, and 0x40 of ROM span
//   data_020a8a40  2560  the ten 0x100 command buffers WM_Init enqueues
//
// THE ONE PAIR THAT MUST COME OUT IN ROM ORDER: data_020a89ec and
// data_020a8a00. src/func_02061188.c declares the message as a 0x20-byte struct
// at data_020a89ec and writes its +0x1a and +0x1c fields, while the SAME
// function copies six halfwords to &data_020a8a00 -- and 0x020a8a00 is
// 0x020a89ec + 0x14. So on the DS these are ONE object with two names, exactly
// the shape hal/cxx_aliases.cpp's GX bank band and hal/boot2_ipc.cpp's
// data_020a8138/data_020a813c pair describe. Hosted apart at arbitrary
// addresses, the struct's +0x1a and +0x1c stores would land past the end of a
// 20-byte object and the MultiCopyHalf would land somewhere else again.
// The remedy is the tree's own: numbered grouped sections in ROM order, so the
// linker lays the run out contiguously and +0x14 of the first IS the second.
// port/tools/gxband_guard.py is the guard that exists for this class; the pair
// is not a declared band there because it is not one of that tool's two, and
// adding one is a change to a file this lane does not own.
//
// AND THE WHOLE RUN SORTS PAST EVERY EXISTING SUFFIX. ".dsstate$ywmc00" and up
// sort after ".dsstate$ywmb" (fifth character, "" < "c") and before the "$zzz"
// high sentinel, so the captured save-state span grows AT ITS TAIL and NOT ONE
// existing hosted global moves one byte. That is what keeps every BMP baseline
// in this tree comparable across this commit -- some hosted DS data reaches the
// geometry stream as a POINTER VALUE, so a shifted global is a changed pixel
// (port/tools/battery.py's own header measures it). port/tools/dsstate_guard.py
// reads the low bound back: it must be the number it was, with the count
// nineteen higher.
//
// ALL OF IT IS REAL DS STATE, so all of it is inside the save-state bracket.
// The work-buffer pointer decides where every WM reply is read from; a restore
// that rolled the world back and left these holding the pre-save heap addresses
// is precisely the bug hal/dsstate_seg.h exists for.
#define WMBSS(sec, name, size, algn)                              \
    __pragma(section(sec, read, write))                           \
    extern "C" __declspec(allocate(sec)) __declspec(align(algn))  \
    unsigned char name[size] = {0}

WMBSS(".dsstate$ywmc00", data_020a0f14, 4, 4);
WMBSS(".dsstate$ywmc01", data_020a0f44, 4, 4);
WMBSS(".dsstate$ywmc02", data_020a0f48, 4, 4);
WMBSS(".dsstate$ywmc03", data_020a0f4c, 4, 4);
WMBSS(".dsstate$ywmc04", data_020a0f54, 4, 4);
WMBSS(".dsstate$ywmc05", data_020a0f58, 4, 4);
WMBSS(".dsstate$ywmc06", data_020a0f60, 4, 4);
WMBSS(".dsstate$ywmc07", data_020a0f64, 4, 4);
WMBSS(".dsstate$ywmc08", data_020a0f68, 4, 4);
WMBSS(".dsstate$ywmc09", data_020a0f74, 4, 4);
WMBSS(".dsstate$ywmc10", data_020a0f80, 4, 4);
WMBSS(".dsstate$ywmc11", data_020a1064, 64, 4);
WMBSS(".dsstate$ywmc12", data_020a89a8, 4, 4);
WMBSS(".dsstate$ywmc13", data_020a89ac, 4, 4);
WMBSS(".dsstate$ywmc14", data_020a89b0, 20, 4);
WMBSS(".dsstate$ywmc15", data_020a89c4, 40, 4);
// The pair. align(4) on both and the sizes are the ROM's, so data_020a8a00
// lands at data_020a89ec + 0x14 and the 0x20-byte message struct at the first
// name reaches its last field inside the second's storage, as it does on the DS.
WMBSS(".dsstate$ywmc16", data_020a89ec, 20, 4);
WMBSS(".dsstate$ywmc17", data_020a8a00, 64, 4);
WMBSS(".dsstate$ywmc18", data_020a8a40, 2560, 4);

#undef WMBSS

// Reads the pair's layout back, the way port_gxbank_layout_check does for the
// GX band. Nonzero when data_020a8a00 came out at data_020a89ec + 0x14, which
// is the precondition for src/func_02061188.c's synthesised message being one
// object rather than two. Called from comms_arm7_turn's first turn.
extern "C" int port_wm_message_layout_check(void)
{
    return (int)(data_020a8a00 - data_020a89ec) == 0x14;
}

// src/func_0203e20c.c: DS DOWNLOAD PLAY, an eight-state multiboot server that
// feeds the game to three other consoles over the radio. A PC port with a
// socket transport does not need it and cannot honestly provide it -- there is
// no cartridge to send. REFUSED, and the refusal is the ROM's own shape: role
// 3 is cleared back to solo, exactly what src/func_0203e20c.c does when its
// distribution ends.
// PORT_HOST_ABI: hosted WM/radio seam face; DS Download Play multiboot server over the radio, which a PC port with a socket transport cannot honestly provide (no cartridge to distribute).
void func_0203e20c(void) {
    static bool said;
    if (!said) {
        said = true;
        std::fprintf(stderr, "[comms] role 3 (DS Download Play) refused: there "
                     "is no cartridge to distribute. Falling back to solo.\n");
    }
    data_020a0f04[0] = port::kCommsRoleSolo;
}

}  // extern "C"

// ===========================================================================
// THE PAD BLOCK src/func_0203bc7c.c OWNS.
//
// data_020a0e44/48/50 are ROM bss the port never hosted, because nothing was
// linked that touched them. src/func_0203bc7c.c is, now. Sized by ROM SPAN
// off config/arm9/symbols.txt (0x020a0e44 -> 0x020a0e48 -> 0x020a0e50 ->
// 0x020a0e58) and not by the width of the first field that reads them, which
// is the undersized-host-global trap this tree has been bitten by before.
// ===========================================================================
//
// THEY ARE DS STATE, so they go inside the save-state bracket -- but NOT at
// hal/dsstate_seg.h's default $mmm suffix, and the reason is a measurement
// rather than a preference.
//
// port/tools/battery.py's own header: some hosted DS data reaches the geometry
// stream as a POINTER VALUE, so the rendered frame depends on the ABSOLUTE
// ADDRESS of hosted globals, and an insertion INTERIOR to .dsstate shifts
// every global past it while leaving the section base exactly where it was.
// Its measured table puts that at 1354 changed pixels for 64 inserted bytes at
// 296 frames. So three new globals at $mmm would make every BMP comparison
// against a pre-seam build a layout artifact, and "byte-identical" would have
// been unavailable as evidence for the whole regression proof.
//
// MSVC merges grouped sections in lexical order of the text after the `$`.
// $wcomms sorts after every family this tree uses ($aaa, $aab, $after, $bulk,
// $camcomm, $camrec, $gxbank, $hvsstar, $l2_, $mg, $mmblk, $mmcray, $mmm,
// $oamsh, $pk*, $savblk, $touch) and before the $zzz high sentinel, so these
// three land at the TOP of the captured span and NOT ONE existing hosted
// global moves. The span grows; the span is read only by lk6_savestate.cpp's
// memcpy and reaches no frame. That is what makes the BMP pairs in
// runs/mg15/out/MP1/ mean something.
//
// RETIREMENT CONDITION: if these ever have to be ROM-SPACED against
// data_020a0e58 (they are its immediate ROM neighbours), they move into that
// band and every BMP baseline taken against this layout has to be retaken.
// Nothing needs that today -- src/func_0203bc7c.c indexes each array within
// its own bounds and never reaches one as an interior address of another,
// which is the only thing the gxband rule is about.
//
// Nothing below re-declares them, which is the silent way out of the bracket
// dsstate_seg.h warns about.
#pragma section(".dsstate$wcomms", read, write)
extern "C" {
__declspec(allocate(".dsstate$wcomms")) __declspec(align(4))
unsigned char data_020a0e44[4] = {0};       // 0x020a0e44 .. 0x020a0e48
__declspec(allocate(".dsstate$wcomms")) __declspec(align(2))
unsigned short data_020a0e48[port::kCommsMaxPlayers] = {0};   // 0x020a0e48 .. 0x020a0e50 on the DS; sixteen here (0.3.2, the wide fan-out walks it)
__declspec(allocate(".dsstate$wcomms")) __declspec(align(2))
unsigned short data_020a0e50[port::kCommsMaxPlayers] = {0};   // 0x020a0e50 .. 0x020a0e58 on the DS; sixteen here (0.3.2)
}
