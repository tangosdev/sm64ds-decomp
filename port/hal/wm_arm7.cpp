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
// THE CONNECTED-CHILD BITMAP, AND WHY IT IS THE ARM7'S TO WRITE (run link100,
// lane WM4, gate A). Rung W4 left +0x86 and +0x17e at zero and called that the
// truthful value. It was truthful about the ROM's LOBBY handshake and wrong
// about the RADIO, and the difference is what gate A settles.
//
// Two different facts wear the same word on the DS:
//
//   THE ASSOCIATION. Which AIDs are associated with this parent right now.
//   That is 802.11 state and it lives in arm7.bin: the ARM9 never computes it,
//   it READS it out of the status block the ARM7 writes. Three ROM bodies read
//   it and each says what it is for:
//     src/func_02062428.c:31-36  a parent refuses WM_SetMPData with error 7
//                                while *(u16 *)(status + 0x17e) is zero -- "I
//                                have nobody to send to".
//     src/func_020627e8.c:31     takes *(u16 *)(status + 0x86) as `mask` and
//                                sends with `H(0x40e) & mask` -- the recipient
//                                set of an MP frame.
//     src/func_02062df0.c:43-44  and src/func_020631dc.c:50-51 read the same
//                                +0x86 into f86 and build the frame's own
//                                participant word from `H(0x40e) & (f86 | 1)`.
//   In the port the association IS the transport: a peer that has joined the
//   carrier is a peer the radio is holding a link to. So the bitmap is filled
//   from the transport, from the ARM7's turn, and it is not a fabrication --
//   it is the one field on this rung whose value only the radio knows.
//
//   THE ACCEPTANCE. Which peers the game's own lobby agreed to play with.
//   That is ARM9 state, it lives in data_020a0f9d / data_020a0fbe, and it is
//   NOT what any of the three readers above touch.
//
// SO THE CONNECT INDICATION IS STILL NOT POSTED, and now for a narrower reason
// than W4's. On the DS the ARM7 announces a join with apiid 8 carrying state
// code 7, and src/func_02040014.c's case 7 answers it by looking the joining
// child's six MAC bytes up in data_020a0f9d / data_020a0fbe -- the lobby's
// accepted-peer table, which nothing in this build fills, so every lookup
// misses and the ROM hangs up on its own peer with func_02061d30. Filling that
// table would mean the port inventing a lobby decision the player never made,
// and it would also make src/func_02040014.c:65's
// `CpuCopy8(buf, data_020a0fa0[idx], 6)` REACHABLE -- a write at
// data_020a0fa0 + aid*6, into a six-byte host object whose ROM neighbours
// data_020a0fa6..0fd0 are hosted in a different section run (the residual
// named at data_020a0fa0's own line below). The whole of that arm's effect on
// an ACCEPTED child is to record its MAC in that table and return; nothing
// else in the linked tree reads it. Zero gain, two real hazards: not taken,
// and named in this lane's report.
//
// AND THE SIXTEEN-PLAYER WIDTH IS UNAFFECTED, because it never passes through
// here. hal/vs_width.h's mod lives BELOW this protocol, in the transport and
// the fan-out tails; the ROM's WM session is the four-player one the cartridge
// shipped (its parent parameters say three children at +0x0e) and this stub
// answers for exactly those parameters and claims no limit the SDK would not.
// The ROM-visible session is four; the wide session the transport carries is
// still sixteen, and both are measured in this lane's report.
//
// ===========================================================================
// THE PORT SPLIT, AND WHY THE 0x82 PORT-RECEIVE INDICATION IS STILL UNPOSTED
// Run link100, lane WM9, rung W8.
// ===========================================================================
//
// Rung W7 (lane WM8) closed with one sentence: everything under
// src/func_020417d0.c is linked, correctly addressed, and waiting on an 0x82
// port-receive indication the host ARM7 never posts. This rung went to post
// it. It did not, and the reason is a fact about the cartridge that no lane
// had read yet: THE HOST'S DATAGRAMS AND src/func_020417d0.c ARE ON DIFFERENT
// WM PORTS.
//
// THE 0x82 RECORD, FIELD BY FIELD, off the two bodies that read one. Whoever
// takes this next needs no SDK header for it; both readings agree to the byte.
// src/func_02061188.c's own BufT and its 0x82 arm fix six of the fields and
// src/func_020417d0.c and src/func_02062bdc.cpp read the rest:
//
//   +0x00  u16  0x82, the apiid src/func_02061188.c dispatches on
//   +0x02  u16  the errcode. Both readers refuse everything but zero
//               (src/func_02062bdc.cpp `if (m->f2 == 0)`, else state = 3)
//   +0x04  u16  THE STATE CODE. 7 connected, 9 disconnected, 0x15 port
//               receive -- src/func_020417d0.c cases 7/9/0x15 and
//               src/func_02062bdc.cpp's three arms, independently
//   +0x06  u16  THE PORT. src/func_02061188.c indexes work[0x2b + msg[3]] for
//               the callback and work[0x3b + msg[3]] for its argument, which
//               is src/func_02061c88.c's `WM_GetSystemWork() + idx*4 + 0xac`
//               and `+ 0xec` exactly
//   +0x08  u32  the whole receive buffer. src/func_02061188.c invalidates it
//               for *(u16 *)(status + 0x46) bytes before dispatching
//   +0x0c  u32  THE DATA. src/func_020417d0.c reads it as r0 on case 0x15;
//               src/func_02062bdc.cpp passes it to func_02062990
//   +0x10  u16  THE LENGTH (src/func_020417d0.c's arg2;
//               src/func_02062aa4.cpp compares it with self+0x414)
//   +0x12  u16  THE SENDER'S AID (the shift count in both readers)
//   +0x14  6    the sender's MAC, copied by src/func_02061188.c's own
//               synthesised connect/disconnect record
//   +0x1a  u16  the sequence number (src/func_02062aa4.cpp stores it >> 1)
//   +0x1c  u32  the port's registered argument. THE ARM7 DOES NOT WRITE THIS:
//               src/func_02061188.c overwrites it from work[0x3b + port]
//               before every dispatch
//
// AND THE CONNECT AND DISCONNECT RECORDS ARE NOT THE ARM7'S AT ALL. States 7
// and 9 are SYNTHESISED BY THE ARM9: src/func_02061188.c's apiid-8 and
// apiid-0xc arms build data_020a89ec themselves when the reply's state code is
// 7 or 9 and hand it to all sixteen port callbacks in a loop. So a hosted ARM7
// that wants those two posts an ordinary apiid 8 or 0xc reply and the ROM does
// the rest; only 0x15 is a message the ARM7 composes.
//
// WHICH PORT IS WHOSE. Exactly two ports are registered in this image, by two
// different bodies, for two different protocols:
//
//   PORT 1  -- src/func_02040c34.c:58 `func_02061c88((u16)a1, func_020417d0, 0)`
//              with a1 = 1 from src/func_0203ea5c.c:209/212. That is the WFS,
//              the wireless file system: its datagrams are built by
//              src/func_02065538.c (parent) or src/func_020653cc.c (child)
//              into data_020a1fc0 + 0x40 and sent by src/func_02041930.c with
//              `func_020623ec(func_020418f0, sub, r, 0xffff, *(u16 *)(g+0x30),
//              3)`, where g+0x30 is the same 1. Its receive parser is
//              src/func_02063ea8.c and its wake is func_020412f0.
//
//   PORT 0xc -- src/func_020631dc.c:96/:123 register src/func_02062bdc.cpp
//              (parent) and src/func_02062aa4.cpp (child), and
//              src/func_0203fa50.c:7 passes unitIdx 0xc with mask 0xf and
//              elemSize 0x20. THAT is the game's own MP unit -- the 0x20-byte
//              comms block, four slots -- and it is what the host carrier
//              moves. func_020406b4 -> func_02062df0 sends it and
//              func_0204068c -> func_02062778 reads it back.
//
// SO THE BRIEF'S CHANGE SPLITS IN TWO, and neither half is available on this
// rung:
//
//   (1) AN 0x82 BUILT FROM THE CARRIER'S DATAGRAMS BELONGS ON PORT 0xc, not on
//       port 1. It would reach src/func_02062bdc.cpp / src/func_02062aa4.cpp
//       and never src/func_020417d0.c. That is a real rung and it is the "one
//       path, not two" the port wants -- the ROM's own func_02062990 banks the
//       peer's block into the MP unit's four-deep ring at the offset
//       src/func_02062734.c computes, func_020627e8 advances the ring and
//       re-sends, and src/func_02062df0.c's own MultiCopyHalf then fills
//       data_020a0f80, retiring port_wm_publish_mp_recv below. It is also a
//       rung that rewrites the live VS timing (func_020627e8 SENDS from inside
//       the receive), so it wants its own lane, its own pair and its own
//       ladder, not a corner of this one.
//
//   (2) AN 0x82 ON PORT 1 NEEDS WFS BYTES, AND THERE IS NO CARRIER FOR THEM.
//       The ROM's WFS sender already runs on this host -- the census below
//       counts its commands -- and this rung is what made that visible: the
//       ARM7 was answering WM_SetMPData with the 0x81 completion and dropping
//       the payload unread. Delivering it to the other window needs a channel,
//       and the only one the seam has is contract v2's send_aux/recv_aux, which
//       is classified by a four-byte kind tag in hal/comms_loopback.cpp
//       (kAuxKinds = 4, and lb_recv_aux SKIPS the voice kind so two consumers
//       can share one socket). A WFS kind is one more entry in that enum and a
//       reader beside lb_recv_voice -- in hal/comms_loopback.cpp, which this
//       lane does not own. Reported as a hunk rather than made.
//
// WHAT WOULD BE A FABRICATION, so that it is refused in writing rather than
// discovered later: posting the carrier's 0x20-byte game block on port 1.
// src/func_02063ea8.c would then parse the game's flag word as a WFS message
// type and its stylus bytes as a recipient mask, and on a parent
// (data_0209a074 == 0 after src/func_020652fc.c) a set bit 0 takes it into the
// arms that call func_02041ce0 / func_02041c64 and dereference what comes
// back. That is not the ROM's receive path running; it is the ROM's receive
// path fed another protocol's bytes.

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

