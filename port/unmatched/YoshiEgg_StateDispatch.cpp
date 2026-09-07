/* THE SEAT for YOSHI_EGG's state table (actor 9, ov002 _ZTV8YoshiEgg /
 * _ZTV8daYegg_c 0x0210adb4). Run link100, lane UNMATCH. Until this lane the
 * file also held HOST COPIES of the two dispatchers -- the state SETTER
 * func_ov002_020ed63c and the per-frame MAIN dispatch func_ov002_020ed684 --
 * and they are retired: the ROM's own two TUs carry them now
 * (port/slice_unmatch2.txt).
 *
 * THE TABLE. data_ov002_02110a5c is ov002 bss, four 16-byte entries of two
 * mwcc {function, delta} pairs each, indexed by the egg's state word at
 * +0x3f0: pair 0 the ENTER half, pair 1 the MAIN half. __sinit_ov002_02107118
 * (slice_gate10, run by the harness at startup) copies the eight source pairs
 * at 0x0210ad38..0x0210ad70 into it, and those words are the image's own, DS
 * CODE ADDRESSES (the ovdata contract). Re-read for this lane out of
 * extracted/overlays/overlay_0002.bin at (addr - 0x020ad660), all eight deltas
 * ZERO:
 *
 *   state 0  enter 0x020ed5b0  main 0x020ed0d4   held in the laying Yoshi's
 *                                                mouth, aimed (dsd misnamed
 *                                                the main daWarpkun_c::Kill)
 *   state 1  enter 0x020ecfc8  main 0x020ecf94   thrown, in flight
 *   state 2  enter 0x020ecad4  main 0x020ec9c4   landed / breaking
 *   state 3  enter 0x020ec978  main 0x020ec938   the blue-coin variant's rest
 *
 * ---- WHY THE HOST COPIES ARE GONE -----------------------------------------
 * The old header said mwcc's eight-byte {code, adjustment} pair "has no MSVC
 * equivalent". That was written before the port built with /vmg /vmm. MEASURED
 * on 2026-09-07 with the port's own flags out of build.ninja,
 * src/func_ov002_020ed684.cpp:
 *
 *     mov  eax, DWORD PTR _c$[ebp]
 *     mov  edx, DWORD PTR [eax+1008]                       ; the state, +0x3f0
 *     shl  edx, 4                                          ; the ROM's stride
 *     mov  ecx, DWORD PTR ?data_ov002_02110a5c@@3PAUEntry@@A[edx+12]
 *     add  ecx, eax
 *     mov  eax, DWORD PTR ?data_ov002_02110a5c@@3PAUEntry@@A[edx+8]
 *     pop  ebp
 *     jmp  eax
 *
 * -- the ROM's own 16-byte element, the ROM's own second pair at +8/+12, and a
 * TAIL JUMP, which pops the frame before jumping so the callee's [esp+4] is
 * still this function's own `c`. That is where a plain cdecl ov002 state body
 * reads its receiver, and ecx carries it as well. The setter reads [edx]/[edx+4]
 * the same way. The name bridge those two mangled references need already
 * exists: hal/cxx_aliases.cpp:1180 spells
 * ?data_ov002_02110a5c@@3PAUEntry@@A=_data_ov002_02110a5c.
 *
 * So nothing about the dispatch needed a copy. What it needed is a host
 * address in the word, which is what this file is now.
 *
 * ---- WHY THE RUNTIME TABLE AND NOT THE SOURCE PAIRS ------------------------
 * The gate-178 shape seats the .data SOURCE pairs before the sinit copies
 * them. That is not available here: __sinit_ov002_02107118 runs from the
 * harness at startup (tests/walk_window.cpp, tests/smoke_player.cpp), long
 * before any actor overlay pass, and data_ov002_0210ad38 is mounted eight
 * bytes wide -- one pair, not eight. So this seat rewrites the DESTINATION,
 * data_ov002_02110a5c, from port_actor_overlays_sinits() at level boot. That
 * ordering is safe by construction: a YoshiEgg only exists inside a level, and
 * a level boot is what calls that pass (hal/level_boot.cpp).
 *
 * Each word is matched three ways -- the ROM's address (seat it), the host
 * address already written (nothing to do), anything else (abort by name) -- so
 * the seat is idempotent and a .dsstate rollback that puts DS addresses back is
 * repaired by the next pass instead of aborting on its own second run.
 *
 * ---- THE TRACE MOVED, IT DID NOT GO -------------------------------------
 * port/tools/yoshi_egg_throw_proof.py reads one `[eggstate]` line per egg tick
 * and three of its six assertions rest on it. That line used to be printed by
 * the host copy of the main dispatcher, which no longer exists, so the four
 * MAIN-half records are seated through a one-line face that prints it and then
 * calls the body. The four ENTER-half records take the body directly -- there
 * is nothing to instrument on the way in and a face nobody needs is a face
 * that can be wrong. The line, the fields and the env knob are unchanged.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned code; int delta; };

/* the mount defines the storage; __sinit_ov002_02107118 fills it */
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

