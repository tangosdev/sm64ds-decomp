/* HOST COPIES for gate 178's two ov064 riders, METAL_NET_LIFT (69,
 * daObjFl_Amilift_c) and LAVA_BUBBLE (214, daBbl_c):
 *   - the Amilift Render func_ov064_02117cfc (the ModelAnim/Model slot-5 shadow
 *     collision, the Whomp/Scuttlebug case),
 *   - the two PMF-dispatching bodies of each class, and
 *   - the seat of the two source PMF tables their sinits copy into bss.
 *
 * WHY THESE ARE HOST COPIES, NOT SLICED
 * -------------------------------------
 * 1. func_ov064_02117cfc dispatches its model's slot 5 through a LOCAL six-
 *    virtual shadow struct whose sixth method takes an int (`Base *b = &d->base;
 *    b->m(0)`, Model at +0xd4). The host _ZTV5Model / _ZTV9ModelAnim are MSVC-
 *    ordered (one dtor slot where Itanium spends two), so ROM slot 5 lands on
 *    Virtual18 and reads a null matrix -> c0000005. The fix is to dispatch the
 *    qualified Model::Render, exactly as unmatched/ModelAnim_Renders.cpp does for
 *    Whomp/Bully/RotatingFirebar. (Kept in this file rather than appended to
 *    ModelAnim_Renders.cpp because gate 178 must add no lines to existing files.)
 *
 * 2. func_ov064_02117d24, func_ov064_021187ec and _ZN10LavaBubble8BehaviorEv all
 *    invoke an mwcc pointer-to-member: `(obj->*pmf)()`. On MSVC that is a
 *    __thiscall (this in ECX) against a plain extern-C state body that wants self
 *    on the stack -> calling-convention corruption. func_ov064_02117d24 also
 *    indexes `data_ov064_0211c750[idx]` with the mwcc 8-byte record stride that
 *    MSVC would make 4 (the Scuttlebug stride trap). Each is copied here reading
 *    the record as a plain { fn, delta } (delta 0, non-virtual complete-class)
 *    and calling fn with `this`.
 *
 * THE SEAT (the Scuttlebug/MontyMole "seat the source before the copy" order)
 * --------------------------------------------------------------------------
 * Both runtime tables are bss, filled at static-init:
 *   - __sinit_ov064_0211afc0 copies 3 SOURCE PMFs data_ov064_0211bc0c/14/1c into
 *     data_ov064_0211c750 (Amilift Behavior states, idx 0..2).
 *   - __sinit_ov064_0211b150 copies 4 SOURCE PMFs data_ov064_0211be90/98/a0/a8
 *     into data_ov064_0211c7b8[0..1] and data_ov064_0211c7c8[0..1] (LavaBubble).
 * The ovdata mount lays the source blocks down as raw overlay bytes; their fn
 * words point at CODE (func_ov064_*), which the mount's pointer pass does NOT
 * rebase (it only rebases pointers into other mounted DATA). So the raw words are
 * DS addresses. port_ov064_gate178_states_seat() rewrites each source fn word
 * with its host body BEFORE the two sinits run, validating each mounted word
 * against the ROM address first so a wrong mount says so instead of copying an
 * overlay-image address into the live table. All seven .delta halves are 0
 * (verified against the overlay image).
 *
 * The state bodies (func_ov064_02117c24/bdc/b8c, func_ov064_02118644/873c/8760/
 * 87d0) are plain matched src in slice_gate178.txt -- called here by explicit
 * host pointer with `this` as arg 0, which is what they expect.
 */
#include <cstdio>
#include <cstdlib>

#include "Model.h"

