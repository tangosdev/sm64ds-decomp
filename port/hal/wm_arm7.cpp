// THE HOSTED WM ARM7: the half of the radio the cartridge does not contain.
//
// Run link100, lane WM3, rung W4. NONE OF THIS IS DECOMPILED CODE, the same
// way hal/boot2_ipc.cpp is not: the DS has two processors and the port runs
// one, and this file stands in for what the other one's NITRO WM driver does
// when the ARM9 pushes a command down PXI channel 0xa. Nothing here carries a
// PORT_HOST_ABI tag -- that tag is a ruling about a MATCHED body, and there is
// no matched body in this file.
//
// WHAT CHANGED ON THE ARM9 SIDE, AND WHY THIS FILE EXISTS NOW. Rung W1 (lane
// WM2) linked src/func_020408b0.c and src/func_020616e8.c, so the ROM's own
// WM_Init runs: it lays out the 0x1300 work buffer and registers
// func_0205ba64(0xa, func_02061188), the ARM9's channel-0xa reply handler. It
// sends NOTHING, which is why hal/boot2_ipc.cpp could hold channel 0xa
// observed-only and say so honestly. Rung W4 links func_02040820 and
// func_02040790 -- the ROM's own become-parent and become-child state machines
// -- and they DO send. From here the ROM asks and something has to answer.
//
// ===========================================================================
// THE PROTOCOL, READ OUT OF THE ROM AND NOT OUT OF AN SDK HEADER
// ===========================================================================
//
// src/WM_SendCommand.c is the whole ARM9->ARM7 direction: take a 0x100 buffer
// off the queue, write the apiid as a halfword at +0, write `count` vararg
// words at +4, flush, IPCSend(0xa, buf, 0). So a command is (apiid, argv[]).
//
// src/func_02061188.c is the whole ARM7->ARM9 direction. It invalidates the two
// ARM7-owned regions -- work[4] (0x100, THE REPLY MESSAGE) and work[1] (0xa00,
// THE STATUS BLOCK) -- reads msg[0] as the apiid and dispatches through
// work[apiid + 5], the table src/WM_SetCallbackTable.c writes.
//
// So the ARM7 owes three things and no others: a reply message, a status
// block, and (for MP, rung W3) a receive buffer. What each reply must CARRY is
// read off the ROM body that consumes it -- every field below names the src
// line that reads it, because an SDK header would be a label and not evidence:
//
//   apiid  ROM callback         fields that body reads
//   -----  -------------------  --------------------------------------------
//   0x01   src/func_020402a0.c  msg[1] (self->f2, :20 `if (self->f2 != 0)`)
//   0x03   src/func_020405b4.c  msg[1] (:8 `if (p[1] == 0)`)
//   0x04   src/func_02040504.c  msg[1] (:10 `if (o[1] == 0)`)
//   0x05   src/func_02040408.c  msg[1] (:13 `if (arg[1] != 0)`)
//   0x06   src/func_02040388.c  msg[1] (:9 `if (p[1] == 0)`)
//   0x07   src/func_020401e8.c  msg[1] (:9 `if (p[1] == 0)`)
//   0x08   src/func_02040014.c  msg[1] as *(u16*)(a+2) (:19), the state code
//                               as *(u16*)(a+8) = msg[4] (:23), and on state 7
//                               six MAC bytes at a+0xa = msg[5..7] (:56) and
//                               the aid at *(u16*)(a+0x10) = msg[8] (:63)
//   0x0c   src/func_0203fec4.c  msg[1] (:11), the state code msg[4] (:22),
//                               the aid msg[5] (:25 `data_020a0f24 = p[5]`)
//   0x0d   (callback is 0)      src/func_02040014.c:73 passes self = 0, so
//                               src/func_02061188.c's table lookup finds null
//   0x0e   src/func_0203fdac.c  msg[1] as a->h2 (:11), the state code msg[2]
//                               as a->h4 (:13 `case 0xa:`)
//   0x0f   src/func_02062d10.cpp  posted as an 0x81 indication; the errcode at
//                               +2, the port index at +0xa, the handler
//                               pointer at +0x14 (src/func_02061188.c's 0x81
//                               arm) and the length at +0x1c
//
// THE STATE HALFWORD AND ITS LADDER. src/WM_CheckStateEx.c reads ONE halfword,
// *(u16 *)work[1] + 0, and every command gates on it. The ladder is not
// assumed; it is what the ROM's own preconditions force, read off the senders:
//
//   src/func_02062380.c   apiid 3  requires state 0   -> so 3 leaves state 1
//   src/func_020622f0.c   apiid 5  requires state 1   -> so 5 leaves state 2
//   src/func_020620b0.c   apiid 7  requires state 2   -> 7 leaves it at 2
//   src/func_02061fdc.c   apiid 8  requires state 2   -> 8 leaves state 7
//   src/func_02061e38.c   apiid 0xc requires state 2  -> 0xc leaves state 8
//   src/func_020625fc.c   apiid 0xe requires 7 or 8   -> 0xe leaves 9 or 0xa
//   src/func_020631dc.c            requires 9 or 0xa  (the MP data setup)
//   src/func_020614d0.c   apiid 1  requires state > 1 -> 1 leaves state 2
//   src/func_020622a8.c   apiid 6  requires state 2   -> 6 leaves state 1
//   src/func_02062338.c   apiid 4  requires state 1   -> 4 leaves state 0
//
// Those ten constraints have exactly one consistent assignment and it is the
// SDK's documented one (READY 0, STOP 1, IDLE 2, PARENT 7, CHILD 8, MP_PARENT
// 9, MP_CHILD 0xa). The names are the SDK's; the NUMBERS are the ROM's.
//
// THE FOUR STATUS FIELDS THAT ARE NOT THE STATE, and the body that reads each:
//
//   +0x10  u32 read as one word.  src/func_020625fc.c:35 refuses when it is 1
//          ("MP is already running") and src/func_020631dc.c:41 refuses when it
//          is 0 ("MP is not running"). One halfword at +0x10 with +0x12 zero
//          satisfies both readings exactly: 0 before WM_StartMP, 1 after.
//   +0x184 u16, the AID. src/func_020631dc.c:48 uses it as a SHIFT COUNT --
//          `bits = mask | (1 << flag5)` -- and branches on it being zero to
//          pick the parent path. Zero is the parent; a child's own aid is
//          whatever the connect answered.
//   +0x18e u16, MY max MP send size. src/func_02061b9c.c returns it rounded up
//          to 32, and src/func_020625fc.c refuses unless the ROM's own send
//          buffer is at least that. src/func_02062428.c:34 also refuses a send
//          longer than it.
//   +0x190 u16, THE OTHER SIDE's max MP data size, and +0xf2 the child count.
//          src/func_02061ab0.c computes the minimum receive buffer from them:
//          a child needs ((v + 0x51) & ~0x1f) << 1 and a parent needs
//          ((((v + 0xc) * children) + 0x29) & ~0x1f) << 1.
//
// AND THE ROM'S OWN ALLOCATION IS THE CORROBORATION. src/func_020408b0.c:44-47
// takes a 0x220 send buffer and a 0x480 receive buffer off the game heap, and
// the parent parameters at data_02075358 (copied into data_020a1064 by
// src/func_0203db64.c:57) are parentMaxSize 0x200 at +0x32 and childMaxSize
// 0x20 at +0x34. Put those numbers in the fields above and the child's minimum
// receive size comes out ((0x200 + 0x51) & ~0x1f) << 1 = 0x480 EXACTLY -- the
// ROM allocated precisely the SDK's own minimum for the parameters it ships.
// That is why these fields are filled from the parent parameter block the ROM
// itself sends with apiid 7 and from nothing else.
//
// ===========================================================================
// THE THREE LAWS THIS FILE OBEYS
// ===========================================================================
//
// 1. REPLIES ARE QUEUED, NEVER POSTED FROM INSIDE arm7_recv. ntr/ipc.cpp's
//    fifo_send hands the word to the host ARM7 INLINE on the ARM9's store, so
//    arm7_recv runs inside WM_SendCommand, inside IPCSend. If the reply were
//    posted there, ntr::ipc_arm7_send -> raise_rx_irq would dispatch
//    func_02061188 inside the send -- and the WM callback path SENDS THE NEXT
//    COMMAND FROM INSIDE THE CALLBACK, whose reply would then hit
//    raise_rx_irq's g_in_rx guard and be SILENTLY DROPPED. That is the exact
//    shape hal/boot2_ipc.cpp's channel-0xb note calls "THE WAIT CLOSES EARLY",
//    and for the radio it is fatal rather than survivable: one dropped reply
//    and the ROM's state machine waits forever. So a command is answered by
//    QUEUEING a reply, and the reply is posted from port_wm_arm7_turn(), one
//    per turn in arrival order, at a point where the ARM9 has yielded.
//
// 2. THE TRANSPORT IS THE RADIO. hal/comms_seam.h's CommsTransport is not a
//    peer of this file, it is what is UNDER it: become_parent() when the ROM
//    sends apiid 8, become_child() on 0xc, close() when a RESET (apiid 1)
//    arrives from a session state, and state()/slot() are what the status
//    block's state halfword and aid are FILLED FROM. The ROM's own callbacks
//    then write data_020a0f94 and data_020a0f24, which is where the DS keeps
//    them. WMSCOUT's transport table put close() on apiid 0x0d; the ROM says
//    otherwise, and the reason is at the apiid-1 arm below.
//
// 3. AN APIID THIS STUB CANNOT ANSWER IS A HARD FAULT NAMING IT. Never a
//    silent swallow: a swallowed command is a state machine that waits for
//    ever, which is the failure ntr/ipc.cpp's bounded-spin guard exists to
//    turn into an exit code. Same shape here, exit 24.
//
// WHAT THIS STUB DOES NOT DO, STATED RATHER THAN LEFT TO BE FOUND. It never
// posts a CONNECT INDICATION to a parent (apiid 8 with state code 7). On the
// DS that is how a parent learns a child joined, and src/func_02040014.c's
// case 7 answers it by looking the child's MAC up in data_020a0f9d /
// data_020a0fbe -- the lobby's accepted-peer table, which NOTHING in this
// build fills, so every lookup misses and the ROM answers a joining child with
// func_02061d30 (DISCONNECT). Fabricating the indication would therefore make
// the parent hang up on its own peer. The parent reaches MP_PARENT on its own
// (8 then 0xe) and the child reaches MP_CHILD on its own (0xc then 0xe);
// neither needs the other's indication, and the session's actual bytes are the
// transport's, exactly as they were before this rung. The connected-child
// bitmaps at +0x86 and +0x17e stay ZERO for the same reason and it is the
// truthful value: no child has completed the ROM's own accept handshake.
//
// AND THE SIXTEEN-PLAYER WIDTH IS UNAFFECTED, because it never passes through
// here. hal/vs_width.h's mod lives BELOW this protocol, in the transport and
// the fan-out tails; the ROM's WM session is the four-player one the cartridge
// shipped (its parent parameters say three children at +0x0e) and this stub
// answers for exactly those parameters and claims no limit the SDK would not.
// The ROM-visible session is four; the wide session the transport carries is
// still sixteen, and both are measured in this lane's report.

