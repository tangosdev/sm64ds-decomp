//cpp
// @symbol _ZN11VolcanoRing8BehaviorEv
#include "VolcanoRing.h"

/* VolcanoRing::Behavior -- vtable slot 6, ov022 0x021112ac.
 *
 * mState/mSpawnGuard/mTriggered/mCooldown/mSpawnCount are this class's own
 * fields (include/VolcanoRing.h); mPosX/Y/Z, mAreaId, mFlags, mAngleY,
 * mPrevAngleY, mPrevAngleZ are dActor_c's own (include/dActor_c.h).
 * UpdateModelPosAndRotY/UpdateClsnPosAndRot/IsClsnInRange are dBgActor_c's
 * own, called unqualified. DecIfAbove0_Short/Byte, ApproachAngle,
 * Sound::PlayBank3 and dActor_c::Spawn keep their verified ROM symbols
 * (#defines preserved from the pre-migration recovery for readability).
 * func_020393a4/02039394 (SetCollisionRadius-shaped) stay on mMeshCollider,
 * unmigrated. */
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int ApproachAngle(short *p, int target, int a, int b, int c);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *pos);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *v16, int e, int f);
extern void func_020393a4(void *p, int v);
extern void func_02039394(void *p, int v);
/* Fix12<int>-by-value in the mangled name, but the bytes refuse a real
   Fix12<int> parameter here (include/dBgActor_c.h's own note on this exact
   symbol) -- called with plain ints, same as every other already-migrated
   caller (src/_ZN10DonutBlock8BehaviorEv.cpp and siblings). */
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
}
#define Sound_PlayBank3 _ZN5Sound9PlayBank3EjRK7Vector3
#define Actor_Spawn _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as

int VolcanoRing::Behavior()
{
    switch (mState) {
    case 0:
        if (DecIfAbove0_Short(&mCooldown) != 0)
            break;
        if (mTriggered == 0)
            break;
        if (ApproachAngle(&mPrevAngleZ, 0, 0xf, 0x30, 2) != 0)
            break;
        mCooldown = 0x96;
        mState = 1;
        Sound_PlayBank3(0xc, &mCamSpacePosX);
        break;
    case 1:
        if (DecIfAbove0_Short(&mCooldown) == 0) {
            if (ApproachAngle(&mPrevAngleZ, -0x100, 0xf, 0x30, 2) != 0)
                break;
            mCooldown = 0x96;
            mState = 0;
            break;
        }
        {
            int b8;
            b8 = (int)((mFlags & 8) != 0);
            if (b8 != 0)
                break;
        }
        if (DecIfAbove0_Byte(&mSpawnGuard) != 0)
            break;
        {
            int v[3];
            void *s;
            v[0] = mPosX;
            v[1] = mPosY;
            v[2] = mPosZ;
            v[1] = mPosY + 0x1f4000;
            s = Actor_Spawn(0xf3, 0, v, 0, mAreaId, -1);
            *(void **)((unsigned char *)s + 0x10c) = this;
            *(int *)((unsigned char *)s + 0x118) = mPosY;
            {
                unsigned short *t = &mSpawnCount;
                *t = *t + 1;
            }
            mSpawnGuard = 2;
        }
        break;
    }

    {
        short *ip = &mPrevAngleY;
        *ip = (short)(*ip + mPrevAngleZ);
    }
    mAngleY = mPrevAngleY;
    func_020393a4(&mMeshCollider, 0x780000);
    func_02039394(&mMeshCollider, 0x1000);
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x780000, 0x1000) != 0)
        UpdateClsnPosAndRot();
    mTriggered = 0;
    return 1;
}
