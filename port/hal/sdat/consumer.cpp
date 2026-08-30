// The hosted ARM7: command-queue consumer.
//
// THE PROTOCOL (all ARM9 halves are matched src/ in slice_gate10):
//   Snd_SendCommand   takes a node off the free list at data_020a6484 and
//                     appends it to the pending list data_020a648c/6490.
//   func_0205b070     moves the whole pending list into ring slot
//                     data_020a64a8[data_020a649c], bumps the in-flight
//                     count data_020a64a0 and the batch tick data_020a64a4,
//                     and pokes the ARM7 with IPCSend(7, head, 0).
//   func_0205b274     when the ARM7's progress word has moved past
//                     data_020a6488, pops ring slot data_020a6498 and
//                     splices that batch back onto the free list.
//   func_0205b5d4     reads the ARM7's progress word (data_020a7fc0[0]).
//   func_0205b608     reads the ARM7's PLAYER BITMASK (data_020a7fc0[1]).
//                     That word is how the ARM9 learns a sound has FINISHED,
//                     and it is the only way it can: see
//                     publish_player_status and sd_sound_frame_host below.
//
// THE DEADLOCK, AND HOW IT IS CLOSED. func_0205b1d8 ends with
//     do { func_0205b274(1); p = func_0205adf8(); } while (p == 0);
// and func_0205b274(1) is
//     while (func_0205b5d4() == data_020a6488) {}
// -- a spin on a counter only the consumer advances. On hardware a second
// CPU advances it. Here there is no second CPU, so if the free list ever
// empties inside game code the spin is forever.
//
// The fix is not a bigger pool or a per-frame drain (game code can exhaust
// 256 nodes between two frames and never return). It is to host the seam
// where it actually is: func_0205b5d4 is "read the other core's progress",
// and on a single-threaded host, reading the other core's progress has to
// MAKE the other core run. So this file owns func_0205b5d4 (removed from
// the slice) and pumps the consumer before returning the counter.
//
// That makes termination provable. If the spin is entered, the free list is
// empty, so all 256 nodes are on the pending list or in the ring. Whichever
// they are, func_0205b1d8 has already called func_0205b070 (directly, or via
// the func_0205b274(0) drain) before the spin, so at least one batch is in
// the ring and data_020a64a4-1 > consumed. The pump therefore consumes at
// least one batch and advances the progress word, the comparison in
// func_0205b274 fails, the spin exits, the batch is reclaimed and the free
// list is non-empty. One iteration, always.
//
// The same hook makes the whole subsystem self-starting: seeding runs from
// the first func_0205b5d4, so even a sound call that arrives before the
// frame loop's tick finds a seeded pool instead of a null free list.
#include "sdat.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---- the ARM9-side globals, as the matched code sees them ---------------
extern "C" {
extern void *data_020a6484;         // free list head
extern void *data_020a6494;         // free list tail NODE
extern void *data_020a648c;         // pending list head
extern void *data_020a6490;         // pending list tail
extern int   data_020a6488;         // batches reclaimed
extern int   data_020a6498;         // ring read index
extern int   data_020a649c;         // ring write index
extern int   data_020a64a0;         // batches in flight
extern int   data_020a64a4;         // next batch tick (starts at 1)
extern void *data_020a64a8[];       // 9-slot ring of batch heads
extern int   data_020a6760[];       // the 256 x 0x18 node pool
extern unsigned int *data_020a7fc0; // -> the ARM7 status block

int func_0205b070(int blocking);

/* The game's own sound init, in pieces. See sd_sound_init_host below. */
void func_0204f1e8(void);            /* three counters */
void func_02050950(void);            /* two more */
void func_0204fc40(void);            /* voice free list + 32 player records */
void func_0204f94c(void *p);         /* clear one player's voice pointer */
void func_02011a28(void *table);     /* PlayLong's 0x40-slot handle table */
void func_02048f34(void *owner);     /* 3D voice pools */
extern int data_0209b4a0[], data_0209b4b0[], data_0209b4a4[];
extern int data_0209b53c[];
extern unsigned char data_0209b4b4[];
extern unsigned char data_0209b480;  /* the master "sound effects on" flag */

/* The ARM9's own voice bookkeeping, for the census the trace prints.
 * data_020a4d54 is the FREE voice list and data_020a4d60 the ACTIVE one;
 * both are 0xc-byte NestedHeapIterators whose element count is the u16 at
 * +8 (see NestedHeapIterator::AddLast / ::Remove, which are the only two
 * functions that move it). data_020a4bf8 and data_020a4c18 are the 3D
 * positional slot tables func_02048720 hands out, 8 bytes per slot, and a
 * slot is free exactly when its first word is null. */
extern unsigned char data_020a4d54[];
extern unsigned char data_020a4d60[];
extern int data_020a4bf8[];
extern int data_020a4c18[];
extern int data_02099fb0;            /* the type-9 pool's live count */

/* The ARM9's per-frame sound maintenance. See sd_sound_frame_host. */
void func_0204fafc(void);
int func_0205b274(int blocking);
void func_020119c8(void *table);     /* the per-frame looping-handle reaper */
void func_02011974(void *table);     /* the level-change looping-handle reaper */

/* The game's music/jingle crossfade, the half of func_020132d8 that runs in
 * sd_sound_fade_host below.
 *
 * data_0209b490 is the MAIN music player's (data_0209b4a0) volume and
 * data_0209b49c is the SUB-music, i.e. jingle, player's (data_0209b4b0)
 * volume. Sound::PlaySub stamps their TARGETS -- data_0208e42c for the main
 * duck, data_0209b470 for the jingle -- and a shared step data_0209b494, then
 * reports back "have both arrived yet". func_02012d64 walks the first pair and
 * the ApproachLinear here walks the second. data_0208e430 is the latched
 * jingle id; the ROM releases it to -1 exactly when the jingle volume has
 * ramped all the way down. */
void func_02012d64(void);            /* the main-music volume ramp */
void func_02013524(void *player, int vol, int mode);  /* push a volume */
void func_0204fa2c(int *player, int fade);            /* stop a player */
void _Z14ApproachLinearRiii(int *ref, int target, int step);
extern int data_0208e430;            /* the latched jingle id, -1 when free */
extern signed char data_0209b470;    /* the jingle volume TARGET */
extern int data_0209b494[];          /* the shared ramp step */
extern int data_0209b49c[];          /* the jingle volume, ramped here */
}

