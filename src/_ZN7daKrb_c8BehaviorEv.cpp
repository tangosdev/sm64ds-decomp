//cpp
// @symbol _ZN7daKrb_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_dCapEnemy_c.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daKrb_c.h"
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed char s8;


extern s8 data_0209f2f8;

extern "C" {
extern void _ZN8dActor_c8PoofDustEv(char* c);
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(char* c, int f);
extern int _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(char* c, void* w, void* m, u32 j);
extern void func_ov084_02129498(char* c);
extern void _ZN11dCapEnemy_c10ReleaseCapERK7Vector3(char* c, Vector3* v);
extern void _ZN11dCapEnemy_c15RespawnIfHasCapEv(char* c);
extern void func_ov084_021296cc(char* c);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(char* c, void* cyl);
extern void _ZN9Animation7AdvanceEv(void* c);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(char* c, void* cyl);
extern int _ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actr5Fix12IiEsbbS3_(char* c, void* w, int f, int s, int b1, int b2, int g);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(char* c, void* w, u32 j);
extern void _ZN5dCc_c5ClearEv(void* c);
extern void _ZN5dCc_c6UpdateEv(void* c);
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
extern void _ZN12dEnemyBase_c9SpawnCoinEv(char* c);
}

int daKrb_c::Behavior()
{
    Vector3 v1;
    Vector3 v2;
    int r;
    int st;

    func_ov084_02129864(((char*)this));
    func_ov084_021290d4(((char*)this));
    r = _ZN11dCapEnemy_c11GetCapStateEv(((char*)this));
    if (r == 0)
        return 1;
    if (r == 1) {
        *(u32*)((char*)&mFlags) |= 0x10000000;
        _ZN8dActor_c8PoofDustEv(((char*)this));
    }
    if (mGoombaType != 3 && mState != 3 &&
        mEatenByYoshi == 0 && mDeathState == 0 &&
        _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x5dc000) != 0)
    {
        _ZN11dCapEnemy_c12Unk_02005d94Ev(((char*)this));
        return 1;
    }

    if (mDeathState != 0) {
        r = _ZN12dEnemyBase_c26UpdateKillByInvincibleCharER10dBgCh_ActrR9ModelAnimj(((char*)this), ((char*)this) + 0x1b4, ((char*)this) + 0x370, 3);
        if (r != 0) {
            if (r == 2) {
                func_ov084_02129498(((char*)this));
                v1.x = 0;
                v1.y = 0x6c000;
                v1.z = 0;
                _ZN11dCapEnemy_c10ReleaseCapERK7Vector3(((char*)this), &v1);
                mPosX = mHomePos.x;
                mPosY = mHomePos.y;
                mPosZ = mHomePos.z;
                mAngleX = 0;
                mAngleY = 0;
                mAngleZ = 0;
                _ZN11dCapEnemy_c15RespawnIfHasCapEv(((char*)this));
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
        (mGoombaType == 3 && (int)mModelAnim.file == data_ov084_02130cc8[1]))
    {
        mModelAnim.speed = 0x1000;
    } else {
        int v = _ZN4cstd4fdivEii(mHorzSpeed, mScaleX * 2);
        if (v > 0x3000)
            v = 0x3000;
        mModelAnim.speed = v;
    }

    _ZN8dActor_c19MakeVanishLuigiWorkER5dCc_c(((char*)this), ((char*)this) + 0x180);

    if (mState != 2) {
        func_ov084_0212934c(((char*)this));
        _ZN9Animation7AdvanceEv((char*)(Animation*)&mModelAnim);
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
        u16* hp = (u16*)((char*)&mStateTimer);
        *hp += 1;
        if (st != mState)
            *hp = 0;
    }

    func_ov084_02129ed4(((char*)this));

    if (mCapId < 6)
        _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), 0);
    else
        _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), ((char*)this) + 0x180);

    if (mDeathState == 0 && mState != 2 && mState != 3) {
        if (_ZN12dEnemyBase_c15IsGoingOffCliffER10dBgCh_Actr5Fix12IiEsbbS3_(((char*)this), ((char*)this) + 0x1b4, 0x32000, 0x1f49, 0, 1, 0x32000) != 0) {
            mPosX = mSafePos.x;
            mPosY = mSafePos.y;
            mPosZ = mSafePos.z;
        } else {
            mSafePos.x = mPosX;
            mSafePos.y = mPosY;
            mSafePos.z = mPosZ;
        }
    }

    {
        int lvl = mGoombaType;
        if (lvl == 0) {
            _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char*)this), ((char*)this) + 0x1b4, 0);
        } else if (data_0209f2f8 == 6 || data_0209f2f8 == 0x1b) {
            if (unk_444 == data_ov084_02130228[lvl] && mDeathState != 7)
                _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char*)this), ((char*)this) + 0x1b4, 3);
            else
                _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char*)this), ((char*)this) + 0x1b4, 2);
        } else {
            _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char*)this), ((char*)this) + 0x1b4, 2);
        }
    }

    func_ov084_021294d0(((char*)this));
    _ZN5dCc_c5ClearEv((char*)&mdCcAc_c);
    if (mDeathState == 0)
        _ZN5dCc_c6UpdateEv((char*)&mdCcAc_c);
    func_ov084_0212a580(((char*)this));
    func_ov084_02129238(((char*)this));

    if (mState == 0) {
        int b = (mFlags & 8) ? 1 : 0;
        if (b == 0) {
            if (Vec3_Dist((Vector3*)((char*)&mPosX), &mStuckCheckPos) < 0xa000) {
                *(u16*)((char*)&mStuckTimer) += 1;
                if (mCapId < 6 && mStuckTimer == 0x1e) {
                    func_ov084_02129c9c(((char*)this));
                    mTimer458 = 0x5a;
                }
                if (mStuckTimer >= 0x12c && mTimer458 == 0) {
                    func_ov084_021296cc(((char*)this));
                    _ZN12dEnemyBase_c9SpawnCoinEv(((char*)this));
                    func_ov084_02129498(((char*)this));
                    v2.x = 0;
                    v2.y = 0x6c000;
                    v2.z = 0;
                    _ZN11dCapEnemy_c10ReleaseCapERK7Vector3(((char*)this), &v2);
                    mPosX = mHomePos.x;
                    mPosY = mHomePos.y;
                    mPosZ = mHomePos.z;
                    _ZN11dCapEnemy_c15RespawnIfHasCapEv(((char*)this));
                    return 1;
                }
            } else {
                mStuckTimer = 0;
                mStuckCheckPos.x = mPosX;
                mStuckCheckPos.y = mPosY;
                mStuckCheckPos.z = mPosZ;
            }
            goto done;
        }
    }

    if (mTimer458 == 0)
        mStuckTimer = 0;
done:
    return 1;
}