#include "ntr/ipc.h"
#include "comms_seam.h"
#include "dsstate_seg.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <ctime>

#if defined(_WIN32)
#include <windows.h>
#endif

// ---------------------------------------------------------------------------
// RUNG W4'S STORAGE: TWENTY DS GLOBALS THE ROM'S OWN WM CHAIN NAMES
//
// Measured with port/tools/closure.py over the 131 TUs of the rung before a
// line of this was written: twenty-four unresolved externals, of which twenty
// are these. EVERY SPAN IS THE ROM'S -- the distance to the next symbol in
// config/arm9/symbols.txt -- except where a body addresses PAST its own next
// symbol, which is called out at the line. That is the undersized-hosted-global
// trap hal/comms_seam.cpp's data_020a3fc0 note and hal/boot2_ipc.cpp's
// data_020a8138 note both name.
//
// THE RUN SORTS PAST EVERY EXISTING SUFFIX. ".dsstate$ywmd00" and up sort after
// lane WM2's ".dsstate$ywmc18" (fifth character, "c" < "d") and before the
// "$zzz" high sentinel, so the captured save-state span grows AT ITS TAIL and
// not one existing hosted global moves a byte. port/tools/dsstate_guard.py
// reads the low bound back.
//
// SIX OF THE TWENTY ARE INITIALISED DATA, NOT BSS. config/arm9/symbols.txt
// gives them kind:data and they live inside the loaded image, so their values
// were read out of extracted/arm9_dec.bin rather than assumed to be zero:
// data_02099e6c is the four bytes "rom\0", data_0209a064/68/6c and data_0209a074
// are -1, data_0209a070 is 1, and data_0209a7d8 is a twenty-byte constant. A
// zeroed stand-in for any of them would be a different program.
// ---------------------------------------------------------------------------

