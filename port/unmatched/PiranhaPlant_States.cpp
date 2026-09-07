/* HOST COPIES for the two Whomp's Fortress piranhas (ov084): the PIRANHA_PLANT's
 * nine-state pointer-to-member dispatch and both classes' ModelAnim slot-5
 * Renders.
 *
 * ---- the PIRANHA_PLANT's state machine -------------------------------------
 *
 * PiranhaPlant::Behavior dispatches
 *
 *     struct Cls { virtual void dummy(); };
 *     typedef void (Cls::*PMF)();
 *     extern PMF data_ov084_02130e80[];
 *     ...
 *     (((Cls *)this)->*data_ov084_02130e80[mState])();
 *
 * over a COMPLETE single-inheritance class, which MSVC represents with a
 * FOUR-byte pointer-to-member -- but the ROM's mwcc table is nine EIGHT-byte
 * {function, delta} records (__sinit_ov084_02130654 copies nine 8-byte statics
 * into it). So MSVC would index data_ov084_02130e80[mState] at a 4-byte stride
 * against 8-byte data and read state 2 out of the middle of state 1 -- the
 * gate-16 stride collision, the BobOmbBuddy case again, this time narrowing
 * rather than widening.
 *
 * The port-faithful fix is a HOST COPY of Behavior that reads the table as two
 * plain ints and calls the body directly (the Fish/ChainChomp treatment), so
 * MSVC's pointer-to-member representation never enters. All nine deltas are
 * zero in the ROM (a complete class), so the pair is just the function pointer,
 * and the states stay ordinary cdecl `void func(void *)` bodies -- the goomba's
 * dispatch shape. This is NOT a src edit and NOT undecompiled code: every one
 * of the nine states and both helpers are matched src.
 *
 * THE NINE STATICS ARE SEATED on both sides of the sinit, the treatment
 * BobOmbBuddy_States.cpp settled on: the sinit runs from the window's boot and
 * the registry runs later, so seating only the source side would leave the
 * copy full of DS addresses. Measured against the ROM address each host body
 * was compiled from, so a mount pointing at the wrong bytes says so.
 *
 * ---- the two Renders -------------------------------------------------------
 *
 * Both dispatch ModelAnim slot 5 (Render, with a Vector3 arg) through a local
 * shadow class -- the collision hal/cxxname_bridge.cpp cannot dual-fill because
 * Virtual18 already occupies the MSVC slot Render needs. The BobEnemy_Renders
 * case for the fifth gate. PiranhaPlant::Render also dispatches Model slot 5
 * (the dual-filled one) for its second draw.
 */
#include <cstdio>
#include <cstdlib>

#include "Model.h"
#include "ModelAnim.h"

extern "C" {

/* ---- the PIRANHA_PLANT's nine states -------------------------------------- */
struct PortPirPmf { unsigned fn; int delta; };
/* the nine ROM statics the sinit copies, in the order it stores them, and the
   bss table it copies into */
extern PortPirPmf data_ov084_02130bb4[], data_ov084_02130ba4[],
    data_ov084_02130bbc[], data_ov084_02130bac[], data_ov084_02130bc4[],
    data_ov084_02130bcc[], data_ov084_02130bdc[], data_ov084_02130bd4[],
    data_ov084_02130be4[];
extern PortPirPmf data_ov084_02130e80[];

/* the nine state bodies (all matched src) */
void func_ov084_0212fc10(void *self); void func_ov084_0212fa7c(void *self);
void func_ov084_0212f6d8(void *self); void func_ov084_0212f630(void *self);
void func_ov084_0212f588(void *self); void func_ov084_0212f460(void *self);
void func_ov084_0212f33c(void *self); void func_ov084_0212f2dc(void *self);
void func_ov084_0212f298(void *self);

/* the two per-frame helpers Behavior closes over, plus the Enemy kill check */
void func_ov084_0212f204(void *self);
void func_ov084_0212ec60(void *self);
int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(
    void *self, void *clsn, void *anim, unsigned n);
void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void *self, void *clsn);
void _ZN9Animation7AdvanceEv(void *self);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(
    void *self, void *v);

}  /* extern "C" */

typedef void (*PortPirFn)(void *);

/* the nine states in the sinit's store order (== the data_ov084_02130e80
   index order) */
static const struct { PortPirPmf *slot; unsigned rom; PortPirFn host; }
g_piranha_states[] = {
    {data_ov084_02130bb4, 0x0212fc10, func_ov084_0212fc10},
    {data_ov084_02130ba4, 0x0212fa7c, func_ov084_0212fa7c},
    {data_ov084_02130bbc, 0x0212f6d8, func_ov084_0212f6d8},
    {data_ov084_02130bac, 0x0212f630, func_ov084_0212f630},
    {data_ov084_02130bc4, 0x0212f588, func_ov084_0212f588},
    {data_ov084_02130bcc, 0x0212f460, func_ov084_0212f460},
    {data_ov084_02130bdc, 0x0212f33c, func_ov084_0212f33c},
    {data_ov084_02130bd4, 0x0212f2dc, func_ov084_0212f2dc},
    {data_ov084_02130be4, 0x0212f298, func_ov084_0212f298},
};

