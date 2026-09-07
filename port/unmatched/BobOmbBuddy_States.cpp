/* HOST COPIES of src/func_ov084_0212c960.cpp and src/func_ov084_0212c9a8.cpp
 * -- the BOB_OMB_BUDDY's state machine, and the mwcc pointer-to-member
 * dispatch for the ninth time in this port.
 *
 * The matched pair is
 *
 *     struct C; typedef void (C::*PMF)();
 *     struct Entry { PMF pmf[2]; };
 *     extern Entry data_ov084_02130dc4[];
 *     ...
 *     c->idx = i;  (c->*data_ov084_02130dc4[i].pmf[0])();     // change state
 *     ...          (c->*data_ov084_02130dc4[c->idx].pmf[1])(); // run it
 *
 * and `C` is INCOMPLETE where the typedef is formed. MSVC answers that with
 * the four-word general representation, so an Entry becomes 32 bytes where
 * mwcc's is 16 and state 2 reads out of the middle of state 0.
 *
 * The ROM's table is three states of two {function, delta} halves --
 * __sinit_ov084_02130558 copies six 8-byte statics into it -- so that is what
 * is read here, with the same virtual-bit test the Butterfly's matched source
 * spells out for itself.
 *
 * THE SIX STATICS ARE SEATED on both sides of the sinit, the treatment
 * port/unmatched/Enemy_UpdateDeath.cpp settled on: the sinit runs from the
 * window's boot and the registry runs later, so seating only the source side
 * would leave the copy full of DS addresses.
 *
 * STATES, in the order the sinit stores them:
 *   0  {0212c92c, 0212c8b0}   idle by the cannon, facing the player
 *   1  {0212c89c, 0212c508}   the talk, and the cannon-open path
 *   2  {0212c4a0, 0212c1a0}   walking back to the post
 *
 * All six halves are matched src, so this file carries no trap.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortBuddyPmf { unsigned fn; int delta; };
struct PortBuddyState { PortBuddyPmf init, main; };

/* the six ROM statics the sinit copies, and the bss table it copies into */
extern PortBuddyPmf data_ov084_021309dc[], data_ov084_021309ec[],
    data_ov084_021309cc[], data_ov084_021309e4[], data_ov084_021309c4[],
    data_ov084_021309d4[];
extern PortBuddyState data_ov084_02130dc4[];

void func_ov084_0212c92c(void *self);
void func_ov084_0212c8b0(void *self);
void func_ov084_0212c89c(void *self);
void func_ov084_0212c508(void *self);
void func_ov084_0212c4a0(void *self);
void func_ov084_0212c1a0(void *self);

}  /* extern "C" */

typedef void (*PortBuddyFn)(void *);

static const struct { PortBuddyPmf *slot; unsigned rom; PortBuddyFn host; }
g_buddy_states[] = {
    {data_ov084_021309dc, 0x0212c92c, func_ov084_0212c92c},
    {data_ov084_021309ec, 0x0212c8b0, func_ov084_0212c8b0},
    {data_ov084_021309cc, 0x0212c89c, func_ov084_0212c89c},
    {data_ov084_021309e4, 0x0212c508, func_ov084_0212c508},
    {data_ov084_021309c4, 0x0212c4a0, func_ov084_0212c4a0},
    {data_ov084_021309d4, 0x0212c1a0, func_ov084_0212c1a0},
};

static void port_buddy_seat_one(PortBuddyPmf *p, unsigned rom, PortBuddyFn host,
                                const char *where, unsigned i)
{
    if (p->fn == (unsigned)(size_t)host)
        return;
    if (p->fn != rom || p->delta != 0) {
        std::fprintf(stderr, "FATAL: ov084 state half %u (%s): the mount "
                     "holds %08x/%d, the ROM's own table says %08x/0 -- WRONG "
                     "BYTES\n", i, where, p->fn, p->delta, rom);
        std::abort();
    }
    p->fn = (unsigned)(size_t)host;
}

