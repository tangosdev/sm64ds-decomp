#ifndef SM64DS_MATH_MTXSTACK_H
#define SM64DS_MATH_MTXSTACK_H

#include "common.h"

/* The arm9 matrix library (0x0203bd28..0x0203c1b4; MulMat4x3Mat4x3 sits apart
 * at 0x02052914, beside MulVec3Mat4x3 and InvMat4x3) and the scratch matrix
 * the actors build in.
 *
 * Every routine comes as a "set" form (FromRotationY: overwrite with a
 * rotation) and an "apply" form (ApplyInPlaceToRotationY: post-multiply in
 * place), the same S/M pairing EAD's GameCube library uses. data_020a0e68 is
 * one global Matrix4x3 that more than a hundred files load, transform, and
 * read back, which is how that library's matrix stack is used. MtxStack spells
 * that use as inline statics. Its name is coined; nothing of it reaches the
 * ROM, because every member inlines into a plain call on &data_020a0e68.
 *
 * The routines are C: their definitions are .c files, so the prototypes
 * read as C too, and a .c file can include this header for them. Matrices
 * pass as Matrix4x3 *. Angles pass as int to the "set" forms and as short to
 * the "apply" forms, as each definition takes them.
 *
 * Each named parameter below takes a compiler @NNN number in every file that
 * includes this header, so naming an unnamed one renumbers those files.
 *
 * Only the members d_a_mky.cpp needs exist so far. */
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
void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZXYExt(Matrix4x3 *, int, int, int);
void Matrix4x3_FromRotationXYZExt(Matrix4x3 *, int, int, int);
void Matrix4x3_FromRotationZXYExt(Matrix4x3 *, int, int, int);
void MulMat4x3Mat4x3(const Matrix4x3 *a, const Matrix4x3 *b, Matrix4x3 *out);

extern Matrix4x3 data_020a0e68;

#ifdef __cplusplus
}

struct MtxStack {
    static Matrix4x3 &get() {
        return data_020a0e68;
    }
    static void copy(const Matrix4x3 &m) {
        data_020a0e68 = m;
    }
    /* now = m * now */
    static void concat(const Matrix4x3 &m) {
        MulMat4x3Mat4x3(&m, &data_020a0e68, &data_020a0e68);
    }
    static void transS(int x, int y, int z) {
        Matrix4x3_FromTranslation(&data_020a0e68, x, y, z);
    }
    static void transM(int x, int y, int z) {
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, x, y, z);
    }
    static void transM(const Vector3 &t) {
        transM(t.x, t.y, t.z);
    }
    static void YrotM(short y) {
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, y);
    }
    static void XYZrotM(int x, int y, int z) {
        Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, x, y, z);
    }
    static void XYZrotM(const Vector3_16 &r) {
        XYZrotM(r.x, r.y, r.z);
    }
};
#endif /* __cplusplus */

#endif
