//cpp
// @symbol _ZN14UnchainedChomp8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "UnchainedChomp.h"
struct CylinderClsn;
struct Actor;
typedef void (Actor::*PMF)();
struct Holder { char pad[8]; PMF fn; };

extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _Z14ApproachLinearRiii(int &x, int target, int step);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(Actor *thiz, CylinderClsn *c);
extern void _ZN12CylinderClsn5ClearEv(void *thiz);
extern void _ZN12CylinderClsn6UpdateEv(void *thiz);
extern void func_02012694(int, void *);
extern void _ZN5Actor15HugeLandingDustEb(Actor *thiz, bool b);
extern Actor *_ZN5Actor13ClosestPlayerEv(Actor *thiz);
extern Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const Vector3 &pos, const Vector3_16 *rot, int e, int f);
extern void _ZN7PathPtrC1Ev(void *thiz);
extern void _ZN7PathPtr6FromIDEj(void *thiz, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *thiz, Vector3 &out, unsigned int idx);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern int LenVec3(Vector3 *v);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
extern void ApproachAngle(short *cur, short target, int step, int a, int b);

extern unsigned char data_0209f2d8[];
}

struct Actor { char pad[0x800]; };

int UnchainedChomp::Behavior()
{
    char *c = (char *)((Actor *)this);
    DecIfAbove0_Short((unsigned short *)(c + 0x6ca));
    DecIfAbove0_Short((unsigned short *)(c + 0x6a8));
    if (DecIfAbove0_Short((unsigned short *)(c + 0x6a6)) != 0) {
        _Z14ApproachLinearRiii(*(int *)(c + 0x80), 0x1000, 0x500);
        *(int *)(c + 0x88) = *(int *)(c + 0x80);
        *(int *)(c + 0x84) = *(int *)(c + 0x88);
        func_ov100_02143b68(c);
        *(int *)(c + 0x98) = 0;
        _ZN5Actor9UpdatePosEP12CylinderClsn(((Actor *)this), (CylinderClsn *)(c + 0x110));
        if (func_ov100_02143370(c) != 0) {
            *(int *)(c + 0xa0) = 0;
        }
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        _ZN12CylinderClsn6UpdateEv(c + 0x110);
        return 1;
    }

    *(int *)(c + 0xa0) = -0x3c000;

    {
        Holder *q = *(Holder **)(c + 0x668);
        if (q->fn != 0) {
            (((Actor *)this)->*(q->fn))();
        }
    }

    *(int *)(c + 0x98) = 0x17000;
    _ZN5Actor9UpdatePosEP12CylinderClsn(((Actor *)this), (CylinderClsn *)(c + 0x110));

    if (func_ov100_02143370(c) != 0) {
        if (*(unsigned short *)(c + 0x6a8) == 0) {
            func_02012694(0x39, c + 0x74);
        }
        *(int *)(c + 0xa8) = 0x14000;
        _ZN5Actor15HugeLandingDustEb(((Actor *)this), true);
    }

    int flag = (data_0209f2d8[0] == 1);
    if (flag != 0 && *(unsigned short *)(c + 0x6ca) == 0) {
        int q16 = __aeabi_idiv(0x10000, *(int *)(c + 0x6b4));
        short spd = (short)q16;
        Actor *pl = _ZN5Actor13ClosestPlayerEv(((Actor *)this));
        (void)pl;

        volatile Vector3 v;
        v.x = 0;
        v.y = 4;
        v.z = 0;

        Actor *sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0x120, 2, *(Vector3 *)(c + 0x5c), (const Vector3_16 *)0,
            *(signed char *)(c + 0xcc), -1);
        if (*(unsigned short *)(c + 0x6a8) == 0) {
            func_02012694(0x3a, c + 0x74);
        }
        if (sp != 0) {
            *(short *)((char *)sp + 0x92) = 0;
            *(short *)((char *)sp + 0x94) = spd;
            *(short *)((char *)sp + 0x96) = 0;
            int vx = v.x;
            int vy = v.y;
            *(int *)((char *)sp + 0xa4) = vx << 12;
            *(int *)((char *)sp + 0xa8) = vy << 12;
            *(int *)((char *)sp + 0xac) = vx << 12;
        }
        *(unsigned short *)(c + 0x6ca) = 0xc8;
    }

    {
        char path[8];
        Vector3 node;
        Vector3 diff;
        _ZN7PathPtrC1Ev(path);
        _ZN7PathPtr6FromIDEj(path, *(unsigned int *)(c + 0x6ac));
        _ZNK7PathPtr7GetNodeER7Vector3j(path, node, *(unsigned int *)(c + 0x6b4));

        Vec3_Sub(&diff, (Vector3 *)(c + 0x5c), &node);

        if (LenVec3(&diff) < 0x190000) {
            *(int *)(((long long)(int)(c + 0x6b4)) & 0xffffffffffffffffLL) += 1;
            if (*(int *)(c + 0x6b4) >= *(int *)(c + 0x6b0)) {
                *(int *)(c + 0x6b4) = 0;
            }
            _ZNK7PathPtr7GetNodeER7Vector3j(path, node, *(unsigned int *)(c + 0x6b4));
        }

        short ang = Vec3_HorzAngle((Vector3 *)(c + 0x5c), &node);
        *(short *)(c + 0x6a4) = ang;

        ApproachAngle((short *)(c + 0x94), *(short *)(c + 0x6a4), 0x10, 0x20, 0x500);

        *(short *)(c + 0x8e) = *(short *)(c + 0x94);

        func_ov100_02143b68(c);
        _ZN12CylinderClsn5ClearEv(c + 0x110);

        Actor *p = _ZN5Actor13ClosestPlayerEv(((Actor *)this));
        if (p != 0 && *(unsigned char *)((char *)p + 0x6fb) == 0) {
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
        }
        return 1;
    }
}