// 0x020a0f9d (3) and 0x020a0fa0 (6), align(1) on both, AND NOTHING IN THE
// LINKED TREE REQUIRES THEM TO BE ADJACENT. The note that stood here said
// src/func_02040014.c walks data_020a0f9d as u8[4] -- four bytes over a
// three-byte span -- so that its last entry had to be data_020a0fa0[0]. That
// reading was wrong, and Andrew's third review of PR #2474 is right: the TU
// spells the array u8[4] in its extern at :4, but the walk at :58-73 sets its
// counter to 1 and runs `while (cnt < 4)`, so it dereferences flag_p exactly
// THREE times -- data_020a0f9d[0], [1] and [2]. The pointer is incremented a
// third time on the way out of the last iteration and never read again.
// src/func_0203e20c.c agrees three separate ways: its clear loop (:116-129),
// its accept loop (:133-157, whose `*pIdx` store is gated on `idx < 4` even
// though the counter runs to 0x10) and its case-4 sweep (:181-190) all start
// at 1 and stop at 4. Three flags, three bytes, every one of them inside this
// object -- so the three-byte host span is the span the ROM's own code reads,
// wherever the linker puts these two sections.
//
// AN x86 CONTROL AGREES, and it is the same reading Andrew's own control
// reported. The case-7 loop transcribed verbatim from src/func_02040014.c with
// its two calls replaced by counters, built /W4 /O2 by the same 32-bit MSVC
// this port uses and run with ALL FOUR flags set, prints
// `flag reads=3, compares=3, final flag_p offset=3`: three dereferences,
// three comparisons, and the pointer left one past the last byte it read
// without being read again. Source and output are in this lane's out/ dir.
//
// The DS layout does put them three apart, and port_wm_arm7_turn() below
// records that gap once as an OBSERVATION of this build -- from integer
// addresses, not by subtracting pointers into two separately declared objects,
// and without refusing anything. See that check for the reasoning.
//
// AND ONE RESIDUAL, NAMED, WHICH IS A DIFFERENT LIMITATION AND STAYS ONE. It
// is about data_020a0fa0's own object, not about the flags above. On the DS
// data_020a0fa0 is the first entry of a FOUR-entry six-byte MAC table that
// runs to 0x020a0fb8, whose other three entries are data_020a0fa6 -- already
// hosted, eighteen bytes, in hal/comms_conductor.cpp's ".dsstate$ymp3c0000".
// So `data_020a0fa0[idx]` for idx > 0 addresses that neighbour on hardware and
// does not here. The ONE writer is src/func_02040014.c:65, and it is reached
// only when one of the three flags read at :62 is nonzero -- the lobby's
// accepted-peer flags, which nothing in this build writes, and which are zero
// in the storage right above. So the out-of-range store is unreachable rather
// than merely unlikely. Moving
// data_020a0fa6 out of the conductor's run to sit behind data_020a0fa0 would
// close it, and it would also shift every later global in that run, which is
// the one thing this rung's hosting is designed not to do.
WMBSS(".dsstate$ywmd10", data_020a0f9d, 3, 1) = { 0 };
WMBSS(".dsstate$ywmd11", data_020a0fa0, 6, 1) = { 0 };

