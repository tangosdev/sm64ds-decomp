/* HOST COPIES of the EXCLAMATION_SWITCH's (StarSwitch / daObjSwitch_c, actor
 * 11) two state dispatchers, and the seat of the table they read.
 *
 * StarSwitch::Behavior drives a five-state machine through data_ov002_0210e00c,
 * ten {function, delta} records that __sinit_ov002_02101588 copies out of the
 * statics at ov002 0x0210987c..0x021098c4. The records pair up: state j is
 * {init = record[2j], main = record[2j+1]}, five states, and the ROM reaches
 * them through two dispatchers:
 *
 *   func_ov002_020ba4d8(c, i)  sets c->idx = i and calls state i's INIT
 *   func_ov002_020ba520(c)     calls state c->idx's MAIN, every frame
 *
 * Both TUs are the OneUpMushroom case and must be host copies for the same two
 * reasons:
 *
 *   1. MSVC forms a pointer-to-member of the INCOMPLETE `struct C;` they both
 *      declare as the four-word general representation, which strides the table
 *      at 0x20 instead of 0x10 and dispatches a neighbour's body. Reading the
 *      record as a plain { function, 0 } pair (all ten deltas are zero, a
 *      complete class) and calling the function with `this` is the fix.
 *
 *   2. The words the sinit copied are the ov002 image's own -- DS CODE
 *      ADDRESSES, the ovdata contract. The seat rewrites each record's first
 *      word with its host body after checking it against the ROM address that
 *      body was compiled from, so a mount pointing at the wrong bytes says so
 *      instead of calling into the overlay image.
 *
 * The table is the switch's OWN -- only StarSwitch's methods reference
 * func_ov002_020ba4d8/020ba520 -- so there is no cross-class sharing to guard,
 * and all ten functions are matched src, so no state traps.
 *
 * ONE MOUNT FIX RODE IN WITH THIS. Registering the switch was the first thing
 * to read data_ov002_0210e00c, and it read back {0, a Bullet function} in its
 * first two records. The cause was an undersized host global three symbols
 * away: data_ov002_02111190 is ov002's LAST bss symbol, so its next-symbol
 * delta under-sized it to 4 bytes, but __sinit_ov002_02108094 writes it as a
 * 16-byte struct (the Bullet's own two-PMF state table) -- and MSVC had packed
 * it right before this table, so the twelve-byte overrun stomped records 0/1.
 * Pinning data_ov002_02111190:0x10 in ov002_syms.txt (its real ROM span to the
 * overlay's bss end) gives the sinit room and the stomp is gone. The seat's
 * ROM-check is what caught it.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {
/* the ten state halves, in record order */
void func_ov002_020ba3fc(void *);   /* [0] state 0 init */
void func_ov002_020ba3a8(void *);   /* [1] state 0 main */
void func_ov002_020ba4c0(void *);   /* [2] state 1 init (the shared empty init) */
void func_ov002_020ba2d0(void *);   /* [3] state 1 main */
void func_ov002_020ba2ac(void *);   /* [4] state 2 init */
void func_ov002_020ba1ac(void *);   /* [5] state 2 main */
/* [6] state 3 init is 020ba4c0 again */
void func_ov002_020ba0f8(void *);   /* [7] state 3 main */
/* [8] state 4 init is 020ba4c0 again */
void func_ov002_020ba0bc(void *);   /* [9] state 4 main */

struct PortSwitchPair { unsigned fn; int delta; };
extern PortSwitchPair data_ov002_0210e00c[];   /* ten flat records */

/* the switch's own idx field is at +0x340 (the incomplete C's `int idx`) */
}  /* extern "C" */

enum { PORT_SWITCH_RECS = 10 };

static const struct { unsigned rom; void (*host)(void *); }
g_switch_states[PORT_SWITCH_RECS] = {
    {0x020ba3fc, func_ov002_020ba3fc}, {0x020ba3a8, func_ov002_020ba3a8},
    {0x020ba4c0, func_ov002_020ba4c0}, {0x020ba2d0, func_ov002_020ba2d0},
    {0x020ba2ac, func_ov002_020ba2ac}, {0x020ba1ac, func_ov002_020ba1ac},
    {0x020ba4c0, func_ov002_020ba4c0}, {0x020ba0f8, func_ov002_020ba0f8},
    {0x020ba4c0, func_ov002_020ba4c0}, {0x020ba0bc, func_ov002_020ba0bc},
};

extern "C" void port_exclamation_switch_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (int i = 0; i < PORT_SWITCH_RECS; ++i) {
        if (data_ov002_0210e00c[i].fn != g_switch_states[i].rom ||
            data_ov002_0210e00c[i].delta != 0) {
            std::fprintf(stderr, "FATAL: ExclamationSwitch state record %d: the "
                         "sinit left %08x/%d, the ROM's own table says %08x/0 -- "
                         "WRONG BYTES\n", i, data_ov002_0210e00c[i].fn,
                         data_ov002_0210e00c[i].delta, g_switch_states[i].rom);
            std::abort();
        }
        data_ov002_0210e00c[i].fn =
            (unsigned)(size_t)g_switch_states[i].host;
    }
}

/* BOTH DISPATCHERS ARE BACK ON THE SLICE. src/func_ov002_020ba4d8.cpp and
   src/func_ov002_020ba520.cpp are on port/slice_pmf3.txt (run link100 lane
   PMF3). With /vmg /vmm target-wide MSVC's pointer-to-member is the ROM's
   8-byte {function, delta} record, so the table strides 0x10 the way the ROM
   does and the emitted body TAIL JUMPS through the record's function word.
   The only thing left was the NAME: those TUs spell the table outside
   extern "C", so the reference is mangled; port/hal/pmf3_aliases.cpp bridges
   ?data_ov002_0210e00c@@3PAUEntry@@A onto the mount's C symbol.
   The seat above is what makes it safe, and it is unchanged: it aborts on any
   nonzero adjustment word and rewrites every function word with a host body
   before a switch can spawn. The ten source pairs at ov002
   0x0210987c..0x021098c4 were also re-read out of overlay_0002.bin with their
   relocations and every adjustment word is ROM zero. */
