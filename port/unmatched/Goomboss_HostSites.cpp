/* HOST COPIES of src/_ZN8Goomboss6RenderEv.cpp and src/func_ov074_021222e0.cpp
 * -- run rel0215 wave 2, lane w2-ov074. Both are the ModelAnim slot-5
 * collision, the Butterfly/Fish/QuestionBlock/Whomp/FlyingCarpet case
 * documented at length in port/unmatched/ModelAnim_Renders.cpp.
 *
 * WHY BOTH: Goomboss HOLDS a ModelAnim at +0x210 (Goomboss_Spawn.cpp and
 * ExplosionGoomba_Spawn.cpp both construct it there with _ZN9ModelAnimC1Ev,
 * and both destructors tear it down with _ZN9ModelAnimD1Ev at the same
 * offset). Each of these two TUs dispatches through a LOCAL SIX-VIRTUAL
 * ROM-order shadow over that member:
 *
 *   Render:            struct Sub { v0..v4; virtual void m(void *); };
 *                      ((Sub *)&mModelAnim)->m(&mScaleX);
 *   func_ov074_021222e0: struct Obj210 { v0..v4; virtual void v5(int); };
 *                      ((Obj210 *)(c + 0x210))->v5(0);
 *
 * so their "slot 5" is the ROM's ModelAnim::Render(Vector3 const *), while
 * hal/cxxname_bridge.cpp fills _ZTV9ModelAnim in MSVC numbering where slot 5
 * is Virtual18 -- a two-argument method reached with the shadow's one. Model's
 * table is dual-filled and would have served this; ModelAnim's cannot be,
 * because Virtual18 really occupies that slot.
 *
 * NOT MEASURED AS A CRASH HERE, and neither body claims it was: the collision
 * is predicted from the member's own constructor/destructor pair and from the
 * fourteen prior instances of the identical shape, and both TUs are hosted
 * before the first boot rather than after a fault. Both matched sources stay
 * byte-locked in src/ as proof and are dropped from port/slice_ov074.txt.
 *
 * THE ARGUMENT DIFFERS BETWEEN THE TWO and that is the ROM's own doing.
 * Render passes &mScaleX (this+0x80), the Fix12 scale triple the ROM hands
 * ModelAnim::Render; func_ov074_021222e0 passes 0, the null-scale form. Both
 * are spelled as the qualified ModelAnim::Render below, with the ROM's own
 * argument.
 *
 * func_ov074_021222e0 IS ALSO ONE HALF OF AN ALIAS RACE. src/_ZN3Amp16Cleanup-
 * ResourcesEv.c (ov070) reads `struct SharedFilePtr *func_ov074_021222e0[2]`
 * -- ov070's own two-pointer file table at the same shared-window address --
 * under this FUNCTION's spelling, and hal/cxx_aliases.cpp used to alias that
 * name onto data_ov070_021222e0. Defining the name here (or in src/) defeats
 * that alias silently, so this lane re-cuts it as a per-source -D on the AMP
 * TU and deletes the pragma; see port/slice_ov074.txt section 4b.
 *
 * PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case.
 */
#include "Model.h"
#include "ModelAnim.h"
#include "common.h"

extern "C" {

extern void _ZN15TextureSequence6UpdateER15ModelComponents(void *a, void *b);
extern void _ZN15MaterialChanger6UpdateER15ModelComponents(void *a, void *b);
extern void _ZN18TextureTransformer6UpdateER15ModelComponents(void *a, void *b);
extern void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);
extern Matrix4x3 data_020a0e68;
extern int func_ov074_021222e0(char *c);

/* ---- (1) Goomboss::Render, ROM 0x02121b70 -------------------------------
   Control flow is the matched source line for line. mParam is at +0x008,
   unk_60a at +0x60a, mScaleX at +0x080, mModelAnim at +0x210 and the
   ModelComponents the three Updates take at +0x218. */
// PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case.
int _ZN8Goomboss6RenderEv(void *selfv)
{
    char *c = (char *)selfv;
    if (*(int *)(c + 0x008) == 0x1111)
        return func_ov074_021222e0(c);
    if (*(unsigned char *)(c + 0x60a) == 0)
        return 1;
    /* ((Sub *)&mModelAnim)->m(&mScaleX) -- ROM slot 5, spelled qualified */
    ((ModelAnim *)(c + 0x210))->ModelAnim::Render((const Vector3 *)(c + 0x080));
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0x3e4, c + 0x218);
    _ZN15MaterialChanger6UpdateER15ModelComponents(c + 0x3d0, c + 0x218);
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0x3f8, c + 0x218);
    return 1;
}

/* ---- (2) func_ov074_021222e0, ROM 0x021222e0 ----------------------------
   The mParam == 0x1111 Render. Same slot-5 collision, null scale.
   PORT_HOST_ABI: ROM-order ModelAnim slot-5 dispatch, the Whomp/Fish case. */
int func_ov074_021222e0(char *c)
{
    Vector3 v;
    Vec3_Asr(&v, (Vector3 *)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x6400, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
                                           *(short *)(c + 0x8c),
                                           *(short *)(c + 0x8e),
                                           *(short *)(c + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -0x6400, 0);
    *(Matrix4x3 *)(c + 0x22c) = data_020a0e68;

    if (*(unsigned char *)(c + 0x60a) == 0)
        return 1;

    /* ((Obj210 *)(c + 0x210))->v5(0) -- ROM slot 5, spelled qualified */
    ((ModelAnim *)(c + 0x210))->ModelAnim::Render(0);
    _ZN15MaterialChanger6UpdateER15ModelComponents(c + 0x3d0, c + 0x218);
    return 1;
}

}  /* extern "C" */