// The ARM9 half of the voice trace. The switch and the printer live in
// mixer.cpp, which is the one file every sound harness links; this census
// reads the game's own voice records, so it belongs where they are.
void sd_vtrace_arm9_census(const char *when)
{
    if (!g_voice_trace) return;

    int freeN   = *(unsigned short *)(data_020a4d54 + 8);
    int activeN = *(unsigned short *)(data_020a4d60 + 8);

    int p2 = 0, p9 = 0;
    for (int i = 0; i < 2; i++) if (data_020a4bf8[i * 2]) p2++;
    int n9 = data_02099fb0;
    if (n9 < 0 || n9 > 6) n9 = 6;
    for (int i = 0; i < n9; i++) if (data_020a4c18[i * 2]) p9++;

    static int lastFree = -1, lastActive = -1, lastP2 = -1, lastP9 = -1;
    if (freeN == lastFree && activeN == lastActive && p2 == lastP2 &&
        p9 == lastP9)
        return;
    lastFree = freeN; lastActive = activeN; lastP2 = p2; lastP9 = p9;

    sd_vtrace("arm9 voices free %2d active %2d | 3d slots type2 %d/2 "
              "type9 %d/%d  (%s)%s\n", freeN, activeN, p2, p9, n9, when,
              freeN == 0 ? "  <- FREE LIST EMPTY" : "");

    // On a new low water mark, name the voices that are sitting on the pool,
    // so "the pool is draining" can be told from "the pool is busy". The
    // active list is a NestedHeapIterator with link offset 0x14 (its ctor
    // argument in func_0204fc40), so the next pointer is at node+0x18.
    static int low = 99;
    if (freeN >= low) return;
    low = freeN;
    for (unsigned char *n = *(unsigned char **)data_020a4d60; n;
         n = *(unsigned char **)(n + 0x18)) {
        int id = n[0x3c];
        sd_vtrace("  held by voice %2d: state %d, started %d, priority %3d, "
                  "player %s\n", id, n[0x2c], n[0x2d], n[0x3d],
                  sd_seq_active(id) ? "STILL SOUNDING" : "quiet");
    }
}

