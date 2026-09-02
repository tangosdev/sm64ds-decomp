/* KOOPA_SHELL's PMF state table, seated the way BobOmbBuddy_States.cpp seats
 * the buddy's: the mounted ov102 data carries the state handlers as raw DS code
 * addresses, and nothing in the link NAMES them (they are reached only by
 * dispatching a pointer out of the state struct), so a bare spawn jumps into
 * unmapped memory. The class's Behavior reads a state out of +0x3ac and calls
 * its `fn` member; the four states are data_ov102_0214ea48/58/68/78, each a
 * pair of {init, main} halves, and __sinit_ov102_0214dfac copies them out of
 * eight 8-byte source statics (data_ov102_0214e5d4..e60c).
 *
 * SEATED ON BOTH SIDES OF THE SINIT (the treatment the other state seats in
 * this port settled on): the sinit runs from the window's boot and the registry
 * runs later, so the copy already holds DS addresses by the time this runs; and
 * seating only the copy would leave the source full of them if the sinit re-ran.
 *
 * The eight halves, in the sinit's own order (state.half = source -> handler):
 *   ea48.init = e5fc -> func_ov102_0214d0bc      ea48.main = e5dc -> 0214d044
 *   ea58.init = e5ec -> func_ov102_0214d020      ea58.main = e5d4 -> 0214cfe4
 *   ea68.init = e5e4 -> func_ov102_0214d1b8      ea68.main = e5f4 -> 0214d1b0
 *   ea78.init = e604 -> func_ov102_0214d148      ea78.main = e60c -> 0214d114
 * ea68.init (0214d1b8) is the state InitResources seeds first, so a bare spawn
 * faults there. All eight are matched src (slice_gate209), so no trap here.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct KshPmf { unsigned fn; int delta; };
struct KshState { KshPmf init, main; };

/* the eight ROM source statics the sinit copies, and the four runtime states */
extern KshPmf data_ov102_0214e5fc, data_ov102_0214e5dc,
    data_ov102_0214e5ec, data_ov102_0214e5d4,
    data_ov102_0214e5e4, data_ov102_0214e5f4,
    data_ov102_0214e604, data_ov102_0214e60c;
extern KshState data_ov102_0214ea48, data_ov102_0214ea58,
    data_ov102_0214ea68, data_ov102_0214ea78;

void func_ov102_0214d0bc(void); void func_ov102_0214d044(void);
void func_ov102_0214d020(void); void func_ov102_0214cfe4(void);
void func_ov102_0214d1b8(void); void func_ov102_0214d1b0(void);
void func_ov102_0214d148(void); void func_ov102_0214d114(void);

}  /* extern "C" */

typedef void (*KshFn)(void);

static void ksh_seat_one(KshPmf *p, unsigned rom, KshFn host,
                         const char *where, unsigned i)
{
    if (p->fn == (unsigned)(size_t)host)
        return;
    if (p->fn != rom || p->delta != 0) {
        std::fprintf(stderr, "FATAL: ov102 KoopaShell state half %u (%s): the "
                     "mount holds %08x/%d, the ROM's own table says %08x/0 -- "
                     "WRONG BYTES\n", i, where, p->fn, p->delta, rom);
        std::abort();
    }
    p->fn = (unsigned)(size_t)host;
}

extern "C" void port_koopa_shell_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    struct Row { KshPmf *src; KshState *state; int is_main; unsigned rom; KshFn host; };
    static const Row rows[8] = {
        {&data_ov102_0214e5fc, &data_ov102_0214ea48, 0, 0x0214d0bc, func_ov102_0214d0bc},
        {&data_ov102_0214e5dc, &data_ov102_0214ea48, 1, 0x0214d044, func_ov102_0214d044},
        {&data_ov102_0214e5ec, &data_ov102_0214ea58, 0, 0x0214d020, func_ov102_0214d020},
        {&data_ov102_0214e5d4, &data_ov102_0214ea58, 1, 0x0214cfe4, func_ov102_0214cfe4},
        {&data_ov102_0214e5e4, &data_ov102_0214ea68, 0, 0x0214d1b8, func_ov102_0214d1b8},
        {&data_ov102_0214e5f4, &data_ov102_0214ea68, 1, 0x0214d1b0, func_ov102_0214d1b0},
        {&data_ov102_0214e604, &data_ov102_0214ea78, 0, 0x0214d148, func_ov102_0214d148},
        {&data_ov102_0214e60c, &data_ov102_0214ea78, 1, 0x0214d114, func_ov102_0214d114},
    };
    for (unsigned i = 0; i < 8; ++i) {
        KshPmf *dst = rows[i].is_main ? &rows[i].state->main : &rows[i].state->init;
        ksh_seat_one(rows[i].src, rows[i].rom, rows[i].host, "static", i);
        ksh_seat_one(dst,         rows[i].rom, rows[i].host, "table",  i);
    }
}
