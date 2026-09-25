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

// Sound::Play IS THE ROM'S OWN BODY NOW (run linkfull wave 31, lane RS5A).
// This file carried a host copy of it. The copy's first reason was an ARM
// argument ride-through: the pre-sync src declared the resolver
// func_02050cdc(void) and let the kind and id ride in r0/r1. That spelling
// went at the 09-14 sync (the src calls func_02050cdc(j1, j2)), and main
// caca8949ed made the four Sound:: callers forward the kind and id, so
// src/_ZN5Sound4PlayEjjRK7Vector3.cpp compiles here unchanged. It was on
// port/slice_gate10.txt all along and /OPT:REF dropped it, because every
// caller spells the flat name below and only the copy defined it.
//
// What stays here is that FLAT NAME, as a bridge into the ROM body. It keeps
// the two host duties the copy carried and none of the game's logic:
//
//   1. sd_consumer_init, the idempotent self-seat. Not every harness has a
//      frame loop calling sdat_host_tick before its first sound (smoke_player
//      reaches Player::Behavior -> Sound::Play directly), and the resolver's
//      table walk reads data_020a5bb8 + 0x84 unconditionally, so an unseated
//      root is a null dereference rather than a quiet no-op. The same seat
//      hal/reverse_bridges.cpp puts in front of PlaySub, Play2D and
//      LoadAndSetMusic_Layer1.
//   2. The null-resolver guard. func_02050cdc returns 0 for an id whose group
//      was never loaded, and the ROM body's next line reads the entry's +5.
//      On hardware the game is never in that state; on the host that read is
//      a page fault inside whichever actor asked. The bridge asks the same
//      lookup first (three table reads, no side effects), and on 0 skips the
//      call with one line per id, exactly as the copy did. No measured run
//      takes this branch.
//
// SM64DS_SND_REQLOG=1 keeps its one line per request at the front door, with
// the inputs the decision is made on (the listener-relative vector, its
// distance, the limit, bank, group and the sound-effects switch). The copy
// also printed a verdict for each request, and SM64DS_SND_TRACE /
// SM64DS_VOICE_TRACE lit up its two culls; those were decided inside the
// copy's own branches, and the ROM body decides them inside matched callees
// (func_02048720, func_02048a1c) with no host function on the path, so they
// retired with it. A request line now ends "sent to the ROM body" or, for
// the guard, "DROPPED: no SEQARC entry".
struct Vector3 { int x, y, z; };
void *func_02050cdc(int kind, int idx);
int   func_02049018(int *v);          /* listener-relative distance */
extern int data_02099fac;             /* the 3D distance limit, romdata */

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

}  // extern "C"

namespace Sound {
/* src/_ZN5Sound4PlayEjjRK7Vector3.cpp: ?Play@Sound@@YAXIIABUVector3@@@Z */
void Play(unsigned int j1, unsigned int j2, const Vector3 &v);
}

extern "C" {

// PORT_HOST_ABI: the flat name every caller spells, with the self-seat and the
// null-resolver guard in front of the ROM's own Sound::Play (see above).
void _ZN5Sound4PlayEjjRK7Vector3(unsigned kind, unsigned id, struct Vector3 *v)
{
    if (g_snd_reqlog < 0)
        g_snd_reqlog = getenv("SM64DS_SND_REQLOG") != 0;
    sd_consumer_init();
    const unsigned char *s = (const unsigned char *)func_02050cdc((int)kind, (int)id);
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
    snd_req(kind, id, s[5], v, "sent to the ROM body");
    Sound::Play(kind, id, *v);
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