void port_yoshi_egg_states_seat(void);

}  /* extern "C" */

namespace {

/* TEST INSTRUMENT (SM64DS_EGG_TRACE=1), off by default: one line per egg tick
   naming the state, the target uniqueID at +0x410 and the re-home count at
   +0x41c. This is what lets a headless proof say "the egg was really thrown
   and really flew" rather than inferring it. Reads only. */
void egg_trace(void *sv)
{
    static int tr = -1;
    if (tr < 0)
        tr = std::getenv("SM64DS_EGG_TRACE") != 0;
    if (!tr)
        return;
    char *c = (char *)sv;
    std::fprintf(stderr, "[eggstate] self=%p state=%d target=%u hops=%d\n",
                 (void *)c, *(int *)(c + 0x3f0), *(unsigned *)(c + 0x410),
                 (int)*(unsigned char *)(c + 0x41c));
}

/* THE FOUR MAIN HALVES, FACED. A face is safe in either emitted shape: the
   matched dispatcher TAIL JUMPS, which leaves the receiver both at [esp+4] and
   in ecx, and __fastcall takes it from ecx and returns `ret 0`, which balances
   a jump into a reused frame exactly as it balances a call. */
void __fastcall egg_main_020ed0d4(void *s, void *) { egg_trace(s); func_ov002_020ed0d4((char *)s); }
void __fastcall egg_main_020ecf94(void *s, void *) { egg_trace(s); func_ov002_020ecf94((char *)s); }
void __fastcall egg_main_020ec9c4(void *s, void *) { egg_trace(s); func_ov002_020ec9c4((char *)s); }
void __fastcall egg_main_020ec938(void *s, void *) { egg_trace(s); func_ov002_020ec938((char *)s); }

struct EggRow { unsigned slot; unsigned rom; void *host; const char *what; };

const EggRow g_egg_rows[8] = {
    { 0, 0x020ed5b0u, (void *)&func_ov002_020ed5b0, "state 0 enter" },
    { 1, 0x020ed0d4u, (void *)&egg_main_020ed0d4,   "state 0 main"  },
    { 2, 0x020ecfc8u, (void *)&func_ov002_020ecfc8, "state 1 enter" },
    { 3, 0x020ecf94u, (void *)&egg_main_020ecf94,   "state 1 main"  },
    { 4, 0x020ecad4u, (void *)&func_ov002_020ecad4, "state 2 enter" },
    { 5, 0x020ec9c4u, (void *)&egg_main_020ec9c4,   "state 2 main"  },
    { 6, 0x020ec978u, (void *)&func_ov002_020ec978, "state 3 enter" },
    { 7, 0x020ec938u, (void *)&egg_main_020ec938,   "state 3 main"  },
};

} /* namespace */

/* Called from port_actor_overlays_sinits() in hal/actor_overlays.cpp. */
extern "C" void port_yoshi_egg_states_seat(void)
{
    for (int i = 0; i < 8; ++i) {
        PortPmf *p = &data_ov002_02110a5c[g_egg_rows[i].slot];
        unsigned host = (unsigned)(size_t)g_egg_rows[i].host;

        if (p->delta != 0) {
            std::fprintf(stderr, "FATAL: YoshiEgg %s: adjustment word is %d, "
                         "the ROM's own table says 0 -- that is the VIRTUAL arm "
                         "and no measurement covers it\n",
                         g_egg_rows[i].what, p->delta);
            std::abort();
        }
        if (p->code == host)
            continue;               /* already seated; a rollback re-ran us */
        if (p->code != g_egg_rows[i].rom) {
            std::fprintf(stderr, "FATAL: YoshiEgg %s: the table holds 0x%08x, "
                         "the ROM's own source pair says 0x%08x -- WRONG BYTES "
                         "(has __sinit_ov002_02107118 run?)\n",
                         g_egg_rows[i].what, p->code, g_egg_rows[i].rom);
            std::abort();
        }
        p->code = host;
    }
}