extern "C" {

#define WMBSS(sec, name, size, algn)                              \
    __pragma(section(sec, read, write))                           \
    extern "C" __declspec(allocate(sec)) __declspec(align(algn))  \
    unsigned char name[size]

// 0x02099e6c, 4. kind:data, "rom\0". src/func_020423dc.c:21 passes its address
// to func_0205d23c(&data_02099e6c, 3) -- a three-character name.
WMBSS(".dsstate$ywmd00", data_02099e6c, 4, 4) = { 'r', 'o', 'm', 0 };

// 0x0209a064..0x0209a078, five ints, kind:data. src/func_0206491c.c compares
// against the first three and src/func_02065538.c walks the fourth as an index;
// src/func_02065a74.c returns the fifth and src/func_02065a84.c writes it.
WMBSS(".dsstate$ywmd01", data_0209a064, 4, 4) = { 0xff, 0xff, 0xff, 0xff };
WMBSS(".dsstate$ywmd02", data_0209a068, 4, 4) = { 0xff, 0xff, 0xff, 0xff };
WMBSS(".dsstate$ywmd03", data_0209a06c, 4, 4) = { 0xff, 0xff, 0xff, 0xff };
WMBSS(".dsstate$ywmd04", data_0209a070, 4, 4) = { 0x01, 0x00, 0x00, 0x00 };
WMBSS(".dsstate$ywmd05", data_0209a074, 4, 4) = { 0xff, 0xff, 0xff, 0xff };

// 0x0209a7d8, 20, kind:data. src/func_0205db88.c:32 passes its address to
// func_0205de9c; the bytes are the ROM's.
WMBSS(".dsstate$ywmd06", data_0209a7d8, 20, 4) = {
    0x7a, 0x42, 0x80, 0x27, 0x13, 0xad, 0xc5, 0x9f, 0x06, 0xec,
    0xf7, 0xdb, 0x7e, 0xf7, 0xa1, 0x24, 0x99, 0xb3, 0xd3, 0xe7 };

// 0x020a0f20, 4. The tgid counter src/func_02040638.c bumps into the parent
// parameter block's +0x0a before each StartParent.
WMBSS(".dsstate$ywmd07", data_020a0f20, 4, 4) = { 0 };

// 0x020a0f5c, 4. THE ROLE REQUEST -- 3 parent, 4 child, 0 leave -- that
// src/func_02040820.c and src/func_02040790.c write and every WM callback in
// the chain switches on. Twelve of the rung's TUs read it.
WMBSS(".dsstate$ywmd08", data_020a0f5c, 4, 4) = { 0 };

// 0x020a0f84, 4. src/func_02040aa4.c's guard: the whole body is skipped while
// it is zero, and only rung W6's wireless thread ever sets it.
WMBSS(".dsstate$ywmd09", data_020a0f84, 4, 4) = { 0 };

// 0x020a0f9d (3) and 0x020a0fa0 (6) ARE ADJACENT ON THE DS AND HERE, align(1)
// on both so the linker inserts no padding. src/func_02040014.c:52-64 walks
// data_020a0f9d as u8[4] -- FOUR bytes over a three-byte span -- so its last
// entry IS data_020a0fa0[0], exactly the interleaved-table shape
// hal/boot2_ipc.cpp's data_020a8138/data_020a813c pair describes.
// port_wm_lobby_layout_check() below reads the adjacency back.
//
// AND ONE RESIDUAL, NAMED. On the DS data_020a0fa0 is the first entry of a
// FOUR-entry six-byte MAC table that runs to 0x020a0fb8, whose other three
// entries are data_020a0fa6 -- already hosted, eighteen bytes, in
// hal/comms_conductor.cpp's ".dsstate$ymp3c0000". So `data_020a0fa0[idx]` for
// idx > 0 addresses that neighbour on hardware and does not here. The ONE
// writer is src/func_02040014.c:59, and it is gated on some entry of
// data_020a0f9d being nonzero -- the lobby's accepted-peer flags, which nothing
// in this build writes, and which are zero in the storage right above. So the
// out-of-range store is unreachable rather than merely unlikely. Moving
// data_020a0fa6 out of the conductor's run to sit behind data_020a0fa0 would
// close it, and it would also shift every later global in that run, which is
// the one thing this rung's hosting is designed not to do.
WMBSS(".dsstate$ywmd10", data_020a0f9d, 3, 1) = { 0 };
WMBSS(".dsstate$ywmd11", data_020a0fa0, 6, 1) = { 0 };

// 0x020a11e4. ROM span to data_020a15e4 is 0x400, and src/func_020421b4.c
// addresses base + 0x400 and hands it to the OS thread-queue calls -- so the
// object really runs to the end of data_020a15e4's own 156-byte span. Hosted at
// 0x400 + 156 = 1180 for that reason, and nothing names data_020a15e4.
WMBSS(".dsstate$ywmd12", data_020a11e4, 1180, 4) = { 0 };

// 0x020a2400. ROM span to data_020a2404 is 4, and src/func_02040bb0.c:20-22
// takes its ADDRESS as a base and reaches +0x340 and +0x4c4. The next symbol
// after this block's four names is data_020a3fc0, so the block is 0x1bc0 bytes
// and that is what is hosted. data_020a2404/2408/2409 are names INSIDE it; no
// TU on this rung uses them, so they are not defined here -- a later lane that
// needs one must carve it out of this object rather than allocate beside it.
WMBSS(".dsstate$ywmd13", data_020a2400, 0x1bc0, 4) = { 0 };

// 0x020a94c0..0x020a94d4, five ints: the 0x02065xxx layer's own state.
WMBSS(".dsstate$ywmd14", data_020a94c0, 4, 4) = { 0 };
WMBSS(".dsstate$ywmd15", data_020a94c4, 4, 4) = { 0 };
WMBSS(".dsstate$ywmd16", data_020a94c8, 4, 4) = { 0 };
WMBSS(".dsstate$ywmd17", data_020a94cc, 4, 4) = { 0 };
WMBSS(".dsstate$ywmd18", data_020a94d0, 4, 4) = { 0 };

// 0x020a9570, 1976. src/func_02065af0.c copies 0x9c bytes into it.
WMBSS(".dsstate$ywmd19", data_020a9570, 1976, 4) = { 0 };

#undef WMBSS

// ---------------------------------------------------------------------------
// TWO NAMES, ONE ADDRESS. src/func_0205cd5c.c declares func_02059d1c and
// func_02059d30, and config/arm9/symbols.txt says 0x02059d1c IS
// _ZN3IRQ7DisableEv and 0x02059d30 IS _ZN3IRQ7RestoreEj -- the same two bodies
// nine other TUs on this rung call by their C++ names, already in the link.
// This is the house's "verify by ROM ADDRESS, never by filename" rule handing
// back a spelling, so the answer is a linker alias and not a forwarding body:
// no new code, no new call, the same address under both names.
// ---------------------------------------------------------------------------
#pragma comment(linker, "/alternatename:_func_02059d1c=__ZN3IRQ7DisableEv")
#pragma comment(linker, "/alternatename:_func_02059d30=__ZN3IRQ7RestoreEj")

// AND ONE SPELLING OF THE SAME KIND, ONE LEVEL UP. src/func_02062d10.cpp is one
// of the four C++ TUs on this rung, and it reaches CP15::InvalidateDataCache as
// a STATIC MEMBER, so MSVC emits ?InvalidateDataCache@CP15@@SAXII@Z while the
// port defines the body under its Itanium-ABI C name. Same address, same
// function, two manglings -- the exact shape hal/boot_hw.cpp already carries
// for CP15::FlushAndInvalidateDataCache, and the reason that file states there
// holds here: a forwarding body would be a second call on a cache primitive
// that is a no-op on the host, where an alias is nothing at all.
#pragma comment(linker, \
    "/alternatename:?InvalidateDataCache@CP15@@SAXII@Z=__ZN4CP1519InvalidateDataCacheEjj")

