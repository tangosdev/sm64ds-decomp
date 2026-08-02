//cpp
#include "types.h"
#include "common.h"

extern "C" {
int DotVec3(const Vector3 *a, const Vector3 *b);
void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
int _ZN4cstd4fdivEii(int a, int b);
void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int fx1, int fx2, int fx3, const void *vec, void *cb);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thisPtr, void *file, int i, int fix, unsigned int flags);
void func_ov006_020e6df0(int a0, int a1, int a2);
int func_ov006_020e6e3c(int a, int b);
void func_ov006_020c91ac(char *c);
}

extern int data_ov006_021405a8;
extern int data_ov006_021405b0;
extern int data_ov006_0213b1d4[2];
extern void *data_ov006_0214059c;
extern int data_ov006_0213b1ec[2];

struct VtObj {
    virtual void d0();
    virtual void d1();
    virtual void d2();
    virtual void d3();
    virtual void m4();
};


extern "C" void func_ov006_020c94e0(char *c)
{
    Vector3 tmp, tmp2, tmp3;
    int r5v, r4v, dot, scale, fdivr, v, fx, cur;

    r5v = *(int *)(c + 0x10);
    r4v = *(int *)(c + 0x14);
    if (r5v < 0) r5v = -r5v;
    {
        int *src = (int *)(((long long)(int)(c + 4)));
        tmp.x = src[0]; tmp.y = src[1]; tmp.z = src[2];
    }

    dot = DotVec3((Vector3 *)(c + 0x3c), &tmp);
    scale = (int)(((long long)dot * 0x1400 + 0x800) >> 12);
    Vec3_MulScalar(&tmp2, &tmp, scale);
    SubVec3((Vector3 *)(c + 0x3c), &tmp2, (Vector3 *)(c + 0x3c));

    fdivr = _ZN4cstd4fdivEii(r4v, r4v + r5v);
    scale = (int)(((long long)data_ov006_021405a8 * fdivr + 0x800) >> 12);
    Vec3_MulScalar(&tmp3, &tmp, scale);
    AddVec3((Vector3 *)(c + 0x3c), &tmp3, (Vector3 *)(c + 0x3c));

    v = *(int *)(c + 0x3c);
    if (v < -0x3000) v = -0x3000;
    else if (v > 0x3000) v = 0x3000;
    *(int *)(c + 0x3c) = v;

    fx = (int)(((long long)data_ov006_021405a8 * 0xc00 + 0x800) >> 12);
    cur = *(int *)(c + 0x40);
    if (cur >= fx) fx = cur;
    *(int *)(c + 0x40) = fx;

    *(int *)(c + 0x48) = data_ov006_021405b0;


    {
        Vector3_16 vec16;
        int y = tmp.y;
        int z = tmp.z;
        int x = tmp.x;
        vec16.x = (s16)x;
        vec16.y = (s16)y;
        char *bp = c + 0x70;
        int *s = (int *)(((long long)(int)bp));
        vec16.z = (s16)(z + (int)((unsigned)bp - (unsigned)bp));
        
        int *q = data_ov006_0213b1d4;
        if (s[0] != q[0] || (s[1] != q[1] && *(int *)(c + 0x70) != 0)) {

            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                0, 0xf3, (*(int *)(c + 0x24)) << 3, (*(int *)(c + 0x28)) << 3,
                (*(int *)(c + 0x2c)) << 3, &vec16, 0);
            _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                0, 0xf4, (*(int *)(c + 0x24)) << 3, (*(int *)(c + 0x28)) << 3,
                (*(int *)(c + 0x2c)) << 3, &vec16, 0);
            *(int *)(c + 0x5c) = (int)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned int *)(c + 0x5c), 0xf5, (*(int *)(c + 0x24)) << 3, (*(int *)(c + 0x28)) << 3,
                (*(int *)(c + 0x2c)) << 3, 0, 0);

        }
    }

    *(int *)(c + 0x4c) = *(int *)(c + 0x40);
    ((VtObj *)c)->m4();
    *(int *)(c + 0x64) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void *)(c + 0x78), data_ov006_0214059c, 0x40000000, 0x800, 0);
    *(int *)(c + 0xd0) = 0;
    func_ov006_020e6df0(0, 4, *(int *)(c + 0x24));
    func_ov006_020e6e3c(0x1b5, *(int *)(c + 0x24));
    *(int *)(c + 0x68) = 0;
    {
        int a = data_ov006_0213b1ec[0];
        int b = data_ov006_0213b1ec[1];
        a = b ? a : a;
        *(int *)(c + 0x70) = a;
        *(int *)(c + 0x74) = b;
    }
    func_ov006_020c91ac(c);
}
