//cpp
// @symbol _ZN3Key13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Key.h"
extern "C" {
extern void LoadKeyModels(int idx);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* bca, int a, int fx, unsigned int f);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* thiz, void* actor, void* pos, int r, int s, unsigned int a, unsigned int b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int a, int b, void* v, void* w);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* pos);
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN5Event8ClearBitEj(unsigned int n);
}

extern char data_ov002_02110964;
extern char data_ov002_0211094c;
extern char data_ov089_02132b40;
extern char data_ov089_02132c60;
extern char data_ov089_02132c40;
extern char data_ov089_02132c70;
extern char data_ov089_02132c48;
extern int data_ov089_021328b4[];
extern char data_ov089_02132ca4;
extern int data_0209cef0;

int Key::InitResources()
{
    int v[3];
    int v2[3];
    int kind = param1 & 7;
    mState = kind;
    LoadKeyModels(mState);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_02110964);

    if (mState == 7) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x114, *(void**)(&data_ov002_0211094c + 4), 1, 1) == 0)
            return 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this) + 0x114, *(void**)(&data_ov002_02110964 + 4), 0x40000000, 0x1000, 0);
        mScaleX = 0x2000;
        mScaleY = 0x2000;
        mScaleZ = 0x2000;
        v[0] = *(int*)(&data_ov089_02132b40);
        v[1] = *(int*)(&data_ov089_02132b40 + 4);
        v[2] = *(int*)(&data_ov089_02132b40 + 8);
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(((char*)this) + 0x220, ((char*)this), v, 0x50000, 0xfa000, 0x800003, 0x8000);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x260, ((char*)this), 0xa0000, 0, 0, 0);
        mSpinSpeed = 0x400;
        mVertAccel = 0;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x57, ((char*)this) + 0x74);
    } else {
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c60);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c40);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c70);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov089_02132c48);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x114, *(void**)((char*)data_ov089_02132894[mState] + 4), 1, 1) == 0)
            return 0;
        {
            void* m = (void*)data_ov089_021328b4[mState];
            if (m != 0) {
                if (_ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x178, *(void**)((char*)m + 4), 1, -1) == 0)
                    return 0;
            }
        }
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this) + 0x114, *(void**)(&data_ov002_02110964 + 4), 0x40000000, 0x1000, 0);
        mScaleX = 0x3000;
        mScaleY = 0x3000;
        mScaleZ = 0x3000;
        mVertSpeed = 0x23000;
        v2[0] = *(int*)(&data_ov089_02132ca4);
        v2[1] = *(int*)(&data_ov089_02132ca4 + 4);
        v2[2] = *(int*)(&data_ov089_02132ca4 + 8);
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(((char*)this) + 0x220, ((char*)this), v2, 0x50000, 0x64000, 0x800003, 0x8000);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x260, ((char*)this), 0x48000, 0, 0, 0);
        mVertAccel = -0x2000;
        mSpinSpeed = 0;
    }

    if (_ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel) == 0)
        return 0;
    _ZN12WithMeshClsn13SetLimMovFlagEv((char*)&mWithMeshClsn);
    unk_41c = mPosX;
    unk_420 = mPosY;
    unk_424 = mPosZ;
    unk_100 = 0;
    unk_442 = 0;
    unk_443 = 0;
    mTerminalVelocity = -0x32000;
    unk_448 = 0;
    unk_110 = 0;
    unk_46c = 0;
    unk_468 = unk_46c;
    unk_464 = unk_468;
    if (data_0209cef0 == 0)
        _ZN5Event8ClearBitEj(0x1d);
    return 1;
}