// The other pool in the sound stack, one level up from the voices, and the
// one the voice census cannot see.
//
// data_0209b53c is func_0201226c's "start this looping sound, or refresh the
// one I started last frame" table: an 8-byte header (a birth counter at +0
// that becomes the handle, a round-robin allocation cursor at +4) followed by
// 0x40 entries of 0x14 bytes. An entry is LIVE exactly when its first word --
// the handle its caller is holding -- is non-zero, and +6 is the
// refreshed-this-frame flag that func_020119c8 clears and then acts on.
//
// It gets its own line because a handle can be live here while the voice
// underneath it is long gone, and because a live count that only ever climbs
// is the signature of nothing reaping.
void sd_vtrace_loop_census(const char *when)
{
    if (!g_voice_trace) return;

    unsigned char *t = (unsigned char *)data_0209b53c;
    int live = 0, fresh = 0;
    for (int i = 0; i < 0x40; i++) {
        unsigned char *e = t + 8 + i * 0x14;
        if (*(int *)e == 0) continue;
        live++;
        if (e[6]) fresh++;
    }

    // How many sequencer players are actually running is what "louder"
    // means: each leaked handle leaves one more player sounding the same
    // sequence, and they sum.
    unsigned mask = sd_seq_player_mask();
    int players = 0;
    for (unsigned m = mask; m; m &= m - 1) players++;

    static int lastLive = -1, lastFresh = -1, lastPlayers = -1;
    if (live == lastLive && fresh == lastFresh && players == lastPlayers)
        return;
    lastLive = live; lastFresh = fresh; lastPlayers = players;

    sd_vtrace("3d loop handles live %2d/64 refreshed %2d, births %d, "
              "seq players sounding %2d  (%s)%s\n",
              live, fresh, *(int *)t, players, when,
              live == 0x40 ? "  <- HANDLE TABLE FULL" : "");
}

