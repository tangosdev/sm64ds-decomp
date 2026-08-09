//cpp
// @symbol _ZN7Skeeter8BehaviorEv
/* recovered: named members + shared header, real C++ method
 *
 * One frame of the skeeter -- the water strider -- in four mutually exclusive
 * branches, each returning 1 on its own: in Yoshi's mouth, killed by a mega
 * character, dying, or the ordinary tail.
 *
 * unk_3ac is the WATER LINE, found once in InitResources by raycasting down.
 * The ordinary path clamps mPosY UP to it every frame, so the skeeter floats
 * rather than falls. The death branch reuses the same value as a TEST instead:
 * dying below the line is what pays out coins, so drowning and being squashed
 * on the surface end differently.
 *
 * Two branches share the same trigger check verbatim -- level 0x15, unk_0cc
 * == 1, and the mesh reporting water -- and both respond by zeroing the four
 * motion words and setting unk_10c = 1, the knocked-into-water state.
 *
 * The final cylinder Update is gated on the closest player's +0x6fb, so the
 * skeeter stops colliding while that player is in some state of their own.
 */
#include "Skeeter.h"

struct CoinVec3 { s32 x, y, z; };

class ActorC {};
typedef void (ActorC::*ActorFn)();
struct PmfNode { char pad[8]; ActorFn fn; };

extern "C" {
int  _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void* self, void* wm);
void _ZN12CylinderClsn5ClearEv(void* self);
void _ZN12CylinderClsn6UpdateEv(void* self);
void func_ov090_02131e50(void* c);
int  _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void* self, void* wm, void* anim, u32 n);
void func_02035684(int* p, int v);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* self, void* wm, u32 n);
int  _ZN5Enemy11UpdateDeathER12WithMeshClsn(void* self, void* wm);
void func_ov090_02131378(void* c);
void _ZN5Actor8PoofDustEv(void* self);
void _Z14ApproachLinearRsss(s16* p, s16 a, s16 b);
int  AngleDiff(int a, int b);
void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* self, const void* v, u32 n, s32 fix, s16 s);
void _ZN5Actor24KillAndTrackInDeathTableEv(void* self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cyl);
int  func_02035638(u8* p);
void* _ZN5Actor13ClosestPlayerEv(void* self);
void func_020aea30(void* self, void* p, int n);
u16  DecIfAbove0_Short(u16* p);
void _ZN9Animation7AdvanceEv(void* self);
void func_ov090_021310b4(void* c);
extern signed char data_0209f2f8;
}

int Skeeter::Behavior()
{
    char* c = (char*)this;

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(c, &mWithMeshClsn)) {
        _ZN12CylinderClsn5ClearEv(&mMovingCylinderClsnWithPos);
        if (unk_107 != 0 && unk_104 == 0)
            _ZN12CylinderClsn6UpdateEv(&mMovingCylinderClsnWithPos);
        func_ov090_02131e50(c);
        return 1;
    }

    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, &mWithMeshClsn, &mModelAnim, 3))
        return 1;

    if (unk_10c != 0) {
        func_02035684((int*)(&mWithMeshClsn), 0xd2000);
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, &mWithMeshClsn, 0);
        if (_ZN5Enemy11UpdateDeathER12WithMeshClsn(c, &mWithMeshClsn))
            return 1;
        func_ov090_02131378(c);
        func_ov090_02131e50(c);
        if (unk_10c == 0)
            _ZN5Actor8PoofDustEv(c);
        if (unk_3a1 == 3) {
            _Z14ApproachLinearRsss(&mAngleX, -32767, 0x500);
            if (AngleDiff(*&mAngleX, -32767) < 0x1000) {
                s16* p8e = &mAngleY;
                *p8e += 0x1000;
            }
        }
        if (unk_10c != 1 && mPosY <= unk_3ac) {
            CoinVec3 v;
            v.x = mPosX;
            v.y = mPosY;
            v.z = mPosZ;
            _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, unk_10a + 1, 0xa000, 0);
            _ZN5Actor8PoofDustEv(c);
            _ZN5Actor24KillAndTrackInDeathTableEv(c);
        }
        return 1;
    }

    {
    int flag = (mFlags & 8) != 0;
    if (flag) {
        unk_098 = 0;
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, &mMovingCylinderClsnWithPos);
        func_ov090_02131378(c);
        if (data_0209f2f8 == 0x15 && unk_0cc == 1) {
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, &mWithMeshClsn, 2);
            if (func_02035638((u8*)(&mWithMeshClsn))) {
                unk_09c = 0;
                unk_0a4 = 0;
                unk_0a8 = 0;
                unk_0ac = 0;
                unk_10c = 1;
                func_020aea30(c, _ZN5Actor13ClosestPlayerEv(c), 0);
                return 1;
            }
        }
        return 1;
    }
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, &mMovingCylinderClsnWithPos);
    func_ov090_02131378(c);
    DecIfAbove0_Short(&unk_100);
    DecIfAbove0_Short(&unk_394);
    DecIfAbove0_Short(&unk_396);
    DecIfAbove0_Short(&unk_398);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, &mWithMeshClsn, 2);
    if (mPosY <= unk_3ac)
        mPosY = unk_3ac;
    if (data_0209f2f8 == 0x15 && unk_0cc == 1 && func_02035638((u8*)(&mWithMeshClsn))) {
        unk_09c = 0;
        unk_0a4 = 0;
        unk_0a8 = 0;
        unk_0ac = 0;
        unk_10c = 1;
        func_020aea30(c, _ZN5Actor13ClosestPlayerEv(c), 0);
        return 1;
    }

    {
        PmfNode* n = (PmfNode*)mState;
        if (n->fn)
            (((ActorC*)c)->*(n->fn))();
    }
    mAngleY = mPrevAngleY;
    /* MEASURED: these two reach inside mModelAnim (0x368 and 0x35c are its
       +0x5c and +0x50) and they are the ONLY spelling in this function that is
       not free. Writing them as `(char*)&mModelAnim + 0x5c` costs bytes --
       greedy-tested alone against build_pin, everything else here substituted
       for free. Offsetting from a typed sub-object's address is not the same
       to mwcc as offsetting from `this`, so these stay as they are. */
    *(s32*)(c + 0x368) = *(s32*)(c + 0x3a4);
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02131e50(c);
    func_ov090_021310b4(c);
    _ZN12CylinderClsn5ClearEv(&mMovingCylinderClsnWithPos);
    {
        void* p = _ZN5Actor13ClosestPlayerEv(c);
        if (p != 0 && *(u8*)((char*)p + 0x6fb) == 0)
            _ZN12CylinderClsn6UpdateEv(&mMovingCylinderClsnWithPos);
    }
    return 1;
}
