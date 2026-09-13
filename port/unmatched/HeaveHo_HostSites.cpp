/* HOST COPIES of three ov077 HEAVE_HO (actor 238, daPopoi_c) sites MSVC
 * cannot deliver from the matched source, run rel0215 lane cast-ov077. Each
 * matched TU stays byte-locked in src/ and is simply not referenced from
 * port/slice_ov077.txt. Lakitu_HostSites.cpp's header carries the full
 * derivation of both shapes; only what is DIFFERENT about HeaveHo is written
 * out here.
 *
 * HEAVEHO'S DESTRUCTORS NEED NOTHING. Unlike Lakitu's and Spiny's,
 * src/_ZN7HeaveHoD1Ev.c and src/_ZN7HeaveHoD0Ev.c are already plain C bodies
 * that store their table by name (_ZTV7HeaveHo and _ZTV9daPopoi_c) and call
 * flat member destructors, so both stay in the slice as matched source. That
 * asymmetry is the ROM's, not a choice: the three classes were recovered by
 * different routes.
 *
 * (1) src/_ZN7HeaveHo6RenderEv.cpp -- the ModelAnim slot-5 collision. The
 *     whole body is a draw guard plus one `((Cls *)&mModelAnim)->method5(0)`
 *     through a six-virtual local shadow. mPosY 0x60, mModelAnim 0x340.
 *
 * (2) src/func_ov077_02126d5c.cpp -- the PMF disease, and the ONE of the six
 *     ov077 sites that is a SETTER as well as a dispatch:
 *         struct C; typedef int (C::*PMF)();
 *         struct C { char pad[0x3fc]; PMF *pp; };
 *         c->pp = p; PMF *q = c->pp;
 *         if (*q == 0) return 1;
 *         return (c->**q)();
 *     -- store the cell pointer at self+0x3fc and immediately dispatch its
 *     enter half, with a NULL GUARD the Lakitu/Spiny pair does not have. The
 *     guard is kept exactly: `*q == 0` in the ROM's encoding is the fn word
 *     being zero, since every delta word in ov077's own records is 0 (verified
 *     against overlay_0077.bin, port/ov077_syms.txt). This is the
 *     func_ov094_02136188 body one overlay over, same shape, same treatment.
 *     Callers pass a bss CELL address (&data_ov077_02127ce8 from
 *     InitResources, &data_ov077_02127cd8 from func_ov077_02126528), so the
 *     stored pointer is a cell, not a source record -- which is what makes
 *     HeaveHo::Behavior's `!= data_ov077_02127cd8` comparison below meaningful.
 *
 * (3) src/_ZN7HeaveHo8BehaviorEv.cpp -- the SAME PMF, dispatched INLINE:
 *         struct M { char pad[8]; PMF pmf; };
 *         m = *(M **)&unk_3fc;
 *         if (m->pmf != 0) (((Klass *)this)->*(m->pmf))();
 *     M's `pmf` at +8 is the TICK half of the two-record cell (records are 8
 *     bytes, so +8 is record[1]), read off the same stored cell pointer. Every
 *     other statement is transcribed line for line from the matched body with
 *     HeaveHo.h's own offsets; only the pointer-to-member call is respelled.
 *
 * THE SEAT: port_ov077_states_seat() in hal/actor_classes_ov077.cpp rewrites
 * each mounted source record's fn word with its host body's address, validated
 * against the ROM's own address first, BEFORE __sinit_ov077_021275fc copies
 * the ten records into the five bss cells.
 */
#include "ModelAnim.h"
#include "HeaveHo.h"

extern "C" {

struct PortOv077Pmf { unsigned int fn; int delta; };
typedef int (*PortOv077StateFnI)(void *);

/* ---- (1) Render -------------------------------------------------------- */
extern int data_0209f32c;

/* _ZN7HeaveHo6RenderEv RETIRED (run link100, lane EXCEPT). Its stated reason -- the
   ROM-order model slot-5 dispatch -- died with lane SLOT5F's
   respelling of include/ModelBase.h: hal/cxxname_bridge.cpp:522/578
   put Render back on index 5 of _ZTV5Model and _ZTV9ModelAnim, so the
   matched source's local six-virtual shadow reaches the body it means.
   The C name is defined in hal/except_faces.cpp onto the matched
   __thiscall method; the ROM vtable word and the kind:function record
   are in port/slice_except2.txt. */


/* ---- (2) the state setter + enter dispatch ----------------------------- */

/* func_ov077_02126d5c IS NOT A HOST COPY ANY MORE. Run link100 lane PMF2 put
   src/func_ov077_02126d5c.cpp back on port/slice_pmf2.txt: with /vmg /vmm global (the
   R8 block in port/CMakeLists.txt) MSVC's pointer-to-member IS the ROM's
   8-byte {function, delta} pair, and the matched TU compiles to the same
   tail jump this body was -- measured, listing in that slice's header.
   The reading above is kept because it is the derivation. */
/* ---- (3) Behavior ------------------------------------------------------ */
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cc);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *self, void *v);
int func_02010844(void *unused, void *v, s16 angle);
int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(
        void *self, void *wm, Fix12i a, s16 b, int c, int d, void *e);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wm, unsigned int j);
void func_ov077_02126dac(char *t);
void func_ov077_02126528(char *c);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN12CylinderClsn6UpdateEv(void *self);
void _ZN9Animation7AdvanceEv(void *self);
extern int data_ov077_02127cd8[];

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 2. src/_ZN7HeaveHo8BehaviorEv.cpp
   dispatches its own field now: with /vmg /vmm (block R8) MSVC's pointer to
   member IS the ROM's eight-byte {code, adjust} pair, so the widening this
   banner was written for does not happen. The per-frame half of every state
   cell holds a zero-argument __fastcall face; the enter half does not change,
   because the helper that dispatches it tail-jumps. Measurements in
   port/slice_pmfb7.txt and runs/link100/out/PMFB7/. */

}  /* extern "C" */
