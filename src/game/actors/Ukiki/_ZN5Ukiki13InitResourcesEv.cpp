//cpp
#include "types.h"
// @symbol _ZN5Ukiki13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Ukiki.h"
#define false 0
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int idx, int speed, u32 flags);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, u32 c, u32 d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v0, void *v1);
extern char *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern char *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 id, u32 param, void *pos, void *rot, int a, int b);
extern void func_ov030_021141a8(void *self, int a);
extern void func_ov030_02112094(void *self);
}

extern char data_ov002_0210da40;
extern char data_ov002_0210d9a0;
extern char data_ov002_0210d9c0;

int Ukiki::InitResources()
{
    int i;
    int b;
    u16 h;
    int *py;

    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da40);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a0);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov030_02115d00), 1, 1);
    for (i = 0; i < 10; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov030_02114824[i]);
    if (_ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel) == 0)
        return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char *)this) + 0xd4, data_ov030_02115cf0[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x160, ((char *)this), 0x28000, 0x64000, 0x800004, 0x49000);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x194, ((char *)this), 0x32000, 0x32000, 0, 0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    py = (int *)(((int)((char *)this) + 0x384));
    unk_380 = mPosX;
    unk_384 = mPosY;
    unk_388 = mPosZ;
    *py += 0x64000;
    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    unk_3a8 = 0;
    mCapUniqueID = 0;
    unk_3cb = 0x96;
    h = actorID;
    b = 0;
    if (h == 0x10b)
        b = 1;
    if (b != 0) {
        if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
            char *player = _ZN8dActor_c13ClosestPlayerEv(((char *)this));
            if (*(u32 *)(player + 8) >= 3)
                goto ov030_no_spawn;
            {
                char *spawned;
                mCapPlayerNo = *(u32 *)(player + 8);
                spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x10d, (mCapPlayerNo << 8) | 2, ((char *)this) + 0x5c, 0, mAreaId, -1);
                mCapUniqueID = *(int *)(spawned + 4);
                mHasSpawnedCap = 1;
                func_ov030_021141a8(((char *)this), 1);
                goto ov030_cap_done;
            }
ov030_no_spawn:
            func_ov030_021141a8(((char *)this), 0);
ov030_cap_done:
            ;
        } else {
            func_ov030_021141a8(((char *)this), 0);
        }
    } else {
        int t = (int)(h == 0x10c);
        if (t != 0)
            func_ov030_021141a8(((char *)this), 1);
    }
    func_ov030_02112094(((char *)this));
    return 1;
}