extern "C" void port_bob_omb_buddy_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < 6; ++i) {
        PortBuddyPmf *dst =
            (i & 1) ? &data_ov084_02130dc4[i / 2].main
                    : &data_ov084_02130dc4[i / 2].init;
        port_buddy_seat_one(g_buddy_states[i].slot, g_buddy_states[i].rom,
                            g_buddy_states[i].host, "static", i);
        port_buddy_seat_one(dst, g_buddy_states[i].rom, g_buddy_states[i].host,
                            "table", i);
    }
}

/* BOTH DISPATCHERS ARE BACK ON THE SLICE. src/func_ov084_0212c960.cpp and
   src/func_ov084_0212c9a8.cpp are on port/slice_pmf3.txt (run link100 lane
   PMF3). Under /vmg /vmm an Entry is the ROM's sixteen bytes rather than
   MSVC's thirty-two, the emitted bodies stride `shl edx, 4` and TAIL JUMP, and
   port/hal/pmf3_aliases.cpp bridges the mangled table name onto the mount's C
   symbol. The seat above is unchanged and is the gate: it aborts on a nonzero
   adjustment word and rewrites the function word with a host body on BOTH
   sides of the sinit, the six statics and the six destination halves, so what
   the matched dispatchers read is a host address with a zero delta. The six
   source pairs at ov084 0x021309c4..0x021309ec were re-read out of
   overlay_0084.bin with their relocations, every adjustment word ROM zero.

   port_buddy_call went with them -- it was the hand-rolled {function, delta}
   call and nothing else used it. It carried a virtual-member-pointer refusal
   that tested bit 0 of the FUNCTION word, which is not where the ROM keeps
   that flag: the ROM tests bit 0 of the ADJUSTMENT word (`ands r1,r1,#1` on
   the word it has just loaded from record+4). The seat's own abort covers
   that case properly and earlier, because it refuses any nonzero adjustment
   at all. port_buddy_seat_one, which the goomba half below still calls,
   stays. */

/* ---- and the GOOMBA's five, which need SEATING and nothing else ------------
   Goomba::Behavior dispatches data_ov084_02130d74[state * 2] the way the
   Butterfly's matched source does -- it reads the pair as two plain ints and
   does the virtual-bit and this-adjustment arithmetic itself -- so MSVC's
   pointer-to-member representation never comes into it and there is no host
   copy. What it does need is the same seat: __sinit_ov084_0213035c copies five
   8-byte statics into that table and every one of them is a DS code address.
   Measured: with the table unseated, spawning a goomba jumped to 0x0212b2dc,
   which is state 0's own body in the overlay image.

   STATES: 0 walk, 1 turn, 2 squashed, 3 knocked back, 4 the spawner's wait. */
extern "C" {
extern PortBuddyPmf data_ov084_021308bc[], data_ov084_021308b4[],
    data_ov084_021308c4[], data_ov084_021308ac[], data_ov084_021308cc[];
extern PortBuddyPmf data_ov084_02130d74[];

void func_ov084_0212b2dc(void *self);
void func_ov084_0212ab48(void *self);
void func_ov084_0212aab0(void *self);
void func_ov084_0212a774(void *self);
void func_ov084_0212a6f8(void *self);
}

static const struct { PortBuddyPmf *slot; unsigned rom; PortBuddyFn host; }
g_goomba_states[] = {
    {data_ov084_021308bc, 0x0212b2dc, func_ov084_0212b2dc},
    {data_ov084_021308b4, 0x0212ab48, func_ov084_0212ab48},
    {data_ov084_021308c4, 0x0212aab0, func_ov084_0212aab0},
    {data_ov084_021308ac, 0x0212a774, func_ov084_0212a774},
    {data_ov084_021308cc, 0x0212a6f8, func_ov084_0212a6f8},
};

extern "C" void port_goomba_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < 5; ++i) {
        port_buddy_seat_one(g_goomba_states[i].slot, g_goomba_states[i].rom,
                            g_goomba_states[i].host, "goomba static", i);
        port_buddy_seat_one(&data_ov084_02130d74[i], g_goomba_states[i].rom,
                            g_goomba_states[i].host, "goomba table", i);
    }
}
