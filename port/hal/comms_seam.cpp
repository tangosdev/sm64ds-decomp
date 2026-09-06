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

// src/func_020408b0.c: allocates the WM work buffers off the game heap, reads
// the boot indicator to pick an init branch, and starts the SDK. A transport
// needs none of that; it needs to know it is being opened.
// PORT_HOST_ABI: hosted WM/radio seam face; the ROM body starts the NITRO WM SDK over arm7.bin, which this repo does not decompile (file header). Behind the transport seam.
void func_020408b0(unsigned short mode) {
    // Run vs16, hosted-conductor follow-up. The ROM clears its four records at
    // session start (src/func_0203db64.c:64 zeroes 0x90 at data_020a1154); the
    // host run behind them is sixteen records (hal/camera_bridges.cpp), and
    // records 4..15 are the port's, so the port clears them at the same moment
    // in the lifecycle -- this face runs exactly once per session-arm, off the
    // one-shot at src/func_0203ea5c.c:137-140. Without this, a wide session
    // formed after an earlier one in the same process would read the dead
    // session's live bits out of slots 4..15 and wait on ghosts. Done for
    // narrow sessions too: nothing narrow reads past 0x90, and the seam's own
    // per-slot report prints the full run, which should not show stale rows.
    std::memset(data_020a1154 + 4 * 0x24, 0, 12 * 0x24);
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->open(mode); return; }
    port::g_solo_state = port::kCommsIdle;
    port::g_solo_slot = 0;
}

// src/func_02040820.c: the ROM's become-parent state machine over
// data_020a0f94/data_020a0f5c, with Wireless_Reset on a role flip.
// PORT_HOST_ABI: hosted WM/radio seam face; ROM body is the become-parent state machine over the NITRO WM SDK / arm7.bin radio, which this repo does not decompile.
void func_02040820(void) {
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->become_parent(); return; }
    // No radio: the request is recorded and refused the way an unanswered
    // radio refuses it -- the state never leaves idle, so the ROM's own wait
    // bound expires and src/func_0203ea5c.c:487 drops the session to solo.
    port::g_solo_state = port::kCommsIdle;
}

// src/func_02040790.c, the child half of the same machine.
// PORT_HOST_ABI: hosted WM/radio seam face; the become-child half of the same WM SDK / arm7.bin state machine this repo does not decompile.
void func_02040790(void) {
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->become_child(); return; }
    port::g_solo_state = port::kCommsIdle;
}

// src/func_02040724.c: leave.
// PORT_HOST_ABI: hosted WM/radio seam face; the ROM body closes the NITRO WM SDK session over arm7.bin, which this repo does not decompile.
void func_02040724(void) {
    const port::CommsTransport *t = port::comms_transport();
    if (t) { t->close(); return; }
    port::g_solo_state = port::kCommsIdle;
    port::g_solo_slot = 0;
}

// ===========================================================================
// THE NEXT TWO ARE CLOSABLE, AND THIS LANE DID NOT CLOSE THEM.
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
// ===========================================================================

// src/func_02040714.c is `return data_020a0f94`, the link state.
// PORT_HOST_ABI: hosted WM/radio seam face; the seam, not the ROM's own word, is where this port keeps the link state -- see the CLOSABLE note directly above, which is the ruling this tag stands on.
int func_02040714(void) {
    const port::CommsTransport *t = port::comms_transport();
    return t ? t->state() : port::g_solo_state;
}

// src/func_02040704.c is `return data_020a0f24`, my slot index.
// THE ROM'S CALLER PASSES AN ARGUMENT IT DOES NOT DECLARE:
// src/func_0203ea5c.c:252 is `data_020a0f10 = func_02040704(temp_r0_5)`, which
// on ARM is a dead r0 write the callee overwrites. Declared with the argument
// here so a stack ABI agrees with the ROM's own call sites.
// PORT_HOST_ABI: hosted WM/radio seam face plus a register ride-through; the ROM caller passes a dead r0 the callee overwrites, declared as an argument so the host stack ABI matches the ROM call sites, and the seam keeps the slot index the ROM keeps in data_020a0f24 -- see the CLOSABLE note above func_02040714.
int func_02040704(int ignored) {
    (void)ignored;
    const port::CommsTransport *t = port::comms_transport();
    return t ? t->slot() : port::g_solo_slot;
}

// src/func_020406b4.c: hands the staged block to func_02062df0 (the WM send)
// and reports back through data_020a0f80. Returns 1 when the round is in.
// PORT_HOST_ABI: hosted WM/radio seam face; the ROM body hands the staged block to the NITRO WM send over arm7.bin, which this repo does not decompile.
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
