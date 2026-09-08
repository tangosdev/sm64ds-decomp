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

/* ---- THE NINE FACES, AND THE ALIAS (run link100, lane PMFB2) ------------
   src/_ZN12PiranhaPlant8BehaviorEv.cpp is a real pointer-to-member dispatch
   and MSVC emits it as `mov ecx, <table>[esi*8+4] / mov eax, <table>[esi*8] /
   add ecx, <this> / call eax`.
   -- `call <reg>` with the receiver in ECX and nothing pushed. The state
   bodies are plain cdecl and read the receiver off the stack, so the code word
   holds a __fastcall FACE and the face calls the body.

   BOTH THE STATICS AND THE TABLE take the face, because the seat below writes
   both: the nine ROM source pairs and the bss copy the sinit fills from them.
   Leaving the statics on the raw body would make a second run of the sinit
   quietly reinstate the wrong convention.

   THE TAG ON THE RETIRED BODY WAS WRONG IN ITS SECOND HALF, and the
   measurement is why this row is takeable. It said MSVC "narrows PMF over a
   complete class to a 4-byte stride against the ROM's 8-byte table"; under
   /vmg /vmm (block R8) the complete class emits the same eight-byte pair as
   an incomplete one, and the listing shows [esi*8] against the ROM's
   `add r3, r0, r4, lsl #3` at 0x0212fde0. All nine source pairs
   (0x02130ba4..0x02130be4) read {code, 0} in
   extracted/overlays/overlay_0084.bin.

   THE ALIAS. The matched TU's table comes in as
   ?data_ov084_02130e80@@3PAP8Cls@@AEXXZA; the mount defines the plain C name.
   alternatename_guard's rule holds: the LHS is never DEFINED, only referenced. */
#pragma comment(linker, "/alternatename:?data_ov084_02130e80@@3PAP8Cls@@AEXXZA=_data_ov084_02130e80")

#define PIR_FACE(sym)                                                         \
    static void __fastcall pmf_face_##sym(void *self, void *dead_edx)         \
    { sym(self); }
PIR_FACE(func_ov084_0212fc10)
PIR_FACE(func_ov084_0212fa7c)
PIR_FACE(func_ov084_0212f6d8)
PIR_FACE(func_ov084_0212f630)
PIR_FACE(func_ov084_0212f588)
PIR_FACE(func_ov084_0212f460)
PIR_FACE(func_ov084_0212f33c)
PIR_FACE(func_ov084_0212f2dc)
PIR_FACE(func_ov084_0212f298)
#undef PIR_FACE

/* the nine states in the sinit's store order (== the data_ov084_02130e80
   index order) */
#define PF(sym) (PortPirFn)pmf_face_##sym
static const struct { PortPirPmf *slot; unsigned rom; PortPirFn host; }
g_piranha_states[] = {
    {data_ov084_02130bb4, 0x0212fc10, PF(func_ov084_0212fc10)},
    {data_ov084_02130ba4, 0x0212fa7c, PF(func_ov084_0212fa7c)},
    {data_ov084_02130bbc, 0x0212f6d8, PF(func_ov084_0212f6d8)},
    {data_ov084_02130bac, 0x0212f630, PF(func_ov084_0212f630)},
    {data_ov084_02130bc4, 0x0212f588, PF(func_ov084_0212f588)},
    {data_ov084_02130bcc, 0x0212f460, PF(func_ov084_0212f460)},
    {data_ov084_02130bdc, 0x0212f33c, PF(func_ov084_0212f33c)},
    {data_ov084_02130bd4, 0x0212f2dc, PF(func_ov084_0212f2dc)},
    {data_ov084_02130be4, 0x0212f298, PF(func_ov084_0212f298)},
};
#undef PF

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

/* _ZN12PiranhaPlant8BehaviorEv RETIRED (run link100, lane PMFB2). It is on
   port/slice_pmfb2.txt and compiles from
   src/_ZN12PiranhaPlant8BehaviorEv.cpp, which recovered as a real C++ method
   (?Behavior@PiranhaPlant@@QAEHXZ); the Itanium C name its fill site calls is
   one cdecl line in hal/except_faces.cpp and no fill site changes. The
   derivation, including why the retired tag's "4-byte stride" reading no
   longer holds, is in the face block above. */
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

