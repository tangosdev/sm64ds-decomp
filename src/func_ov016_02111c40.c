#include "types.h"
// @symbol func_ov016_02111c40
/* recovered: Unagi (eel) body update. Builds a translation matrix at
 * data_020a0e68 from pos>>3, spins a (0,0,-0x190000) offset by the yaw
 * (+0x8e), rebuilds the matrix from (pos + offset)>>3 with the ZXY rotation
 * (+0x8c/+0x8e/+0x90), and copies it into the actor's base matrix (+0x36c).
 * Eases +0x424 toward +0x426 by 0x40 and mirrors it to +0x422. Then walks
 * the 7 tail segments: each segment's world position (+0x448 + 12*i) is
 * zeroed, the segment's local matrix (0x30 bytes each at *(+0x364)) is
 * multiplied onto the base, and the translation column is stored back and
 * scaled by 8. If a linked actor (+0x49c) exists, its attach matrix (+0x3c0)
 * is the base times the 5th segment, offset (0x28000,0,0) with an XYZ
 * rotation (0x4000, -0x8000, +0x416), and the actor's +0xc8 is pointed at it.
 *
 * Codegen notes: the per-segment zero comes from a memory-resident local
 * (`zero[1]`, a one-element array): the ROM stores i's register to [sp]
 * once before the loop and reloads it every iteration. A `volatile int`
 * gives the same loop but pins its store in emission order (7 words of
 * pre-loop schedule stay swapped); a plain array stores and reloads with a
 * free schedule. The three <<= 3 are in-place read-modify-writes through
 * the bone pointer in address order, i++ is the last statement of the body,
 * and the translation column is read back through the loop-local dst
 * pointer (CSE colours it onto the hoisted &data_020a0e68 in sb).
 */
#include "common.h"

extern void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void *m, s16 ang);
extern void MulVec3Mat4x3(const void *v, const void *m, void *out);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, s16 x, s16 y, s16 z);
extern void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 step);
extern void MulMat4x3Mat4x3(void *m1, void *m0, void *mF);
extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, s16 x, s16 y, s16 z);
extern Matrix4x3 data_020a0e68;

void func_ov016_02111c40(char *c)
{
    struct Vector3 in;
    struct Vector3 out;
    struct Vector3 asr;
    int zero[1];
    int i;
    char *bone;
    int base;
    int *pos;
    Matrix4x3 *mat_src;
    Matrix4x3 *scratch;
    void *found;
    int off424;
    char *p400;
    u32 id;
    Matrix4x3 *scratch1;
    Matrix4x3 *scratch0;

    Vec3_Asr(&asr, (struct Vector3 *) (c + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, asr.x, asr.y, asr.z);
    in.z = 0;
    in.z = -0x190000;
    in.x = 0;
    in.y = 0;
    scratch0 = &data_020a0e68;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, *((s16 *) (c + 0x8e)));
    scratch1 = &data_020a0e68;
    MulVec3Mat4x3(&in, scratch1, &out);
    Matrix4x3_FromTranslation(&data_020a0e68, ((*((int *) (c + 0x5c))) + out.x) >> 3, (*((int *) (c + 0x60))) >> 3,
                              ((*((int *) (c + 0x64))) + out.z) >> 3);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, *((s16 *) (c + 0x8c)), *((s16 *) (c + 0x8e)),
                                           *((s16 *) (c + 0x90)));
    *((Matrix4x3 *) (c + 0x36c)) = data_020a0e68;
    p400 = c + 0x400;
    off424 = 0x424;
    _Z14ApproachLinearRsss((s16 *) (c + off424), *((s16 *) (p400 + 0x26)), 0x40);
    i = 0;
    scratch = &data_020a0e68;
    mat_src = (Matrix4x3 *) (c + 0x36c);
    *((s16 *) (c + 0x422)) = *((s16 *) (c + 0x424));
    bone = c;
    base = i;
    pos = (int *) (c + 0x448);
    zero[0] = i;
    do {
        Matrix4x3 *dst;
        *((int *) (bone + 0x448)) = zero[0];
        *((int *) (bone + 0x44c)) = zero[0];
        *((int *) (bone + 0x450)) = zero[0];
        dst = &data_020a0e68;
        *dst = *mat_src;
        MulMat4x3Mat4x3(((char *) (*((void **) (c + 0x364)))) + base, dst, dst);
        pos[0] = *((int *) (((char *) dst) + 0x24));
        pos[1] = *((int *) (((char *) dst) + 0x28));
        pos[2] = *((int *) (((char *) dst) + 0x2c));
        *((int *) (bone + 0x448)) = *((int *) (bone + 0x448)) << 3;
        *((int *) (bone + 0x44c)) = *((int *) (bone + 0x44c)) << 3;
        *((int *) (bone + 0x450)) = *((int *) (bone + 0x450)) << 3;
        bone += 0xc;
        base += 0x30;
        pos += 3;
        i++;
    } while (i < 7);
    id = *((u32 *) (c + 0x49c));
    if (id == 0) {
        return;
    }
    found = _ZN8dActor_c10FindWithIDEj(id);
    if (found == 0) {
        return;
    }
    *((int *) (c + 0x43c)) = 0;
    *((int *) (c + 0x440)) = 0;
    *((int *) (c + 0x444)) = 0;
    MulMat4x3Mat4x3(((char *) (*((void **) (c + 0x364)))) + 0xc0, c + 0x36c, c + 0x3c0);
    Matrix4x3_FromTranslation(scratch0, 0x28000, 0, 0);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x4000, -0x8000, *((s16 *) (c + 0x416)));
    MulMat4x3Mat4x3(&data_020a0e68, c + 0x3c0, c + 0x3c0);
    {
        void *live = (void *) (c + 0x3c0);
        data_020a0e68 = *(Matrix4x3 *) (c + 0x3c0);
        *(int *) (c + 0x43c) = data_020a0e68.m[9];
        *(int *) (c + 0x440) = data_020a0e68.m[10];
        *(int *) (c + 0x444) = data_020a0e68.m[11];
        *(int *) (((int) c + 0x43c)) <<= 3;
        *(int *) (((int) c + 0x440)) <<= 3;
        *(int *) (((int) c + 0x444)) <<= 3;
        *((void **) (((char *) found) + 0xc8)) = live;
    }
}