// 0x020a11e4. ROM span to data_020a15e4 is 0x400, and src/func_020421b4.c
// addresses base + 0x400 and hands it to the OS thread-queue calls -- so the
// object really runs to the end of data_020a15e4's own 156-byte span. Hosted at
// 0x400 + 156 = 1180 for that reason.
//
// SPLIT IN TWO BY RUNG W6 (run link100, lane WM5), ADDRESS-NEUTRALLY. The note
// that stood here ended "and nothing names data_020a15e4"; rung W6 names it.
// src/func_02042200.c is
//     func_02058200(base + 0x400, func_02042254, base, base + 0x400, 0x400, 0xf)
// with base = data_020a11e4, so the first 0x400 bytes are the wireless worker
// thread's STACK and data_020a15e4's own 0x9c span is its OSThread RECORD --
// the record src/func_020423c8.c wakes by name. Two sections, $ywmd12a and
// $ywmd12b: MSVC merges same-prefix sections in the lexical order of the text
// after the '$', "ywmd12a" sorts after "ywmd11" and before "ywmd13", the two
// together are the same 0x400 + 0x9c = 1180 bytes at the same first byte, so
// nothing in the bracket moves. hal/wm_thread.cpp's band check reads the 0x400
// offset back at startup rather than trusting this paragraph.
WMBSS(".dsstate$ywmd12a", data_020a11e4, 0x400, 4) = { 0 };
WMBSS(".dsstate$ywmd12b", data_020a15e4, 0x9c, 4) = { 0 };

// 0x020a2400 IS NOT HOSTED HERE ANY MORE (run link100, lane WM5, rung W6).
//
// The note that stood here was right about the span -- 0x1bc0 to data_020a3fc0,
// with data_020a2404/2408/2409 as names inside it -- and it ended "a later lane
// that needs one must carve it out of this object rather than allocate beside
// it". Rung W6 needs all three, and it needs one thing this section could not
// give: src/func_02040c34.c derives the same base as `data_020a1fc0 + 0x440`
// (0x02040CF8: add fp, sl, #0x440) while src/func_020412f0.c and
// src/func_02042254.c spell it `data_020a2400`, and the third of its three
// 0xcc0-byte work nodes runs from +0x1e80 straight through data_020a3fc0. So
// the four names and data_020a3fc0 are ONE contiguous 0x2b80-byte band whose
// head is data_020a1fc0, and a band cannot start in the middle of this run.
// All five are in hal/wm_thread.cpp's ".dsstate$ymp3s0010..0014", which sort
// immediately behind data_020a1fc0's own ".dsstate$ymp3s0009" in
// hal/comms_conductor.cpp. The 0x1bc0 that left here and the 0xb80 that left
// hal/comms_seam.cpp's "$ywm" are exactly the 0x2740 that arrived there, so
// every hosted global from ".dsstate$ywmd14" onward is at the byte it was at
// before this lane.

// 0x020a94c0..0x020a94d4, five ints: the 0x02065xxx layer's own state.
WMBSS(".dsstate$ywmd14", data_020a94c0, 4, 4) = { 0 };
WMBSS(".dsstate$ywmd15", data_020a94c4, 4, 4) = { 0 };
WMBSS(".dsstate$ywmd16", data_020a94c8, 4, 4) = { 0 };
WMBSS(".dsstate$ywmd17", data_020a94cc, 4, 4) = { 0 };
WMBSS(".dsstate$ywmd18", data_020a94d0, 4, 4) = { 0 };

