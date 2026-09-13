/* HOST COPIES of BULLET's one pointer-to-member state machine (actor 233,
 * ov002 _ZTV6Bullet / _ZTV24daPropeller_Heyho_Fire_c 0x0210d654), the seat of
 * its two state-handler records, and nothing stubbed: both handlers are matched
 * src.
 *
 * BULLET is the projectile Snufit (func_ov065_02116364) fires. It runs a SINGLE
 * state: a 16-byte descriptor (data_ov002_02111190, ov002 bss) holding two mwcc
 * {function, delta} records, the ENTER half at +0 and the MAIN half at +8.
 * func_ov002_020fed2c (the state setter) stores the descriptor pointer at
 * this+0x350 and dispatches the ENTER half once; Bullet::Behavior dispatches the
 * MAIN half every frame. Both matched TUs form the pointer-to-member -- the
 * setter over a FORWARD-DECLARED struct (`struct C;`), Behavior over a complete
 * one -- so MSVC's pointer-to-member representation and __thiscall dispatch do
 * NOT reproduce the ROM's plain {function,delta} pair called cdecl with `this`.
 * The Klepto / PushBlock case: each body below is the matched source's control
 * flow line for line, only the dispatch read as a plain { fn, delta } and the fn
 * called with `this`.
 *
 * THE SEAT (the ov002 house order)
 * --------------------------------
 * data_ov002_02111190 is bss; __sinit_ov002_02108094 -- already in
 * slice_gate10.txt, already linked, run from walk_window / smoke_player -- copies
 * two SOURCE records (ov002 data 0x0210d600 -> func_ov002_020fed18 and
 * 0x0210d5f8 -> func_ov002_020fec94, each a real {fn,0}) into the descriptor:
 * cell[0].fn = 0x020fed18 (enter), cell[1].fn = 0x020fec94 (main). The ov002
 * mount lays those words down as DS CODE ADDRESSES, so after the sinit copy the
 * descriptor holds DS addresses. port_bullet_states_seat() rewrites the two DEST
 * fn words with the host bodies at vtable-fill time (the PushBlock/SignPost
 * gate-51 order: sinit first, seat the dest), validating each mounted word
 * against its ROM address first (WRONG-BYTES abort). Both deltas are 0.
 *
 * NO HOLE. Both handlers are matched src (slice_bullet.txt): func_ov002_020fed18
 * (enter, sets the lifetime timer) and func_ov002_020fec94 (main, the per-frame
 * fly/hit/expire logic a live fired Bullet runs). A Bullet reaches the main half
 * every frame after Snufit fires it; there is no stub in that path.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

struct PortPmf { unsigned fn; int delta; };

/* Behavior's C-linkage helpers (matched src or hosted) */
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm,
                                              unsigned int j);
void func_ov002_020fed7c(void *self);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);

/* the descriptor the sinit fills: [0] = enter half, [1] = main half */
extern PortPmf data_ov002_02111190[2];

/* the two matched handlers (called by host pointer with `this` as arg 0) */
int func_ov002_020fed18(void *c);   /* enter: sets the lifetime timer to 200 */
int func_ov002_020fec94(void *c);   /* main : per-frame fly/collide/expire     */

}  /* extern "C" */

/* func_ov002_020fed2c IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov002_020fed2c.cpp back on port/slice_pmf2.txt (batch 2): with /vmg /vmm
   global MSVC's pointer-to-member IS the ROM's 8-byte {function, delta}
   pair, the matched TU compiles to the same tail jump this body was, and
   the seat in this file aborts the binary on a nonzero delta so the two
   agree word for word. The reading above is kept as the derivation. */
/* HOST COPY RETIRED, run link100 lane PMFB7 gate 1. src/_ZN6Bullet8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

/* ---- THE SEAT -------------------------------------------------------------
   The two dest fn words __sinit_ov002_02108094 filled with DS addresses, each
   matched against its own ROM address before the overwrite. Called from
   hal_fill_bullet_vtable (registration runs after the ov002 sinit chain). */
/* run link100 lane PMFB7 gate 1: THE MAIN HALF IS A FACE.
   src/_ZN6Bullet8BehaviorEv.cpp dispatches the descriptor's +8 half as a real
   pointer to member -- mov eax,[m+8] / test / je / mov ecx,[m+12] /
   add ecx,this / call eax, ARITY ZERO, /Zp4 diff 0 lines -- so the receiver
   arrives in ecx with nothing pushed. The ENTER half does not change:
   func_ov002_020fed2c dispatches it and compiles to a tail jump. */
static void __fastcall bullet_main_face(void *self, void *dead_edx)
{
    (void)dead_edx;
    func_ov002_020fec94(self);
}

extern "C" void port_bullet_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    static const struct { unsigned half; unsigned rom; void *host; }
    seats[] = {
        {0, 0x020fed18, (void *)func_ov002_020fed18},   /* enter */
        {1, 0x020fec94, (void *)bullet_main_face},      /* main  */
    };
    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        PortPmf *p = &data_ov002_02111190[seats[i].half];
        if (p->fn != seats[i].rom || p->delta != 0) {
            std::fprintf(stderr, "FATAL: BULLET state %u: the sinit left "
                         "%08x/%d, the ROM's own table says %08x/0 -- WRONG "
                         "BYTES\n", seats[i].half, p->fn, p->delta,
                         seats[i].rom);
            std::abort();
        }
        p->fn = (unsigned)(size_t)seats[i].host;
    }
}