extern "C" {

/* ---- METAL_NET_LIFT (69) Render: the slot-5 collision -----------------------
   src/func_ov064_02117cfc.cpp: `Derived{ char pad[0xd4]; Base base; }` shadow,
   Model at +0xd4, dispatched as slot 5. Spelled qualified. */
/* PORT_HOST_ABI: ModelAnim/Model slot-5 shadow-vtable dispatch (MSVC folds
   the dtor slot, ROM slot 5 lands on Virtual18); the Whomp/Fish case. */
int func_ov064_02117cfc(void *selfv)
{
    /* PORT_HOST_ABI: ROM-order Model slot-5 dispatch, the Whomp/Fish case. */
    ((Model *)((char *)selfv + 0xd4))->Model::Render(0);
    return 1;
}

/* record layout of every ov064 gate-178 PMF table: the ROM's 8-byte { fn, delta }
   with delta 0. */
struct PortPmf { unsigned fn; int delta; };

/* ---- METAL_NET_LIFT (69) Behavior: the 3-state PMF machine ------------------
   func_ov064_02117d24 line for line, only the `(c->*table[idx].pmf)()` dispatch
   spelled as an explicit host call on the 8-byte record. */
extern PortPmf data_ov064_0211c750[3];      /* bss runtime table, sinit-filled */
extern short data_02082214[];                /* arm9 sin/cos-ish table (already linked) */
int _Z14ApproachLinearRiii(int *v, int target, int step);
void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch over an 8-byte {fn,delta}
   record (the Scuttlebug stride trap); MSVC would make the stride 4. */
int func_ov064_02117d24(void *cv)
{
    char *c = (char *)cv;
    int idx = *(unsigned char *)(c + 0x33b);
    /* PORT_HOST_ABI: mwcc `(c->*data_ov064_0211c750[idx].pmf)()` with the 8-byte
       record stride; call fn(this). */
    ((void (*)(void *))(size_t)data_ov064_0211c750[idx].fn)(c);
    unsigned short *p338 = (unsigned short *)(c + 0x338);
    *p338 = (unsigned short)(*p338 + 1);
    if (idx != *(unsigned char *)(c + 0x33b))
        *(short *)(c + 0x300 + 0x38) = 0;
    int target = *(unsigned char *)(c + 0x33a) ? -0x28000 : 0;
    if (_Z14ApproachLinearRiii((int *)(c + 0x320), target, 0x5000)) {
        short *pAng = (short *)(c + 0x328);
        *pAng = (short)(*pAng + 0xa00);
        unsigned short h = *(unsigned short *)(c + 0x300 + 0x28);
        short s = data_02082214[(h >> 4) * 2];
        *(int *)(c + 0x324) = s * 10;
    }
    int t330 = *(int *)(c + 0x330);
    int t320 = *(int *)(c + 0x320);
    int t324 = *(int *)(c + 0x324);
    int saved = *(int *)(c + 0x60);
    *(int *)(c + 0x60) = t324 + (t330 + t320);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    *(int *)(c + 0x60) = saved;
    *(unsigned char *)(c + 0x33a) = 0;
    return 1;
}

/* ---- LAVA_BUBBLE (214) seeder-dispatcher func_ov064_021187ec ----------------
   src: `c->pp = p; if (*p == 0) return 1; return (c->**p)()` -- store the table
   base at +0x300, call record[0] (the ENTER state). The record is 8 bytes. */
/* func_ov064_021187ec IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov064_021187ec.cpp back on port/slice_pmf2.txt (batch 2): with /vmg /vmm
   global MSVC's pointer-to-member IS the ROM's 8-byte {function, delta}
   pair, the matched TU compiles to the same tail jump this body was, and
   the seat in this file aborts the binary on a nonzero delta so the two
   agree word for word. The reading above is kept as the derivation. */
/* ---- LAVA_BUBBLE (214) Behavior _ZN10LavaBubble8BehaviorEv ------------------
   The matched .cpp line for line, with the two `(this->*(m->pmf))()` dispatches
   (m = the table base stored at +0x300, m->pmf = record[1].fn, the MAIN state)
   spelled as explicit host calls. */
int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void *self, int d);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
unsigned short DecIfAbove0_Short(unsigned short *p);
void *_ZN5Actor10FindWithIDEj(unsigned int id);
void _ZN6Player4BurnEv(void *self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm, unsigned int j);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch; MSVC's PMF over an
 * incomplete class is the wider general representation. See the header. */