// 0x020a94d4, 0x9c. ADDED BY RUNG W7 (run link100, lane WM8), AND IT IS THE
// HEAD OF THE SAME ROM OBJECT data_020a9570 IS THE TAIL OF.
//
// src/func_02065b94.c returns this address and src/func_02065b88.c returns
// data_020a9570's, both as a decimal literal that hides which symbol it is:
//
//     src/func_02065b94.c   return 34247892;   = 0x020A94D4
//     src/func_02065b88.c   return 34248048;   = 0x020A9570
//
// Lane WM5's note read those two decimals as 0x020A9594 and 0x020A9630 and
// refused the whole 0x02063E-0x020655 band on the strength of it. The
// cartridge says otherwise twice: the literal pools at 0x02065B9C and
// 0x02065B90 hold d4940a02 and 70950a02, and config/arm9/relocs.txt carries a
// named relocation for each --
//
//     from:0x02065b9c kind:load to:0x020a94d4 module:main
//     from:0x02065b90 kind:load to:0x020a9570 module:main
//
// -- so neither accessor points outside the port's own hosted storage.
//
// ONE OBJECT, 0x854 BYTES, AND THE ROM'S OWN LOOP MEASURES IT. Taking
// p = 0x020A94D4, src/func_020652fc.c runs the cartridge's own sixteen
// iterations (0x020652FC + 0x80: strb r1,[r0,#0x1d7]; cmp r2,#0x10;
// add r0,r0,#0x68), so the table is sixteen 0x68-byte entries at p + 0x1d4 and
// it ends at 0x020A94D4 + 0x1d4 + 0x680 = 0x020A9D28 -- the next symbol in
// config/arm9/symbols.txt after data_020a9570, exactly. The furthest byte the
// loop writes is 0x020A94D4 + 0x618 + 0x1d7 = 0x020A9CC3, inside
// data_020a9570. So this section and the one below it are ONE 0x854-byte host
// object and must stay contiguous; ".dsstate$ywmd18a" sorts after "$ywmd18"
// (a longer string with the same prefix) and before "$ywmd19" ('8' < '9' at
// the sixth character), so the linker lays it down between data_020a94d0 and
// data_020a9570 in the ROM's own order. hal/wm_thread.cpp's band check reads
// both offsets back at startup rather than trusting this paragraph.
//
// kind:bss, so there is nothing to read out of the image: config/arm9/
// symbols.txt gives it bss, and arm9 .data ends at 0x0209b000
// (config/arm9/delinks.txt), well below this address. No port/tools/romdata.py
// NAMED row is needed or allowed for it -- that list is initialised .data.
//
// WHAT MOVES. 0x9c bytes are inserted here, so data_020a9570 (".dsstate
// $ywmd19"), this file's own ".dsstate$ywme00" and hal/wm_thread.cpp's four
// ".dsstate$ywmf00..03" tail names each move up by 0x9c and nothing else in
// the captured span moves at all. All five are lanes WM1-WM5's wireless state;
// none is geometry and none reaches a display list as a pointer value. The
// battery's BMP rows are what checks that claim rather than this paragraph.
WMBSS(".dsstate$ywmd18a", data_020a94d4, 0x9c, 4) = { 0 };

// 0x020a9570, 1976. src/func_02065af0.c copies 0x9c bytes OUT of it with
// MultiCopy_Int, into the int[0x27] its callers keep on the stack -- the same
// 0x9c the header above is long, one block further on. It is the tail of
// data_020a94d4's object: see that note.
WMBSS(".dsstate$ywmd19", data_020a9570, 1976, 4) = { 0 };

#undef WMBSS

// ---------------------------------------------------------------------------
// THE TWO ACCESSOR FACES FOR THE 0x02063E-0x020655 BAND (rung W7, lane WM8).
//
// src/func_02065b94.c and src/func_02065b88.c are matched ROM bodies and they
// stay in the link exactly as they are; what they cannot do is answer with an
// address this process can use, because the port hosts data_020a94d4 and
// data_020a9570 wherever the linker put the .dsstate section while the ROM's
// answer is the cartridge's own 0x020A94D4 and 0x020A9570.
//
// So the nine TUs that CALL them are compiled with the call renamed to one of
// these two (port/CMakeLists.txt, the rung W7 block), which hand back the same
// two objects at their host addresses. Nothing else changes: the two ROM
// bodies keep their names, their bytes and their place in the link.
//
// THEY LIVE HERE AND NOT IN hal/wm_thread.cpp BECAUSE OF WHERE THE CALLERS DO.
// Five of the nine (src/func_02065170.c, src/func_020653cc.c,
// src/func_02065538.c, src/func_020659a0.c and src/func_02065af0.c) came in
// with rung W4 on port/slice_wm3.txt, which goes to smoke_player as well as to
// walk_window and walk_window_hires; hal/wm_thread.cpp goes to the first two
// only. This file goes exactly where slice_wm3 goes, which is what the rename
// needs. The return type is void * and the nine callers spell their own extern
// as int, int *, char * or void * -- all four are one C symbol and one machine
// word, and the port's own "verify by ROM ADDRESS" rule is what says they are
// the same thing.
//
// AND THEY CALL THE ROM BODY RATHER THAN REPLACING IT, WHICH IS MEASURED AND
// NOT A FLOURISH. The first build of this rung returned the host address
// straight out of these two functions, and linkage.py came back +9 instead of
// +11: with all nine call sites renamed, NOTHING in the image referenced
// func_02065b94 or func_02065b88 any more and the linker discarded both. That
// is the honest consequence of a plain rename and it is a loss, so the shape
// changed. Each face now ASKS the ROM body for its answer and TRANSLATES it:
// the cartridge's own DS address is what the offset is computed from, so the
// ROM body is genuinely called, at the ROM's own call site, with its return
// value genuinely used. This is not a keep-alive reference -- delete the ROM's
// answer from the expression and the face stops working -- and it is the
// romdata pointer-table contract in its smallest form: a DS address the port
// cannot use is mapped onto the host object before anything dereferences it.
//
// The translation is offset-preserving rather than a constant, so an accessor
// that ever answered with an interior address would still land in the right
// place, and anything outside the object's own 0x854 bytes is reported once by
// name and clamped to the base rather than silently turned into a wild
// pointer.
// ---------------------------------------------------------------------------
extern "C" int func_02065b94(void);   /* the ROM body: returns 0x020A94D4 */
extern "C" int func_02065b88(void);   /* the ROM body: returns 0x020A9570 */

