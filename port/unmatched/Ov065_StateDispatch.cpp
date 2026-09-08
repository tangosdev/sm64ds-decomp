/* HOST SIDE of ov065's enemy state machines (run linkw wave 5, lane w5-B):
 * the seat that rewrites the THREE HMC classes' state-record source statics,
 * and host copies of the two one-line dispatch helpers.
 *
 * THE SHAPE (the UpDownLift / Crate / MrBlizzard class of trap, measured by
 * gate 173 and reconfirmed here): each ov065 enemy keeps a pointer to a
 * 16-byte state pair {recA, recB} in its object (+0x3bc Snufit, +0x420
 * Swoop), or indexes an 8-byte-stride record array directly (Dorrie,
 * data_ov065_0211d7fc[state]). The records are {fn, 0} pairs the class
 * sinits copy out of SOURCE statics in ov065 .data:
 *
 *   Snufit  __sinit_ov065_0211c110 copies 0x0211cb20..cb58 (8 records) into
 *           d680={cb20,cb40}  d650={cb28,cb50}  d670={cb30,cb58}
 *           d660={cb38,cb48} -- each table a pair of (enter-fn, per-frame-fn)
 *   Swoop   __sinit_ov065_0211c2a8 copies 0x0211cc20..cc58 (8) the same way
 *   Dorrie  __sinit_ov065_0211c440 copies 0x0211cd2c/cd1c/cd24 (3) into
 *           data_ov065_0211d7fc[0..2]
 *
 * Every record's fn word is an ov065 CODE address (read off
 * extracted/overlays/overlay_0065.bin; the ovdata pointer pass rebases only
 * data-covered words, so mounted code words keep their DS values). The
 * matched dispatchers cannot ride the slice either way:
 *
 *   1. func_ov065_0211691c / func_ov065_02117944 form their PMF type over a
 *      FORWARD-DECLARED class, which MSVC widens to the general 16-byte
 *      representation -- reading 16 bytes where the ROM laid 8 and running
 *      the general dispatch form over garbage adjustment fields (the
 *      gate-173 measurement, not a guess).
 *   2. The three Behaviors' dispatch sites have the same disease in milder
 *      forms (see each host copy's own banner).
 *
 * THE FIX, the gate-173 recipe verbatim: port_ov065_states_seat() rewrites
 * each SOURCE static's fn word with the host address of that state's MATCHED
 * body BEFORE the sinits copy them, checking the mounted word against the
 * ROM address first so a mount pointing at the wrong bytes aborts instead of
 * calling into the overlay image. The dispatchers below then read the
 * records as plain {fn, 0} and call fn(self) -- cdecl, self as the one
 * argument, exactly the shape every matched state body exports.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
/* Snufit's eight state bodies, all matched src (slice_w5b.txt) */
int func_ov065_0211672c(char *c);
short func_ov065_02116588(char *c);
int func_ov065_021168a8(char *c);
int func_ov065_02116328(char *c);
int func_ov065_021165d8(char *c);
int func_ov065_021162c0(char *c);
int func_ov065_02116364(char *c);
int func_ov065_02116744(char *c);
/* Swoop's eight */
int func_ov065_021177e4(char *c);
int func_ov065_021176fc(char *c);
int func_ov065_021178fc(char *c);
int func_ov065_02117624(char *c);
int func_ov065_02117780(char *c);
int func_ov065_02117404(char *c);
int func_ov065_02117888(char *c);
int func_ov065_021175b0(char *c);
/* Dorrie's three */
int func_ov065_021183c8(char *c);
int func_ov065_021182e4(char *c);
int func_ov065_02118634(char *c);

/* the mounted source statics (port/ov065_syms.txt) */
extern unsigned data_ov065_0211cb20[], data_ov065_0211cb28[],
    data_ov065_0211cb30[], data_ov065_0211cb38[], data_ov065_0211cb40[],
    data_ov065_0211cb48[], data_ov065_0211cb50[], data_ov065_0211cb58[];
extern unsigned data_ov065_0211cc20[], data_ov065_0211cc28[],
    data_ov065_0211cc30[], data_ov065_0211cc38[], data_ov065_0211cc40[],
    data_ov065_0211cc48[], data_ov065_0211cc50[], data_ov065_0211cc58[];
extern unsigned data_ov065_0211cd1c[], data_ov065_0211cd24[],
    data_ov065_0211cd2c[];
}

/* RUN link100, LANE FWD: DORRIE'S THREE CELLS HOLD __fastcall FACES NOW.
 * src/_ZN6Dorrie8BehaviorEv.cpp dispatches data_ov065_0211d7fc itself since
 * the host copy in port/unmatched/Dorrie_Behavior.cpp was retired, and a
 * matched TU dispatches a pointer to member as
 *     mov ecx, TAB[i*8+4] / mov eax, TAB[i*8] / add ecx, this / call eax
 * -- receiver in ecx, nothing pushed. The OTHER SIXTEEN ROWS DO NOT CHANGE:
 * they feed the two tables func_ov065_0211691c and func_ov065_02117944
 * dispatch, and those two TAIL JUMP (`jmp eax`), which leaves the caller's own
 * first argument in place at [esp+4] -- so a plain cdecl body is right there
 * and a face would be wrong. Dorrie's site is a CALL, not a jump, because the
 * method has work to do after it.
 */
