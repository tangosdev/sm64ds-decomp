//cpp
// @symbol _ZN12Flamethrower8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
#include "Flamethrower.h"
// 6e: keep pointer-induction un-reduced
#pragma opt_strength_reduction off
extern "C" {
extern signed char data_0209f2f8;
extern short data_02082214[];
extern short data_ov095_02136f98[];
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, const Vector3 *v, s16 e);
extern void MulVec3Mat4x3(int *v, void *m, int *dst);
extern void Vec3_Add(int *out, int *a, int *b);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN6Player4BurnEv(char *thiz);
extern void func_02022774(Fix12i x, Fix12i y, Fix12i z, s16 ang1, int ang2);
extern void func_020226fc(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2);
extern void func_02022864(Fix12i x, Fix12i y, Fix12i z, s16 ang1, int ang2);
extern void func_020227ec(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2);
extern void _ZN12CylinderClsn5ClearEv(void *thiz);
extern void _ZN12CylinderClsn6UpdateEv(void *thiz);
}

int Flamethrower::Behavior()
{
    char *self;
    int count;
    int i;
    int im1;
    int zero;
    int one;
    int no;
    int ang0;
    int ang1;
    char *ppos;
    int rot;
    char *pclsn;
    int j;
    int in[3];
    int dst[3];
    volatile int part[3]; /* y-add interleave pin; not a live value */
    int sum[3];
    int sum2[3];
    int y;
    int t;
    u32 id;
    char *pl;
    int b;
    s16 ang;
    int a2;
    u8 *p;

    self = (char *)this;
    count = 0xc;
    if (data_0209f2f8 == 0xc)
        count = 9;
    if (count <= 1)
        count = 1;
    if (count >= 0xc)
        count = 0xc;

    switch (*((u8 *)(self + 0x464))) {
    case 0:
        if (*((u8 *)(self + 0x465)) != 0) {
            p = (u8 *)((self + 0x400) + 0x65);
            *p = (u8)(*p - 1);
        }
        if (DecIfAbove0_Short((u16 *)(self + 0x466)) == 0) {
            *((u8 *)(self + 0x464)) = 1;
            *((u16 *)((self + 0x400) + 0x66)) = 0x3c;
        }
        break;
    case 1:
        if ((int)(*((u8 *)(self + 0x465))) < (count - 1)) {
            p = (u8 *)((self + 0x400) + 0x65);
            *p = (u8)(*p + 1);
        }
        if (DecIfAbove0_Short((u16 *)(self + 0x466)) == 0) {
            *((u8 *)(self + 0x464)) = 0;
            *((u16 *)((self + 0x400) + 0x66)) = 0x3c;
        }
        break;
    }

    if (*((u8 *)(self + 0x465)) != 0) {
        *((u32 *)(self + 0x468)) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            *((u32 *)(self + 0x468)), 3, 0x180, (const Vector3 *)(self + 0x74), 0);
    }

    if (*((u32 *)(self + 0xb0)) & 8)
        t = 1;
    else
        t = 0;
    if (t != 0)
        return 1;

    if (((*((u32 *)(self + 8)) & 0xff) != 0xff) || (data_0209f2f8 == 0xc)) {
        Matrix4x3_FromRotationXYZExt(self + 0x434,
            *((s16 *)(self + 0x8c)), *((s16 *)(self + 0x8e)), *((s16 *)(self + 0x90)));
    }

    if (*((u8 *)(self + 0x465)) != 0) {
        zero = 0;
        one = 1;
        no = 0;
        ang0 = 0;
        ang1 = 0;
        i = 0;
        if (i < count) {
            ppos = self;
            rot = i;
            pclsn = self;
            do {
                if (i == (int)(*((u8 *)(self + 0x465))))
                    break;
                if (((*((u32 *)(self + 8)) & 0xff) != 0xff) || (data_0209f2f8 == 0xc)) {
                    im1 = i - 1;
                    in[0] = zero;
                    in[1] = zero;
                    in[2] = zero;
                    dst[0] = zero;
                    dst[1] = zero;
                    dst[2] = zero;
                    in[2] = data_ov095_02136fb0[i];
                    MulVec3Mat4x3(in, self + 0x434, dst);
                    if (im1 < 0) {
                        Vec3_Add(sum, (int *)(self + 0x5c), dst);
                        *((int *)(ppos + 0x3a4)) = sum[0];
                        *((int *)(ppos + 0x3a8)) = sum[1];
                        *((int *)(ppos + 0x3ac)) = sum[2];
                    } else {
                        Vec3_Add(sum2, (int *)(self + 0x3a4 + im1 * 0xc), dst);
                        *((int *)(ppos + 0x3a4)) = sum2[0];
                        *((int *)(ppos + 0x3a8)) = sum2[1];
                        *((int *)(ppos + 0x3ac)) = sum2[2];
                    }
                    if (data_0209f2f8 == 0xc) {
                        s16 ts = data_02082214[(((u16)(s16)rot) >> 4) * 2];
                        *((int *)(ppos + 0x3a8)) = (*((int *)(self + 0x60)))
                            + ((int)(((((long long)ts) * (-0xaa000)) + 0x800) >> 12));
                    }
                    *((int *)(pclsn + 0x104)) = *((int *)(ppos + 0x3a4));
                    *((int *)(pclsn + 0x108)) = *((int *)(ppos + 0x3a8));
                    *((int *)(pclsn + 0x10c)) = *((int *)(ppos + 0x3ac));
                }
                id = *((u32 *)(pclsn + 0xf8));
                if (id != 0) {
                    pl = _ZN5Actor10FindWithIDEj(id);
                    if (pl != 0) {
                        if (*((u16 *)(pl + 0xc)) == 0xbf)
                            b = one;
                        else
                            b = no;
                        if (b != 0)
                            _ZN6Player4BurnEv(pl);
                    }
                }
                part[0] = *((int *)(ppos + 0x3a4));
                y = *((int *)(ppos + 0x3a8));
                part[1] = y;
                y = y + 0x32000;
                part[2] = *((int *)(ppos + 0x3ac));
                part[1] = y;
                if ((*((u32 *)(self + 8)) & 0xff) != 0xff) {
                    ang = data_ov095_02136f80[i];
                    a2 = ang0;
                    func_02022774(part[0], part[1], part[2], ang, a2);
                    func_020226fc(part[0], part[1], part[2], ang, data_ov095_02136f98[i]);
                } else {
                    ang = data_ov095_02136f80[i];
                    a2 = ang1;
                    func_02022864(part[0], part[1], part[2], ang, a2);
                    func_020227ec(part[0], part[1], part[2], ang, data_ov095_02136f98[i]);
                }
                ppos += 0xc;
                rot += 0x1000;
                pclsn += 0x3c;
                i++;
            } while (i < count);
        }
    }

    j = 0;
    if (count > 0) {
        char *pc = self + 0xd4;
        do {
            _ZN12CylinderClsn5ClearEv(pc);
            if (j < (int)(*((u8 *)(self + 0x465))))
                _ZN12CylinderClsn6UpdateEv(pc);
            j++;
            pc += 0x3c;
        } while (j < count);
    }
    return 1;
}