static const unsigned kWmqDsBase = 0x020A94D4u;  /* config/arm9/relocs.txt:10761 */
static const unsigned kWmqDsSize = 0x854u;       /* to data_020a9d28, the next symbol */

static void *wm8_translate(unsigned ds, const char *who)
{
    const unsigned off = ds - kWmqDsBase;
    if (off >= kWmqDsSize) {
        static int said;
        if (!said) {
            said = 1;
            std::fprintf(stderr,
                         "  [wm8] %s answered %08x, which is outside the "
                         "0x854-byte object at %08x that config/arm9/"
                         "relocs.txt says it names; using the object's base\n",
                         who, ds, kWmqDsBase);
            std::fflush(stderr);
        }
        return data_020a94d4;
    }
    return data_020a94d4 + off;
}

extern "C" void *port_wm8_wmq_base(void)
{
    return wm8_translate((unsigned)func_02065b94(), "func_02065b94");
}

extern "C" void *port_wm8_wmq_head(void)
{
    return wm8_translate((unsigned)func_02065b88(), "func_02065b88");
}

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

// GATE B's two pointers, hosted by hal/comms_seam.cpp (".dsstate$ywmc09" and
// ".dsstate$ywmc10") and FILLED by the ROM's own src/func_020408b0.c:41-42.
// data_020a0f74 is the 0x420 MP work area -- four 0x100 frame slots and the
// field block at +0x400, whose +0x410 halfword is the per-slot stride
// src/func_02062734.c walks. data_020a0f80 is the MP receive buffer. Both are
// four-byte spans holding a pointer, so they are READ the way data_020a89ac is
// read above and never dereferenced as arrays.
extern unsigned char data_020a0f74[];
extern unsigned char data_020a0f80[];

// And the ROM's own unpacker by its own name, for the once-a-session readback
// at the foot of this file. src/func_0204068c.c is `int func_0204068c(int)`;
// src/func_0203ea5c.c:29 and hal/comms_conductor_wide.cpp:109 both declare it
// `void *func_0204068c(u16)` and call it that way, so this file uses the
// callers' spelling. All three are extern "C" __cdecl on a PE32 target (the
// map names it _func_0204068c with no @N), so the caller pops and a 16-bit
// argument reaching a 32-bit parameter is zero-extended into its own stack
// slot -- the same reading rung W0 wrote down for func_02040704.
void *func_0204068c(unsigned short aid);

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

// GATE A's own bookkeeping: the association bitmap this ARM7 has published,
// and how many times it changed. `g_kids` is the last value written to both
// +0x86 and +0x17e, so a census can say what the radio was holding at exit.
unsigned      g_kids = 0;            // the bitmap last published
unsigned long g_kid_events = 0;      // times it changed (joins and leaves)
unsigned      g_kid_peak = 0;        // every child bit ever set this session

// GATE B's bookkeeping: how many rounds were published into the ROM's own MP
// receive buffer, what the last one carried, and whether the port's own wider
// buffer had to be armed for a session the cartridge's allocation cannot
// describe.
unsigned long g_mp_fills = 0;
unsigned      g_mp_last_mask = 0;
unsigned      g_mp_stride = 0;
unsigned      g_mp_peak_slots = 0;
bool          g_mp_wide_armed = false;
bool          g_mp_said_nostride = false;
bool          g_mp_said_first = false;

// ---- RUNG W8'S LEDGER: THE PORT DATAGRAMS THE ARM9 HANDS DOWN --------------
// See THE PORT SPLIT in the header. Indexed by the WM port number the ROM's
// own WM_SetMPData carries at word 4 of the command; sixteen ports, which is
// the range src/func_020616e8.c:59 clears and src/func_02061c88.c indexes.
enum : unsigned { kWmPorts = 0x10 };
unsigned long      g_pd_cmds[kWmPorts]      = { 0 };
unsigned long long g_pd_bytes[kWmPorts]     = { 0 };
unsigned           g_pd_last_len[kWmPorts]  = { 0 };
unsigned           g_pd_last_dest[kWmPorts] = { 0 };
unsigned           g_pd_first[kWmPorts]     = { 0 };  // first payload halfword
unsigned long      g_pd_empty[kWmPorts]     = { 0 };  // len 0: nothing to send
unsigned           g_pd_seen = 0;                     // bitmap of ports used
unsigned long      g_pd_out_of_range = 0;

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