namespace {

// A queue node: next, then the five words Snd_SendCommand writes.
struct Node {
    Node *next;
    int op, a, b, c, d;
};

enum { NODES = 256, RING = 9, STATUS_WORDS = 0x280 / 4 };

unsigned int g_status[STATUS_WORDS];   // the ARM7 status block we publish
int g_seeded;
int g_pumping;                          // reentrancy guard for the hook
unsigned g_consumed;                    // batches this consumer has executed
int g_readIdx;                          // our own cursor into the ring
int g_trace;                            // SM64DS_SND_TRACE=1
sd_u8 g_sawOp[256];

const char *op_name(int op)
{
    switch (op) {
    case 0x00: return "START_SEQ";
    case 0x01: return "STOP_SEQ";
    case 0x02: return "PREPARE_SEQ";
    case 0x03: return "PLAYER_PARAM";
    case 0x04: return "TRACK_PARAM";
    case 0x05: return "MUTE_TRACKS";
    case 0x09: return "SETUP_CAPTURE";
    case 0x0a: return "START_CHANNEL";
    case 0x0b: return "CHANNEL_SETUP";
    case 0x0e: return "CHANNEL_FLAGS";
    case 0x0f: return "CALLBACK_REG";
    case 0x11: return "SHARED_WORK";
    case 0x13: return "STOP_UNUSED";
    case 0x15: return "STRM_SETUP";
    case 0x16: return "STRM_PARAM";
    case 0x17: return "STRM_PARAM2";
    case 0x19: return "SET_STATUS_BLOCK";
    case 0x1b: return "LOAD_SEQ";
    case 0x1c: return "LOAD_BANK";
    case 0x1d: return "LOAD_WAVEARC";
    default:   return "?";
    }
}

// An opcode this consumer handles can still carry a PARAMETER it does not.
// Those would otherwise vanish without a word, which is the one thing the
// port is not allowed to do quietly.
void note_param(int op, int param)
{
    static sd_u8 seen[2][256];
    int row = (op == 3) ? 0 : 1;
    if (param < 0 || param > 255 || seen[row][param]) return;
    seen[row][param] = 1;
    fprintf(stderr, "[snd] command 0x%02x parameter 0x%02x not implemented "
            "-- ignored\n", op, param);
}

void exec(const Node *n)
{
    int op = n->op;
    if (g_trace)
        fprintf(stderr, "[snd] op %02x %-16s a=%08x b=%08x c=%08x d=%08x\n",
                op, op_name(op), (unsigned)n->a, (unsigned)n->b,
                (unsigned)n->c, (unsigned)n->d);

    switch (op) {
    case 0x00: {                        // START_SEQ
        // a = voice id (the byte at voice+0x3c), b = sequence data base,
        // c = offset of this entry within it, d = resident SBNK.
        //
        // b and c are the SEQARC case: func_02051a98 passes the SSAR's DATA
        // base in b and the entry's own offset in c, so a sound effect is
        // one entry inside a packed archive. Music passes c = 0, so the same
        // pair covers both.
        //
        // They go to sd_seq_start SEPARATELY, and adding them here was a bug
        // rather than a shortcut: b is the base every branch target in the
        // stream is measured from, so an entry folded into the base sends
        // its own first jump 0xc off into the neighbouring entry. See the
        // header on sd_seq_start.
        int slot = n->a & 31;
        const sd_u8 *base = (const sd_u8 *)(size_t)(unsigned)n->b;
        sd_u32 off = (sd_u32)n->c;
        const sd_u8 *bnk = (const sd_u8 *)(size_t)(unsigned)n->d;
        if (!base || !bnk) break;
        if (g_voice_trace) {
            const sd_u8 *seq = base + off;
            long rel = (seq >= g_sdat.base && seq < g_sdat.base + g_sdat.size)
                       ? (long)(seq - g_sdat.base) : -1;
            sd_vtrace("cmd  START player %2d seq sdat+0x%lx%s\n", slot, rel,
                      sd_seq_active(slot) ? "  (over a player still sounding)"
                                          : "");
        }
        // func_0205b78c has normally already patched the bank's wave-link
        // slots by now; this fills any that are still empty and is a no-op
        // otherwise.
        sdat_link_bank_waves((sd_u8 *)bnk);
        sd_seq_start(slot, base, off, bnk);
        break;
    }
    case 0x01:                          // STOP_SEQ
        SD_VT("cmd  STOP  player %2d%s\n", n->a & 31,
              sd_seq_active(n->a & 31) ? "  (CUTS a player still sounding)"
                                       : "");
        sd_seq_stop(n->a & 31);
        break;
    case 0x03: {                        // PLAYER_PARAM: b = param, c = value
        // Param 4 is the 0..127 volume func_0205ad24 sends once at start.
        // Param 6 is func_0205ad3c's, and it is a different animal: a SIGNED
        // attenuation in tenths of a dB out of the ROM's own table, sent
        // every frame by func_0204fafc for every voice that is sounding. It
        // is the game's 3D distance volume and its fade ramps both.
        int slot = n->a & 31;
        if (n->b == 4) sd_seq_set_volume(slot, n->c);
        else if (n->b == 6) sd_seq_set_volume_db10(slot, n->c);
        else note_param(3, n->b);
        break;
    }
    case 0x04: {                        // TRACK_PARAM
        // a = voice id | (size << 24), b = track mask, c = param, d = value.
        //
        // The top byte of a is the WIDTH of the field, not a mode: every one
        // of the five matched emitters pins it (func_0205acac sends 1 with
        // the byte-wide pan, func_0205acfc and func_0205acd4 send 2 with the
        // two halfword params below). It selects the store the ARM7 makes,
        // and the host does not need it -- each param is handled in its own
        // domain here -- so it is read out of the slot and otherwise unused.
        //
        // Param 9 is PAN, and the value is SIGNED: func_02048d80 derives it
        // from the listener-relative X as (dx >> 12) / 2 clamped to
        // -0x40..0x3f and hands it straight to func_0204f7cc, so 0 means
        // centre. Reading it as an absolute 0..127 put every centred sound
        // hard left.
        //
        // Params 0x0a and 0x0c are the per-track attenuation (tenths of a dB
        // out of data_02086384) and the per-track pitch (1/64 semitone). They
        // are the reason a rolling or moving sound effect changes as it moves
        // -- func_02048af4 and func_02012860 re-send both every frame beside
        // the pan -- and both used to reach note_param and be dropped. The
        // full derivation is on the definitions in sseq.cpp. THEY ARE NOT A
        // MUSIC PATH: no matched TU sends either one for a BGM layer, and the
        // sequence a layer plays is started by op 0x00 and nothing else.
        int slot = (n->a & 0xffffff) & 31;
        unsigned mask = (unsigned)n->b;
        if (n->c == 9) sd_seq_set_pan(slot, 64 + (int)(signed char)n->d);
        else if (n->c == 0x0a) sd_seq_set_track_volume_db10(slot, mask, n->d);
        else if (n->c == 0x0c) sd_seq_set_track_pitch(slot, mask, n->d);
        else note_param(4, n->c);
        break;
    }
    case 0x1b: case 0x1c: case 0x1d:
        // Load commands. Never expected: sdat_init pre-seats every FAT
        // residency slot with the resident address, so the game correctly
        // finds nothing to load. If one appears, the archive is already in
        // memory and there is nothing for the consumer to do.
        break;
    default:
        if (!g_sawOp[op & 0xff]) {
            g_sawOp[op & 0xff] = 1;
            fprintf(stderr, "[snd] command 0x%02x %s not implemented "
                    "(a=%08x b=%08x c=%08x d=%08x) -- skipped\n",
                    op, op_name(op), (unsigned)n->a, (unsigned)n->b,
                    (unsigned)n->c, (unsigned)n->d);
        }
        break;
    }
}

// Execute every batch the ARM9 has flushed but the consumer has not run.
void drain(void)
{
    while (g_consumed < (unsigned)(data_020a64a4 - 1)) {
        Node *head = (Node *)data_020a64a8[g_readIdx];
        if (++g_readIdx > 8) g_readIdx = 0;
        for (Node *n = head; n; n = n->next) exec(n);
        g_consumed++;
        g_status[0] = g_consumed;       // the word func_0205b5d4 reads
    }
}

// THE OTHER HALF OF THE STATUS BLOCK.
//
// SNDSharedWork is not one word. Word 0 is the progress counter this file
// already published; word 1 is the PLAYER BITMASK -- bit p set while ARM7
// player p is still holding a sequence -- and it is not decoration. It is the
// answer to "did that sound finish", and it is the only answer the ARM9 has:
// func_0205b608 reads it and func_0204fafc recycles, on that bit alone, every
// voice record whose player has gone quiet.
//
// Left at zero it reads as "no player is playing", which sounds harmless and
// is not: func_0204fafc is the ONLY caller of func_0204f2d4 that runs on a
// sound finishing, so with the word unpublished (or the function unrun) a
// voice is never given back. The 16 records and the 3D positional slots
// become one-way, and func_0204f364 and func_0204f63c spend the rest of the
// session in their steal-or-refuse paths.
void publish_player_status(void)
{
    g_status[1] = sd_seq_player_mask();
}

// The ARM9's own per-frame sound work, the same shape as sd_sound_init_host
// above: func_0204f03c is the ROM's sound frame, called from func_020132d8,
// and the port's frame loop has never reached either. Three of its five calls
// run here; the two that do not say why.
//
//   RUN  func_0205b274(0) loop   reclaim every command batch the consumer has
//                                finished, back onto the free list
//   RUN  func_0204fafc           the voice maintenance: for each active voice,
//                                confirm its START was consumed, then either
//                                recycle it (its player has gone quiet) or
//                                re-apply its distance volume and finish its
//                                fade
//   RUN  func_0205b070(0)        flush whatever the above queued
//
//   SKIP func_020508a0   the streamed-sound frame. It reads data_020a5634,
//                        which sd_sound_init_host leaves zeroed, and returns
//                        on its first line for that reason -- but it would
//                        also be driving a stream this port does not have:
//                        exec() has no handler for STRM_SETUP/STRM_PARAM and
//                        would print if one ever arrived.
//   SKIP func_020522c4   the four streamed-sound players, same reason, and
//                        their data_020a5bd4 record array is defined nowhere
//                        in the port because nothing has ever reached it.
//
// If the port grows streamed sound, those two come back here, not somewhere
// new.
//
// AND ONE CALL FROM ONE LEVEL UP. func_020132d8 is the game's sound frame and
// func_0204f03c is only its second-to-last line; the last is
// func_020119c8(data_0209b53c), the looping-handle reaper, and it belongs
// here for the same reason func_0204fafc does. func_0201226c hands a caller a
// handle and keeps the sound alive only while the caller keeps presenting it
// back; func_020119c8 is the half that notices when a caller stopped asking
// and stops the sound. Left out, no looping sound in the game is ever stopped
// on purpose. Every stop in an 1800-frame trace was a voice STEAL.
//
// Its position is the ROM's, after the flush rather than before: on hardware
// the reaper's stop commands go into the queue behind everything
// func_0204f03c already flushed, and the ARM7 picks them up on its own. Here
// sd_consumer_tick drains immediately after, which is the same order.
// AND THE HALF OF func_020132d8 THAT COMES BEFORE ALL OF THAT.
//
// func_0204f03c is only func_020132d8's second-to-last line. Everything above
// it in that function is the game's music/jingle crossfade, and the port has
// never run any of it. src/func_020132d8.cpp:22-39, in order:
//
//   RUN  the data_0209b480 gate       the master SFX flag. sd_sound_init_host
//                                     sets it, so this is not a no-op; it is
//                                     the ROM's own early-out and it is kept.
//   RUN  func_02012d64                ramp data_0209b490 toward
//                                     data_0208e42c<<12 and push it at the
//                                     main music player. Linked from src now
//                                     (slice_gate10.txt).
//   RUN  the data_0208e430 >= 0 arm   ramp data_0209b49c toward
//                                     data_0209b470<<12, push it at the jingle
//                                     player, and when it reaches 0 release
//                                     the latch (data_0208e430 = -1) and stop
//                                     the player. Written out here rather than
//                                     linked because func_020132d8.cpp cannot
//                                     be linked whole -- see the three SKIPs.
//
//   SKIP func_02013078   |            the underwater music filter, and it is
//   SKIP func_020494cc   |- one unit  one mechanism in three parts: 02013078
//   SKIP func_020490b0   |            reports a change in the player's swim
//                        state, 020494cc programs a two-channel ramp table
//                        (data_020a4c48/4c/54) from it, and 020490b0 advances
//                        that table and pushes it at the player. It is skipped
//                        as a UNIT and for one reason: func_020494cc reads
//                        data_02082200, _02082204, _02082208, _0208220c and
//                        _02082210, and not one of those five is defined
//                        anywhere in the port, so it would not link.
//                        Running only func_02013078 -- which does link --
//                        would be worse than skipping all three: it latches
//                        its answer in data_0208e438 and returns -1 on every
//                        later frame that agrees, so it would consume the
//                        state changes that the filter, once seated, needs to
//                        see. If the port grows the underwater filter, the
//                        three come back here together.
//
// This is why func_020132d8 is written out instead of linked: two of its seven
// calls have no port-side home, and its body calls them unconditionally.
//
// Its position is the ROM's. On hardware func_020132d8 is phase 9 of the main
// game loop (src/func_020197b8.c:60-63), the last thing in the frame after the
// VBlank wait, and these ramps are the FIRST thing in it -- so the volume
// commands they queue are flushed by the func_0205b070 below in the same
// frame, exactly as func_0204f03c flushes them on hardware.
void sd_sound_fade_host(void)
{
    if (data_0209b480 == 0)
        return;

    func_02012d64();

    if (data_0208e430 >= 0) {
        _Z14ApproachLinearRiii(&data_0209b49c[0],
                               (int)data_0209b470 << 12,
                               data_0209b494[0]);
        func_02013524(data_0209b4b0, data_0209b49c[0] >> 12, 0);
        if (data_0209b49c[0] == 0) {
            data_0208e430 = -1;
            func_0204fa2c(data_0209b4b0, 0);
        }
    }
}

void sd_sound_frame_host(void)
{
    sd_sound_fade_host();
    publish_player_status();
    while (func_0205b274(0) != 0)
        ;
    func_0204fafc();
    func_0205b070(0);
    func_020119c8(data_0209b53c);
}

// The game's own sound init, minus the four things that are hardware.
//
// func_020133bc is Sound::Init on the DS. It is not in any slice, and it
// cannot be called wholesale here, so this runs the six matched sub-inits
// that carry real state and skips the rest deliberately:
//
//   RUN  func_0204f1e8, func_02050950   counter resets
//   RUN  func_0204fc40                  builds the 16 voice records (the
//                                       +0x3c byte it stores is the voice id
//                                       every command carries) and the 32
//                                       player records with their default
//                                       playable-sequence limit of 1
//   RUN  func_0204f94c x3               clears the music, sub-music and SFX
//                                       player objects
//   RUN  func_02011a28                  Sound::PlayLong's handle table
//   RUN  func_02048f34                  the 3D voice pools
//   SET  data_0209b480 = 1              the master SFX flag. Without it
//                                       Player_PlaySoundEffect returns at its
//                                       first line and NOTHING makes a sound.
//
//   SKIP func_0205a82c   seeds the command pool via func_0205b358, which
//                        depends on three DS symbol adjacencies (see
//                        sd_consumer_init); this file seeds it instead.
//   SKIP func_02050f34   opens the SDAT off the card into a 1MB sound heap;
//                        hal/sdat/sdat.cpp seats an equivalent root already.
//   SKIP func_020134d8   loads group 1 into that heap; residency is
//                        pre-seated, so there is nothing to load.
//   SKIP func_020506fc   starts the ARM9 sound THREAD that would drain the
//                        queue. This consumer is that drain.
//
// If a sound plays that this init did not prepare for, the failure is a
// missing voice or a skipped command, both of which print -- not silence
// that pretends to be working.
void sd_sound_init_host(void)
{
    func_0204f1e8();
    func_02050950();
    func_0204fc40();
    func_0204f94c(&data_0209b4a0);
    func_0204f94c(&data_0209b4b0);
    func_0204f94c(&data_0209b4a4);
    func_02011a28(data_0209b53c);
    func_02048f34(data_0209b4b4);
    data_0209b480 = 1;
    fprintf(stderr, "[snd] sound init: 16 voices, 32 players, SFX enabled\n");
}

}  // namespace

