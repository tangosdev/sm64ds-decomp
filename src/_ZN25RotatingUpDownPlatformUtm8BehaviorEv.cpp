//cpp
#include "types.h"
// @symbol _ZN25RotatingUpDownPlatformUtm8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatformUtm.h"
typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct RaycastGround { char pad[0x54]; } RaycastGround;

extern void *_ZN5Actor15FindWithActorIDEjPS_(u32 id, void *p);
extern int Vec3_HorzDist(const Vec3 *a, const Vec3 *b);
extern int _ZN5Actor13DistToCPlayerEv(void *thiz);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, const Vec3 *pos, u32 e);
extern void _ZN13RaycastGroundC1Ev(RaycastGround *rc);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround *rc, const Vec3 *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround *rc);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(void *src, void *m, void *dst);
extern void Vec3_Add(Vec3 *out, Vec3 *a, Vec3 *b);
extern void Vec3_Sub(Vec3 *out, Vec3 *a, Vec3 *b);
extern int LenVec3(Vec3 *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(Vec3 *out, const Vec3 *in, int scale);
extern void SubVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern void func_ov091_02131340(char *t);
extern void func_020393a4(int *p, int v);
extern void func_02039394(int *p, int v);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *thiz, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *thiz);
extern void _ZN13RaycastGroundD1Ev(RaycastGround *rc);

extern char data_020a0e68[];

