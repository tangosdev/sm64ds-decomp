// The src sound functions the host owns, each filtered out of
// SLICE10_CAM_SOURCES in port/CMakeLists.txt.
//
// Most of them are ARM argument ride-throughs. Five functions in the sound
// stack are declared in src/ with fewer
// parameters than their callers pass. That is not a decomp error -- on ARM
// the extra arguments sit in r1..r3 across a call that never names them, and
// the callee's own call passes them straight on. mwccarm reproduces the ROM
// bytes exactly that way, so the src stays as it is and the host spells the
// arguments out instead. Same pattern as the SharedFilePtr::Construct
// veneers in hal/cxx_aliases.cpp and the gate-14 ride-throughs in
// port/unmatched/.
//
// Each of these src files is filtered out of SLICE10_CAM_SOURCES in
// port/CMakeLists.txt; the bodies below are the same code with the riders
// named.
//
//   func_0204f600  (1 -> 4)  START: the sequence pointer, entry offset and
//                            bank all ride into Snd_SendCommand(0, ...).
//   func_0204f89c  (1 -> 2)  volume rides into Snd_SendCommand(3, ...).
//   func_0204f7cc  (1 -> 3)  pan mode + value ride into Snd_SendCommand(4).
//   func_0204f86c  (1 -> 3)  two riders into Snd_SendCommand(5, ...).
//   func_0204fa2c  (1 -> 2)  the fade length rides into func_0204f5a0.
//
// The rest are not ride-throughs; each says why below.
#include "sdat.h"

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char u8;

extern "C" {

void func_0205adc4(void *a, int b, int c, int d);
void func_0205ad24(int a, int b);
int  func_0205acac(int a, int b, int c);
void func_0205aaf4(void *a, int b, int c);
void func_0204f4bc(void *obj);
void *func_0205afb4(void);
void func_0204f5a0(u8 *thiz, int arg1);

// func_0204f600(thiz) on ARM; r1 = sequence data, r2 = entry offset,
// r3 = resident bank, all of which ride into func_0205adc4.
// PORT_HOST_ABI: ARM r1..r3 argument ride-through (1 named param -> 4).
int func_0204f600(void *thiz, int seqData, int entryOff, int bank)
{
    func_0205adc4((void *)(size_t)(unsigned)*(u8 *)((char *)thiz + 0x3c),
                  seqData, entryOff, bank);
    func_0204f4bc(thiz);
    *(void **)((char *)thiz + 0x30) = func_0205afb4();
    *(u8 *)((char *)thiz + 0x2c) = 1;
    return 1;
}

// func_0204f89c(c) on ARM; r1 = volume, riding into func_0205ad24's second
// parameter (which the src file's own declaration does not name either).
// PORT_HOST_ABI: ARM r1 argument ride-through (volume).
void func_0204f89c(char **c, int volume)
{
    u8 *p = (u8 *)*c;
    if (p) func_0205ad24(p[0x3c], volume);
}

// func_0204f7cc(c) on ARM; r1/r2 = the pan mode and value.
// PORT_HOST_ABI: ARM r1/r2 argument ride-through (pan mode + value).
void func_0204f7cc(char **c, int mode, int pan)
{
    u8 *p = (u8 *)*c;
    if (p) func_0205acac(p[0x3c], mode, pan);
}

// func_0204f86c(c) on ARM; r1/r2 ride into func_0205aaf4.
// PORT_HOST_ABI: ARM r1/r2 argument ride-through.
void func_0204f86c(char **c, int b, int d)
{
    u8 *p = (u8 *)*c;
    if (p) func_0205aaf4((void *)(size_t)(unsigned)p[0x3c], b, d);
}

// func_0204fa2c(p) on ARM; r1 = the fade length in frames, riding into
// func_0204f5a0's second parameter. 0 means stop now, non-zero ramps down.
// PORT_HOST_ABI: ARM r1 argument ride-through (fade length).
int func_0204fa2c(int *p, int fade)
{
    func_0204f5a0((u8 *)(size_t)(unsigned)*p, fade);
    return 0;
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
// RULED (w6-c item 3). This is the ride-through class, the same one the five
// functions at the top of this file are in, and it is documented as such
// rather than left in the replacement queue: the src is not wrong and there
// is nothing to replace it with. mwccarm passes kind and id in r0/r1 and the
// callee is declared (void), which is a spelling MSVC cannot reproduce at
// any optimisation level -- a cdecl host callee reads its own stack slots.
// The two host additions above it (sd_consumer_init and the null guard) do
// not change the ruling; they are why the ride-through is survivable on a
// host, and both are argued in their own comments.
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

// Sound::Player::SetPlayableSeqCount. Not a ride-through -- an ALIAS. The
// src writes *(u32 *)(data_020a4d84 + id * 0x1c), and on the DS
// data_020a4d84 is data_020a4d6c + 0x18, i.e. field +0x18 of the same
// 32-entry player array. Host symbols are separate objects, so the src
// version would drop the write into a different block from the one
// func_0204f63c reads it back out of -- and func_0204f63c uses that field as
// "how many sequences may this player run", so a lost write means it thinks
// the limit is 0 and evicts a voice on every single sound. Writing through
// data_020a4d6c keeps the two views aliased.
//
// RULED (w6-c item 3), and the comment above is right that it is not a
// ride-through, so the reason is spelled for what it is: two ROM symbols
// naming ONE array at a fixed 0x18 offset, which separate host objects
// cannot express. The src is correct about the ROM and unlinkable on a host
// for a reason that has nothing to do with argument passing.
//
// THE RETIREMENT RECIPE, since this one has a real one and the port already
// owns the machinery: give data_020a4d6c and data_020a4d84 adjacent grouped
// sections the way hal/level_boot.cpp's SAVEBLK macro puts the five-way
// split of data_0209caa0 back in ROM order, sized so data_020a4d84 lands at
// data_020a4d6c + 0x18. tools/ovdata.py --pack does the same thing per
// overlay symbol. With the two symbols genuinely overlapping, the matched TU
// links and this host body retires. It is a seat, not a ruling, so it wants
// its own lane rather than a line in this one.
extern unsigned char data_020a4d6c[];
// PORT_HOST_ABI: two ROM symbols over one array (data_020a4d84 IS
// data_020a4d6c + 0x18); separate host objects cannot alias.
void _ZN5Sound6Player19SetPlayableSeqCountEii(int playerId, int maxSeq)
{
    if (playerId < 0 || playerId >= 32) return;
    *(unsigned int *)(data_020a4d6c + playerId * 0x1c + 0x18) =
        (unsigned short)maxSeq;
}

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
