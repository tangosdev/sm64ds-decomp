//cpp
// @symbol _ZN4Toad13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Toad's spawn-time setup, and most of it is unpacking his spawn parameter.
 *
 * mParam carries three things at once. The low half becomes unk_208, the
 * message/dialogue id, with 0xffff meaning "none" and normalising to 0. The
 * TOP byte of mAngleX becomes unk_20b, which selects a variant -- value 1 is
 * the one that preloads the three ov002 models and, in multiplayer, spawns a
 * companion actor 0x10d keyed to the nearest player's character. The LOW byte
 * of mAngleX becomes unk_20c, a star index; 0xff means "not tied to a star".
 *
 * When it IS tied to a star, the dialogue id shifts by one unless the star is
 * already collected or the player has fewer stars than the table at
 * data_ov085_0212f27c requires -- which is how one Toad says different things
 * before and after you have earned what he is guarding.
 *
 * Finally he is dropped onto the ground: a RaycastGround is aimed 0x14000
 * above his spawn point and, if it hits, mPosY is snapped to the surface. That
 * `(char *)&ray + 0x44` in the placeholder body is RaycastGround::clsnY, so it
 * is a member read now, and the local's `char buf[0x50]` stand-in is gone.
 *
 * `#pragma opt_propagation off` IS LOAD-BEARING and stays. Without it mwcc
 * propagates the reloaded unk_208 through the 0xffff test and the function
 * changes shape.
 */
#pragma opt_propagation off
#include "Toad.h"
#include "RaycastGround.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov085_02130488, data_ov085_02130490, data_ov085_02130480;
extern SharedFilePtr data_ov002_0210da40, data_ov002_0210d9a0, data_ov002_0210d9c0;
extern u8 data_ov085_0212f27c[];
extern BMD_File *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr *p);
extern BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr *p);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, BMD_File *f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void *thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor, Fix12i a, Fix12i b, u32 c, u32 d);
extern int func_02013a44(void);
extern void *_ZN5Actor13ClosestPlayerEv(void *thiz);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const Vector3 *pos, const void *rot, int e, int f);
extern u8 NumStars(void);
extern int IsStarCollectedInCurLevel(int s);
extern void func_ov085_02129524(void *c, int i);
extern void _ZN13RaycastGroundC1Ev(void *thiz);
extern int _ZN13RaycastGround10DetectClsnEv(void *thiz);
extern void _ZN13RaycastGroundD1Ev(void *thiz);
}

int Toad::InitResources()
{
    RaycastGround ray;
    Vector3 objPos;
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_02130488);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_02130490);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_02130480), 1, 0x16);
    _ZN11ShadowModel12InitCylinderEv(&mShadowModel);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(&mMovingCylinderClsn, this,
        0x78000, 0x8c000, 0x4200004, 0);
    unk_20d = 0xff;
    unk_20e = 0xff;
    unk_208 = mParam;
    unk_20c = (u8)mAngleX;
    unk_20b = (u8)(mAngleX >> 8);
    if (unk_208 == 0xffff)
        unk_208 = 0;
    unk_1f4 = 0;
    if (unk_20b == 1) {
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da40);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a0);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c0);
        if (func_02013a44() != 0) {
            void *p = _ZN5Actor13ClosestPlayerEv(this);
            if (p == 0)
                goto after_spawn;
            {
                u32 k = 0xd;
                u8 pl = *(u8 *)((char *)p + 0x6d9);
                s8 cc = mAreaId;
                int m1 = -1;
                void *spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d, k | ((u32)pl << 8), (Vector3 *)&mPosX, 0, cc, m1);
                if (spawned != 0)
                    unk_1f4 = *(s32 *)((char *)spawned + 4);
            }
        after_spawn: ;
        }
    }
    if (unk_20c != 0xff) {
        u16 v = unk_208;
        switch (v) {
        case 0xaf5: unk_20f = 0; break;
        case 0xaf7: unk_20f = 1; break;
        case 0xaf9: unk_20f = 2; break;
        }
        if (IsStarCollectedInCurLevel(unk_20c) == 0) {
            if (NumStars() >= data_ov085_0212f27c[unk_20f])
                goto after_inc;
        }
        {
            u16 *pm = (u16 *)((char *)&unk_208);
            *pm = (u16)(*pm + 1);
        }
    after_inc: ;
    }
    func_ov085_02129524(this, 0);
    objPos.x = mPosX;
    objPos.y = mPosY;
    objPos.z = mPosZ;
    objPos.y = objPos.y + 0x14000;
    _ZN13RaycastGroundC1Ev(&ray);
    ray.SetObjAndPos(objPos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&ray))
        mPosY = ray.clsnY;
    _ZN13RaycastGroundD1Ev(&ray);
    return 1;
}
