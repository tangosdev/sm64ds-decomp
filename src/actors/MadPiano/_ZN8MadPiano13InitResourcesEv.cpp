//cpp
// @symbol _ZN8MadPiano13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MadPiano.h"
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
extern void _ZN11ShadowModel10InitCuboidEv(void *);
extern void _ZN11ShadowModel12InitCylinderEv(void *);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *, void *, int, int, void *, int);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *, void *, void *, int, int, unsigned int, unsigned int);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, void *, void *, int, short, void *);
extern void func_ov063_0211d5f4(char *);
}


int MadPiano::InitResources()
{
    int i;
    char *p;
    void *f;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov063_0211ef80);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0x320, f, 1, -1);
    _ZN11ShadowModel10InitCuboidEv((char *)&mShadowModel1);
    _ZN11ShadowModel10InitCuboidEv((char *)&mShadowModel2);
    _ZN11ShadowModel12InitCylinderEv((char *)&mShadowModel3);
    f = _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov063_0211ef90);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char *)this) + 0x320, f, 0, 0x1000, 0);
    unk_09c = -0x2000;
    unk_0a0 = -0x3c000;
    unk_6d4 = mPosX;
    unk_6d8 = mPosY;
    unk_6dc = mPosZ;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x50c, ((char *)this), 0x159000, 0x159000, 0, 0);
    for (i = 0, p = ((char *)this) + 0x48c; i < 2; i++) {
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
            p, ((char *)this), ((char *)this) + 0x5c, 0x9b000, 0xfa000, 0x200004, 0);
        p += 0x40;
    }
    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov063_0211ef88);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char *)this) + 0x124, f, ((char *)this) + 0x2ec, 0x199, mAngleY, &data_ov063_0211ecb8);
    func_ov063_0211d88c(((char *)this));
    func_ov063_0211d828(((char *)this));
    func_ov063_0211d5f4(((char *)this));
    return 1;
}
