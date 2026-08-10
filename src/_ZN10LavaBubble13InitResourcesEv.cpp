//cpp
// @symbol _ZN10LavaBubble13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "LavaBubble.h"
extern "C" {
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(char* thiz, char* actor, int b, int d, unsigned int e, unsigned int f);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* thiz, char* actor, int b, int d, void* v, int f);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(char* thiz);
extern void func_ov064_021187ec(char* c, void* p);
}
extern char data_ov064_0211c7c8[];
extern char data_ov064_0211c7b8[];

int LavaBubble::InitResources()
{
    unk_310 = (mParam & 1) ^ 1;
    if (unk_310 == 0) {
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this) + 0x110, ((char*)this), 0, 0, 1, 0);
    } else {
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this) + 0x110, ((char*)this), 0x32000, 0x50000, 0x200002, 0x8000);
    }
    unk_304 = mPosX;
    unk_308 = mPosY;
    unk_30c = mPosZ;
    if (unk_310 == 0) {
        *(int *)((char*)&unk_0b0) |= 1;
    } else {
        unk_09c = -0x4000;
        unk_0a0 = -0x3c000;
    }
    unk_100 = 0;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x144, ((char*)this), 0x32000, 0x32000, 0, 0);
    _ZN12WithMeshClsn13SetLimMovFlagEv((char*)&mWithMeshClsn);
    if (unk_310 != 0) {
        func_ov064_021187ec(((char*)this), data_ov064_0211c7c8);
    } else {
        func_ov064_021187ec(((char*)this), data_ov064_0211c7b8);
    }
    return 1;
}