#define DORRIE_FACE(cell, sym)                                            \
    static void __fastcall dorrie_c##cell(void *self, void *dead_edx)     \
    {                                                                     \
        (void)dead_edx;                                                   \
        sym((char *)self);                                                \
    }

DORRIE_FACE(0, func_ov065_021183c8)
DORRIE_FACE(1, func_ov065_021182e4)
DORRIE_FACE(2, func_ov065_02118634)

namespace {
struct SeatRow {
    unsigned *rec;        /* the mounted {fn, 0} source record */
    unsigned rom;         /* the fn word the ROM's own image carries  */
    int (*host)(char *);  /* the matched body compiled for the host   */
};
typedef int (*StateFn)(char *);

/* every fn word re-read from extracted/overlays/overlay_0065.bin for this
   lane (raw dwords at each record address), never carried from a comment */
const SeatRow g_ov065_states[] = {
    { data_ov065_0211cb20, 0x0211672c, func_ov065_0211672c },
    { data_ov065_0211cb28, 0x02116588, (StateFn)func_ov065_02116588 },
    { data_ov065_0211cb30, 0x021168a8, func_ov065_021168a8 },
    { data_ov065_0211cb38, 0x02116328, func_ov065_02116328 },
    { data_ov065_0211cb40, 0x021165d8, func_ov065_021165d8 },
    { data_ov065_0211cb48, 0x021162c0, func_ov065_021162c0 },
    { data_ov065_0211cb50, 0x02116364, func_ov065_02116364 },
    { data_ov065_0211cb58, 0x02116744, func_ov065_02116744 },
    { data_ov065_0211cc20, 0x021177e4, func_ov065_021177e4 },
    { data_ov065_0211cc28, 0x021176fc, func_ov065_021176fc },
    { data_ov065_0211cc30, 0x021178fc, func_ov065_021178fc },
    { data_ov065_0211cc38, 0x02117624, func_ov065_02117624 },
    { data_ov065_0211cc40, 0x02117780, func_ov065_02117780 },
    { data_ov065_0211cc48, 0x02117404, func_ov065_02117404 },
    { data_ov065_0211cc50, 0x02117888, func_ov065_02117888 },
    { data_ov065_0211cc58, 0x021175b0, func_ov065_021175b0 },
    /* DORRIE's three: __fastcall faces, run link100 lane FWD. The cast goes
       through void* because the column's type is the table's, not the face's;
       what the seat stores is an ADDRESS. */
    { data_ov065_0211cd1c, 0x021183c8, (StateFn)(void *)dorrie_c0 },
    { data_ov065_0211cd24, 0x021182e4, (StateFn)(void *)dorrie_c1 },
    { data_ov065_0211cd2c, 0x02118634, (StateFn)(void *)dorrie_c2 },
};
}  // namespace

extern "C" void port_ov065_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_ov065_states / sizeof g_ov065_states[0];
         ++i) {
        unsigned *r = g_ov065_states[i].rec;
        if (r[0] != g_ov065_states[i].rom || r[1] != 0) {
            std::fprintf(stderr, "FATAL: ov065 state %u: the mount holds "
                         "%08x/%08x, the ROM's own record says %08x/0 -- "
                         "WRONG BYTES\n", i, r[0], r[1],
                         g_ov065_states[i].rom);
            std::abort();
        }
        r[0] = (unsigned)(size_t)g_ov065_states[i].host;
    }
}

/* ---- func_ov065_0211691c, HOST COPY -----------------------------------------
 * Matched source: `c->pp = p; if (*q == 0) return 1; return (c->**q)();` with
 * PMF over a forward-declared class (the 16-byte trap above). ROM semantics,
 * read off the body at 0x0211691c: store the table pointer at self+0x3bc,
 * load the FIRST record's fn word, return 1 if null, else tail-call fn(self).
 * Snufit's Behavior reads the SECOND record of the same table (+8) -- the
 * pair is {enter/act, per-frame} -- so the two dispatchers together cover
 * both words the seat rewrote. */

/* func_ov065_0211691c IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov065_0211691c.cpp back on port/slice_pmf2.txt: with /vmg /vmm global (the
   R8 block in port/CMakeLists.txt) MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the matched TU compiles to the same
   tail jump this body was -- measured, listing in that slice's header.
   The reading above is kept because it is the derivation. */

/* func_ov065_02117944 IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov065_02117944.cpp back on port/slice_pmf2.txt: with /vmg /vmm global (the
   R8 block in port/CMakeLists.txt) MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the matched TU compiles to the same
   tail jump this body was -- measured, listing in that slice's header.
   The reading above is kept because it is the derivation. */
