/* HOST COPIES of the UNAGI's (Unagi / eel, actor 242, ov016) state dispatch,
 * and the seat of the ten PMF halves the five dispatch cells read.
 *
 * Unagi drives a five-cell state machine through the bss cells
 * data_ov016_02114d8c / d9c / dac / dbc / d7c. Each cell is 16 bytes: an
 * {ENTER, MAIN} pair of mwcc { fn, delta } PMFs. func_ov016_02111bf0(c, &cell)
 * stores &cell at c+0x34c and dispatches the ENTER half (cell+0) once, on
 * SetState; Unagi::Behavior reads the stored pointer back and dispatches the
 * MAIN half (cell+8) every frame. Both are the Scuttlebug __thiscall/stride
 * trap and must be host copies:
 *
 *   - func_ov016_02111bf0 forms the ROM's `add r0,r0,r1,asr#1` PMF this-adjust
 *     + virtual-bit test over cell[0]; on MSVC the recovered `(c->**q)()` reads
 *     the wrong stride. Read cell+0 as a plain { fn, 0 } and call fn(this).
 *   - Unagi::Behavior inlines the same dispatch over cell+8 (the MAIN half,
 *     ROM offset +8 confirmed by disassembly: ldr r0,[r1,#8]; add r3,r1,#8;
 *     ldreq r1,[r3]; blx r1). So the whole Behavior is host-copied here (the
 *     LavaBubble/Amilift-Behavior treatment) with the dispatch respelled.
 *
 * All ten .delta halves are 0 (non-virtual complete-class form, per the overlay
 * bytes), so the host dispatch is just "read the fn word, call it with this".
 *
 * THE SEAT rewrites each of the ten SOURCE records' fn word with its host body
 * BEFORE __sinit_ov016_021136ec copies the ten 8-byte source values into the
 * five cells (the MontyMole/Scuttlebug order: seat the source, before the copy).
 * Nine of the ten are matched src (func_ov016_02111bac / 19ec / 1860 / 1758 /
 * 1994 / 18b4 / 1718 / 15c0 / 1534); the tenth, cell d7c's ENTER, is the tiny
 * 7-instruction path-arrival state-enter at 0x021115a4 that dsd mis-symbolized
 * as "BookSwitch_Spawn" -- it is NOT a factory, it sets a timer and a speed and
 * returns 1. Reproduced as unagi_state_arrive below.
 *
 * Each source's ROM fn is checked against the mount before it is overwritten, so
 * a mount pointing at the wrong bytes aborts instead of seating a stale address.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

/* the five 16-byte dispatch cells (bss, filled by __sinit_ov016_021136ec) */
struct PortPmf { unsigned fn; int delta; };
extern PortPmf data_ov016_02114d8c[2], data_ov016_02114d9c[2],
    data_ov016_02114dac[2], data_ov016_02114dbc[2], data_ov016_02114d7c[2];

/* the ten SOURCE records the sinit copies from (each { fn, 0 }); nine matched
   src, the tenth the mis-named 021115a4 path-arrival enter. Named for the seat
   check; addresses are the ov016 mount's own. */
extern PortPmf data_ov016_021148a0, data_ov016_021148a8;  /* d8c enter/main */
extern PortPmf data_ov016_021148b8, data_ov016_021148c0;  /* d9c enter/main */
extern PortPmf data_ov016_02114888, data_ov016_02114880;  /* dac enter/main */
extern PortPmf data_ov016_021148b0, data_ov016_02114890;  /* dbc enter/main */
extern PortPmf BookSwitch_SpawnInfo, data_ov016_02114878; /* d7c enter/main */

/* the nine matched state bodies (slice_gate188.txt) */
void func_ov016_02111bac(void *c);   /* d8c enter */
void func_ov016_021119ec(void *c);   /* d8c main  */
void func_ov016_02111860(void *c);   /* d9c enter */
void func_ov016_02111758(void *c);   /* d9c main  */
void func_ov016_02111994(void *c);   /* dac enter */
void func_ov016_021118b4(void *c);   /* dac main  */
void func_ov016_02111718(void *c);   /* dbc enter */
void func_ov016_021115c0(void *c);   /* dbc main  */
void func_ov016_02111534(void *c);   /* d7c main  */

/* the whole-Behavior call graph (everything except the inlined PMF dispatch) */
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cc);
void *_ZN5Actor10FindWithIDEj(unsigned id);
void *_ZN5Actor13ClosestPlayerEv(void *self);
int Vec3_Dist(const void *a, const void *b);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned a, unsigned b,
        const void *pos, const void *rot, int e, int f);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
void _ZN14BlendModelAnim7AdvanceEv(void *self);
void func_ov016_02111c40(void *c);
void func_ov016_02111284(void *c);
extern unsigned char data_0209f220;

}  /* extern "C" */

/* The d7c ENTER (0x021115a4, size 0x1c): sets the arrival timer and speed,
 * returns 1. Reproduced from the overlay bytes:
 *   add r1,r0,#0x100; mov r2,#0x64; strh r2,[r1]   -> *(u16*)(this+0x100)=100
 *   mov r1,#0x14000;  str r1,[r0,#0x98]             -> *(int*)(this+0x98)=0x14000
 *   mov r0,#1; bx lr                                -> return 1
 */
static void unagi_state_arrive(void *c)
{
    *(unsigned short *)((char *)c + 0x100) = 100;
    *(int *)((char *)c + 0x98) = 0x14000;
}

