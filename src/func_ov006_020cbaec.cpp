//cpp
// @symbol func_ov006_020cbaec
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
typedef short s16;




struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void m4();
};

extern "C" {
int DotVec3(const Vector3 *a, const Vector3 *b);
void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
int _ZN4cstd4fdivEii(int a, int b);
void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int c, int d, int e, const void *f, void *g);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisPtr, void *file, int i, int fix, unsigned int flags);
void Sound_PlayBank1Panned(int a0, int a1, int a2);
int func_ov006_020e6e3c(int a, int b);
void func_ov006_020cb838(char *c);
}


extern "C" void func_ov006_020cbaec(char *c)
{
    Vector3 tmp;
    Vector3 tmp2;
    Vector3 tmp4;
    int r5v;
    int r4v;
    int dot;
    int scale;
    int fdivr;

    r5v = *(int *)(c + 0x10);
    r4v = *(int *)(c + 0x14);
    if (r5v < 0) r5v = -r5v;
    {
        int *src = (int *)(((long long)(int)(c + 4)));
        tmp.x = src[0];
        tmp.y = src[1];
        tmp.z = src[2];
    }

    dot = DotVec3((Vector3 *)(c + 0x34), &tmp);
    scale = (int)(((long long)dot * 0x1400 + 0x800) >> 12);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    SubVec3((Vector3 *)(c + 0x34), &tmp2, (Vector3 *)(c + 0x34));

    fdivr = _ZN4cstd4fdivEii(r4v, r4v + r5v);

    *(int *)(c + 0x40) = data_ov006_02140544;

    {
        Vector3_16 v;
        v.x = (s16)tmp.x;
        v.y = (s16)tmp.y;
        v.z = (s16)tmp.z;

        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0xf3, *(int *)(c + 0x1c) << 3, *(int *)(c + 0x20) << 3, *(int *)(c + 0x24) << 3, &v, 0);
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 0xf4, *(int *)(c + 0x1c) << 3, *(int *)(c + 0x20) << 3, *(int *)(c + 0x24) << 3, &v, 0);
    }

    *(unsigned int *)(c + 0x54) = (unsigned int)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(unsigned int *)(c + 0x54), 0xf5, *(int *)(c + 0x1c) << 3, *(int *)(c + 0x20) << 3, *(int *)(c + 0x24) << 3, 0, 0);

    scale = (int)(((long long)data_ov006_02140570 * fdivr + 0x800) >> 12);
    Vec3_MulScalar(&tmp4, &tmp, scale);
    AddVec3((Vector3 *)(c + 0x34), &tmp4, (Vector3 *)(c + 0x34));

    {
        int v = *(int *)(c + 0x34);
        if (v < -0x1000) v = -0x1000;
        else if (v > 0x1000) v = 0x1000;
        *(int *)(c + 0x34) = v;
    }

    {
        int half = data_ov006_02140570 >> 1;
        int cur = *(int *)(c + 0x38);
        if (cur >= half) half = cur;
        *(int *)(c + 0x38) = half;
    }

    *(int *)(c + 0x44) = *(int *)(c + 0x38);
    ((VtObj *)c)->m4();

    if (*(int *)(c + 0x58) != 0) {
        *(int *)(c + 0x58) = 0;
    } else {
        *(int *)(c + 0x58) = 1;
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x6c), data_ov006_0214059c, 0x40000000, 0x800, 0);

    *(int *)(c + 0xc4) = 0;
    Sound_PlayBank1Panned(0, 4, *(int *)(c + 0x1c));

    func_ov006_020e6e3c(0x1b5, *(int *)(c + 0x1c));

    *(int *)(c + 0x5c) = 0;
    {
        int a = data_ov006_0213b1ac[0];
        int b = data_ov006_0213b1ac[1];
        a = b ? a : a;
        *(int *)(c + 0x64) = a;
        *(int *)(c + 0x68) = b;
    }
    func_ov006_020cb838(c);
}