// ---------------------------------------------------------------------------
// THE ONE BODY ON THIS RUNG THAT IS NOT MATCHED CODE.
//
// src/func_0206470c.c carries a NONMATCHING banner: it is hand-written ARM
// assembly on a proven mwccarm register-allocation wall, it does not count as
// matched, and MSVC cannot compile it at all. src/func_02063980.c calls it, so
// the linker wants it. Hosted here, from the ROM's own instructions rather
// than from that file's C struct -- the two disagree, and the ROM wins: the asm
// reads max at +0x04, writes the result at +0x08, reads the cursor at +0x0c and
// the bitmap pointer at +0x14, while the header in that .c has them at +0, +4,
// +8 and +0x10. It is a wrapping search for the next CLEAR bit, -1 if every bit
// from the cursor round to the cursor again is set.
//
// It is UNREACHED in this build: its only caller is behind
// src/func_02065170.c's `data_020a94c0 == 0` early return, and data_020a94c0 is
// written by nothing this rung links. It is here so the link closes honestly,
// not because anything runs it.
// ---------------------------------------------------------------------------
int func_0206470c(void *self)
{
    unsigned char *o = (unsigned char *)self;
    const int max = *(const int *)(o + 0x04);
    const int cur = *(const int *)(o + 0x0c);
    const unsigned int *bits = *(const unsigned int **)(o + 0x14);
    if (max <= 0 || bits == 0) return -1;
    int i = cur + 1;
    if (i >= max) i = 0;
    const int start = i;
    for (;;) {
        if ((bits[i >> 5] & (1u << (i & 0x1f))) == 0) {
            *(int *)(o + 0x08) = i;
            return i;
        }
        ++i;
        if (i >= max) i = 0;
        if (i == start) return -1;
    }
}

// The work-buffer pointer src/WM_GetSystemWork.c returns, hosted by
// hal/comms_seam.cpp. Declared as its ROM span; the value is the 0x1300 buffer
// src/func_020408b0.c allocated and src/func_020616e8.c laid out.
extern unsigned char data_020a89ac[];

}  // extern "C"

