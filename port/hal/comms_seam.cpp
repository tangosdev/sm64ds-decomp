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
#include "dsstate_seg.h"

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

// The ROM's fan-out, steps 0x16 and 0x17 of src/func_020197b8.c.
void func_0203bb60(void);
void func_0203bc7c(void);
}

namespace port {
namespace {

const CommsTransport *g_transport = nullptr;
uint16_t g_boot_indicator = 0;      // 0 = cartridge boot. See comms_seam.h.
uint64_t g_exchanges = 0;
uint64_t g_rounds = 0;

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
        !t->peer_block || !t->poll) {
        std::fprintf(stderr, "[comms] transport '%s' REFUSED: an entry is null. "
                     "Every entry of CommsTransport is required.\n",
                     t->name ? t->name : "(unnamed)");
        return false;
    }
    g_transport = t;
    return true;
}

const CommsTransport *comms_transport() { return g_transport; }

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
        "[comms:%s]   local  frame=%d stylus={%u,%u,%u} flag=%04x key=%04x\n",
        tag ? tag : "-", *(const int *)(loc + 0x00),
        loc[0x04], loc[0x06], loc[0x08],
        *(const unsigned short *)(loc + 0x0C),
        *(const unsigned short *)(loc + 0x0E));

    for (int i = 0; i < kCommsMaxPlayers; ++i) {
        const unsigned char *p = data_020a1154 + i * 0x24;
        const unsigned char *t = data_020a0de8 + i * 4;
        std::fprintf(stderr,
            "[comms:%s]   slot%d frame=%d stylus={%u,%u,%u} flag=%04x key=%04x "
            "| touch{now=%u chg=%u x=%u y=%u} pad{held=%04x pressed=%04x}\n",
            tag ? tag : "-", i, *(const int *)(p + 0x00),
            p[0x04], p[0x06], p[0x08],
            *(const unsigned short *)(p + 0x0C),
            *(const unsigned short *)(p + 0x0E),
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

// src/func_020408b0.c: allocates the WM work buffers off the game heap, reads
// the boot indicator to pick an init branch, and starts the SDK. A transport
// needs none of that; it needs to know it is being opened.
void func_020408b0(unsigned short mode) {
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->open(mode); return; }
    port::g_solo_state = port::kCommsIdle;
    port::g_solo_slot = 0;
}

// src/func_02040820.c: the ROM's become-parent state machine over
// data_020a0f94/data_020a0f5c, with Wireless_Reset on a role flip.
void func_02040820(void) {
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->become_parent(); return; }
    // No radio: the request is recorded and refused the way an unanswered
    // radio refuses it -- the state never leaves idle, so the ROM's own wait
    // bound expires and src/func_0203ea5c.c:423 drops the session to solo.
    port::g_solo_state = port::kCommsIdle;
}

// src/func_02040790.c, the child half of the same machine.
void func_02040790(void) {
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->become_child(); return; }
    port::g_solo_state = port::kCommsIdle;
}

// src/func_02040724.c: leave.
void func_02040724(void) {
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->close(); return; }
    port::g_solo_state = port::kCommsIdle;
    port::g_solo_slot = 0;
}

// src/func_02040714.c is `return data_020a0f94`, the link state.
int func_02040714(void) {
    const port::CommsTransport *t = port::comms_transport();
    return t ? t->state() : port::g_solo_state;
}

// src/func_02040704.c is `return data_020a0f24`, my slot index.
// THE ROM'S CALLER PASSES AN ARGUMENT IT DOES NOT DECLARE:
// src/func_0203ea5c.c:236 is `data_020a0f10 = func_02040704(temp_r0_5)`, which
// on ARM is a dead r0 write the callee overwrites. Declared with the argument
// here so a stack ABI agrees with the ROM's own call sites.
int func_02040704(int ignored) {
    (void)ignored;
    const port::CommsTransport *t = port::comms_transport();
    return t ? t->slot() : port::g_solo_slot;
}

// src/func_020406b4.c: hands the staged block to func_02062df0 (the WM send)
// and reports back through data_020a0f80. Returns 1 when the round is in.
int func_020406b4(const void *block, unsigned short *status) {
    ++port::g_exchanges;
    const port::CommsTransport *t = port::comms_transport();
    if (!t) {
        if (status) *status = 0;
        return 0;               // no partners: the round never completes
    }
    const int done = t->exchange(block, status);
    if (done) ++port::g_rounds;
    return done;
}

// src/func_0204068c.c: player `aid`'s received block, or 0.
const void *func_0204068c(unsigned short aid) {
    const port::CommsTransport *t = port::comms_transport();
    if (!t) return nullptr;
    if ((int)aid >= port::kCommsMaxPlayers) return nullptr;
    return t->peer_block((int)aid);
}

// src/func_02040c34.c: starts the DS's wireless THREAD with two callbacks,
// which are the ROM's own src/func_0203f644.c and src/func_0203f604.c. A host
// transport has no thread to start -- it is polled from the seam's own pump --
// so this records the request and returns. The callbacks are deliberately NOT
// invoked: on the DS they run in the wireless thread's context and mutate the
// same state the poll path does, so calling them from here would double-apply
// it. A transport that wants them driven does it from poll().
void func_02040c34(int role, int b, void *cb_recv, void *cb_send, int e) {
    (void)role; (void)b; (void)cb_recv; (void)cb_send; (void)e;
}

// src/func_02040a5c.c / src/func_02040a84.c: a status word the ROM keeps
// inside the WM work buffer (data_020a3fc0 + 0xB50). There is no WM work
// buffer here, so the seam keeps the word itself.
static unsigned int g_wm_status;
void func_02040a5c(unsigned int val) { g_wm_status = val & ~1u; }
int func_02040a84(void) { return (int)g_wm_status; }

// src/func_0203e20c.c: DS DOWNLOAD PLAY, an eight-state multiboot server that
// feeds the game to three other consoles over the radio. A PC port with a
// socket transport does not need it and cannot honestly provide it -- there is
// no cartridge to send. REFUSED, and the refusal is the ROM's own shape: role
// 3 is cleared back to solo, exactly what src/func_0203e20c.c does when its
// distribution ends.
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
// THEY ARE DS STATE, so they go inside the save-state bracket. Nothing here
// re-declares them further down the file, which is the silent way out of the
// bracket dsstate_seg.h warns about. port/tools/dsstate_guard.py is what
// caught them the first time they were written as plain globals.
DSSTATE_BEGIN
extern "C" {
unsigned char data_020a0e44[4];         // 0x020a0e44 .. 0x020a0e48
unsigned short data_020a0e48[4];        // 0x020a0e48 .. 0x020a0e50
unsigned short data_020a0e50[4];        // 0x020a0e50 .. 0x020a0e58
}
DSSTATE_END
