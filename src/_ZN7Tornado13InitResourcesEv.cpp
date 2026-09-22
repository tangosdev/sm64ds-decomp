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
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, void* actor, int a, int b, unsigned int c, unsigned int d);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int a, int b, void* v, int e);
}

int Tornado::InitResources()
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, (void*)_ZN5Model8LoadFileER13SharedFilePtr(&data_ov096_02137ba8), 1, 0x15);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov096_02137bb0);
    func_02016aac(&mModelAnim, 0x16, 1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (void*)data_ov096_02137bb0[1], 0, 0x1000, 0);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File((void*)data_ov096_02137ba8[1], func_02112968);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(&mTextureTransformer, func_02112968, 0, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0, 0, 0x200002, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x50000, 0x50000, 0, 0);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    unk_35a = 0;
    mState = 0;
    mTriggerCount = 0;
    mVertAccel = -0x1000;
    mTerminalVelocity = -0x1e000;
    {
        unsigned int t = param1 & 0xff;
        if (t == 0xff)
            mChaseRange = 0x5dc000;
        else
            mChaseRange = t * 0x64000;
    }
    mParticleHandle0 = 0;
    mParticleHandle1 = 0;
    return 1;
}
