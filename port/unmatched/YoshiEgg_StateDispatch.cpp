/* HOST COPIES of YOSHI_EGG's two pointer-to-member dispatch TUs (actor 9,
 * ov002 _ZTV8YoshiEgg / _ZTV8daYegg_c 0x0210adb4): the state SETTER
 * func_ov002_020ed63c (enter half) and the per-frame MAIN dispatch
 * func_ov002_020ed684. Nothing is stubbed: all eight state bodies are matched
 * src on slice_gate210.txt.
 *
 * THE TABLE. data_ov002_02110a5c is ov002 bss, four 16-byte entries of two
 * mwcc {function, delta} pairs each, indexed by the egg's state word at
 * +0x3f0: pair 0 the ENTER half, pair 1 the MAIN half. __sinit_ov002_02107118
 * (slice_gate10, run by walk_window at scene load) copies the eight source
 * pairs at 0x0210ad38..0x0210ad70 into it, and those words are the image's
 * own, DS CODE ADDRESSES (the ovdata contract), which is what the switch below
 * keys on. Read off the raw overlay image, delta 0 throughout:
 *
 *   state 0  enter 0x020ed5b0  main 0x020ed0d4   held in the laying Yoshi's
 *                                                mouth, aimed (dsd misnamed
 *                                                the main daWarpkun_c::Kill)
 *   state 1  enter 0x020ecfc8  main 0x020ecf94   thrown, in flight
 *   state 2  enter 0x020ecad4  main 0x020ec9c4   landed / breaking
 *   state 3  enter 0x020ec978  main 0x020ec938   the blue-coin variant's rest
 *
 * WHY A HOST COPY. Both matched TUs form the pointer-to-member over a
 * forward-declared `struct C;`, and mwcc's eight-byte {code, adjustment} pair
 * called through the ARM Itanium sequence has no MSVC equivalent (the
 * MgCurling / Bullet / Cap wall, port/mg_fanout_costs.txt section 4). The
 * gate-10 link carried src/func_ov002_020ed63c.cpp compiled as an MSVC
 * pointer-to-member over the same table -- a call through a raw DS word the
 * moment anything dispatched it; nothing did until this seat, and it leaves
 * that slice here. Each body below is the matched source's control flow line
 * for line with the PMF site read as the plain {code, 0} pair and routed
 * through the address switch (stategen's --emit-switch output for this
 * constructor, arity 0).
 *
 * THE GATE POLICY for a word the switch does not know: one line, an INERT
 * return, never an abort. A state that is not one of the eight is a mount or
 * constructor fault, not a game state, and the egg simply does not tick that
 * half that frame.
 */
#include <cstdio>

extern "C" {

struct PortPmf { unsigned code; int delta; };

/* the mount defines the storage; the ov002 constructor fills it */
extern PortPmf data_ov002_02110a5c[];

/* the eight matched state bodies, address order, (char *self) each */
void func_ov002_020ec938(char *c);
void func_ov002_020ec978(char *c);
void func_ov002_020ec9c4(char *c);
void func_ov002_020ecad4(char *c);
void func_ov002_020ecf94(char *c);
void func_ov002_020ecfc8(char *c);
void func_ov002_020ed0d4(char *c);
void func_ov002_020ed5b0(char *c);

}  /* extern "C" */

static void yegg_state_call(char *c, unsigned code, const char *half)
{
    switch (code) {
    case 0x020ec938: func_ov002_020ec938(c); return;
    case 0x020ec978: func_ov002_020ec978(c); return;
    case 0x020ec9c4: func_ov002_020ec9c4(c); return;
    case 0x020ecad4: func_ov002_020ecad4(c); return;
    case 0x020ecf94: func_ov002_020ecf94(c); return;
    case 0x020ecfc8: func_ov002_020ecfc8(c); return;
    case 0x020ed0d4: func_ov002_020ed0d4(c); return;
    case 0x020ed5b0: func_ov002_020ed5b0(c); return;
    default: {
        static int said;
        if (said < 8) {
            std::fprintf(stderr, "UNHOSTED: YoshiEgg state %d %s word 0x%08x "
                         "is not one of the eight handlers -- inert this "
                         "frame\n", *(int *)(c + 0x3f0), half, code);
            ++said;
        }
        return;
    }
    }
}

/* PORT_HOST_ABI: src/func_ov002_020ed63c.cpp, the setter. `c->idx = i;
   (c->*table[c->idx].pmf[0])();` with the pair read plain. */
extern "C" void func_ov002_020ed63c(void *cv, int i)
{
    char *c = (char *)cv;
    *(int *)(c + 0x3f0) = i;
    {
        int j = *(int *)(c + 0x3f0);
        yegg_state_call(c, data_ov002_02110a5c[2 * j].code, "enter");
    }
}

/* PORT_HOST_ABI: src/func_ov002_020ed684.cpp, the per-frame main half
   YoshiEgg::Behavior calls first. `(c->*table[c->idx].pmf[1])();` */
extern "C" void func_ov002_020ed684(void *cv)
{
    char *c = (char *)cv;
    int j = *(int *)(c + 0x3f0);
    yegg_state_call(c, data_ov002_02110a5c[2 * j + 1].code, "main");
}
