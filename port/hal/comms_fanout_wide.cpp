// THE WIDE FAN-OUT. 0.3.2, the sixteen-player release; reshaped by run link100
// (lane R3C's ruling 2) from REPLACE to ALWAYS-THEN-EXTEND.
//
// src/func_0203bb60.c and src/func_0203bc7c.c are steps 0x16 and 0x17 of the
// ROM's main loop: every frame they copy the per-slot comms records into the
// per-player TouchInfo and PadData arrays that the rest of the game reads its
// input from. Both loop `i < 4`, hard -- the 4 is a bare immediate in the loop
// test (cmp r4,#4 at 0x0203bb84; cmp r8,#4 at 0x0203bcfc), not a table size
// and not a count global, so there is no ROM constant to widen. With the
// conductor hosted wide (hal/comms_conductor_wide.cpp) the records for slots
// 4..15 arrive every round -- and then never left them: sixteen bodies in
// lockstep, twelve of them standing on their spawn with a pad that read zero
// on every peer. The independent review of the sixteen-player merge caught it
// by reading the per-slot pad words in the ladder logs.
//
// WHAT THE TWO SYMBOLS BELOW DO NOW. They run the ROM's own body first, on
// every frame of every session, and then append slots 4..15 when the session
// is wider than four. The ROM TUs stay in the link byte for byte under the
// per-source renames func_0203bb60_narrow / func_0203bc7c_narrow
// (port/CMakeLists.txt), and they are no longer merely what a narrow session
// runs: they are what EVERY session runs, and the host only extends them.
//
// WHY THE SPLIT IS EXACT AND NOT AN APPROXIMATION. func_0203bb60 is purely
// per-slot: it writes data_020a0de8 + 4*i and reads func_0203dabc(i), and
// nothing crosses slots at all. func_0203bc7c writes data_020a0e58[i],
// data_020a0e48[i] and data_020a0e50[i] -- all indexed -- plus exactly one
// shared word, data_020a0e44, which it ZEROES at its head and then OR-SETS per
// slot. So the ROM's body (zero, then accumulate 0..3) followed by a tail that
// accumulates 4..15 WITHOUT re-zeroing produces bit-for-bit what one wide loop
// over 0..15 would produce. That is why pad_fanout_tail has no
// `data_020a0e44 = 0;` line: the ROM body has already done it, and doing it
// again would throw away slots 0..3's accumulation. Nothing reads TouchInfo or
// PadData between the two calls either -- the ROM's loop has one store of the
// phase word between 0x0201984c and 0x02019854 and nothing else.
//
// AND WHY IT MATTERS THAT IT IS AN EXTENSION. The previous shape ran a HOST
// COPY of a matched ROM body above four players and left the ROM's own body
// uncalled. To the linker that reads as a face either way, and the linkage
// tool says so in as many words ("a map cannot prove forwarding [...] equally
// consistent with a host body that duplicates it"). To the port it was a
// duplicate of matched code with a different bound, and the day the ROM's own
// loop drives the frame -- rung R3d, func_020197b8 at phases 0x16 and 0x17 --
// that duplicate is what the cartridge's code would be calling. It forwards
// now, so the caveat stops applying: these are true faces.
//
// THIS IS A MOD, NOT A HOST-ABI EXCEPTION, and it carries no PORT_HOST_ABI
// tag. That tag is a ruling about what MSVC cannot express -- a register
// ride-through, mwcc's pointer-to-member, hand asm, unmodelled hardware -- and
// none of those is in play here. Sixteen players is a deliberate feature
// deviation whose ruling is written in port/hal/vs_width.h ("The cartridge did
// four. This port hosts sixteen, and that is a MOD"). Tagging a mod as a
// host-ABI exception would put a feature decision into the one bucket that
// has to stay honest about what could not be done otherwise.
//
// STORAGE: TouchInfo (data_020a0de8, a placed band, sixteen entries since
// vs16), PadData (data_020a0e58, sixteen pairs since vs16), and the two
// previous-keys arrays data_020a0e48 / data_020a0e50 (sixteen halfwords each,
// widened with this file). The accessors func_0203dabc / func_0203dae4 index
// the record run, which is sixteen records since the conductor was hosted, and
// neither accessor bounds-checks -- they are plain stride-0x24 indexing -- so
// the tails write into storage that exists and is zeroed at session open.

#include <cstdio>
#include <cstdlib>

#include "comms_loopback.h"
#include "vs_width.h"

extern "C" {

typedef unsigned char u8;
typedef unsigned short u16;

struct R {
    u16 field_0;
    u16 field_2;
    u16 field_4;
};

typedef struct InputPair {
    u16 cur;
    u16 pressed;
} InputPair;

extern u8 data_020a0de8[];
extern struct R *func_0203dabc(int idx);

extern unsigned char data_020a0e44;
extern u16 data_020a0e48[kPortMaxPlayers];
extern u16 data_020a0e50[kPortMaxPlayers];
extern InputPair data_020a0e58[kPortMaxPlayers];
extern u16 func_0203dae4(int idx);

// The per-slot comms record run, sixteen 0x24-byte records since the wide
// conductor (hal/camera_bridges.cpp). Read here only by the dump below.
extern unsigned char data_020a1154[];

// The narrow arms: the ROM TUs themselves, under the build's rename.
void func_0203bb60_narrow(void);
void func_0203bc7c_narrow(void);

}  // extern "C"