static void pir_seat_one(PortPirPmf *p, unsigned rom, PortPirFn host,
                         const char *where, unsigned i)
{
    if (p->fn == (unsigned)(size_t)host)
        return;
    if (p->fn != rom || p->delta != 0) {
        std::fprintf(stderr, "FATAL: PiranhaPlant state %u (%s): the mount "
                     "holds %08x/%d, the ROM's own table says %08x/0 -- WRONG "
                     "BYTES\n", i, where, p->fn, p->delta, rom);
        std::abort();
    }
    p->fn = (unsigned)(size_t)host;
}

extern "C" void port_piranha_plant_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < 9; ++i) {
        pir_seat_one(g_piranha_states[i].slot, g_piranha_states[i].rom,
                     g_piranha_states[i].host, "static", i);
        pir_seat_one(&data_ov084_02130e80[i], g_piranha_states[i].rom,
                     g_piranha_states[i].host, "table", i);
    }
}

/* HOST COPY of PiranhaPlant::Behavior (ov084 0x0212fd4c), line for line, with
   the pointer-to-member dispatch read as two plain ints. mState is at +0x458. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch (MSVC narrows PMF over a complete class to a 4-byte stride against the ROM's 8-byte table).
extern "C" int _ZN12PiranhaPlant8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;
    int r = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(
        c, c + 0x1c4, c + 0x110, 1);
    if (r != 0) {
        if (r == 2) {
            *(unsigned char *)(c + 0x108) = 0;
            *(int *)(c + 0x458) = 7;               /* mState = 7 */
            *(int *)(c + 0x80) = 0;                /* mScaleX = 0 */
            *(int *)(c + 0x84) = 0;                /* mScaleY = 0 */
            *(int *)(c + 0x88) = 0;                /* mScaleZ = 0 */
            *(int *)(c + 0x5c) = *(int *)(c + 0x44c); /* mPosX = unk_44c */
            *(int *)(c + 0x60) = *(int *)(c + 0x450);
            *(int *)(c + 0x64) = *(int *)(c + 0x454);
        }
        return 1;
    }
    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x380);
    _ZN9Animation7AdvanceEv(c + 0x160);
    func_ov084_0212f204(c);
    int old = *(int *)(c + 0x458);                 /* old = mState */
    {
        PortPirPmf *m = &data_ov084_02130e80[old];
        if (m->fn & 1) {
            std::fprintf(stderr, "FATAL: PiranhaPlant state %d is a VIRTUAL "
                         "member pointer (%08x/%d); the ROM stores none there\n",
                         old, m->fn, m->delta);
            std::abort();
        }
        ((PortPirFn)(size_t)m->fn)(c + m->delta);
    }
    *(unsigned short *)(c + 0x100) = (unsigned short)(*(unsigned short *)(c + 0x100) + 1);
    int cur = *(int *)(c + 0x458);                 /* cur = mState */
    if (old != cur) {
        if (cur == 5)
            *(int *)(c + 0xb0) &= ~0x10000000;     /* unk_0b0 word */
        *(unsigned short *)(c + 0x100) = 0;
        *(int *)(c + 0x478) = 0;
    }
    func_ov084_0212ec60(c);
    _ZN12CylinderClsn5ClearEv(c + 0x380);
    _ZN12CylinderClsn5ClearEv(c + 0x3b4);
    _ZN12CylinderClsn5ClearEv(c + 0x3e8);
    if (*(unsigned char *)(c + 0x45c) != 0) {
        _ZN12CylinderClsn6UpdateEv(c + 0x380);
        _ZN12CylinderClsn6UpdateEv(c + 0x3b4);
        if (*(int *)(c + 0x458) == 2) {
            _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(
                c + 0x3e8, c + 0x440);
            _ZN12CylinderClsn6UpdateEv(c + 0x3e8);
        }
    }
    return 1;
}

/* _ZN12PiranhaPlant6RenderEv RETIRED (run link100, lane EXCEPT). Its stated reason -- the
   ROM-order model slot-5 dispatch -- died with lane SLOT5F's
   respelling of include/ModelBase.h: hal/cxxname_bridge.cpp:522/578
   put Render back on index 5 of _ZTV5Model and _ZTV9ModelAnim, so the
   matched source's local six-virtual shadow reaches the body it means.
   The C name is defined in hal/except_faces.cpp onto the matched
   __thiscall method; the ROM vtable word and the kind:function record
   are in port/slice_except2.txt. */


/* _ZN19FirePiranhaPlantBig6RenderEv RETIRED (run link100, lane EXCEPT). Its stated reason -- the
   ROM-order model slot-5 dispatch -- died with lane SLOT5F's
   respelling of include/ModelBase.h: hal/cxxname_bridge.cpp:522/578
   put Render back on index 5 of _ZTV5Model and _ZTV9ModelAnim, so the
   matched source's local six-virtual shadow reaches the body it means.
   The C name is defined in hal/except_faces.cpp onto the matched
   __thiscall method; the ROM vtable word and the kind:function record
   are in port/slice_except2.txt. */