// ===========================================================================
namespace {

// --- the ROM's own numbers -------------------------------------------------
enum : unsigned {
    kApiReset      = 0x01,   // src/func_02062200.c
    kApiEnd        = 0x02,   // src/func_020621b8.c (download play only)
    kApiEnable     = 0x03,   // src/func_02062380.c
    kApiDisable    = 0x04,   // src/func_02062338.c
    kApiPowerOn    = 0x05,   // src/func_020622f0.c
    kApiPowerOff   = 0x06,   // src/func_020622a8.c
    kApiSetPParam  = 0x07,   // src/func_020620b0.c
    kApiStartParent= 0x08,   // src/func_02061fdc.c
    kApiStartConn  = 0x0c,   // src/func_02061e38.c
    kApiDisconnect = 0x0d,   // src/func_02061d30.c
    kApiStartMP    = 0x0e,   // src/func_020625fc.c
    kApiSetMPData  = 0x0f,   // src/func_02062428.c
};

// The state halfword at work[1] + 0, as src/WM_CheckStateEx.c reads it.
enum : unsigned {
    kStReady = 0, kStStop = 1, kStIdle = 2,
    kStParent = 7, kStChild = 8, kStMpParent = 9, kStMpChild = 0x0a,
};

// The state CODE a reply carries. src/func_02040014.c switches on it for apiid
// 8 (0 = the parent came up), src/func_0203fec4.c for apiid 0xc (7 = connected,
// 9 = disconnected) and src/func_0203fdac.c for apiid 0xe (0xa = MP started).
enum : unsigned { kCodeStarted = 0, kCodeConnected = 7, kCodeMpStarted = 0x0a };

// A queued reply. `wait_state` is the CommsLinkState the transport has to reach
// before this one may be posted; kCommsIdle means "post on the next turn".
struct Pending {
    unsigned apiid;
    int      wait_state;     // 0 = ready now
    unsigned code;           // the state code the reply carries
    unsigned err;            // the errcode: 0 = it worked
    unsigned arg;            // apiid-specific: the 0x0f handler, or 0
    unsigned len;            // apiid-specific: the 0x0f length, or 0
    unsigned port;           // apiid-specific: the 0x0f port index, or 0
    unsigned long queued_turn;
    unsigned long queued_ms;
};

enum : unsigned { kQueueMax = 16 };
Pending  g_q[kQueueMax];
unsigned g_qn = 0;

// THE COUNTER TRIPLE the rung is gated on, plus the per-apiid census.
unsigned long g_cmds[0x30];          // commands received, by apiid
unsigned long g_replies[0x30];       // replies posted, by apiid
unsigned long g_cmd_total;
unsigned long g_posted_total;
unsigned long g_dispatched_total;    // posts the ROM's own handler consumed
unsigned long g_turns;               // ARM7 turns taken
unsigned long g_held_turns;          // turns where the head was not ready yet
unsigned long g_max_wait_turns;      // worst per-command wait, in turns
unsigned long g_wait_turns[0x30];    // worst wait per apiid, in turns
unsigned long g_unanswered;          // apiids this stub has no answer for
unsigned long g_timed_out;           // replies posted as a failure, see below

// THE BACKSTOP, AND WHOSE DEADLINE IT IS NOT. Two waits look alike here and
// only one of them is this file's.
//
//   THE PROTOCOL WAIT is the one RISK 2 is about: once the radio is up, a reply
//   must reach the ROM within a few ARM7 turns or the SDK's own timeouts fire.
//   That one is MEASURED, per apiid, in g_wait_turns, and it comes out in ones
//   and twos because a turn posts a reply the moment its precondition holds.
//
//   THE SESSION-FORMING WAIT is the parent sitting with no child yet. That is
//   NOT a protocol timeout and it is not this file's deadline: it belongs to
//   hal/comms_conductor.cpp's seat wait (600 turns, x6 when more than two are
//   expected -- about fifteen seconds, sized for a three-second launcher
//   stagger at sixteen windows) and to src/func_0203ea5c.c's own 1200-turn
//   bound, and when either expires the ROM drops to solo, which is a documented
//   outcome and not a fault. An earlier draft of this file bounded the wait in
//   ARM7 TURNS at 1200 and hard-faulted; on a sixteen-window ladder that is
//   about five seconds and it would have killed a legitimate run.
//
// So the backstop is WALL TIME and it is set well past every deadline above:
// thirty seconds, by which point the conductor has long since given up. On
// expiry the reply is posted with a NONZERO ERRCODE rather than held or
// exited -- which is the ROM's own radio-failed path (every callback in the
// chain answers a nonzero errcode by writing data_020a0f94 = 1 and giving up),
// so the game ends up exactly where the pre-rung host face left it.
enum : unsigned long { kWaitBoundMs = 30000 };

unsigned long now_ms()
{
#if defined(_WIN32)
    return (unsigned long)GetTickCount();
#else
    return (unsigned long)(clock() * 1000 / CLOCKS_PER_SEC);
#endif
}

bool g_faulted = false;
bool g_layout_said = false;

// THE REPLY MESSAGE, STAGED IN THE IMAGE, and the reason is arithmetic rather
// than taste. src/IPCSend.c packs the payload into a 26-BIT bitfield (`c : 26`),
// which on the DS is no constraint at all -- main RAM is 0x02000000..0x023fffff
// and every pointer fits. The port's game heap is hal/os_arena.cpp's arena at
// 0x30000000, and work[4] is an allocation out of it, so posting work[4]'s
// address down the FIFO would truncate it and hand src/func_02061188.c a
// pointer into nowhere. The staged buffer lives in the image, well under
// 0x04000000, so the word the ARM9 reads is the address the ARM7 wrote.
//
// AND THE ROM'S OWN REGION IS STILL FILLED: the same bytes are copied into
// work[4] after the message is built, so data_020a89ac[4] holds what it holds
// on hardware for anything that reads it there. The check at the head of
// post_reply refuses rather than truncates if the staging address ever grows
// past 26 bits.
__declspec(align(32)) unsigned char g_reply[0x100];

// ---- the work buffer and the status block ---------------------------------
unsigned int work_base() { return *(const unsigned int *)data_020a89ac; }

unsigned char *status_block()
{
    const unsigned int w = work_base();
    if (!w) return nullptr;
    return (unsigned char *)(uintptr_t)((const unsigned int *)(uintptr_t)w)[1];
}

unsigned char *reply_region()
{
    const unsigned int w = work_base();
    if (!w) return nullptr;
    return (unsigned char *)(uintptr_t)((const unsigned int *)(uintptr_t)w)[4];
}

void st_set16(unsigned off, unsigned v)
{
    unsigned char *s = status_block();
    if (s) *(volatile unsigned short *)(s + off) = (unsigned short)v;
}

unsigned st_get16(unsigned off)
{
    const unsigned char *s = status_block();
    return s ? *(const volatile unsigned short *)(s + off) : 0u;
}

void set_wm_state(unsigned v)
{
    st_set16(0x00, v);
}

// ---- the parameters the ROM sent with apiid 7 -----------------------------
// Captured from the block src/func_020620b0.c passes, which is the ROM's own
// data_020a0f44 -- a copy of data_020a1064, itself a copy of the cartridge's
// parent parameter table at 0x02075358. Read at +0x0e, +0x30, +0x32 and +0x34,
// the four fields src/func_02062024.c validates.
unsigned g_par_children = 3;      // +0x0e, the cartridge ships 3
unsigned g_par_parent_size = 0x200;  // +0x32
unsigned g_par_child_size  = 0x20;   // +0x34
bool     g_par_seen = false;

void capture_parent_params(const unsigned char *o)
{
    if (!o) return;
    const unsigned e = *(const unsigned short *)(o + 0x0e);
    const unsigned n = *(const unsigned short *)(o + 0x30);
    const unsigned p = *(const unsigned short *)(o + 0x32);
    const unsigned c = *(const unsigned short *)(o + 0x34);
    // src/func_02062024.c reads +0x0e as the frame-budget multiplier and +0x30
    // as the entry count in 1..0xe. src/func_020408b0.c:48 overwrites +0x30 of
    // the template with data_020a0f28, which is 0 on a cartridge boot, so +0x0e
    // is the one that carries the cartridge's three children.
    unsigned children = e ? e : n;
    if (children < 1)  children = 1;
    if (children > 15) children = 15;
    g_par_children = children;
    if (p) g_par_parent_size = p;
    if (c) g_par_child_size  = c;
    g_par_seen = true;
}

// Fill the negotiated sizes for the role we are about to take. `am_parent`
// picks which side's size is "mine" and which is "theirs"; see the header note
// for why the ROM's own 0x220 and 0x480 allocations are exactly these numbers.
void publish_sizes(bool am_parent)
{
    st_set16(0xf2,  g_par_children);
    st_set16(0x18e, am_parent ? g_par_parent_size : g_par_child_size);
    st_set16(0x190, am_parent ? g_par_child_size  : g_par_parent_size);
    // src/func_02061188.c reads +0x46 as the length to invalidate for an MP
    // receive. Never reached on this rung (its arm needs state code 0xb or
    // 0xc), and the honest value if it ever is is the receive size.
    st_set16(0x46, am_parent
                   ? (unsigned)(g_par_child_size * g_par_children)
                   : g_par_parent_size);
}

// ---- the census -----------------------------------------------------------
const char *api_name(unsigned a)
{
    switch (a) {
    case kApiReset:       return "reset        (src/func_02062200.c)";
    case kApiEnd:         return "end          (src/func_020621b8.c)";
    case kApiEnable:      return "enable       (src/func_02062380.c)";
    case kApiDisable:     return "disable      (src/func_02062338.c)";
    case kApiPowerOn:     return "power on     (src/func_020622f0.c)";
    case kApiPowerOff:    return "power off    (src/func_020622a8.c)";
    case kApiSetPParam:   return "set p param  (src/func_020620b0.c)";
    case kApiStartParent: return "start parent (src/func_02061fdc.c)";
    case kApiStartConn:   return "start connect(src/func_02061e38.c)";
    case kApiDisconnect:  return "disconnect   (src/func_02061d30.c)";
    case kApiStartMP:     return "start mp     (src/func_020625fc.c)";
    case kApiSetMPData:   return "set mp data  (src/func_02062428.c)";
    default:              return "UNKNOWN";
    }
}

void fault(const char *why, unsigned apiid)
{
    if (g_faulted) return;
    g_faulted = true;
    std::fprintf(stderr,
        "[wm7] HARD FAULT: %s -- apiid 0x%02x (%s). The host ARM7 has no "
        "answer for it, and a swallowed WM command is a ROM state machine that "
        "waits for ever. Refusing loudly instead. Rung W4 answers apiids 01 03 "
        "04 05 06 07 08 0c 0d 0e 0f; anything else belongs to a rung that has "
        "not been taken (0x00/0x02/0x0a/0x18/0x19/0x1d/0x1e are Download "
        "Play's, W7).\n", why, apiid, api_name(apiid));
    std::fflush(stderr);
    std::_Exit(24);
}

// ---- posting --------------------------------------------------------------
void post_reply(const Pending &p)
{
    std::memset(g_reply, 0, sizeof g_reply);
    unsigned short *m = (unsigned short *)g_reply;

    m[0] = (unsigned short)p.apiid;
    m[1] = (unsigned short)p.err;               // errcode: every reader's gate

    switch (p.apiid) {
    case kApiStartParent:
        // src/func_02040014.c: state code at +8, six MAC bytes at +0xa, aid at
        // +0x10. The MAC is a locally administered stand-in -- the host has no
        // DS radio -- and on the code this rung posts (0, "the parent came up")
        // src/func_02040014.c does not read it at all.
        m[4] = (unsigned short)p.code;
        g_reply[0x0a] = 0x02;    // locally administered, the rest zero
        m[8] = 0;                // the parent is always aid 0
        break;
    case kApiStartConn:
        // src/func_0203fec4.c: state code at msg[4], the aid at msg[5].
        m[4] = (unsigned short)p.code;
        m[5] = (unsigned short)p.arg;
        break;
    case kApiStartMP:
        // src/func_0203fdac.c reads the state code at msg[2], not msg[4].
        m[2] = (unsigned short)p.code;
        break;
    case kApiSetMPData:
        // Posted as src/func_02061188.c's 0x81 indication: it rewrites msg[0]
        // to 0x0f and calls the handler stored at byte 0x14 of the message.
        // src/func_02062d10.cpp then reads the errcode at +2 (0 = sent, 0x0a =
        // the queue was full), the port index at +0xa and the length at +0x1c.
        m[0] = 0x81;
        m[5] = (unsigned short)p.port;
        *(unsigned int *)(g_reply + 0x14) = p.arg;
        *(unsigned int *)(g_reply + 0x1c) = p.len;
        break;
    case kApiReset: case kApiEnable: case kApiDisable:
    case kApiPowerOn: case kApiPowerOff: case kApiSetPParam:
    case kApiDisconnect:
        // Nothing beyond the apiid and the errcode: the ROM callback for each
        // of these reads msg[1] and nothing else. Named rather than left to
        // `default` so a future apiid cannot land here by accident.
        break;
    default:
        break;
    }

    // The DS's own reply region gets the same bytes, so anything that reads
    // work[4] sees what it sees on hardware. The FIFO carries the staged
    // address for the 26-bit reason in the note above.
    unsigned char *region = reply_region();
    if (region) std::memcpy(region, g_reply, sizeof g_reply);

    const uintptr_t addr = (uintptr_t)(void *)g_reply;
    if (addr >> 26) {
        std::fprintf(stderr,
            "[wm7] HARD FAULT: the staged reply is at 0x%p and src/IPCSend.c's "
            "payload is 26 bits, so the ARM9 cannot be told where it is.\n",
            (void *)g_reply);
        std::fflush(stderr);
        std::_Exit(24);
    }

    const ntr::IpcCounters before = ntr::ipc_counters();
    ++g_posted_total;
    ++g_replies[p.apiid & 0x2f];
    if (ntr::ipc_log_on())
        std::fprintf(stderr, "[wm7] reply apiid 0x%02x code %u -> the ARM9\n",
                     p.apiid, p.code);
    ntr::ipc_arm7_send(ntr::ipc_word(0xa, (uint32_t)addr, 0));
    const ntr::IpcCounters after = ntr::ipc_counters();

    // DISPATCHED, and what that word is allowed to mean here. The ROM's own
    // func_02061188 cannot be instrumented, so this counts the two things that
    // are observable from outside it: the ROM's receive handler ran
    // (rx_dispatches advanced) and it did NOT hand the word back with the
    // no-handler flag (naks did not advance), which is exactly what it does
    // when data_020a7fc8[0xa] holds no callback. src/func_020616e8.c's closing
    // func_0205ba64(0xa, func_02061188) is what put one there.
    if (after.rx_dispatches > before.rx_dispatches &&
        after.naks == before.naks)
        ++g_dispatched_total;
}

bool transport_ready(int want)
{
    if (want == 0) return true;
    const port::CommsTransport *t = port::comms_transport();
    if (!t) return false;
    return t->state() == want;
}

void queue(unsigned apiid, int wait_state, unsigned code, unsigned err = 0,
           unsigned arg = 0, unsigned len = 0, unsigned prt = 0)
{
    if (g_qn >= kQueueMax) {
        std::fprintf(stderr, "[wm7] HARD FAULT: the reply queue is full (%u) "
                     "with apiid 0x%02x arriving. The ARM9 is sending faster "
                     "than it is being given turns.\n", kQueueMax, apiid);
        std::fflush(stderr);
        std::_Exit(24);
    }
    Pending &p = g_q[g_qn++];
    p.apiid = apiid;
    p.wait_state = wait_state;
    p.code = code;
    p.err = err;
    p.arg = arg;
    p.len = len;
    p.port = prt;
    p.queued_turn = g_turns;
    p.queued_ms = now_ms();
}

void pop_front()
{
    for (unsigned i = 1; i < g_qn; ++i) g_q[i - 1] = g_q[i];
    if (g_qn) --g_qn;
}

}  // namespace

