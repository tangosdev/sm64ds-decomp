#ifndef SM64DS_MATH_MTXSTACK_H
#define SM64DS_MATH_MTXSTACK_H

#include "common.h"
#include "math/Matrix.h"
#include "nitro/fx/fx_mtx33.h"
#include "nitro/fx/fx_mtx43.h"

/* The arm9 matrix library (0x0203bd28..0x0203c1b4; MTX_Concat43 sits apart
 * at 0x02052914, beside MTX_MultVec43 and MTX_Inverse43) and the scratch matrix
 * the actors build in.
 *
 * Every routine comes as a "set" form (FromRotationY: overwrite with a
 * rotation) and an "apply" form (ApplyInPlaceToRotationY: post-multiply in
 * place), the same S/M pairing EAD's GameCube library uses. gScratchMtx is
 * one global Matrix4x3 that more than a hundred files load, transform, and
 * read back, which is how that library's matrix stack is used. MtxStack spells
 * that use as inline statics. Its name is coined; nothing of it reaches the
 * ROM, because every member inlines into a plain call on &gScratchMtx.
 *
 * The routines are C: their definitions are .c files, so the prototypes
 * read as C too, and a .c file can include this header for them. Matrices
 * pass as Matrix4x3 *. Angles pass as int to the "set" forms and as short to
 * the "apply" forms, as each definition takes them.
 *
 * MTX_MultVec33, MTX_MultVec43 and MTX_Inverse43 are NitroSDK's, declared
 * under the SDK's VecFx32 / MtxFx33 / MtxFx43 signatures in
 * nitro/fx/fx_mtx33.h and fx_mtx43.h; C++ includers also get the Vector3 /
 * Matrix4x3 overloads at the end.
 *
 * Each named parameter below takes a compiler @NNN number in every file that
 * includes this header, so naming an unnamed one renumbers those files.
 *
 * Only routines some includer calls are declared so far. */
#ifdef __cplusplus
extern "C" {
#endif

void Matrix4x3_FromRotationX(Matrix4x3 *, int);
void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle);
void Matrix4x3_FromRotationZ(Matrix4x3 *, int);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *, short);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angle);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *, short);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToScale(Matrix4x3 *, int, int, int);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZXYExt(Matrix4x3 *, int, int, int);
void Matrix4x3_FromRotationXYZExt(Matrix4x3 *, int, int, int);
void Matrix4x3_FromRotationZXYExt(Matrix4x3 *, int, int, int);
void MTX_Concat43(const Matrix4x3 *a, const Matrix4x3 *b, Matrix4x3 *out);
void MTX_Identity43_(Matrix4x3 *);
/* m = rotation about one axis, from its sine and cosine. */
void MTX_RotX43_(Matrix4x3 *, int, int);
void MTX_RotY43_(Matrix4x3 *, int, int);
void MTX_RotZ43_(Matrix4x3 *, int, int);

extern Matrix4x3 gScratchMtx;

#ifdef __cplusplus
}

struct MtxStack {
    static Matrix4x3 &get() {
        return gScratchMtx;
    }
    static void copy(const Matrix4x3 &m) {
        gScratchMtx = m;
    }
    /* now = m * now */
    static void concat(const Matrix4x3 &m) {
        MTX_Concat43(&m, &gScratchMtx, &gScratchMtx);
    }
    static void transS(int x, int y, int z) {
        Matrix4x3_FromTranslation(&gScratchMtx, x, y, z);
    }
    static void transM(int x, int y, int z) {
        Matrix4x3_ApplyInPlaceToTranslation(&gScratchMtx, x, y, z);
    }
    static void transM(const Vector3 &t) {
        transM(t.x, t.y, t.z);
    }
    static void YrotM(short y) {
        Matrix4x3_ApplyInPlaceToRotationY(&gScratchMtx, y);
    }
    static void XYZrotM(int x, int y, int z) {
        Matrix4x3_ApplyInPlaceToRotationXYZExt(&gScratchMtx, x, y, z);
    }
    static void XYZrotM(const Vector3_16 &r) {
        XYZrotM(r.x, r.y, r.z);
    }
};

/* NitroSDK's MTX_MultVec33 / MTX_MultVec43 / MTX_Inverse43 take the SDK's
 * VecFx32 and MtxFx33 / MtxFx43 (nitro/fx/fx_mtx33.h, fx_mtx43.h). The game's Vector3,
 * Matrix3x3 and Matrix4x3 hold the same words but are distinct types, so
 * their pointers do not convert; these overloads take the game's types and
 * forward to the C functions, the cast written once here rather than at every
 * call (issue #5207, as math/CalcLib.h does for VEC_*).
 *
 * Each definition costs every C++ includer one compiler @NNN number per
 * parameter plus three, named or not. */
inline void MTX_MultVec33(const Vector3 *vec, const Matrix3x3 *m, Vector3 *dst) {
    MTX_MultVec33((const VecFx32 *)vec, (const MtxFx33 *)m, (VecFx32 *)dst);
}
inline void MTX_MultVec43(const Vector3 *vec, const Matrix4x3 *m, Vector3 *dst) {
    MTX_MultVec43((const VecFx32 *)vec, (const MtxFx43 *)m, (VecFx32 *)dst);
}
inline int MTX_Inverse43(const Matrix4x3 *src, Matrix4x3 *dst) {
    return MTX_Inverse43((const MtxFx43 *)src, (MtxFx43 *)dst);
}
#endif /* __cplusplus */

#endif
