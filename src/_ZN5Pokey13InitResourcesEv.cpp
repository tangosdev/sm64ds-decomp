//cpp
// @symbol _ZN5Pokey13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "Pokey.h"
extern "C" {
void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* file, int a, int b);
void LoadBlueCoinModel(void* actor);
int _ZN11ShadowModel12InitCylinderEv(void* self);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* self, void* actor, int r, int h, unsigned int d, unsigned int e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* self, void* actor, int b, int c, void* v, int e);
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
void func_ov096_02136928(void* self, int a);
void func_ov096_02135efc(void* self);
}

extern void* data_ov096_02137b20;
extern void* data_ov096_02137b28;

struct Block48 { int w[12]; };
struct Block3 { int w[3]; };
extern Block48 IDENTITY_MATRIX4X3;

int Pokey::InitResources()
{
    int t;

    t = (actorID == 0xf0);
    if (t != false) {
        void* m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov096_02137b20);
        _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, 1);
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov096_02137b28);
        LoadBlueCoinModel(((char*)this));
        unk_3a8 = 1;
    } else {
        t = (actorID == 0xf1);
        if (t != false) {
            void* m = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov096_02137b28);
            if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, 1) == 0)
                return 0;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char*)this) + 0x14c, ((char*)this), 0x3c000, 0x78000, 0x200004, 0x6eff0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x180, ((char*)this), 0x3c000, 0x3c000, 0, 0);

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mHorzSpeed = 0x3000;

    t = (actorID == 0xf0);
    if (t != false) {
        unk_36c = mPosX;
        unk_370 = mPosY;
        unk_374 = mPosZ;
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        unk_390 = 0;
        unk_394 = 0;
    } else {
        t = (actorID == 0xf1);
        if (t != false) {
            mScaleX = 0;
            mScaleY = 0;
            mScaleZ = 0;
            *(void**)((char*)&unk_390) = _ZN8dActor_c10FindWithIDEj(param1);
            unk_394 = 0;
            {
                int* p = (int*)(((int)*(char**)((char*)&unk_390) + 0x36c));
                unk_36c = p[0];
                unk_370 = p[1];
                unk_374 = p[2];
            }
        }
    }

    func_ov096_02136928(((char*)this), 1);
    *(Block48*)((char*)&unk_33c) = IDENTITY_MATRIX4X3;
    func_ov096_02135efc(((char*)this));
    return 1;
}