// RUNG W8: WHO OWNS A WM PORT IN THIS CARTRIDGE, read off the two registration
// sites and nowhere else. src/func_02040c34.c:58 registers src/func_020417d0.c
// on the port its second argument names, and src/func_0203ea5c.c:209/212 pass
// 1. src/func_020631dc.c:96 and :123 register src/func_02062bdc.cpp (parent)
// and src/func_02062aa4.cpp (child) on its `unitIdx`, and src/func_0203fa50.c:7
// passes 0xc. Every other port is unregistered: src/func_020616e8.c:59 clears
// all sixteen at WM_Init and nothing else in the image calls func_02061c88.
const char *wm_port_owner(unsigned prt)
{
    switch (prt) {
    case 0x01: return "the WFS      (src/func_020417d0.c)";
    case 0x0c: return "the MP unit  (func_02062bdc/02062aa4)";
    default:   return "no registered callback";
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

// ---- gate A: the association, read off the radio ---------------------------
//
// THE TRANSPORT CONTRACT EXPOSES A COUNT, NOT A MASK, and that is the one
// approximation in this function. hal/comms_seam.h gives player_count() (live
// slots, 1..kCommsMaxPlayers) and peer_block(aid) -- and peer_block is only
// meaningful after a completed round, which is exactly the thing that cannot
// have happened yet when a parent is being asked whether it has anybody to
// send to. So the bitmap is built as the contiguous low run player_count()
// implies: n players means AIDs 0..n-1, and the children are 1..n-1. The
// loopback carrier assigns slots that way (hal/comms_loopback.cpp's g_live is
// filled from slot 0 upward and lb_player_count() is its popcount), so on
// every session this build can form the derived mask IS the live mask.
//
// WHERE IT WOULD DIFFER, AND WHY NO READER CAN SEE IT. A session that lost a
// MIDDLE slot -- 0, 1 and 3 live -- has popcount 3, so this reports bits 1..2
// where the radio holds bits 1 and 3. Both of this rung's readers are blind to
// the difference: src/func_02062428.c tests +0x17e for ZERO and nothing else,
// and +0x86 reaches the wire only through `H(0x40e) & mask`, where H(0x40e) is
// the ROM's own hard-coded 0xf (src/func_0203fa50.c:7 passes mask 0xf) -- so a
// wrong bit inside 1..3 changes which of four already-addressed slots the
// SDK's own frame names, on a frame the transport, not the SDK, actually
// delivers. It is an approximation and it is written down rather than dressed
// up; closing it means the carrier exporting g_live, which is a change to a
// file this lane does not own for a difference nothing on this rung reads.
unsigned popcount16(unsigned v)
{
    unsigned n = 0;
    for (unsigned b = 0; b < 16; ++b) if (v & (1u << b)) ++n;
    return n;
}

unsigned children_mask()
{
    const port::CommsTransport *t = port::comms_transport();
    if (!t) return 0;
    if (t->state() != port::kCommsParentConnected) return 0;
    int n = t->player_count();
    if (n > (int)port::kCommsMaxPlayers) n = (int)port::kCommsMaxPlayers;
    unsigned m = 0;
    for (int aid = 1; aid < n; ++aid) m |= 1u << aid;
    return m;
}

// Published from the ARM7's turn, which is where every other status-block
// field on this rung is published: the ARM9 has yielded, so nothing is reading
// the block mid-update. Only a PARENT has children -- a child's own aid is at
// +0x184 and it never reads either bitmap (src/func_02062428.c:29-32 takes the
// +0x17e branch only when +0x184 is zero).
void publish_association()
{
    const unsigned st = st_get16(0x00);
    if (st != kStParent && st != kStMpParent) return;
    if (st_get16(0x184) != 0) return;

    const unsigned m = children_mask();
    if (m == g_kids) return;

    ++g_kid_events;
    g_kid_peak |= m;
    if (ntr::ipc_log_on())
        std::fprintf(stderr,
            "[wm7] association 0x%04x -> 0x%04x (the radio is holding %u "
            "child link(s)); status +0x86 and +0x17e follow it\n",
            g_kids, m, popcount16(m));
    g_kids = m;
    st_set16(0x86,  m);
    st_set16(0x17e, m);
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
        g_kids = 0;                 // gate A: the association is gone with it
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
        g_kids &= ~(unsigned)c32[1];   // gate A: and the mirror follows
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
        // buffer at word 1, the length at word 2, the destination mask at word
        // 3, the port index at word 4 and the completion handler at word 6.
        // The reply is the 0x81 indication.
        //
        // RUNG W8 READS WORDS 1, 3 AND 4 TOO, and stops dropping the datagram
        // without a record of it. Those three are exactly what an 0x82
        // port-receive indication has to carry -- WHICH bytes, TO WHOM, and ON
        // WHICH PORT -- and until this rung nothing in the port had looked at
        // any of them, so "what traffic is actually on the ROM's ports" had no
        // answer from a run, only from reading. This is a read and a count:
        // not one byte of behaviour changes with it.
        {
            const unsigned prt = c32[4];
            const unsigned len = c32[2];
            if (prt < kWmPorts) {
                ++g_pd_cmds[prt];
                g_pd_seen |= 1u << prt;
                g_pd_last_len[prt]  = len;
                g_pd_last_dest[prt] = c32[3];
                if (len == 0) {
                    // src/func_02062428.c:33 answers a zero length with error
                    // 6 and sends nothing, so this is the ROM asking with an
                    // empty hand rather than a datagram this stub lost.
                    ++g_pd_empty[prt];
                } else {
                    g_pd_bytes[prt] += len;
                    if (c32[1])
                        g_pd_first[prt] =
                            *(const unsigned short *)(uintptr_t)c32[1];
                }
            } else {
                ++g_pd_out_of_range;
            }
        }
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
        // THE LOBBY PAIR'S LAYOUT, AS AN OBSERVATION AND NOT A REQUIREMENT.
        // No reader depends on it: src/func_02040014.c and src/func_0203e20c.c
        // read data_020a0f9d[0..2] and stop, which is inside this file's own
        // three-byte object however the linker orders the sections (the note
        // at the WMBSS lines works the loops out). The gap is still worth one
        // line, because the DS has these two globals three apart and a later
        // change to the section names would move them without anything
        // saying so.
        //
        // MEASURED ON INTEGER ADDRESSES. The two names are separately declared
        // arrays that the linker happens to place adjacently; adjoining
        // sections do not make them one object, so `data_020a0fa0 -
        // data_020a0f9d` was pointer subtraction across distinct array objects
        // and not defined C++ arithmetic. Each address is reduced to an
        // address-sized integer first -- (unsigned)(uintptr_t), the port's own
        // spelling for a host pointer, lossless here because the port builds
        // 32-bit (port/build-port.cmd calls vcvars32) -- and the difference is
        // taken as a signed number so a reordering reads as a negative gap
        // rather than as four billion. Nothing refuses on the result.
        const long gap = (long)(unsigned)(uintptr_t)data_020a0fa0
                       - (long)(unsigned)(uintptr_t)data_020a0f9d;
        if (gap != 3)
            std::fprintf(stderr,
                "[wm7] lobby layout note: data_020a0fa0 sits at "
                "data_020a0f9d %+ld in this build, and +3 on the DS. Nothing "
                "reads past data_020a0f9d[2], so this is recorded, not "
                "refused.\n", gap);
    }

    // GATE A. Before a reply, the association: on the DS the ARM7 writes the
    // status block whether or not the ARM9 asked it anything, and a parent that
    // has been sitting in MP_PARENT since before its child knocked gets no
    // further command until it is told it has somebody to send to.
    publish_association();

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
    std::fprintf(stderr,
        "[wm7:census] association 0x%04x (peak 0x%04x, %lu change(s)), status "
        "+0x86 %u, +0x17e %u\n",
        g_kids, g_kid_peak, g_kid_events, st_get16(0x86), st_get16(0x17e));
    std::fprintf(stderr,
        "[wm7:census] mp receive: %lu round(s) published, last mask 0x%04x, "
        "%u slot(s) at peak, stride %u, wide buffer %s\n",
        g_mp_fills, g_mp_last_mask, g_mp_peak_slots, g_mp_stride,
        g_mp_wide_armed ? "ARMED" : "not needed");
    // RUNG W8: WHAT WENT DOWN EACH WM PORT. The ROM registers exactly two port
    // callbacks in this image and they are on DIFFERENT ports; this says which
    // of them the run used, how much each carried, and where an 0x82 would
    // have had to go.
    std::fprintf(stderr,
        "[wm7:census] port datagrams: ports used 0x%04x, %lu out of range\n",
        g_pd_seen, g_pd_out_of_range);
    for (unsigned prt = 0; prt < kWmPorts; ++prt) {
        if (!g_pd_cmds[prt]) continue;
        std::fprintf(stderr,
            "[wm7:census]   port 0x%02x %-38s %lu command(s), %llu byte(s), "
            "%lu empty, last len %u dest 0x%04x, first halfword 0x%04x\n",
            prt, wm_port_owner(prt), g_pd_cmds[prt],
            (unsigned long long)g_pd_bytes[prt], g_pd_empty[prt],
            g_pd_last_len[prt], g_pd_last_dest[prt], g_pd_first[prt]);
    }
    std::fflush(stderr);
}

// Whether this stub has been asked for anything at all. hal/boot2_ipc.cpp uses
// it to keep a solo run's log as quiet as it was before this rung.
extern "C" int port_wm_arm7_saw_traffic(void) { return g_cmd_total != 0; }

// ===========================================================================
// GATE B: THE ROM'S OWN MP RECEIVE BUFFER, FILLED FROM THE CARRIER'S ROUND
// ===========================================================================
//
// Rung W2 retires hal/comms_seam.cpp's func_0204068c face, so a peer's block
// for the round comes back through the ROM's own unpacker from here on. That
// unpacker is two files and eleven lines, and it is the whole specification of
// what this function has to write:
//
//   src/func_02062778.c  (a = data_020a0f74, b = data_020a0f80, c = the aid)
//       if ((b[1] & (1u << c)) == 0) return 0;
//       return func_02062734(a, b[0], (unsigned)(b + 2), c);
//   src/func_02062734.c
//       for (i = 0; i < count; i++)
//           if (mask & bit) acc += *(unsigned short *)(base + 0x410);
//
// So the buffer is a four-byte header and a packed run of blocks:
//
//   +0x00  u16  THE PACKING MASK. func_02062734 counts its set bits BELOW the
//               requested aid and steps that many strides, so it is the set of
//               aids that have a block in the run and it fixes every offset.
//   +0x02  u16  THE LIVENESS MASK. func_02062778 refuses an aid whose bit is
//               clear here before it computes anything at all.
//   +0x04       the blocks, one per set bit of the packing mask, in ASCENDING
//               AID ORDER, each *(u16 *)(data_020a0f74 + 0x410) bytes long.
//
// THE STRIDE IS THE ROM'S AND IT IS NOT WRITTEN HERE. src/func_020631dc.c:60
// sets it from src/func_0203fa50.c:7's elemSize, which is 0x20 -- the same
// 0x20 hal/comms_seam.h's wire-format section fixes and the same
// kCommsBlockBytes the carrier moves. This function READS it back and refuses
// to publish at all if the two disagree, because that disagreement would hand
// the ROM's unpacker a stride nothing wrote with.
//
// THE LIVENESS MASK IS THE CARRIER'S, EXACTLY. The transport contract says
// peer_block(aid) is "player aid's block for the round exchange() just
// completed, or null if that slot is not live", so asking it for every aid IS
// the live set. That is strictly better evidence than gate A's association,
// which had to infer a shape from a count, and it is why this mask is built
// from the blocks themselves rather than from player_count().
//
// AND THE CARTRIDGE'S OWN BUFFER IS TOO SMALL FOR SIXTEEN, WHICH IS A MEASURED
// CEILING AND NOT A CHOICE. src/func_020408b0.c:41 allocates data_020a0f80 at
// 0x100 bytes, src/func_02062df0.c:77 copies exactly 0x100 into it, and
// src/func_020631dc.c:79 refuses an MP frame longer than 0xfc. Four bytes of
// header and 0x20 a slot puts the cartridge's own ceiling at SEVEN players and
// its own session at four (src/func_0203fa50.c:7 passes mask 0xf). This port
// hosts sixteen. hal/vs_width.h's standing ruling covers exactly this case --
// "every per-slot array the port HOSTS is this wide ... hosting a ROM global
// wider than the cartridge's own run is the deviation, and it is the approved
// one" -- so a session that does not fit the cartridge's allocation is
// published into the port's own buffer and data_020a0f80 is pointed at it.
// AT FOUR PLAYERS AND BELOW NOTHING MOVES: the ROM's own allocation is what is
// written, and the wide buffer is never touched. The census says which
// happened, on every logged run.
#pragma section(".dsstate$ywme00", read, write)
extern "C" __declspec(allocate(".dsstate$ywme00")) __declspec(align(32))
unsigned char port_wm_mp_recv_wide[4 + port::kCommsMaxPlayers *
                                   port::kCommsBlockBytes] = { 0 };

extern "C" void port_wm_publish_mp_recv(void)
{
    const port::CommsTransport *t = port::comms_transport();
    if (!t) return;

    unsigned char *buf = *(unsigned char **)(void *)data_020a0f80;
    if (!buf) return;               // src/func_020408b0.c has not allocated it

    // THE HEADER IS CLEARED FIRST, BEFORE ANY OTHER ANSWER IS POSSIBLE. This
    // buffer is a plain Memory::Allocate off the game heap
    // (src/func_020408b0.c:41) and nothing zeroed it; before this rung nothing
    // read it either. src/func_02062778.c's very first act is to test
    // `b[1] & (1u << aid)`, so a round this function declines to publish must
    // leave a liveness mask of ZERO behind rather than whatever the heap
    // happened to hold, or the ROM's unpacker answers a pointer computed from
    // stale bytes. Every early return below is therefore an honest "no peers".
    *(unsigned short *)(buf + 0) = 0;
    *(unsigned short *)(buf + 2) = 0;

    unsigned char *work = *(unsigned char **)(void *)data_020a0f74;
    if (!work) return;              // the ROM's MP layer has not been laid out

    const unsigned stride = *(const unsigned short *)(work + 0x410);
    if (stride == 0) {
        // src/func_020631dc.c has not run: WM_StartMP's own callback
        // (src/func_0203fdac.c -> src/func_0203fa50.c) is what sets it, so the
        // ROM's MP unit is not up yet and there is nothing to publish INTO.
        if (!g_mp_said_nostride) {
            g_mp_said_nostride = true;
            if (ntr::ipc_log_on())
                std::fprintf(stderr,
                    "[wm7] mp receive: the ROM's stride at "
                    "data_020a0f74+0x410 is still 0, so src/func_020631dc.c "
                    "has not laid the MP unit out yet\n");
        }
        return;
    }
    if (stride != (unsigned)port::kCommsBlockBytes) {
        std::fprintf(stderr,
            "[wm7] HARD FAULT: the ROM's MP stride at data_020a0f74+0x410 is "
            "%u and the carrier moves %d bytes a slot, so src/func_02062734.c "
            "would walk the receive buffer with a stride nothing wrote with.\n",
            stride, (int)port::kCommsBlockBytes);
        std::fflush(stderr);
        std::_Exit(24);
    }
    g_mp_stride = stride;

    unsigned mask = 0, count = 0;
    for (int aid = 0; aid < (int)port::kCommsMaxPlayers; ++aid)
        if (t->peer_block(aid)) { mask |= 1u << aid; ++count; }
    if (!mask) return;

    const unsigned need = 4 + count * stride;
    if (need > 0x100) {
        if (!g_mp_wide_armed) {
            g_mp_wide_armed = true;
            std::fprintf(stderr,
                "[wm7] mp receive: %u live slot(s) need %u bytes and the "
                "cartridge's own allocation is 0x100 "
                "(src/func_020408b0.c:41). Publishing into the port's own "
                "%u-byte buffer instead, per hal/vs_width.h's ruling; the "
                "ROM's allocation is left where it is.\n",
                count, need, (unsigned)sizeof port_wm_mp_recv_wide);
            std::fflush(stderr);
        }
        buf = port_wm_mp_recv_wide;
        *(unsigned char **)(void *)data_020a0f80 = buf;
    }

    *(unsigned short *)(buf + 0) = (unsigned short)mask;
    *(unsigned short *)(buf + 2) = (unsigned short)mask;
    unsigned k = 0;
    for (int aid = 0; aid < (int)port::kCommsMaxPlayers; ++aid) {
        if (!(mask & (1u << aid))) continue;
        const void *b = t->peer_block(aid);
        std::memcpy(buf + 4 + k * stride, b, stride);
        ++k;
    }
    ++g_mp_fills;
    g_mp_last_mask = mask;
    if (count > g_mp_peak_slots) g_mp_peak_slots = count;

    // THE PROOF LINE, once a session and only with the log on: the first round
    // read back THROUGH THE ROM'S OWN UNPACKER rather than through the
    // arithmetic this function just performed. func_0204068c is the ROM's own
    // body as of this rung, so this prints the bytes the game itself will read
    // and the address this function believed it wrote them to.
    if (!g_mp_said_first && ntr::ipc_log_on()) {
        g_mp_said_first = true;
        for (int aid = 0; aid < (int)port::kCommsMaxPlayers; ++aid) {
            if (!(mask & (1u << aid))) continue;
            const unsigned char *got =
                (const unsigned char *)func_0204068c((unsigned short)aid);
            std::fprintf(stderr,
                "[wm7] mp receive: aid %2d -> src/func_02062778.c answers %p "
                "(expected %p), first word 0x%04x, mask 0x%04x, stride %u\n",
                aid, (const void *)got,
                (const void *)(buf + 4 + popcount16(mask & ((1u << aid) - 1u))
                               * stride),
                got ? (unsigned)*(const unsigned short *)got : 0u,
                mask, stride);
        }
        std::fflush(stderr);
    }
}