// ===========================================================================
// THE COMMAND ARRIVES.  Called from hal/boot2_ipc.cpp's arm7_recv for tag 10,
// INSIDE the ARM9's store into IPCFIFOSEND. Nothing is posted from here; see
// law 1 at the top of the file.
// ===========================================================================
extern "C" void port_wm_arm7_command(uint32_t word)
{
    const uint32_t cmdaddr = ntr::ipc_data(word);
    if (!cmdaddr) { fault("the command word carries no buffer address", 0); return; }

    const unsigned short *c16 = (const unsigned short *)(uintptr_t)cmdaddr;
    const unsigned int   *c32 = (const unsigned int *)(uintptr_t)cmdaddr;
    const unsigned apiid = c16[0];

    ++g_cmd_total;
    if (apiid < 0x30) ++g_cmds[apiid];
    if (ntr::ipc_log_on())
        std::fprintf(stderr, "[wm7] command apiid 0x%02x %s (buffer 0x%08x, "
                     "state %u)\n", apiid, api_name(apiid),
                     (unsigned)cmdaddr, st_get16(0));

    const port::CommsTransport *t = port::comms_transport();

    switch (apiid) {
    case kApiReset: {
        // src/func_020614d0.c refuses below state 2, and the ROM's own reset
        // callback (src/func_020402a0.c) then re-issues SetParentParameter or
        // StartConnect, both of which require state 2. So reset lands on IDLE.
        //
        // AND THIS IS WHERE close() LIVES, which is not where WMSCOUT's table
        // guessed. That table put close() on apiid 0x0d (DISCONNECT), and 0x0d
        // is not what leaving sends: src/func_02040724.c calls Wireless_Reset,
        // which is src/func_02062200.c, which is APIID 1. 0x0d is sent from one
        // place only -- src/func_02040014.c:78, a parent hanging up on a child
        // whose MAC it does not recognise.
        //
        // A RESET FROM A SESSION STATE IS THE SESSION ENDING, and a reset from
        // below one is bring-up. src/func_02040820.c's case-4 arm, every
        // callback's failure arm and the leave path all funnel here, and only
        // the ones that had a session need the transport told. So: state 7, 8,
        // 9 or 0xa means the radio was carrying something and close() is owed;
        // anything lower is the ROM resetting a radio that never carried
        // anything, and closing there would tear down a session the ROM is in
        // the middle of building.
        //
        // WHY IT MATTERS, MEASURED. hal/comms_loopback.cpp:4555 registers
        // `std::atexit([]() { func_02040724(); })` so a window says goodbye on
        // the way out and its peers do not stall for a full wait bound. Before
        // this rung that face called t->close() itself. Now it is the ROM's own
        // body and it sends apiid 1 instead -- so without this line the goodbye
        // reaches the ARM7 and stops there. It is called from the COMMAND, not
        // from the reply, for exactly that reason: at exit nothing turns again,
        // so a reply-side close would never happen.
        const unsigned s = st_get16(0);
        if (t && (s == kStParent || s == kStChild ||
                  s == kStMpParent || s == kStMpChild)) {
            if (ntr::ipc_log_on())
                std::fprintf(stderr, "[wm7] reset from state %u: the session is "
                             "over, closing the transport\n", s);
            t->close();
        }
        set_wm_state(kStIdle);
        st_set16(0x10, 0);          // MP is not running any more
        st_set16(0x184, 0);
        st_set16(0x86, 0);
        st_set16(0x17e, 0);
        queue(apiid, 0, 0);
        break;
    }

    case kApiEnable:
        set_wm_state(kStStop);
        queue(apiid, 0, 0);
        break;

    case kApiDisable:
        set_wm_state(kStReady);
        queue(apiid, 0, 0);
        break;

    case kApiPowerOn:
        set_wm_state(kStIdle);
        queue(apiid, 0, 0);
        break;

    case kApiPowerOff:
        set_wm_state(kStStop);
        queue(apiid, 0, 0);
        break;

    case kApiSetPParam:
        // src/func_020620b0.c sends the parameter block pointer as its one
        // argument. Read it -- this is where the ARM7 learns what session the
        // ROM is asking for -- and answer from state 2, where it started.
        capture_parent_params((const unsigned char *)(uintptr_t)c32[1]);
        queue(apiid, 0, 0);
        break;

    case kApiStartParent:
        // THE TRANSPORT IS THE RADIO. The reply waits until it says the parent
        // side is up, and the state halfword becomes PARENT at that moment --
        // src/func_02040014.c answers this reply with WM_StartMP, which
        // requires 7 or 8 already. NO TRANSPORT MEANS NO RADIO, and the honest
        // answer to "start the parent" with no radio is a failure, which is
        // what the retired host face amounted to as well ("the state never
        // leaves idle, so the ROM's own wait bound expires").
        if (!t) { queue(apiid, 0, kCodeStarted, 1); break; }
        t->become_parent();
        queue(apiid, port::kCommsParentConnected, kCodeStarted);
        break;

    case kApiStartConn:
        if (!t) { queue(apiid, 0, kCodeConnected, 1); break; }
        t->become_child();
        queue(apiid, port::kCommsChildConnected, kCodeConnected);
        break;

    case kApiDisconnect:
        // src/func_02061d30.c sends the child's bit as its one argument. No
        // child ever entered this session (see the header note), so there is
        // nothing to drop; the ROM's callback table entry for 0x0d is null on
        // the one path that sends it, so this reply is dispatched and ignored.
        st_set16(0x17e, st_get16(0x17e) & ~(unsigned)c32[1]);
        st_set16(0x86,  st_get16(0x86)  & ~(unsigned)c32[1]);
        queue(apiid, 0, 0);
        break;

    case kApiStartMP: {
        const unsigned s = st_get16(0);
        if (s != kStParent && s != kStChild) {
            fault("start mp from a state that is neither parent nor child", apiid);
            break;
        }
        const bool am_parent = (s == kStParent);
        publish_sizes(am_parent);
        st_set16(0x10, 1);                       // MP is running
        set_wm_state(am_parent ? kStMpParent : kStMpChild);
        queue(apiid, 0, kCodeMpStarted);
        break;
    }

    case kApiSetMPData:
        // src/func_02062428.c's WM_SendCommand(0xf, 7, buf, len, ...) puts the
        // buffer at word 1, the length at word 2, the port index at word 4 and
        // the completion handler at word 6. The reply is the 0x81 indication.
        queue(apiid, 0, 0, 0, c32[6], c32[2], c32[4]);
        break;

    default:
        ++g_unanswered;
        fault("no answer for this apiid on rung W4", apiid);
        break;
    }
}

