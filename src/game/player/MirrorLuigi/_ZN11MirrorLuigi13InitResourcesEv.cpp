//cpp
// @symbol _ZN11MirrorLuigi13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MirrorLuigi.h"
extern "C" void* _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void*);
extern "C" void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void*, void*);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
struct M48 { int w[12]; };

extern "C" void func_02016b24(void *self, int v);
extern "C" void Vec3_Asr(void *d, void *s, int sh);
extern "C" void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(void *m, int angY);

extern void *data_ov002_0210ebb8;
extern void *data_ov002_0210eb20;
extern void *data_ov002_0210e8d0;
extern void *data_ov002_0210ebd8;
extern int data_020a0e68;
extern unsigned char data_0209f250;
extern int data_0209f394[];

int MirrorLuigi::InitResources()
{
    int t[3];

    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModelAnim, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210ebb8), 1, -1);
    func_02016acc(&mModelAnim, 0x80);
    func_02016b24(&mModelAnim, 0x40);

    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210eb20), 1, -1);
    func_02016acc(&mModel, 0x80);
    func_02016b24(&mModel, 0x40);

    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((&data_ov002_0210ebb8)[1], (&data_ov002_0210e8d0)[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequences[0], (&data_ov002_0210e8d0)[1], 0, 0x1000, 0);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((&data_ov002_0210eb20)[1], (&data_ov002_0210ebd8)[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequences[1], (&data_ov002_0210ebd8)[1], 0, 0x1000, 0);

    _ZN11ShadowModel12InitCylinderEv(&mShadowModel);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210eaa0), 0, 0x1000, 0);

    Vec3_Asr(t, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, t[0], t[1], t[2]);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);

    *(M48 *)&mModelAnim.mat4x3 = *(M48 *)&data_020a0e68;
    *(M48 *)&mModel.mat4x3 = *(M48 *)&data_020a0e68;

    func_ov055_021112c4(this, &data_ov055_02111b70, data_0209f394[data_0209f250]);

    data_ov055_02111b68 = 0;
    data_ov055_02111a90 = 0x1ffff;
    data_ov055_02111b6c = 0;
    return 1;
}
