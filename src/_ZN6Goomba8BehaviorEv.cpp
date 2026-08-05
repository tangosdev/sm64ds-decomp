//cpp
// @symbol _ZN6Goomba8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_CapEnemy.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Goomba.h"
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed char s8;
typedef unsigned char u8;

typedef s32 Fix12;

extern s8 data_0209f2f8;

extern "C" {
extern void _ZN5Actor8PoofDustEv(char* c);
extern int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(char* c, Fix12 f);
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(char* c, void* w, void* m, u32 j);
extern void func_ov084_02129498(char* c);
extern void _ZN8CapEnemy10ReleaseCapERK7Vector3(char* c, Vector3* v);
extern void _ZN8CapEnemy15RespawnIfHasCapEv(char* c);
extern void func_ov084_021296cc(char* c);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(char* c, void* cyl);
extern void _ZN9Animation7AdvanceEv(void* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* c, void* cyl);
extern int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(char* c, void* w, Fix12 f, int s, int b1, int b2, Fix12 g);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(char* c, void* w, u32 j);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void _ZN12CylinderClsn6UpdateEv(void* c);
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
extern void _ZN5Enemy9SpawnCoinEv(char* c);
}

int Goomba::Behavior()
{
    Vector3 v1;
    Vector3 v2;
    int r;
    int st;

    func_ov084_02129864(((char*)this));
    func_ov084_021290d4(((char*)this));
    r = _ZN8CapEnemy11GetCapStateEv(((char*)this));
    if (r == 0)
        return 1;
    if (r == 1) {
        *(u32*)(((long long)(int)((char*)&unk_0b0))) |= 0x10000000;
        _ZN5Actor8PoofDustEv(((char*)this));
    }
    if (mGoombaType != 3 && mState != 3 &&
        unk_107 == 0 && mDeathType == 0 &&
        _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x5dc000) != 0)
    {
        _ZN8CapEnemy12Unk_02005d94Ev(((char*)this));
        return 1;
    }

    if (mDeathType != 0) {
        r = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(((char*)this), ((char*)this) + 0x1b4, ((char*)this) + 0x370, 3);
        if (r != 0) {
            if (r == 2) {
                func_ov084_02129498(((char*)this));
                v1.x = 0;
                v1.y = 0x6c000;
                v1.z = 0;
                _ZN8CapEnemy10ReleaseCapERK7Vector3(((char*)this), &v1);
                mPosX = unk_41c;
                mPosY = unk_420;
                mPosZ = unk_424;
                mAngleX = 0;
                mAngleY = 0;
                mAngleZ = 0;
                _ZN8CapEnemy15RespawnIfHasCapEv(((char*)this));
                func_ov084_021296cc(((char*)this));
            }
            return 1;
        }
        if (func_ov084_021298d0(((char*)this)) == 0)
            func_ov084_0212a580(((char*)this));
        return 1;
    }

    if (func_ov084_02129a00(((char*)this)) != 0)
        return 1;

    if (mState >= 3 ||
        (mGoombaType == 3 && unk_3d0 == data_ov084_02130cc8[1]))
    {
        unk_3cc = 0x1000;
    } else {
        int v = _ZN4cstd4fdivEii(unk_098, mScaleX * 2);
        if (v > 0x3000)
            v = 0x3000;
        unk_3cc = v;
    }

    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(((char*)this), ((char*)this) + 0x180);

    if (mState != 2) {
        func_ov084_0212934c(((char*)this));
        _ZN9Animation7AdvanceEv((char*)&mAnimation);
    }

    st = mState;
    {
        int* q = &data_ov084_02130d74[st * 2];
        int adj = q[1];
        char* thiz = ((char*)this) + (adj >> 1);
        void (*fn)(char*);
        if (adj & 1)
            fn = *(void(**)(char*))(*(char**)thiz + q[0]);
        else
            fn = (void(*)(char*))q[0];
        fn(thiz);
    }

    {
        u16* hp = (u16*)((char*)&unk_100);
        *hp += 1;
        if (st != mState)
            *hp = 0;
    }

    func_ov084_02129ed4(((char*)this));

    if (unk_113 < 6)
        _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), 0);
    else
        _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), ((char*)this) + 0x180);

    if (mDeathType == 0 && mState != 2 && mState != 3) {
        if (_ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(((char*)this), ((char*)this) + 0x1b4, 0x32000, 0x1f49, 0, 1, 0x32000) != 0) {
            mPosX = unk_410;
            mPosY = unk_414;
            mPosZ = unk_418;
        } else {
            unk_410 = mPosX;
            unk_414 = mPosY;
            unk_418 = mPosZ;
        }
    }

    {
        int lvl = mGoombaType;
        if (lvl == 0) {
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char*)this), ((char*)this) + 0x1b4, 0);
        } else if (data_0209f2f8 == 6 || data_0209f2f8 == 0x1b) {
            if (unk_444 == data_ov084_02130228[lvl] && mDeathType != 7)
                _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char*)this), ((char*)this) + 0x1b4, 3);
            else
                _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char*)this), ((char*)this) + 0x1b4, 2);
        } else {
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char*)this), ((char*)this) + 0x1b4, 2);
        }
    }

    func_ov084_021294d0(((char*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
    if (mDeathType == 0)
        _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
    func_ov084_0212a580(((char*)this));
    func_ov084_02129238(((char*)this));

    if (mState == 0) {
        int b = (unk_0b0 & 8) ? 1 : 0;
        if (b == 0) {
            if (Vec3_Dist((Vector3*)((char*)&mPosX), (Vector3*)((char*)&unk_428)) < 0xa000) {
                *(u16*)(((long long)(int)((char*)&mStuckTimer))) += 1;
                if (unk_113 < 6 && mStuckTimer == 0x1e) {
                    func_ov084_02129c9c(((char*)this));
                    unk_458 = 0x5a;
                }
                if (mStuckTimer >= 0x12c && unk_458 == 0) {
                    func_ov084_021296cc(((char*)this));
                    _ZN5Enemy9SpawnCoinEv(((char*)this));
                    func_ov084_02129498(((char*)this));
                    v2.x = 0;
                    v2.y = 0x6c000;
                    v2.z = 0;
                    _ZN8CapEnemy10ReleaseCapERK7Vector3(((char*)this), &v2);
                    mPosX = unk_41c;
                    mPosY = unk_420;
                    mPosZ = unk_424;
                    _ZN8CapEnemy15RespawnIfHasCapEv(((char*)this));
                    return 1;
                }
            } else {
                mStuckTimer = 0;
                unk_428 = mPosX;
                unk_42c = mPosY;
                unk_430 = mPosZ;
            }
            goto done;
        }
    }

    if (unk_458 == 0)
        mStuckTimer = 0;
done:
    return 1;
}
