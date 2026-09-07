//cpp
// @symbol _ZN10HootTheOwl13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "HootTheOwl.h"
#include "SharedFilePtr.h"
extern "C" {
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void*, void*, int*, int, int, unsigned int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void*, void*, int, int, void*, int);
extern int func_ov094_02136188(void*, void*);
extern int IsStarCollectedInCurLevel(int);
}
extern void* data_ov094_02136b40;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

int HootTheOwl::InitResources()
{
    int v0[3];
    void* f;

    f = Model::LoadFile(*(SharedFilePtr *)data_ov094_02136ae0);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(*(SharedFilePtr *)data_ov094_02136af8);
    Animation::LoadFile(*(SharedFilePtr *)data_ov094_02136ae8);
    Animation::LoadFile(*(SharedFilePtr *)data_ov094_02136af0);
    v0[0] = data_ov094_02136a1c[0];
    v0[1] = data_ov094_02136a1c[1];
    v0[2] = data_ov094_02136a1c[2];
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, v0, 0x64000, 0x64000, 0x800004, 0);

    unk_3cc = 0;
    mdCcAcPos_c.flags |= 2;
    mTerminalVelocity = -0x1e000;
    mAnimSpeed = 0x1000;
    unk_3e4 = 0x1f;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x50000, 0x64000, (void*)0, 0);
    func_ov094_02136188(this, &data_ov094_02136b40);

    if (data_0209f2f8 != 7) goto ret1;
    if (data_0209f220 != 1) {
        if (IsStarCollectedInCurLevel(1) != 0) goto ret1;
    }
    MarkForDestruction();
    return 0;
ret1:
    return 1;
}
