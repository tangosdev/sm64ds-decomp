//cpp
// @symbol _ZN11BobOmbBuddy13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "BobOmbBuddy.h"
#include "SharedFilePtr.h"
#include "dBgCh_Gnd.h"

extern "C" {
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, Fix12i b, Fix12i c, unsigned int d, unsigned int e);
extern void func_ov084_0212c960(void* c, int i);
extern int func_ov084_0212ca60(void* p);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* self);
extern int IsStarCollected(int r0, int r1);
extern int SublevelToLevel(int sublevelID);
extern int func_ov084_0212cac0(void *c);

extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
}

extern SharedFilePtr data_ov084_02130da4;
extern SharedFilePtr data_ov084_02130d9c;

int BobOmbBuddy::InitResources()
{
    Vector3 pos;

    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov084_02130da4);
    mModelAnim.SetFile(modelFile, 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(data_ov084_02130d9c);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x8c000, 0x8c000, 0x4200004, 0);
    func_ov084_0212c960(this, 0);
    unk_198 = 0;

    {
        int z = mPosZ;
        int y = mPosY + 0x64000;
        pos.x = mPosX;
        pos.y = y;
        pos.z = z;
    }

    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn() != 0)
        mPosY = ground.clsnY;

    if (func_ov084_0212ca60(this) != 0) {
        void* player = _ZN8dActor_c13ClosestPlayerEv(this);
        unsigned char state = *(unsigned char*)((char*)player + 0x6d9);

        if (state != 0) goto state_check1;
        if (param1 == 0xb26)
            goto state_return0;

    state_check1:
        if (state != 1) goto state_check2;
        if (param1 == 0xb27)
            goto state_return0;

    state_check2:
        if (state != 2) goto state_lostcap;
        if (param1 == 0xb28)
            goto state_return0;

    state_lostcap:
        if (!_ZN8SaveData16HasPlayerLostCapEv())
            goto after_lostcap;

    state_return0:
        return 0;
    }

after_lostcap:
    if (func_ov084_0212cac0(this) == 0)
        goto return1;
    if (data_0209f2f8 != 8)
        goto return1;
    if (data_0209f220 == 1)
        goto return0_2;
    if (IsStarCollected(SublevelToLevel(8), 1) != 0)
        goto return1;

return0_2:
    return 0;

return1:
    return 1;
}