int _ZN10LavaBubble8BehaviorEv(void *cv)
{
    char *c = (char *)cv;
    int flags = *(int *)(c + 0x0b0);
    int b20 = (flags & 0x20000) != 0;

    if (b20) {
        PortPmf *m = *(PortPmf **)(c + 0x300);
        if (m[1].fn != 0)
            /* PORT_HOST_ABI: mwcc `(this->*(m->pmf))()`, m->pmf = record[1].fn */
            ((void (*)(void *))(size_t)m[1].fn)(c);
        return 1;
    }

    if ((flags & 0x40000) != 0)
        return 1;

    if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(c, 0x5dc000)) {
        if (*(unsigned char *)(c + 0x310) != 0)
            _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));

    {
        unsigned int id = *(unsigned int *)(c + 0x134);
        if (id != 0) {
            if ((*(int *)(c + 0x130) & 0x8000) == 0) {
                char *a = (char *)_ZN5Actor10FindWithIDEj(id);
                if (a != 0 && *(unsigned short *)(a + 0xc) == 0xbf)
                    _ZN6Player4BurnEv(a);
            } else {
                *(int *)(c + 0x128) |= 1;
            }
        }
    }

    {
        PortPmf *m = *(PortPmf **)(c + 0x300);
        if (m[1].fn != 0)
            /* PORT_HOST_ABI: mwcc `(this->*(m->pmf))()`, m->pmf = record[1].fn */
            ((void (*)(void *))(size_t)m[1].fn)(c);
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x110);
    if (*(int *)(c + 0x09c) != 0)
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x144, 0);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    return 1;
}

/* ---- THE SEAT ---------------------------------------------------------------
   The seven source PMF fn words, by ROM address, and the host bodies that
   replace them. Rewritten BEFORE __sinit_ov064_0211afc0 / __sinit_ov064_0211b150
   copy them into the bss runtime tables. */
int func_ov064_02117c24(void *c);   /* Amilift state 0 */
int func_ov064_02117bdc(void *c);   /* Amilift state 1 */
int func_ov064_02117b8c(void *c);   /* Amilift state 2 */
int func_ov064_02118760(void *c);   /* LavaBubble c7b8[1] / MAIN of unk_310==0 */
int func_ov064_02118644(void *c);   /* LavaBubble c7c8[1] / MAIN of unk_310!=0 */
int func_ov064_021187d0(void *c);   /* LavaBubble c7b8[0] / ENTER of unk_310==0 */
int func_ov064_0211873c(void *c);   /* LavaBubble c7c8[0] / ENTER of unk_310!=0 */

/* the source PMF blocks, as 8-byte { fn, delta } records, mounted from ov064 */
extern PortPmf data_ov064_0211bc0c, data_ov064_0211bc14, data_ov064_0211bc1c;
extern PortPmf data_ov064_0211be90, data_ov064_0211be98,
               data_ov064_0211bea0, data_ov064_0211bea8;
}  /* extern "C" */

static const struct { PortPmf *slot; unsigned rom; int (*host)(void *); }
g_ov064_gate178_sources[] = {
    /* Amilift Behavior states, source -> __sinit_ov064_0211afc0 -> c750[0..2] */
    {&data_ov064_0211bc0c, 0x02117c24, func_ov064_02117c24},
    {&data_ov064_0211bc14, 0x02117bdc, func_ov064_02117bdc},
    {&data_ov064_0211bc1c, 0x02117b8c, func_ov064_02117b8c},
    /* LavaBubble states, source -> __sinit_ov064_0211b150 -> c7b8/c7c8 */
    {&data_ov064_0211be90, 0x02118760, func_ov064_02118760},
    {&data_ov064_0211be98, 0x02118644, func_ov064_02118644},
    {&data_ov064_0211bea0, 0x021187d0, func_ov064_021187d0},
    {&data_ov064_0211bea8, 0x0211873c, func_ov064_0211873c},
};

extern "C" void port_ov064_gate178_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_ov064_gate178_sources /
                             sizeof g_ov064_gate178_sources[0]; ++i) {
        PortPmf *p = g_ov064_gate178_sources[i].slot;
        if (p->fn != g_ov064_gate178_sources[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: ov064 gate-178 source %u: the mount holds "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", i, p->fn, p->delta,
                         g_ov064_gate178_sources[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)g_ov064_gate178_sources[i].host;
    }
}
