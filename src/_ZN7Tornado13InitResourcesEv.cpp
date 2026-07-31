//cpp
// @symbol _ZN7Tornado13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Tornado.h"
extern "C" {
extern int _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* f, int a, int b);
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, int a, int fix, unsigned int u);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void* a, void* b);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void* thiz, void* f, int a, int fix, unsigned int u);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int a, int b, void* v, int e);
}

int Tornado::InitResources()
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0x2c4, (void*)_ZN5Model8LoadFileER13SharedFilePtr(&data_ov096_02137ba8), 1, 0x15);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov096_02137bb0);
    func_02016aac(((char*)this) + 0x2c4, 0x16, 1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this) + 0x2c4, (void*)data_ov096_02137bb0[1], 0, 0x1000, 0);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File((void*)data_ov096_02137ba8[1], func_02112968);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(((char*)this) + 0x328, func_02112968, 0, 0x1000, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this) + 0xd4, ((char*)this), 0, 0, 0x200002, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char*)this) + 0x108, ((char*)this), 0x50000, 0x50000, 0, 0);

    unk_340 = mPosX;
    unk_344 = mPosY;
    unk_348 = mPosZ;
    unk_35a = 0;
    unk_35c = 0;
    unk_360 = 0;
    unk_09c = -0x1000;
    unk_0a0 = -0x1e000;
    {
        unsigned int t = mParam & 0xff;
        if (t == 0xff)
            unk_34c = 0x5dc000;
        else
            unk_34c = t * 0x64000;
    }
    unk_364 = 0;
    unk_368 = 0;
    return 1;
}