// THE LEVEL-CHANGE LOOPING-SOUND REAP, and it is a different function from the
// per-frame one above. func_020119c8 (sd_sound_frame_host) only reaps a handle
// that was NOT refreshed this frame, so a looping sound whose owner is torn down
// mid-frame -- the wall it was sliding on, the enemy it was chasing -- keeps its
// +6 refreshed flag and survives the frame it should have died on. func_02011974
// is the ROM's answer: it walks the same 0x40-entry data_0209b53c table and stops
// EVERY live handle unconditionally, ignoring the refreshed flag. Compare
// func_020119c8 (skip if +6==1) with func_02011974 (stop if field_0 != 0) -- the
// second is the first with the refresh test removed.
//
// It is Scene::BeforeCleanupResources (_ZN5Scene22BeforeCleanupResourcesEv,
// vtable slot 4) that fires it: func_02011974(&data_0209b53c) is that override's
// whole body past the ActorBase base call. On the ROM the Scene actor is torn
// down and respawned per level, so slot 4 runs on every level change and every
// looping sound the old level started is stopped before the new level boots. The
// port keeps the Scene alive across levels (hal/level_change.cpp, "NOT torn down,
// deliberately"), so that slot never dispatches and the reap was lost --
// func_02011974 was matched src in no slice, called from nowhere, exactly like
// func_020119c8 was before the frame reaper was wired. hal/level_change.cpp calls
// this from its teardown, at the point the ROM's Scene::BeforeCleanupResources
// would have fired, before the new level boots.
extern "C" void sd_sound_level_reap(void)
{
    func_02011974(data_0209b53c);
}

