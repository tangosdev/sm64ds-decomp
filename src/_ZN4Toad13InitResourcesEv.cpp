//cpp
// @symbol _ZN4Toad13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Toad's spawn-time setup, and most of it is unpacking his spawn parameter.
 *
 * mParam carries three things at once. The low half becomes mMessageID, the
 * message/dialogue id, with 0xffff meaning "none" and normalising to 0. The
 * TOP byte of mAngleX becomes mVariant, which selects a variant -- value 1 is
 * the one that preloads the three ov002 models and, in multiplayer, spawns a
 * companion actor 0x10d keyed to the nearest player's character. The LOW byte
 * of mAngleX becomes mStarID, a star index; 0xff means "not tied to a star".
 *
 * When it IS tied to a star, the dialogue id shifts by one unless the star is
 * already collected or the player has fewer stars than the table at
 * data_ov085_0212f27c requires -- which is how one Toad says different things
 * before and after you have earned what he is guarding.
 *
 * Finally he is dropped onto the ground: a dBgCh_Gnd is aimed 0x14000
 * above his spawn point and, if it hits, mPosY is snapped to the surface. That
 * The local dBgCh_Gnd is declared at that exact mid-function construction
 * point, so the compiler emits both its constructor and destructor naturally.
 *
 * `#pragma opt_propagation off` IS LOAD-BEARING and stays. Without it mwcc
 * propagates the reloaded mMessageID through the 0xffff test and the function
 * changes shape.
 */
#pragma opt_propagation off
#include "Toad.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov085_02130488, data_ov085_02130490, data_ov085_02130480;
extern SharedFilePtr data_ov002_0210da40, data_ov002_0210d9a0, data_ov002_0210d9c0;
extern u8 data_ov085_0212f27c[];
extern BMD_File *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr *p);
extern BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr *p);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, BMD_File *f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void *thiz);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *thiz, void *actor, Fix12i a, Fix12i b, u32 c, u32 d);
extern int func_02013a44(void);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *thiz);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const Vector3 *pos, const void *rot, int e, int f);
extern u8 NumStars(void);
extern int IsStarCollectedInCurLevel(int s);
}

int Toad::InitResources()
{
    Vector3 objPos;
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_02130488);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_02130490);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_02130480), 1, 0x16);
    _ZN11ShadowModel12InitCylinderEv(&mShadowModel);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mCollider, this,
        0x78000, 0x8c000, 0x4200004, 0);
    mOpacity = 0xff;
    mTargetOpacity = 0xff;
    mMessageID = param1;
    mStarID = (u8)mAngleX;
    mVariant = (u8)(mAngleX >> 8);
    if (mMessageID == 0xffff)
        mMessageID = 0;
    mCapUniqueID = 0;
    if (mVariant == 1) {
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da40);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c0);
        if (func_02013a44() != 0) {
            void *p = _ZN8dActor_c13ClosestPlayerEv(this);
            if (p == 0)
                goto after_spawn;
            {
                u32 k = 0xd;
                u8 pl = *(u8 *)((char *)p + 0x6d9);
                s8 cc = mAreaId;
                int m1 = -1;
                void *spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d, k | ((u32)pl << 8), (Vector3 *)&mPosX, 0, cc, m1);
                if (spawned != 0)
                    mCapUniqueID = *(s32 *)((char *)spawned + 4);
            }
        after_spawn: ;
        }
    }
    if (mStarID != 0xff) {
        u16 v = mMessageID;
        switch (v) {
        case 0xaf5: mStarReqIndex = 0; break;
        case 0xaf7: mStarReqIndex = 1; break;
        case 0xaf9: mStarReqIndex = 2; break;
        }
        if (IsStarCollectedInCurLevel(mStarID) == 0) {
            if (NumStars() >= data_ov085_0212f27c[mStarReqIndex])
                goto after_inc;
        }
        {
            u16 *pm = (u16 *)((char *)&mMessageID);
            *pm = (u16)(*pm + 1);
        }
    after_inc: ;
    }
    SetState(0);
    objPos.x = mPosX;
    objPos.y = mPosY;
    objPos.z = mPosZ;
    objPos.y = objPos.y + 0x14000;
    {
        dBgCh_Gnd ground;
        ground.SetObjAndPos(objPos, 0);
        if (ground.DetectClsn())
            mPosY = ground.clsnY;
    }
    return 1;
}
