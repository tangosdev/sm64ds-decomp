//cpp
// @symbol _ZN6ShipUp13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "ShipUp.h"
#include "dBgW.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void*);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void*);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void*, void*, void*, int, short, void*);
extern void func_020393d4(int* p, int v);
extern int IsStarCollected(int a, int b);
}
extern void* _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern unsigned char data_0209f220;

int ShipUp::InitResources()
{
    void* sp8[2];
    unsigned int idx;
    void* f;
    int b;
    sp8[0] = data_ov016_021149d4[0];
    sp8[1] = data_ov016_021149d4[1];
    b = (int)(actorID == 0x39);
    if (b != 0) mModelIndex = 0;
    else mModelIndex = 1;
    idx = mModelIndex;
    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov016_021136e4[idx]);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, f, 1, -1);
    func_ov016_021126a8(((char*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    idx = mModelIndex;
    f = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov016_021136dc[idx]);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((char*)this)+0x124, f, ((char*)this)+0x2ec, 0x1000, mAngleY, sp8[idx]);
    if (mModelIndex == 0) {
        func_020393d4((int*)((char*)&mMeshCollider), (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    }
    ((dBgW *)(((char*)this)+0x124))->Enable((dActor_c *)(((char*)this)));
    unk_324 = 0;
    unk_328 = 0;
    if (data_0209f220 > 1) {
        if (IsStarCollected(SublevelToLevel(8), 1) != 0) {
            if (mModelIndex == 0) goto ret1;
            return 0;
        }
    }
    if (mModelIndex == 0) return 0;
ret1:
    return 1;
}
