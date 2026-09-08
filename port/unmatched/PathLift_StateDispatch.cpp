/* HOST COPIES of src/func_ov002_020efa54.cpp (the state installer) and
 * src/_ZN8PathLift12BaseBehaviorEv.cpp (the per-frame tick dispatcher) --
 * the PathLift base branch's own 3-state machine, shared by PATH_LIFT (31,
 * ov100) and, when its overlay mounts, FLYING_CARPET (130, ov036).
 * The PushBlock/Unagi/MrBlizzard/BabyPenguin/HootTheOwl shape: a REAL C++
 * pointer-to-member dispatched through a DELIBERATELY INCOMPLETE class.
 *
 * THE RECORD WIDTH IS NOT INFERRED. Both dispatchers literally
 *
 *     mov r0, #0x14 ; mla r0, r1, r0, r2 ; add r3, r0, #8
 *     ldr r1, [r3, #4] ; add r0, r4, r1, asr #1 ; ands r1, r1, #1
 *     ldrne r2, [r0] ; ldrne r1, [r3] ; ldrne r1, [r2, r1]
 *     ldreq r1, [r3] ; blx r1
 *
 * (0x020efaa0 and 0x020efa54, disassembled in this lane), which spells out
 * BOTH the 20-byte stride AND the mwcc PMF contract: an 8-byte
 * { word0, word1 } pair where word1 is (this-adjust << 1) | isVirtual and
 * word0 is either the code address (non-virtual) or the vtable BYTE OFFSET
 * (virtual). The installer takes the pair at record+0, the tick dispatcher
 * the pair at record+8, and the last word of the record is a name string.
 *
 * MSVC widens a pointer-to-member over an INCOMPLETE class to 16 bytes, so
 * the matched TUs' own struct declarations (`{char pad[8]; PLFn fn; char
 * tail[4];}` and `{PMF pmf; char pad[0xc];}`, both 20 bytes under mwcc and
 * both byte-matched) come out 28 and 32 wide -- wrong stride, wrong offsets,
 * wrong decoding. Both sites are host copies, translated field for field
 * with the PMF call replaced by the same two-case dispatch the ROM does.
 *
 * THE TABLE, all six fn words reloc-confirmed out of __sinit_ov002_021071f4's
 * OWN source statics (that sinit is matched src, already linked via
 * slice_gate10.txt:432, and its banner already names data_ov002_0210af2c as
 * the destination -- dsd had merged it with PushBlock's under the 021097bc
 * name, which gate 200 caught and fixed):
 *
 *   rec  name(ROM string)      enter (rec+0)         tick (rec+8)
 *    0   0x0210aec0 "WAIT"     func_ov002_020ef3f0   func_ov002_020ef3ec
 *    1   0x0210aec8 "PATH"     func_ov002_020efa44   func_ov002_020ef670
 *    2   0x0210aeb8 "FALL"     func_ov002_020ef57c   func_ov002_020ef408
 *
 * Every source Pair's word1 is 0 in the ROM image, so every dispatch is the
 * plain non-virtual case; the copies below still implement the virtual half
 * so a future customer of the same table cannot be silently mis-served.
 * WAIT's tick is func_ov002_020ef3ec, which is literally `bx lr`.
 *
 * ORDER: the ov002 sinit chain runs long before any registry fill, so the
 * six dest words hold DS CODE ADDRESSES by the time pl_bringup runs. The
 * seat rewrites them there, with the Unagi WRONG-BYTES guard on every word
 * it overwrites -- a word that is not exactly what the reloc table promises
 * is a loud abort, never a silent wrong dispatch.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
struct PortPathLiftPair { unsigned fn; int delta; };
struct PortPathLiftEntry { PortPathLiftPair enter, tick; const char *name; };
extern PortPathLiftEntry data_ov002_0210af2c[3];

/* the six matched state bodies (port/slice_w6d.txt) */
void func_ov002_020ef3f0(void *c);   /* WAIT enter */
void func_ov002_020ef3ec(void *c);   /* WAIT tick  */
void func_ov002_020efa44(void *c);   /* PATH enter */
void func_ov002_020ef670(void *c);   /* PATH tick  */
void func_ov002_020ef57c(void *c);   /* FALL enter */
void func_ov002_020ef408(void *c);   /* FALL tick  */

void port_pathlift_states_seat(void);
void _ZN8PathLift12BaseBehaviorEv(void *c);
}