// ===========================================================================
// THE ARM7'S TURN.  One reply, in arrival order, from a point where the ARM9
// has yielded -- hal/comms_conductor.cpp's seat wait and its pump, and
// hal/comms_seam.cpp's remaining exchange face.
// ===========================================================================
extern "C" void port_wm_arm7_turn(void)
{
    ++g_turns;

    if (!g_layout_said) {
        g_layout_said = true;
        if ((data_020a0fa0 - data_020a0f9d) != 3)
            std::fprintf(stderr,
                "[wm7] LOBBY TABLE LAYOUT BROKEN: data_020a0fa0 is not at "
                "data_020a0f9d + 3, so src/func_02040014.c's four-entry walk "
                "over a three-byte span would read past its object.\n");
    }

    if (!g_qn) return;

    Pending &p = g_q[0];
    if (p.err == 0 && !transport_ready(p.wait_state)) {
        ++g_held_turns;
        if (now_ms() - p.queued_ms < kWaitBoundMs) return;
        // THE BACKSTOP FIRED. Not a fault and not a hang: the radio never came
        // up, so the reply says so, and the ROM's own callbacks answer a
        // nonzero errcode by writing data_020a0f94 = 1 and giving up -- which
        // is where the pre-rung host face left the game too. See kWaitBoundMs.
        ++g_timed_out;
        std::fprintf(stderr,
            "[wm7] apiid 0x%02x waited %lu ms (%lu turns) for the transport to "
            "reach state %d and it never did. Answering with a failure, which "
            "is the ROM's own radio-failed path.\n",
            p.apiid, now_ms() - p.queued_ms, g_turns - p.queued_turn,
            p.wait_state);
        std::fflush(stderr);
        p.err = 1;
    }

    // The state halfword only becomes PARENT/CHILD when the radio really is,
    // which is the moment the reply is postable and not the moment it was
    // asked for. A failed reply changes no state at all.
    if (p.err != 0) {
        /* nothing to publish */
    } else if (p.apiid == kApiStartParent) {
        set_wm_state(kStParent);
        st_set16(0x184, 0);
        publish_sizes(true);
    } else if (p.apiid == kApiStartConn) {
        const port::CommsTransport *t = port::comms_transport();
        const unsigned aid = t ? (unsigned)t->slot() : 0u;
        set_wm_state(kStChild);
        st_set16(0x184, aid);
        publish_sizes(false);
        p.arg = aid;
    }

    const unsigned long waited = g_turns - p.queued_turn;
    if (waited > g_max_wait_turns) g_max_wait_turns = waited;
    if (p.apiid < 0x30 && waited > g_wait_turns[p.apiid])
        g_wait_turns[p.apiid] = waited;

    const Pending copy = p;
    pop_front();
    post_reply(copy);
}

