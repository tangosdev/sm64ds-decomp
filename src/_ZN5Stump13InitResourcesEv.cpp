//cpp
// @symbol _ZN5Stump13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Stump.h"
extern "C" {
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor, int fix, int t, unsigned int a, unsigned int b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, int fix, int t, void *v, int t2);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *f, int a, int b);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *f);
}

extern char data_ov002_0210da40[];
extern char data_ov002_0210d9a0[];
extern char data_ov002_0210d9c0[];
extern char data_ov091_02135674[];
extern char data_ov091_0213567c[];
extern char data_ov091_02135684[];
extern char data_ov091_021356d0[];

int Stump::InitResources()
{
    mVariant = (*(s32 *)&param1) & 0xff;
    mTerminalVelocity = -0x1e000;
    if (mVariant == 0xff)
        mVariant = 0;
    if (mVariant == 1) {
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x110, ((char *)this), 0x3c000, 0x14000, 0x200002, 0);
        func_ov091_02134044(((char *)this), data_ov091_021356c0);
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x144, ((char *)this), 0x14000, 0, 0, 0);
        (*(s32 *)&mFlags) = 0;
        return 1;
    }
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char *)this) + 0x110, ((char *)this), 0x64000, 0x64000, 0x200002, 0x4a010);
    (*(s32 *)&mFlags) = 0x10000000;
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da40);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9a0);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9c0);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x300, _ZN5Model8LoadFileER13SharedFilePtr(data_ov091_02135674), 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov091_0213567c);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov091_02135684);
    (*(s32 *)((char *)&mModelAnim + 0x5c)) = 0x1000;
    func_ov091_02134044(((char *)this), data_ov091_021356d0);
    return 1;
}
