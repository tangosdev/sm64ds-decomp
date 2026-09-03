//cpp
// @symbol _ZN14daObjTtWater_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjTtWater_c.h"
#include "dBgW.h"
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* bmd, void* bta);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* thiz, void* bta, int a, int b, unsigned int e);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void* thiz);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* thiz);
extern void* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, void* kcl, void* mtx, int fix, short s, void* clps);
extern int _ZN5Event6GetBitEj(unsigned int n);
}

int daObjTtWater_c::InitResources()
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov033_021124f0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4, m, 1, 0x14);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*(void**)((char*)data_ov033_021124f0 + 4), data_ov033_02111bc8);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(((char*)this) + 0x320, data_ov033_02111bc8, 0, 0x1000, 0);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(((char*)this));
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(((char*)this));
    void* mc = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov033_021124e8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124, mc, ((char*)this) + 0x2ec, 0x1000, mAngleY, data_ov033_02111c1c);
    ((dBgW *)(((char*)this) + 0x124))->Enable((dActor_c *)(((char*)this)));
    mMinPosY = mPosY - 0x3c000;
    return _ZN5Event6GetBitEj(0xe) == 0;
}
