//cpp
// @symbol _ZN11BobOmbBuddy13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BobOmbBuddy.h"
struct RaycastGround { char buf0[0x14]; int floor[12]; char buf1[0x50-0x14-0x30]; };

extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void* self);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* actor, Fix12i b, Fix12i c, unsigned int d, unsigned int e);
extern void func_ov084_0212c960(void* c, int i);
extern void _ZN13RaycastGroundC1Ev(RaycastGround* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, const Vector3& v, void* actor);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
extern int func_ov084_0212ca60(void* p);
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern int IsStarCollected(int r0, int r1);
extern void _ZN13RaycastGroundD1Ev(RaycastGround* self);

extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
}

int BobOmbBuddy::InitResources()
{
    RaycastGround rc;
    Vector3 pos;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x108,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov084_02130da4), 1, -1);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov084_02130d9c);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this) + 0xd4, ((char*)this), 0x8c000, 0x8c000, 0x4200004, 0);
    func_ov084_0212c960(((char*)this), 0);
    unk_198 = 0;

    {
        int z = mPosZ;
        int y = mPosY + 0x64000;
        pos.x = mPosX;
        pos.y = y;
        pos.z = z;
    }

    _ZN13RaycastGroundC1Ev(&rc);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, pos, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rc) != 0)
        mPosY = rc.floor[(0x44 - 0x14) / 4];

    if (func_ov084_0212ca60(((char*)this)) != 0) {
        void* player = _ZN5Actor13ClosestPlayerEv(((char*)this));
        unsigned char state = *(unsigned char*)((char*)player + 0x6d9);

        if (state != 0) goto state_check1;
        if (mParam == 0xb26)
            goto state_return0;

    state_check1:
        if (state != 1) goto state_check2;
        if (mParam == 0xb27)
            goto state_return0;

    state_check2:
        if (state != 2) goto state_lostcap;
        if (mParam == 0xb28)
            goto state_return0;

    state_lostcap:
        if (!_ZN8SaveData16HasPlayerLostCapEv())
            goto after_lostcap;

    state_return0:
        _ZN13RaycastGroundD1Ev(&rc);
        return 0;
    }

after_lostcap:
    if (func_ov084_0212cac0(((char*)this)) == 0)
        goto return1;
    if (data_0209f2f8 != 8)
        goto return1;
    if (data_0209f220 == 1)
        goto return0_2;
    if (IsStarCollected(SublevelToLevel(8), 1) != 0)
        goto return1;

return0_2:
    _ZN13RaycastGroundD1Ev(&rc);
    return 0;

return1:
    _ZN13RaycastGroundD1Ev(&rc);
    return 1;
}
