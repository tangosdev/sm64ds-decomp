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
 * Parameter types follow each DEFINITION in src/, which is why some read
 * void * or const int * where Matrix4x3 * is meant. Changing one is matching
 * work on the definition, not a header edit.
 *
 * Only the members d_a_mky.cpp needs exist so far. */
extern "C" {

void Matrix4x3_FromRotationY(void *m, int angle);
void Matrix4x3_FromTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *, short);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, short angle);
void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3 *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *, short);
void MulMat4x3Mat4x3(const int *a, const int *b, int *out);

extern Matrix4x3 data_020a0e68;
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
        MulMat4x3Mat4x3((const int *)&m, (const int *)&data_020a0e68, (int *)&data_020a0e68);
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

#endif
