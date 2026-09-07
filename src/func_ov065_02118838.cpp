//cpp
// @symbol func_ov065_02118838
/* Rebuilds the seven moving collision segments of the ov065 actor (Dorrie's
   body chain) once per frame.

   For each segment it takes the segment's index from the byte table at
   data_ov065_0211c078, copies the source rotation out of the 0x34-byte entry
   array at +0xfc, then multiplies the segment's own 0x30-byte matrix (from the
   array at +0x100) by the actor's base matrix to get the segment's world
   position. Segment 2 is the head: it also picks up the extra rotation stored
   at +0x548 and drives the camera target at +0xd8 and the look-at point at
   +0x1180 with a sine/cosine offset taken from the table at data_02082214.
   Each finished matrix is written to the segment's slot at +0x150 (stride
   0x200) and handed to dBgW_KcMbg::Transform along with the segment's
   collision object at +0x180. */
#include "common.h"

extern "C" {
void Matrix4x3_FromTranslation(Matrix4x3 *m, s32 x, s32 y, s32 z);
void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3 *m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *m, s16 a);
void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3 *m, s16 a);
void MulMat4x3Mat4x3(const Matrix4x3 *a, const Matrix4x3 *b, Matrix4x3 *o);
void SubVec3(const Vector3 *a, const Vector3 *b, Vector3 *o);
void AddVec3(const Vector3 *a, const Vector3 *b, Vector3 *o);
void Vec3_LslInPlace(Vector3 *v, s32 sh);
void _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(void *self, const Matrix4x3 *m, s16 a);
extern Matrix4x3 data_020a0e68;
extern u8 data_ov065_0211c078[];
extern s16 data_02082214[];
}

#pragma opt_common_subs off
#pragma opt_strength_reduction off
extern "C" void func_ov065_02118838(char *c)
{
    char *pm;
    char *pk;
    s32 zero;
    s32 three;
    Vector3s rot;
    Vector3 v;
    Matrix4x3 base;
    s32 i;
    u8 *tbl;

    Matrix4x3_FromTranslation(&data_020a0e68, *(s32 *)(c + 0x5c), *(s32 *)(c + 0x60), *(s32 *)(c + 0x64));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16 *)(c + 0x8e));
    base = data_020a0e68;
    pm = c + 0x150;
    pk = c + 0x180;
    zero = 0;
    three = 3;
    tbl = data_ov065_0211c078;
    for (i = 0; i < 7; i++) {
        char *ent = *(char **)(c + 0xfc) + *tbl * 0x34;
        {
            u16 tz = *(u16 *)(ent + 0x1e);
            u16 ty = *(u16 *)(ent + 0x1c);
            u16 tx = *(u16 *)(ent + 0x1a);
            rot.x = tx;
            rot.y = ty;
            rot.z = tz;
        }
        *(Vector3s *)(c + (i << 9) + 0x348) = rot;
        if (i == 2) {
            *(s16 *)(c + (i << 9) + 0x348) += *(s16 *)(c + 0x548);
            *(s16 *)(c + (i << 9) + 0x34a) += *(s16 *)(c + 0x54a);
            *(s16 *)(c + (i << 9) + 0x34c) += *(s16 *)(c + 0x54c);
        }
        v.x = zero;
        v.y = zero;
        v.z = zero;
        {
            Matrix4x3 *m = &data_020a0e68;
            *m = base;
            MulMat4x3Mat4x3((Matrix4x3 *)(*(char **)(c + 0x100) + *tbl * 0x30), m, m);
            v.x = m->m[9];
            v.y = m->m[10];
            v.z = m->m[11];
        }
        SubVec3(&v, (Vector3 *)(c + 0x5c), &v);
        Vec3_LslInPlace(&v, three);
        AddVec3(&v, (Vector3 *)(c + 0x5c), &v);
        if (i == 2) {
            s32 *cx = (s32 *)(c + 0xd8);
            s32 *cy = (s32 *)(c + 0xdc);
            s32 *cz = (s32 *)(c + 0xe0);
            s32 *hx = (s32 *)(c + 0x1180);
            s32 *hy = (s32 *)(c + 0x1184);
            s32 *hz = (s32 *)(c + 0x1188);
            s32 k1e = 0x1e;
            s32 k82 = 0x82;
            s32 k96 = 0x96;
            s32 rnd = 0x800;
            s32 scaled;
            *(s16 *)(c + 0xe4) = *(s16 *)(c + 0x748);
            *(s32 *)(c + 0x1180) = v.x;
            *(s32 *)(c + 0x1184) = v.y;
            *(s32 *)(c + 0x1188) = v.z;
            scaled = data_02082214[(*(u16 *)(c + 0xe4) >> 4) * 2] * k96;
            *(s32 *)(c + 0xd8) = *(s32 *)(c + 0x1180);
            *(s32 *)(c + 0xdc) = *(s32 *)(c + 0x1184);
            *(s32 *)(c + 0xe0) = *(s32 *)(c + 0x1188);
            *cx += (s32)(((s64)scaled * data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2] + rnd) >> 12);
            *cy += 0x8c000 - data_02082214[(*(u16 *)(c + 0xe4) >> 4) * 2] * k1e;
            *cz += (s32)(((s64)scaled * data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1] + rnd) >> 12);
            *hx += data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2] * k82;
            *hy += 0x50000;
            *hz += data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1] * k82;
        }
        Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, (s16)(*(s16 *)(c + 0x8e) + *(s16 *)(c + (i << 9) + 0x34a)));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16 *)(c + (i << 9) + 0x348));
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16 *)(c + (i << 9) + 0x34c));
        *(Matrix4x3 *)(c + (i << 9) + 0x150) = data_020a0e68;
        _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(pk, (Matrix4x3 *)pm, *(s16 *)(c + 0x8e));
        pm += 0x200;
        pk += 0x200;
        tbl++;
    }
}