/* Put the command queue back in its just-booted shape: free list relinked, ring
   empty, every cursor at zero.
 *
 * Called from sd_consumer_init at boot and from lk6_savestate_load on a restore.
 * The restore needs it because this queue is described TWICE -- by the DS-named
 * globals (data_020a6484 head, data_020a6494 tail, data_020a6498 read index,
 * data_020a64a0 in flight, ...) and by the host statics beside them (g_readIdx,
 * g_consumed) -- and the save state captures the first set but not the second.
 * Rolling back half of a cursor pair leaves the two disagreeing: the restored
 * read index walks a ring slot the host side has already reclaimed, and
 * func_0205b274's `while (*node)` chases a stale link into a null. Resetting
 * both sides together is the same treatment the sequencer and mixer already get
 * (sd_seq_reset / sd_mix_reset) and for the same reason: this is live plumbing,
 * not game state, and the game re-fills it on the next tick. */
void sd_consumer_reset(void)
{
    Node *pool = (Node *)data_020a6760;
    for (int i = 0; i < NODES - 1; i++) pool[i].next = &pool[i + 1];
    pool[NODES - 1].next = 0;
    data_020a6484 = &pool[0];
    data_020a6494 = &pool[NODES - 1];

    data_020a648c = 0;
    data_020a6490 = 0;
    data_020a64a0 = 0;
    data_020a6498 = 0;
    data_020a649c = 0;
    data_020a64a4 = 1;
    data_020a6488 = 0;

    memset(g_status, 0, sizeof g_status);
    data_020a7fc0 = g_status;
    g_consumed = 0;
    g_readIdx = 0;
}

