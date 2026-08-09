//cpp
// @symbol func_ov016_02111758
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern "C" {

extern void Matrix4x3_FromRotationY(struct Matrix4x3 *m, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *m, short angX);
extern void MulVec3Mat4x3(const Vector3 *v, const struct Matrix4x3 *m, Vector3 *out);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vector3 *cur, const Vector3 *tgt, int step);
extern int _ZN9Animation8FinishedEv(void *self);
extern unsigned char data_0209f220;
extern struct Matrix4x3 data_020a0e68;
extern void func_ov016_02111bf0(void *c, void *p);

int func_ov016_02111758(char *c) {
    Vector3 in;
    Vector3 out;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    unsigned int t = (unsigned int)(*(int*)(c + 0x3a8)) << 4 >> 0x10;
    if (t >= 0x15 && t <= 0x3c) {
        if (data_0209f220 == 1) {
            in.z = 0x2bc000;
        } else {
            in.z = 0x1f4000;
        }
        Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(c + 0x8c));
        MulVec3Mat4x3(&in, &data_020a0e68, &out);
        out.x += *(int*)(c + 0x3f0);
        out.y += *(int*)(c + 0x3f4);
        out.z += *(int*)(c + 0x3f8);
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vector3*)(c + 0x5c), &out, 0x14000);
    }
    if (_ZN9Animation8FinishedEv(c + 0x3a0)) {
        *(int*)(c + 0x98) = 0;
        func_ov016_02111bf0(c, &data_ov016_02114dac);
    }
    return 1;
}
}