void port_pathlift_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    static const struct { unsigned rec, half, rom; void (*host)(void *); }
    seats[] = {
        {0, 0, 0x020ef3f0u, func_ov002_020ef3f0},
        {0, 1, 0x020ef3ecu, func_ov002_020ef3ec},
        {1, 0, 0x020efa44u, func_ov002_020efa44},
        {1, 1, 0x020ef670u, func_ov002_020ef670},
        {2, 0, 0x020ef57cu, func_ov002_020ef57c},
        {2, 1, 0x020ef408u, func_ov002_020ef408},
    };
    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        PortPathLiftPair *p = seats[i].half
            ? &data_ov002_0210af2c[seats[i].rec].tick
            : &data_ov002_0210af2c[seats[i].rec].enter;
        if (p->fn != seats[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: PathLift state %u/%u: the sinit left "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", seats[i].rec, seats[i].half, p->fn,
                         p->delta, seats[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)seats[i].host;
    }
}

/* The two-case dispatch both bodies share, spelled once. `pair` is the mwcc
   PMF: delta>>1 adjusts `this`, delta&1 selects the virtual path, and in the
   virtual path word0 is a BYTE OFFSET into the adjusted object's vtable.

   A NULL fn IS FATAL, not skippable. The ROM does an unconditional blx here,
   so a zero would be a jump to zero on the DS; a host `if (fn)` would turn
   that into a silently skipped state tick, which is the one failure this file
   promises never to produce. Loud, named, and stopped -- the ov60_hole shape
   in port/unmatched/Ov060_StateDispatch.cpp. The seat above verifies every
   record against the ROM before any dispatch can run, so reaching this abort
   means something rewrote the table after the seat. */
static void pl_pmf_call(char *c, const PortPathLiftPair *pair)
{
    char *self = c + (pair->delta >> 1);
    unsigned fn = pair->fn;
    if (pair->delta & 1) {
        char *vt = *(char **)self;
        fn = *(unsigned *)(vt + fn);
    }
    if (!fn) {
        std::fprintf(stderr, "FATAL: PathLift state dispatch: null function "
                     "for record {fn=%08x, delta=%d}%s -- the ROM does an "
                     "unconditional blx here, so this is a wrong table, not a "
                     "no-op\n", pair->fn, pair->delta,
                     (pair->delta & 1) ? " (virtual path)" : "");
        std::abort();
    }
    ((void (*)(void *))(size_t)fn)(self);
}

/* func_ov002_020efa54 RETIRED (run link100, lane PMFB1).
   src/func_ov002_020efa54.cpp carries it on port/slice_pmfc.txt. Both halves
   of this file's reading were half right: /vmg /vmm target-wide (block R8)
   already made the pointer-to-member the ROM's eight-byte {fn, delta} pair,
   so the "16-byte incomplete-class PMF" is dead -- but the 20-BYTE RECORD
   STRIDE the header names was still wrong, because MSVC gives a struct that
   contains a pointer-to-member eight-byte alignment and rounds twenty up to
   twenty-four. A per-TU /Zp4 (block R9d in port/CMakeLists.txt) makes the
   matched TU stride the ROM's 0x14 exactly.
   port_pathlift_states_seat above STAYS, and so does the static pl_pmf_call
   below it: _ZN8PathLift12BaseBehaviorEv, the per-frame half, is CALL-shaped
   under MSVC and is still hosted here. */

/* PORT_HOST_ABI: same PMF ruling, the per-frame half. Reads the current
   state index back, dispatches that record's "tick", then clears the
   collision flag at +0x42a -- the ROM's order (blx, then strb 0). */
void _ZN8PathLift12BaseBehaviorEv(void *c)
{
    char *p = (char *)c;
    pl_pmf_call(p, &data_ov002_0210af2c[*(int *)(p + 0x44c)].tick);
    *(unsigned char *)(p + 0x42a) = 0;
}

/* THE FACE. src/func_ov100_021470f4.cpp calls the tick half as a real C++
   method on its own local `struct PathLift { void BaseBehavior(); };`, so
   MSVC wants ?BaseBehavior@PathLift@@QAEXXZ. A @@QAE target can never be an
   /alternatename (the receiver would never have been a `this`), so this is a
   two-line forwarder. It lives here rather than in
   hal/actor_classes_ov100pl.cpp because that file includes PathLift.h, whose
   generated declaration does not carry BaseBehavior; the mangled name only
   depends on the class and method names, and the local declaration below is
   character-for-character the caller's own. */
struct PathLift { void BaseBehavior(); };
void PathLift::BaseBehavior()
{ _ZN8PathLift12BaseBehaviorEv(this); }