void sd_consumer_init(void)
{
    if (g_seeded) return;
    g_seeded = 1;

    g_trace = getenv("SM64DS_SND_TRACE") != 0;
    /* the same switch arms Sound::Play's two cull reports (sound_abi.cpp) */
    g_snd_trace_play = g_trace;

    // Seed the free list. This is func_0205b358's data effect, written out
    // by hand on purpose: that function is unusable on the host because it
    // depends on three symbols being ADJACENT in DS memory --
    //   data_020a6760 (pool) + 0x1800 == data_020a7f60 (callback table),
    //   data_020a7760 + 0x7e8      == the pool's LAST node, and
    //   data_020a7f48              == that same last node,
    // which it uses to zero pool[255].next and to seat the tail pointer.
    // Host symbols are separate objects, so running it would write past two
    // of them. The three lines below are what it means, not where it wrote.
    sd_consumer_reset();

    // func_0205b358 would now send command 0x19 to tell the ARM7 where the
    // status block is. There is no message to send: this consumer owns the
    // block, so the handshake is a no-op rather than a fake.

    sdat_init();
    sd_mix_reset();
    sd_seq_reset();
    sd_sound_init_host();

    const char *wav = getenv("SM64DS_WAV_DUMP");
    if (wav) sd_wav_open(wav);

    fprintf(stderr, "[snd] hosted ARM7: %d-node command pool seeded, "
            "status block at %p\n", NODES, (void *)g_status);
}

void sd_consumer_tick(void)
{
    sd_consumer_init();
    // The ARM9 half: publish whatever is pending. Non-blocking (0) -- with
    // 0 this cannot re-enter func_0205b274, so it cannot re-enter the pump.
    func_0205b070(0);
    drain();
    sd_vtrace_arm9_census("after drain");
    sd_vtrace_loop_census("after drain");
}

// The seam. See the header comment for why this, and not a bigger pool.
// PORT_HOST_ABI: src reads the ARM7's cross-core progress word (data_020a7fc0);
//                the host has no second core, so this file pumps it instead.
extern "C" unsigned int func_0205b5d4(void)
{
    sd_consumer_init();
    if (!g_pumping) {
        g_pumping = 1;
        drain();
        g_pumping = 0;
    }
    return g_status[0];
}

extern "C" void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int seqId);
extern "C" void func_0205a8c4(void *c);   /* Snd_SendCommand(0x13, c, 0,0,0) */

