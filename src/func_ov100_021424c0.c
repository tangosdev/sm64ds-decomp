#include "types.h"
/* func_ov100_021424c0 @ 0x021424c0 (ov100, size 0x26c)
 * Bob-omb buddy/enemy walk update: handles yoshi-eat (poof on eaten),
 * steering toward the current target angle, water/ground handling with
 * landing dust and rolling sound, speed cap 0x23000, and collision update.
 */
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(char *, char *);
extern int func_ov002_020ad660(char *, char *, char *, int);
extern int _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern int func_02012694(int, char *);
extern void func_ov100_02141fb0(char *);
extern int func_ov100_0214233c(char *);
extern void _Z14ApproachLinearRsss(char *, int, int);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(char *);
extern void func_ov100_02142130(char *);
extern int _ZN5Actor12ReflectAngleE5Fix12IiES1_s(char *, int, int, int);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(char *);
extern void _ZN5Actor11LandingDustEb(char *, int);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(int, unsigned int, unsigned int, char *, unsigned int);
extern void _Z14ApproachLinearRiii(char *, int, int);
extern int Vec3_HorzLen(char *);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char *, int);
extern void func_ov100_02142264(char *);
extern void _ZN12CylinderClsn5ClearEv(char *);
extern int _ZN12CylinderClsn6UpdateEv(char *);

int func_ov100_021424c0(char *c)
{
    int r;
    int vy;

    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x374);
    r = func_ov002_020ad660(c, c + 0x110, c + 0x2cc, 3);
    if (r != 0) {
        if (r != 2)
            return r;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xf, *(int *)(c + 0x5c),
                                                       *(int *)(c + 0x60), *(int *)(c + 0x64));
        return func_02012694(0x173, c + 0x74);
    }

    func_ov100_02141fb0(c);
    {
        int m = *(int *)(c + 0x10c);
        if (m == 8)
            return m;
    }

    r = func_ov100_0214233c(c);
    _Z14ApproachLinearRsss(c + 0x94, *(s16 *)(c + 0x3ba), 0x800);

    if (r == -1) {
        *(u8 *)(c + 0x3d0) = 3;
        if (*(u8 *)(c + 0x3d0) != 4 && _ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110) != 0) {
            *(int *)(c + 0xa8) = 0;
            *(int *)(int)(((long long)(int)(c + 0x60))) += 0xf000;
        } else {
            func_ov100_02142130(c);
        }
    } else {
        int had = (*(u8 *)(c + 0x3d1) != 0);
        vy = *(int *)(c + 0xa8);
        func_ov100_02142130(c);
        if (*(u8 *)(c + 0x3d1) != 0 && had == 0) {
            *(s16 *)(c + 0x94) = _ZN5Actor12ReflectAngleE5Fix12IiES1_s(c, *(int *)(c + 0xe0),
                                                                      *(int *)(c + 0xe8),
                                                                      *(s16 *)(c + 0x94));
            *(int *)(c + 0x3cc) = 0;
        } else if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110) != 0) {
            if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x110) != 0) {
                func_02012694(0x40, c + 0x74);
                _ZN5Actor11LandingDustEb(c, 1);
                if (vy < -0x14000)
                    *(int *)(c + 0xa8) = (int)(-vy + ((u32)-vy >> 31)) >> 1;
                *(int *)(c + 0x3cc) = 0;
            } else {
                *(int *)(c + 0x3cc) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(int *)(c + 0x3cc),
                                                                       3, 0x8a, c + 0x74, 0);
            }
            if (*(int *)(c + 0x98) >= 0x23000) {
                *(int *)(c + 0x98) = 0x23000;
            } else if (*(u8 *)(c + 0x3d0) == 4) {
                _Z14ApproachLinearRiii(c + 0x98, 0x23000, 0x400);
            } else {
                *(int *)(int)(((long long)(int)(c + 0xa4))) += *(int *)(c + 0xd4);
                *(int *)(int)(((long long)(int)(c + 0xac))) += *(int *)(c + 0xdc);
                *(int *)(c + 0x98) = Vec3_HorzLen(c + 0xa4);
            }
        }
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    func_ov100_02142264(c);
    _ZN12CylinderClsn5ClearEv(c + 0x374);
    return _ZN12CylinderClsn6UpdateEv(c + 0x374);
}
