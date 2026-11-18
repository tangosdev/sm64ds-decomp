//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_cp.h"
#include "math/CalcLib.h"

extern "C" {

typedef struct {
    int x, y, z, w;
} Quat;

extern int NormalizeVec3IfNonZero(Vector3 *v);
extern void func_0203cebc(Vector3 *out, Vector3 *p, Vector3 *q, Vector3 *r);
extern Quat data_02075b04;

void Quaternion_FromVector3(Quat *out, Vector3 *a, Vector3 *b) {
    Vector3 vb, va, vb2, vcross;
    int r4, absr4, q;

    vb.x = b->x;
    vb.y = b->y;
    vb.z = b->z;
    va.x = a->x;
    va.y = a->y;
    va.z = a->z;
    if (NormalizeVec3IfNonZero(&vb) != 0 && NormalizeVec3IfNonZero(&va) != 0) {
        r4 = FX_Sqrt((VEC_DotProduct(&va, &vb) + 0x1000) << 1);
        vb2.x = vb.x;
        vb2.y = vb.y;
        vb2.z = vb.z;
        func_0203cebc(&vcross, &vb, &va, &vb2);
        vb.x = vcross.x;
        vb.y = vcross.y;
        vb.z = vcross.z;
        absr4 = (r4 < 0) ? -r4 : r4;
        if (absr4 > 4) {
            q = FX_Div(0x1000, r4);
            out->x = FX_MUL(vb.x, q);
            out->y = FX_MUL(vb.y, q);
            out->z = FX_MUL(vb.z, q);
            out->w = r4 >> 1;
            return;
        }
        out->x = data_02075b04.x;
        out->y = data_02075b04.y;
        out->z = data_02075b04.z;
        out->w = data_02075b04.w;
        return;
    }
    out->x = data_02075b04.x;
    out->y = data_02075b04.y;
    out->z = data_02075b04.z;
    out->w = data_02075b04.w;
}
}