/* ---- SM64DS_SND_COINPROBE=<period>: THE COIN, ON DEMAND -------------------
 *
 * "Coins do not play in the VS arenas" is a claim about ONE request, and the
 * only honest way to compare an arena against a normal level is to make the
 * two runs issue THE SAME request from the same place and read the two
 * verdicts. Collecting a real coin cannot be scripted identically in both, so
 * this fires the ROM's own coin call instead, unchanged:
 *
 *     Actor::GivePlayerCoins  ->  Sound::PlayBank3(0x11, actor + 0x74)
 *     (src/_ZN5Actor15GivePlayerCoinsER6Playerhj.c:44-48, and the three ov002
 *      collect paths func_ov002_020af684 / 020b16c4 / 020b1884 make the same
 *      two calls with the same two ids)
 *
 * at the PLAYER's own +0x74 -- the camera-space position Actor::BeforeBehavior
 * computes -- which is what a coin standing next to him would carry. Nothing
 * about the request is invented: same entry point, same kind, same id, same
 * kind of vector. 0x12 is the id the ROM uses when the player has the +0x706
 * byte set, so both are probed and the pair is what a real pickup would pick
 * between. Everything after the call is the game's.
 *
 * It rides sdat_host_tick because that is the one per-frame hook this file
 * already owns; the harness needs no change to arm it. */
extern "C" {
extern void *data_0209f394[];             /* per-player Actor* */
extern unsigned char data_0209f250;       /* local player index */
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *camSpacePos);
}

static void snd_coin_probe(void)
{
    static int period = -1;
    static int frame;
    if (period < 0) {
        const char *e = getenv("SM64DS_SND_COINPROBE");
        period = e ? atoi(e) : 0;
        if (period > 0)
            fprintf(stderr, "[coinprobe] armed: the ROM's own "
                    "Sound::PlayBank3(0x11/0x12, player+0x74) every %d "
                    "frames\n", period);
    }
    if (period <= 0) return;
    if (++frame % period) return;

    void *player = data_0209f394[data_0209f250];
    if (!player) {
        fprintf(stderr, "[coinprobe] frame %d: no local player yet\n", frame);
        return;
    }
    void *cam = (char *)player + 0x74;
    const int *c = (const int *)cam;
    fprintf(stderr, "[coinprobe] frame %d: player camera-space (%d,%d,%d)\n",
            frame, c[0] >> 12, c[1] >> 12, c[2] >> 12);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x11, cam);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x12, cam);
}

extern "C" void sdat_host_tick(void)
{
    sd_consumer_init();
    snd_coin_probe();
    // The ARM9's sound frame first, then the ARM7's: that is the order on
    // hardware (func_020132d8 -> func_0204f03c runs in the game's update, the
    // other core consumes after), and it matters here because the recycle and
    // the volume ramps func_0204fafc queues have to reach the same drain as
    // everything else the frame sent.
    sd_sound_frame_host();
    sd_consumer_tick();

    // SM64DS_SND_MUSIC=<seq id> starts a BGM through the game's OWN front
    // door on the first tick. The walking harness never reaches the level
    // boot's music call, so without this there is no way to exercise
    // LoadAndSetMusic_Layer1 -> func_02011dcc -> func_02051fb4 ->
    // func_02051bd0 -> START in the live binary. 58 is NCS_BGM_CHIJOU, the
    // castle grounds theme.
    static int musicDone;
    if (!musicDone) {
        musicDone = 1;

        // SM64DS_SND_QUEUE_STRESS=N pushes N commands in one burst, with no
        // tick in between, which is exactly the state that used to deadlock:
        // past 256 the free list is empty, func_0205b1d8 falls into
        //     do { func_0205b274(1); p = func_0205adf8(); } while (!p);
        // and func_0205b274(1) spins on a counter only the consumer moves.
        // If this returns for N > 256, the spin terminates. It hangs forever
        // if the func_0205b5d4 hook is ever removed.
        const char *stress = getenv("SM64DS_SND_QUEUE_STRESS");
        if (stress) {
            int n = atoi(stress);
            fprintf(stderr, "[snd] queue stress: pushing %d commands with no "
                            "consumer tick...\n", n);
            for (int i = 0; i < n; i++) func_0205a8c4((void *)(size_t)i);
            fprintf(stderr, "[snd] queue stress: all %d pushed, no deadlock "
                            "(pool is %d nodes)\n", n, NODES);
        }
        const char *m = getenv("SM64DS_SND_MUSIC");
        if (m) {
            fprintf(stderr, "[snd] SM64DS_SND_MUSIC=%s: "
                    "Sound::LoadAndSetMusic_Layer1(%d)\n", m, atoi(m));
            _ZN5Sound22LoadAndSetMusic_Layer1Ei(atoi(m));
            sd_consumer_tick();
        }
    }

    sd_out_push();
}