namespace {

// THE TAIL of src/func_0203bb60.c: its loop body verbatim, over slots
// kPortNarrowPlayers..kPortMaxPlayers-1. The ROM's own body has already run
// slots 0..3 and left p where the DS left it, so this picks up at record 4.
void touch_fanout_tail(void)
{
    u8 *p = data_020a0de8 + 4 * kPortNarrowPlayers;
    int i;
    for (i = kPortNarrowPlayers; i < kPortMaxPlayers; i++) {
        struct R *r = func_0203dabc(i);
        p[1] = (u8)(r->field_4 ^ p[0]);
        p[0] = (u8)r->field_4;
        p[2] = (u8)r->field_0;
        p[3] = (u8)r->field_2;
        p += 4;
    }
}

// THE TAIL of src/func_0203bc7c.c: its loop body verbatim, over slots
// kPortNarrowPlayers..kPortMaxPlayers-1, and WITHOUT the `data_020a0e44 = 0;`
// that stands at the head of the ROM's body. That one line is the whole reason
// this is a tail and not a second wide loop: the ROM zeroes the word once and
// then OR-sets it per slot, so re-zeroing here would discard what slots 0..3
// accumulated. Zero-once-then-accumulate over 0..3 and then over 4..15 is the
// same function as zero-once-then-accumulate over 0..15.
void pad_fanout_tail(void)
{
    InputPair *p = data_020a0e58 + kPortNarrowPlayers;
    int i;
    for (i = kPortNarrowPlayers; i < kPortMaxPlayers; i++)
    {
        u16 keys = func_0203dae4(i);
        u16 old;
        int changed;
        if ((keys & 0x30) == 0x30)
            keys &= ~0x30;
        if ((keys & 0xc0) == 0xc0)
            keys &= ~0xc0;
        if (keys == 0x30c)
            data_020a0e44 = 1;
        old = data_020a0e50[i];
        changed = keys ^ old;
        p->pressed = keys & changed;
        data_020a0e48[i] = old & changed;
        data_020a0e50[i] = keys;
        p->cur = keys;
        p++;
    }
}

inline bool wide(void)
{
    return port::comms_session_players() > kPortNarrowPlayers;
}

// ===========================================================================
// THE BIT-IDENTITY INSTRUMENT. SM64DS_FANOUT_DUMP=1, off and silent unless
// set.
//
// The fan-out's whole output is five regions of hosted storage plus one byte,
// and this prints a hash of each of them once per frame, at the tail of step
// 0x17 -- after BOTH dispatchers have run, because the ROM's loop runs 0x16
// (func_0203bb60) and then 0x17 (func_0203bc7c) with nothing between them
// that reads either array. So one line per frame is the complete state the
// rest of the game reads its stylus and buttons out of.
//
// It exists to answer one question by measurement rather than by argument:
// does moving the fan-out from REPLACE to ALWAYS-THEN-EXTEND change any byte
// the game can see? Two builds, the same scripted runs, and `diff` on these
// lines is the answer. Everything in the line is a pure function of that
// state -- no pointer, no clock, no address -- so two runs that fan out the
// same records print the same file.
// ===========================================================================
unsigned fnv1a(const void *mem, unsigned n)
{
    const unsigned char *b = (const unsigned char *)mem;
    unsigned h = 2166136261u;
    unsigned k;
    for (k = 0; k < n; k++) {
        h ^= b[k];
        h *= 16777619u;
    }
    return h;
}

void fanout_dump(void)
{
    static int on = -1;
    static unsigned long long seq = 0;
    if (on < 0) {
        const char *s = std::getenv("SM64DS_FANOUT_DUMP");
        on = (s && std::atoi(s) != 0) ? 1 : 0;
    }
    if (!on) return;
    std::fprintf(stderr,
        "[fanout] n=%llu wide=%d players=%d e44=%02x touch=%08x pad=%08x "
        "prev48=%08x prev50=%08x recs=%08x\n",
        (unsigned long long)seq++,
        wide() ? 1 : 0,
        port::comms_session_players(),
        (unsigned)data_020a0e44,
        fnv1a(data_020a0de8, 4u * (unsigned)kPortMaxPlayers),
        fnv1a(data_020a0e58, (unsigned)sizeof(InputPair) * (unsigned)kPortMaxPlayers),
        fnv1a(data_020a0e48, 2u * (unsigned)kPortMaxPlayers),
        fnv1a(data_020a0e50, 2u * (unsigned)kPortMaxPlayers),
        fnv1a(data_020a1154, 0x24u * (unsigned)kPortMaxPlayers));
    std::fflush(stderr);
}


// ===========================================================================
// THE WRITER WATCH. Run link100, lane WM9.
//
// THE QUESTION IT ANSWERS. This lane was sent to open the ROM's own WM
// port-receive path, and the risk the brief named is that the game's input
// words would then be written TWICE a frame: once by the host carrier's
// per-frame fan-out (this file) and once by whatever the ROM's own receive
// path did with the same datagram. Which path owns which bytes is settled by
// reading, in hal/wm_arm7.cpp's WM9 block, and the two are disjoint. This is
// the instrument that CHECKS that reading instead of resting on it.
//
// WHAT IT COUNTS, AND WHY THE LAST NUMBER IS THE REAL ONE. The first two are
// plain call counts on the two dispatchers: on a correct frame each runs
// exactly once, so frames, touch and pad climb together, and a second writer
// that came through either dispatcher would show as a count running ahead of
// the frame number.
//
// The last one catches a writer that does not come through here at all, which
// is exactly the shape a second receive path would have. All five regions the
// fan-out owns are hashed at the END of step 0x17 and re-hashed at the START
// of the next step 0x16. Those two steps run back to back in the ROM's loop
// with nothing between them that writes either array -- one store of the phase
// word between 0x0201984c and 0x02019854 and nothing else -- so a byte that
// changes across that boundary changed under a writer outside the fan-out.
// Every such frame is counted and the first one is named, region by region.
//
// ALWAYS ON, for the cost of five FNV-1a passes over 0x2f0 bytes a frame --
// the same arithmetic SM64DS_FANOUT_DUMP already does when it is set. A
// counter that has to be switched on is a counter nobody has on the run that
// mattered; hal/wm_thread.cpp's [wm8] line carries these four numbers on every
// logged run, beside the receive-path census they qualify.
// ===========================================================================

unsigned long long g_wf_frames  = 0;
unsigned long long g_wf_touch   = 0;
unsigned long long g_wf_pad     = 0;
unsigned long long g_wf_foreign = 0;
unsigned g_wf_h[5] = { 0, 0, 0, 0, 0 };
bool     g_wf_armed = false;
bool     g_wf_said  = false;

void wf_hash(unsigned out[5])
{
    out[0] = fnv1a(data_020a0de8, 4u * (unsigned)kPortMaxPlayers);
    out[1] = fnv1a(data_020a0e58,
                   (unsigned)sizeof(InputPair) * (unsigned)kPortMaxPlayers);
    out[2] = fnv1a(data_020a0e48, 2u * (unsigned)kPortMaxPlayers);
    out[3] = fnv1a(data_020a0e50, 2u * (unsigned)kPortMaxPlayers);
    out[4] = fnv1a(&data_020a0e44, 1u);
}

// At the head of step 0x16, before the ROM's own body writes anything.
void wf_check_before(void)
{
    if (!g_wf_armed) return;
    unsigned now[5];
    wf_hash(now);
    int bad = 0;
    for (int i = 0; i < 5; ++i) if (now[i] != g_wf_h[i]) bad = 1;
    if (!bad) return;
    ++g_wf_foreign;
    if (g_wf_said) return;
    g_wf_said = true;
    static const char *kNames[5] = {
        "data_020a0de8 TouchInfo", "data_020a0e58 PadData",
        "data_020a0e48 prev-down", "data_020a0e50 prev-keys",
        "data_020a0e44 the shared flag" };
    for (int i = 0; i < 5; ++i)
        if (now[i] != g_wf_h[i])
            std::fprintf(stderr,
                "[wm9] SECOND WRITER on frame %llu: %s changed between the end "
                "of step 0x17 and the head of step 0x16 (%08x -> %08x). The "
                "fan-out is no longer the only writer of the game's input "
                "words.\n",
                (unsigned long long)g_wf_frames, kNames[i],
                g_wf_h[i], now[i]);
    std::fflush(stderr);
}

// At the tail of step 0x17, after both ROM bodies and both tails.
void wf_settle(void)
{
    wf_hash(g_wf_h);
    g_wf_armed = true;
    ++g_wf_frames;
}

}  // namespace

extern "C" void port_wm9_fanout_writers(unsigned long long *frames,
                                        unsigned long long *touch,
                                        unsigned long long *pad,
                                        unsigned long long *foreign)
{
    if (frames)  *frames  = g_wf_frames;
    if (touch)   *touch   = g_wf_touch;
    if (pad)     *pad     = g_wf_pad;
    if (foreign) *foreign = g_wf_foreign;
}

// Step 0x16 of the ROM's loop. The cartridge's body, then the extension.
extern "C" void func_0203bb60(void)
{
    wf_check_before();                    /* WM9: did anything else write? */
    ++g_wf_touch;
    func_0203bb60_narrow();               /* the ROM's own body, every frame */
    if (wide()) touch_fanout_tail();      /* slots 4..15, and only those */
}

// Step 0x17. Same shape, and the dump hangs off the end of it because the two
// steps run back to back with nothing between them: one line per frame is the
// whole fan-out's output.
extern "C" void func_0203bc7c(void)
{
    ++g_wf_pad;
    func_0203bc7c_narrow();               /* the ROM's own body, every frame */
    if (wide()) pad_fanout_tail();        /* slots 4..15, and only those */
    fanout_dump();
    wf_settle();                          /* WM9: the frame's settled words */
}