// ===========================================================================
// THE CENSUS.  hal/boot2_ipc.cpp's exit report takes this beside its per-tag
// tally. THE THREE NUMBERS THAT MUST AGREE are commands, replies posted and
// replies the ROM's own dispatcher consumed: a gap between the first two is a
// command this stub swallowed, and a gap between the second and third is the
// nested-dispatch drop law 1 exists to prevent.
// ===========================================================================
extern "C" void port_wm_arm7_census(void)
{
    std::fprintf(stderr,
        "[wm7:census] commands %lu, replies posted %lu, replies dispatched "
        "%lu, unanswered %lu\n",
        g_cmd_total, g_posted_total, g_dispatched_total, g_unanswered);
    for (unsigned a = 0; a < 0x30; ++a) {
        if (!g_cmds[a] && !g_replies[a]) continue;
        std::fprintf(stderr, "[wm7:census] apiid 0x%02x  %-30s %lu command(s), "
                     "%lu reply(ies), worst wait %lu turn(s)\n",
                     a, api_name(a), g_cmds[a], g_replies[a], g_wait_turns[a]);
    }
    std::fprintf(stderr,
        "[wm7:census] turns %lu (%lu held for the radio, worst wait %lu, %lu "
        "answered as a failure), final state halfword %u, aid %u, queue depth "
        "%u\n",
        g_turns, g_held_turns, g_max_wait_turns, g_timed_out, st_get16(0),
        st_get16(0x184), g_qn);
    std::fflush(stderr);
}

// Whether this stub has been asked for anything at all. hal/boot2_ipc.cpp uses
// it to keep a solo run's log as quiet as it was before this rung.
extern "C" int port_wm_arm7_saw_traffic(void) { return g_cmd_total != 0; }
