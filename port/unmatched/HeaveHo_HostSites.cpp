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

/* PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int _ZN7HeaveHo6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(int *)(c + 0x60) < data_0209f32c)
        return 1;
    /* ((Cls *)&mModelAnim)->method5(0) -- the ROM slot-5 Render, qualified */
    ((ModelAnim *)(c + 0x340))->ModelAnim::Render(0);
    return 1;
}

/* ---- (2) the state setter + enter dispatch ----------------------------- */
/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
int func_ov077_02126d5c(void *cv, void *pv)
{
    char *c = (char *)cv;
    *(void **)(c + 0x3fc) = pv;
    PortOv077Pmf *q = *(PortOv077Pmf **)(c + 0x3fc);
    if (q[0].fn == 0)
        return 1;
    return ((PortOv077StateFnI)(size_t)q[0].fn)(cv);
}

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

/* PORT_HOST_ABI: mwcc pointer-to-member through an incomplete class. */
int _ZN7HeaveHo8BehaviorEv(void *selfv)
{
    char *c = (char *)selfv;
    Vector3 v;
    int r5;
    int b;

    if (*(int *)(c + 0x60) < data_0209f32c) {          /* mPosY */
        *(int *)(c + 0x5c) = *(int *)(c + 0x404);      /* mPosX = unk_404 */
        *(int *)(c + 0x60) = *(int *)(c + 0x408);      /* mPosY = unk_408 */
        *(int *)(c + 0x64) = *(int *)(c + 0x40c);      /* mPosZ = unk_40c */
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));   /* unk_100 */
    DecIfAbove0_Short((unsigned short *)(c + 0x426));   /* unk_426 */

    {   /* m = *(M **)&unk_3fc; if (m->pmf != 0) (this->*(m->pmf))(); --
           `pmf` at +8 is record[1], the TICK half of the stored cell. */
        PortOv077Pmf *m = *(PortOv077Pmf **)(c + 0x3fc);
        if (m[1].fn != 0)
            ((PortOv077StateFnI)(size_t)m[1].fn)(selfv);
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x144); /* mMovingCylinderClsnWithPos */

    r5 = 0;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x184)) {
        void *fr = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x184);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char *)fr + 4, &v);
        r5 = func_02010844(c, &v, *(s16 *)(c + 0x8e));  /* mAngleY */
    }

    b = _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(
            c, c + 0x184, 0x3c000, (s16)0x2888, 0, 1, (void *)0x32000);
    if (b == 0) {
        if (r5 < 0)
            r5 = (s16)-r5;
        if (r5 <= 0x100)
            goto writeback;
    }
    *(int *)(c + 0x5c) = *(int *)(c + 0x410);
    *(int *)(c + 0x60) = *(int *)(c + 0x414);
    *(int *)(c + 0x64) = *(int *)(c + 0x418);
writeback:
    *(int *)(c + 0x410) = *(int *)(c + 0x5c);
    *(int *)(c + 0x414) = *(int *)(c + 0x60);
    *(int *)(c + 0x418) = *(int *)(c + 0x64);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x184, 2);

    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);           /* mAngleY = mPrevAngleY */
    func_ov077_02126dac(c);

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x184)
        && *(void **)(c + 0x3fc) != (void *)data_ov077_02127cd8) {
        func_ov077_02126528(c);
    }
    _ZN12CylinderClsn5ClearEv(c + 0x110);              /* mMovingCylinderClsn */
    _ZN12CylinderClsn6UpdateEv(c + 0x110);
    _ZN12CylinderClsn5ClearEv(c + 0x144);
    _ZN12CylinderClsn6UpdateEv(c + 0x144);

    _ZN9Animation7AdvanceEv(c + 0x390);                /* mAnimation */
    return 1;
}

}  /* extern "C" */
