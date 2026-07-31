//cpp
// @symbol func_ov006_020ce108
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"


extern "C" {
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int LenVec3(Vector3 *v);
extern int DotVec3(const Vector3 *a, const Vector3 *b);
extern int NormalizeVec3IfNonZero(Vector3 *v);
extern void Vec3_MulScalarInPlace(Vector3 *v, int s);

extern Vector3 data_020a0ebc;
}

struct Obj {
    virtual Vector3 *m00();
    virtual void m04();
    virtual Vector3 *m08();
};

extern "C" void func_ov006_020ce108(char *a, Obj *o)
{
    Vector3 vA;
    Vector3 vB;
    Vector3 vC;
    Vector3 acc;
    Vector3 scaled;
    Vector3 vD;
    Vector3 diff;
    Vector3 t1;
    Vector3 t2;
    Vector3 t3;
    Vector3 *p1;
    int dot44;
    Vector3 *p2;
    int lenA;
    int lenB;
    int dot38;

    {
        int *g = data_ov006_0213b33c;
        int f0 = *(int *)a;
        if (f0 == g[0]) {
            if (*(int *)(a + 4) == g[1])
                return;
            if (f0 == 0)
                return;
        }
    }

    p1 = o->m00();
    p2 = o->m08();

    acc = data_020a0ebc;
    Vec3_Sub(&diff, p1, (Vector3 *)(a + 8));
    vB = diff;
    vA = diff;
    vC = diff;

    Vec3_MulScalar(&scaled, (Vector3 *)(a + 0x44), data_ov006_0212e070[*(short *)(a + 0x96)]);
    AddVec3(&vA, &scaled, &vA);
    SubVec3(&vB, &scaled, &vB);

    vA.y += 0xE000;
    vB.y += 0xE000;
    vA.y >>= 1;
    vB.y >>= 1;

    lenA = LenVec3(&vA);
    lenB = LenVec3(&vB);
    dot44 = DotVec3(&vC, (Vector3 *)(a + 0x44));
    dot38 = DotVec3(&vC, (Vector3 *)(a + 0x38));

    if (lenA < 0xE000 && NormalizeVec3IfNonZero(&vA) != 0) {
        Vec3_MulScalarInPlace(&vA, 0xE000 - lenA);
        vA.y <<= 1;
        Vec3_MulScalar(&t1, &vA, *(int *)(a + 0x88));
        AddVec3(p1, &t1, p1);
        AddVec3(&acc, &vA, &acc);
        *(short *)(((int)o + 0x22) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    } else if (lenB < 0xE000 && NormalizeVec3IfNonZero(&vB) != 0) {
        Vec3_MulScalarInPlace(&vB, 0xE000 - lenB);
        vB.y <<= 1;
        Vec3_MulScalar(&t2, &vB, *(int *)(a + 0x88));
        AddVec3(p1, &t2, p1);
        AddVec3(&acc, &vB, &acc);
        *(short *)(((int)o + 0x22) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    }

    Vec3_MulScalar(&t3, &acc, 0x100);
    AddVec3(p2, &t3, p2);

    {
        int x = p2->x;
        if (x < -0x1000)
            x = -0x1000;
        else if (x > 0x1000)
            x = 0x1000;
        p2->x = x;
    }

    {
        int mx = data_ov006_021405a8[0];
        int mn = data_ov006_021405b4[0];
        int y = p2->y;
        if (y >= mn) {
            if (y <= mx)
                mx = y;
            mn = mx;
        }
        p2->y = mn;
    }

    if ((dot38 < 0 ? -dot38 : dot38) >= 0xE000)
        return;

    if (dot44 < 0)
        dot44 = -dot44;
    if (dot44 >= data_ov006_0212e070[*(short *)(a + 0x96)])
        return;

    Vec3_MulScalar(&vD, (Vector3 *)(a + 0x38), 0x70);
    if (dot38 > 0)
        Vec3_MulScalarInPlace(&vD, -0x1000);
    {
        int t = vD.y;
        if (t > 0x80)
            t = 0x80;
        vD.y = t;
    }
    AddVec3(p2, &vD, p2);
}