/* PORT_HOST_ABI: SetState + dispatch the ENTER half. The matched
 * func_ov016_02111bf0 forms the ROM PMF adjust over cell[0]; here cell+0 is a
 * plain { fn, 0 } and fn is called with `this`. Also stands in for
 * func_ov018_02111bf0, the cross-overlay alias typo in the recovered src,
 * bridged onto this body by port/unmatched/Jrb_Aliases.cpp's own
 * /alternatename pragma (forward-declared on the next line, matching
 * Jrb_Aliases.cpp's own (void*,void*) signature, so this tag documents
 * that bridged name too). */
extern "C" int func_ov018_02111bf0(void *c, void *cell);
/* func_ov016_02111bf0 IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov016_02111bf0.cpp back on port/slice_pmf2.txt (batch 2): with /vmg /vmm
   global MSVC's pointer-to-member IS the ROM's 8-byte {function, delta}
   pair, the matched TU compiles to the same tail jump this body was, and
   the seat in this file aborts the binary on a nonzero delta so the two
   agree word for word. The reading above is kept as the derivation. */
/* PORT_HOST_ABI: Unagi::Behavior, host copy. Only the PMF dispatch (the MAIN
 * half at cell+8) differs from the matched src; the rest is transcribed line
 * for line off _ZN5Unagi8BehaviorEv.cpp with raw offsets. */
extern "C" int _ZN5Unagi8BehaviorEv(void *self)
{
    char *t = (char *)self;

    DecIfAbove0_Short((unsigned short *)(t + 0x100));
    {
        PortPmf *cell = *(PortPmf **)(t + 0x34c);
        if (cell[1].fn)                         /* the MAIN half, cell+8 */
            ((void (*)(void *))(size_t)cell[1].fn)(self);
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, t + 0x110);
    *(short *)(t + 0x8c) = *(short *)(t + 0x92);   /* mAngleX = mPrevAngleX */
    *(short *)(t + 0x8e) = *(short *)(t + 0x94);   /* mAngleY = mPrevAngleY */
    *(short *)(t + 0x90) = *(short *)(t + 0x96);   /* mAngleZ = mPrevAngleZ */
    func_ov016_02111c40(self);

    unsigned id = *(unsigned *)(t + 0x49c);        /* mStarUniqueID (Unagi.h 0x49c) */
    if (id != 0) {
        void *found = _ZN5Actor10FindWithIDEj(id);
        if (found == 0) {
            *(unsigned *)(t + 0x49c) = 0;          /* clear_id */
        } else {
            if (*(int *)(t + 0x408) == 1) {        /* mVariant == 1 (0x408) */
                void *cp = _ZN5Actor13ClosestPlayerEv(self);
                if (cp != 0 &&
                    Vec3_Dist(t + 0x43c, (char *)cp + 0x5c) < 0xfa000) {
                    _ZN9ActorBase18MarkForDestructionEv(found);
                    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                        0xb2, *(unsigned char *)(t + 0x414) | 0x40,
                        t + 0x43c, 0, *(signed char *)(t + 0xcc) /*mAreaId*/, -1);
                    *(unsigned *)(t + 0x49c) = 0;
                }
            }
            if (*(unsigned *)(t + 0x49c) != 0) {
                *(int *)((char *)found + 0x5c) = *(int *)(t + 0x43c);
                *(int *)((char *)found + 0x60) = *(int *)(t + 0x440);
                *(int *)((char *)found + 0x64) = *(int *)(t + 0x444);
                *(short *)(t + 0x416) += 0x1000;
            }
        }
    }

    func_ov016_02111284(self);
    _ZN12CylinderClsn5ClearEv(t + 0x110);
    _ZN12CylinderClsn6UpdateEv(t + 0x110);
    if (data_0209f220 == 1 &&
        *(void **)(t + 0x34c) != (void *)&data_ov016_02114dbc) {
        _ZN12CylinderClsn5ClearEv(t + 0x150);
        _ZN12CylinderClsn6UpdateEv(t + 0x150);
    }
    _ZN14BlendModelAnim7AdvanceEv(t + 0x350);
    return 1;
}

static const struct { PortPmf *rec; unsigned rom; void (*host)(void *); }
g_unagi_sources[] = {
    {&data_ov016_021148a0, 0x02111bac, func_ov016_02111bac},  /* d8c enter */
    {&data_ov016_021148a8, 0x021119ec, func_ov016_021119ec},  /* d8c main  */
    {&data_ov016_021148b8, 0x02111860, func_ov016_02111860},  /* d9c enter */
    {&data_ov016_021148c0, 0x02111758, func_ov016_02111758},  /* d9c main  */
    {&data_ov016_02114888, 0x02111994, func_ov016_02111994},  /* dac enter */
    {&data_ov016_02114880, 0x021118b4, func_ov016_021118b4},  /* dac main  */
    {&data_ov016_021148b0, 0x02111718, func_ov016_02111718},  /* dbc enter */
    {&data_ov016_02114890, 0x021115c0, func_ov016_021115c0},  /* dbc main  */
    {&BookSwitch_SpawnInfo, 0x021115a4, unagi_state_arrive},  /* d7c enter */
    {&data_ov016_02114878, 0x02111534, func_ov016_02111534},  /* d7c main  */
};

extern "C" void port_unagi_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_unagi_sources /
                             sizeof g_unagi_sources[0]; ++i) {
        PortPmf *p = g_unagi_sources[i].rec;
        if (p->fn != g_unagi_sources[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: Unagi source %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta, g_unagi_sources[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_unagi_sources[i].host;
    }
}
