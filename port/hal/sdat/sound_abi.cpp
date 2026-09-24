// The src sound functions the host still owns, each filtered out of
// SLICE10_CAM_SOURCES in port/CMakeLists.txt.
//
// THE FIVE SOUND COMMAND VENEERS LEFT THIS FILE (run linkfull, lane RS3PORT).
// func_0204f600, func_0204f89c, func_0204f7cc, func_0204f86c and
// func_0204fa2c were hosted here as ARM argument ride-throughs: their src
// named fewer parameters than the callers pass, which only works while the
// extra arguments sit in r1..r3 across the call. The src now spells each of
// them with the arguments the ROM passes (main #3102 re-spelled
// func_0204f600 and func_0204fa2c; the other three already were), the
// matched TUs are byte-identical to the cartridge, and they compile here
// unchanged, so their filter lines came out of port/CMakeLists.txt and the
// game runs the ROM's own bodies.
//
// What is left is not that class, and each body below says why. The one
// ride-through still here is func_02009e70's call (3 -> 4): the stop fade
// rides into Sound_PlayIfNotActive.
#include "sdat.h"

#include <stdio.h>
#include <stdlib.h>

extern "C" {

int  Sound_PlayIfNotActive(int a, int b, int c, int d);

// src/func_02009e70.cpp declares Sound_PlayIfNotActive with three
// parameters and calls it with three; the definition takes four and
// stores the fourth as the effect's stop fade (func_0201186c ->
// func_02011b38 a5 -> entry+4 -> func_0204f5a0's arg1, where 0 stops
// the effect at once and non-zero ramps it down). The cartridge's
// fourth argument is the r3 = 0 it materialised two instructions
// earlier for the data_0209b000 = 0 store, so the camera's rotate
// effect is registered with fade 0. On the host the callee read an
// unwritten stack slot, and the effect kept sounding for whatever
// that word happened to say.
// PORT_HOST_ABI: ARM r3 argument ride-through (stop fade, 0).
int hal_Sound_PlayIfNotActive_ridethrough(int a, int b, int c)
{
    return Sound_PlayIfNotActive(a, b, c, 0);
}

// Sound::Play. Its src declares the resolver as func_02050cdc(void) and
// calls it with no arguments -- the kind and id are already in r0/r1 from
// Play's own frame, so on ARM they ride straight through. On the host the
// callee read whatever happened to be in the argument slots, returned 0, and
// the very next line (*(u8 *)(s + 5)) faulted on address 5. That was the
// first crash the unstubbed front door produced.
//
// The null check is a host addition, not the ROM's behaviour: func_02050cdc
// legitimately returns 0 for an id whose group was never loaded, and on
// hardware the game is never in that state. Printing once and returning is
// the honest answer instead of reproducing a crash the DS would not have.
//
// STALE AS OF THE 09-14 SYNC (run link100 wave 10, lane SMOKELINK5). RULED
// (w6-c item 3) read this as the ride-through class, the same one the five
// functions at the top of this file are in: the PRE-SYNC src declared the
// resolver `func_02050cdc(void)` and called it with no arguments, so kind and
// id rode through in r0/r1 from Play's own frame and a cdecl host callee had
// no spelling that reads them -- "the src is not wrong and there is nothing
// to replace it with" was true of THAT declaration.
//
// The synced src/_ZN5Sound4PlayEjjRK7Vector3.cpp no longer declares it that
// way. It now spells `extern "C" char* func_02050cdc(int a, int idx);` and
// calls `func_02050cdc(j1, j2)` with both arguments explicit, which is an
// ordinary two-int cdecl call MSVC reproduces with no ride-through at all --
// the (void) spelling this ruling turned on is simply gone from the source.
// The declaration two lines below this comment, `void *func_02050cdc(int
// kind, int idx);`, already matches the synced shape; only this prose still
// described the pre-sync one. Nothing here argues the whole front door is
// unride-through now (SetPlayableSeqCount's own w6-c note below is about a
// different symbol and a different reason, and is untouched), only that this
// one function's excuse for being a ride-through no longer holds against the
// current source. The two host additions below (sd_consumer_init and the
// null guard) are unaffected either way and are still argued in their own
// comments.
struct Vector3 { int x, y, z; };
void *func_02050cdc(int kind, int idx);
void *func_02048720(struct Vector3 *v, int kind, int id);
void  func_02048908(void *obj, int *p);
int   func_02048a1c(int *v, int kind, int id);
void  func_02048d80(void *obj, int *p);
int   func_02049018(int *v);          /* listener-relative distance */
void  Player_PlaySoundEffect(int x, unsigned a, unsigned b);
extern int data_0209b4a4[];
extern int data_02099fac;             /* the 3D distance limit, romdata */

// SM64DS_SND_REQLOG=1: ONE LINE PER REQUEST, WITH THE VERDICT ON THE SAME LINE.
//
// SM64DS_SND_TRACE already reports the two culls, but a cull report cannot
// answer the question a missing sound actually asks first: did the request
// HAPPEN. A level where a sound is absent because the game never asked for it
// and a level where it was asked for and dropped produce the SAME quiet trace,
// and telling those two apart is the whole opening move of a "sound X does not
// play here" hunt -- it halves the search space before any deeper reading.
//
// So this logs every arrival at the front door together with the inputs the
// decision is made on: the listener-relative vector the caller passed, the
// distance that vector works out to, the limit it is about to be compared
// against, and the bank and group in force. Off by default, latched once.
int g_snd_reqlog = -1;
extern signed char data_0208e428;        /* the bank every kind-3 sound rides */
extern unsigned char data_0209b47c;      /* the loaded sound group */
extern unsigned char data_0209b480;      /* master "sound effects on" */

static void snd_req(unsigned kind, unsigned id, int type,
                    const struct Vector3 *v, const char *verdict)
{
    if (g_snd_reqlog <= 0) return;
    const int *c = (const int *)v;
    fprintf(stderr, "[req] kind=%u id=0x%x type=%d cam=(%d,%d,%d) dist=%d "
            "limit=%d bank=0x%02x group=%d sfx=%d -> %s\n",
            kind, id, type,
            c ? c[0] >> 12 : 0, c ? c[1] >> 12 : 0, c ? c[2] >> 12 : 0,
            c ? func_02049018((int *)c) : -1, data_02099fac,
            (unsigned char)data_0208e428, (int)data_0209b47c,
            (int)data_0209b480, verdict);
}

// PORT_HOST_ABI: ARM r0/r1 argument ride-through into a (void)-declared
// resolver, plus a host null guard where the DS could not reach the state.
void _ZN5Sound4PlayEjjRK7Vector3(unsigned kind, unsigned id, struct Vector3 *v)
{
    if (g_snd_reqlog < 0)
        g_snd_reqlog = getenv("SM64DS_SND_REQLOG") != 0;
    // Self-initialise. Not every harness has a frame loop calling
    // sdat_host_tick -- smoke_player reaches Player::Behavior -> Sound::Play
    // directly -- and the table walkers below read data_020a5bb8 + 0x84
    // unconditionally, so an unseated root is a null dereference rather than
    // a quiet no-op. Idempotent and cheap after the first call.
    sd_consumer_init();
    unsigned char *s = (unsigned char *)func_02050cdc((int)kind, (int)id);
    if (s == 0) {
        static unsigned char seen[8][32];
        unsigned k = kind & 7, b = (id >> 3) & 31, m = 1u << (id & 7);
        if (!(seen[k][b] & m)) {
            seen[k][b] |= (unsigned char)m;
            fprintf(stderr, "[snd] Sound::Play(%u, %u): no SEQARC entry "
                    "(group not loaded?) -- skipped\n", kind, id);
        }
        snd_req(kind, id, -1, v, "DROPPED: no SEQARC entry");
        return;
    }
    // SM64DS_SND_TRACE also lights up the TWO SILENT RETURNS below. Both are
    // the ROM's own 3D culls -- func_02048720 answers "no free positional
    // voice for this priority", func_02048a1c answers "further away than this
    // sound's distance limit" -- and on the ROM they are ordinary. On the
    // port they were the shape of a whole class of bug: data_02099fac, the
    // default distance limit, was zeroed HAL storage rather than the ROM's
    // 550, so func_02048a1c culled EVERY positional sound in the game and
    // Sound::Play returned without a word. A cull that cannot be told apart
    // from silence is the one thing this path is not allowed to be.
    int t = s[5];
    if (t == 9 || t == 2) {
        void *r = func_02048720(v, (int)kind, (int)id);
        if (r == 0) {
            if (g_snd_trace_play)
                fprintf(stderr, "[snd] Play(%u, %u) type %d: no positional "
                        "voice free -- culled\n", kind, id, t);
            // func_02048720 is matched src and refuses for two reasons it
            // does not distinguish to its caller: the sound is further than
            // its limit, or every slot in its pool is held by something it
            // may not take. Print both inputs rather than guess -- the
            // census line right after says which pool is full.
            SD_VT("play REFUSED Sound::Play(%u, %u) type %d: no 3D slot "
                  "(distance %d, limit %d)\n", kind, id, t,
                  func_02049018((int *)v), data_02099fac);
            sd_vtrace_arm9_census("at the refusal");
            snd_req(kind, id, t, v, "DROPPED: no 3D slot (range or pool full)");
            return;
        }
        snd_req(kind, id, t, v, "accepted (positional)");
        Player_PlaySoundEffect((int)(size_t)r, kind, id);
        // func_0204f63c writes the voice it took back into the owner slot and
        // leaves it null if it could not get one (func_0204f934 has already
        // cleared whatever was there). That null is the only place the ARM9's
        // "no voice for you" answer is visible -- Sound::Play never looks at
        // a return value -- so it is the one worth naming.
        if (g_voice_trace && *(void **)r == 0) {
            sd_vtrace("play REFUSED Sound::Play(%u, %u) type %d: the ARM9 "
                      "voice pool gave out no voice\n", kind, id, t);
            sd_vtrace_arm9_census("at the refusal");
        }
        func_02048908(r, (int *)v);
        return;
    }
    if (func_02048a1c((int *)v, (int)kind, (int)id) == 0) {
        if (g_snd_trace_play)
            fprintf(stderr, "[snd] Play(%u, %u) type %d: out of range "
                    "-- culled\n", kind, id, t);
        SD_VT("play REFUSED Sound::Play(%u, %u) type %d: out of range\n",
              kind, id, t);
        snd_req(kind, id, t, v, "DROPPED: out of range");
        return;
    }
    snd_req(kind, id, t, v, "accepted");
    Player_PlaySoundEffect((int)(size_t)data_0209b4a4, kind, id);
    if (g_voice_trace && data_0209b4a4[0] == 0) {
        sd_vtrace("play REFUSED Sound::Play(%u, %u) type %d: the ARM9 voice "
                  "pool gave out no voice\n", kind, id, t);
        sd_vtrace_arm9_census("at the refusal");
    }
    func_02048d80(data_0209b4a4, (int *)v);
}

// Set from the consumer's own SM64DS_SND_TRACE read, so one variable arms
// both halves of the trace.
int g_snd_trace_play;

// Sound::Player::SetPlayableSeqCount LEFT THIS FILE (run linkfull, lane
// RS3PORT). Its host copy wrote through data_020a4d6c + id * 0x1c + 0x18
// because the src wrote through data_020a4d84, a second ROM name for the
// same word that separate host objects cannot alias. Main (#3102) spells
// the store as data_020a4d6c[index].mPlayableSeqCount over the recovered
// record in include/SoundPlayerRecord.h, one object on both machines, so the
// matched TU links through its row in hal/cxx_aliases.cpp instead. The host
// copy's index check went with it: every caller passes 2, 3, 9, a loop
// index below 0x20, or an id from data_0208e448's ten ROM records (all
// below 32), so it never fired.

// func_0203d974: "does anything still need loading off the card?" Every
// group-load seam in the sound stack asks it first. Two independent reasons
// the src version cannot run here, and they push the same way:
//
//   1. It reads *(u16 *)0x027ffc40 -- the DS boot indicator. (THIS HALF IS
//      NOW STALE and is kept because the file's reasoning was built on it:
//      it used to say the address was outside every region ntr maps, and it
//      WAS, until SHARED_BASE 0x027ff000 joined ntr/io.cpp's kRegions as a
//      fatal region. The read is a real read today. What it reads is 0, a
//      cartridge boot -- see the boot-indicator note at ntr/io.cpp's write --
//      so the DS-faithful answer is 0 and reason 2 below is what carries the
//      whole decision now.)
//   2. Even given the read, the DS-faithful answer is 0 -- and 0 is what sends
//      func_02011f7c into func_020510a4(data_0209b498, data_0209b484) with
//      data_0209b498 still null (plain BSS in hal/actor_vtables.cpp, because
//      sd_sound_init_host deliberately SKIPs func_02050f34, the 1MB sound heap
//      builder -- see the SKIP list in consumer.cpp). Null deref one call on.
//
// So 1 is not a convenience, it is the only workable answer. It is also the
// same decision consumer.cpp already made at init ("SKIP func_020134d8:
// residency is pre-seated, so there is nothing to load"), moved to the seam
// where the game asks the question rather than the one place it was skipped.
// sdat.cpp's RESIDENCY PRE-SEAT note is why it is TRUE and not just expedient:
// the whole archive is in memory and all 282 FAT residency slots point at it.
//
// WHAT 1 COSTS, so the next reader does not have to rediscover it:
//   - func_02011f7c still stores data_0209b478, the player voice-group byte.
//     Nothing in src/ READS that byte, so this does not make a character's
//     voice follow a swap; it means the swap does not crash.
//   - Sound::LoadGroupAndSetBank takes its other branch under 1 and returns
//     early for every group but 0x2f. That function is dead on this boot (the
//     kuppa/intro tails), so 1 makes it more inert, not less.
//
// THE DEFINITION LIVES IN hal/star_flow.cpp. The star stream hosted this
// same seam for the course boot (LoadGroupAndSetBank's first line) with the
// same answer for the same two reasons, and the consolidation keeps that one;
// a second `return 1` here was the six-stream merge's fourteenth collision,
// caught by the linker. This block stays because the voice-group note above
// is the character swap's story, not the course boot's.

}  // extern "C"