int RotatingUpDownPlatformUtm::Behavior()
{
    Vec3 pos;
    Vec3 sp10;
    Vec3 sp1C;
    Vec3 sp28;
    Vec3 sp34;
    Vec3 sp40;
    Vec3 sp4C;
    Vec3 sp58;
    RaycastGround rc;
    int r4;
    int r6;
    int r5;
    int len2;
    int len1;
    int fd;
    void *r8;

    if (unk_3a0 != 0) {
        return 1;
    }

    if (mSpawnParam == 0xffff) {
        int is1d;
        r4 = 1;
        is1d = mActorID;
        is1d = (is1d == 0x1d);
        if (is1d != 0) {
            void *a;
            void *b;
            if (*(void **)((char *)&mPlatform0) == 0 || *(void **)((char *)&mPlatform1) == 0) {
                r8 = _ZN5Actor15FindWithActorIDEjPS_(0x1d, 0);
                if (r8 != 0) {
                    do {
                        if (r8 != (void *)((char *)this) &&
                            Vec3_HorzDist((Vec3 *)((char *)&mPosX), (Vec3 *)((char *)r8 + 0x5c)) < 0xa0000) {
                            if (*(void **)((char *)&mPlatform0) == 0) {
                                *(void **)((char *)&mPlatform0) = r8;
                            } else if (*(void **)((char *)&mPlatform1) == 0) {
                                *(void **)((char *)&mPlatform1) = r8;
                            }
                        }
                        r8 = _ZN5Actor15FindWithActorIDEjPS_(0x1d, r8);
                    } while (r8 != 0);
                }
            }
            a = *(void **)((char *)&mPlatform0);
            if (a != 0) {
                b = *(void **)((char *)&mPlatform1);
                if (b != 0) {
                    int b8;
                    if (*(u8 *)((char *)a + 0x3a0) != 0 && *(u8 *)((char *)b + 0x3a0) != 0) {
                        r4 = 0;
                    }
                    b8 = (unk_0b0 & 8) ? 1 : 0;
                    if (b8 != 0) {
                        if (_ZN5Actor13DistToCPlayerEv(((char *)this)) > 0x7d0000) {
                            char *p398 = *(char **)((char *)&mPlatform0);
                            char *p39c;
                            if (*(u8 *)(p398 + 0x3a0) != 0) {
                                func_ov091_02130fac((int)p398);
                            }
                            p39c = *(char **)((char *)&mPlatform1);
                            if (*(u8 *)(p39c + 0x3a0) != 0) {
                                func_ov091_02130fac((int)p39c);
                            }
                        }
                    }
                }
            }
        }
        if (r4 != 0) {
            unk_378 = _ZN5Sound8PlayLongEjjjRK7Vector3j(
                unk_378, 3, 0x8d, (Vec3 *)((char *)&unk_074), 0);
        }
        return 1;
    }

    if (_ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(((char *)this), 0x2000, 0, 0, 0) != 0) {
        return 1;
    }

    {
        int py;
        pos.x = mPosX;
        py = mPosY;
        pos.y = py;
        pos.z = mPosZ;
        pos.y = py - 0x14000;
    }
    _ZN13RaycastGroundC1Ev(&rc);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, &pos, 0);
    unk_37c = pos.y;
    if (_ZN13RaycastGround10DetectClsnEv(&rc) != 0) {
        unk_37c = *(int *)((char *)&rc + 0x44);
    }

    r6 = 0;
    r5 = data_ov091_021344e8[mVariant];
    r4 = mWaypointIndex - 1;
    if (r4 < 0) {
        r4 = 9;
    }
    Matrix4x3_FromRotationY(data_020a0e68, mAngleY);
    MulVec3Mat4x3(data_ov091_02134cdc + mVariant * 0x78 + mWaypointIndex * 0xc,
                  data_020a0e68, &sp10);
    MulVec3Mat4x3(data_ov091_02134cdc + mVariant * 0x78 + r4 * 0xc,
                  data_020a0e68, &sp1C);
    Vec3_Add(&sp34, (Vec3 *)((char *)&mBasePos), &sp10);
    Vec3_Sub(&sp28, (Vec3 *)((char *)&mPosX), &sp34);
    Vec3_Sub(&sp40, &sp10, &sp1C);
    len1 = LenVec3(&sp40);
    len2 = LenVec3(&sp28);
    fd = _ZN4cstd4fdivEii(len1 - len2, len1);
    {
        s16 *b300 = (s16 *)((char *)&unk_300);
        int mul = *(s16 *)((char *)b300 + 0xa4);
        int base = *(s16 *)((char *)b300 + 0xa2);
        mAngleX = (s16)(base + (int)((short)(((long long)mul * fd + 0x800) >> 12)));
    }

    if (len2 == 0 || len2 <= r5) {
        Vec3_Add(&sp4C, (Vec3 *)((char *)&mBasePos), &sp10);
        mPosX = sp4C.x;
        mPosY = sp4C.y;
        mPosZ = sp4C.z;
        {
            s16 *r2 = (s16 *)(((long long)(int)((char *)&unk_3a2)));
            s16 *b300 = (s16 *)((char *)&unk_300);
            r6 = 1;
            *r2 = (s16)(*r2 + *(s16 *)((char *)b300 + 0xa4));
        }
    } else {
        Vec3_MulScalar(&sp58, &sp28, _ZN4cstd4fdivEii(r5, len2));
        SubVec3((Vec3 *)((char *)&mPosX), &sp58, (Vec3 *)((char *)&mPosX));
    }

    if (r6 != 0) {
        u8 *p394 = (u8 *)(((long long)(int)((char *)&mWaypointIndex)));
        *p394 = (u8)(*p394 + 1);
        if ((u32)mWaypointIndex >= 0xa) {
            mWaypointIndex = 0;
        }
        if ((u32)mWaypointIndex > 3 && (u32)mWaypointIndex < 8) {
            unk_3a4 = 0x2000;
        } else {
            unk_3a4 = 0;
        }
    }

    func_ov091_02131340(((char *)this));

    {
    int is1e = mActorID;
    is1e = (is1e == 0x1e);
    if (is1e == 0) {
        func_ov091_02131160(((char *)this));
        func_020393a4((int *)((char *)&mMeshCollider), 0x150000);
        func_02039394((int *)((char *)&mMeshCollider), 0x1000);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0x150000, 0x1000) != 0) {
            _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
        }
    } else {
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char *)this), 0, 0) != 0) {
            _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));
        }
    }
    }

    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
