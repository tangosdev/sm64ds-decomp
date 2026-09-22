//cpp
// @symbol _ZN13RacingPenguin13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RacingPenguin.h"
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
extern "C" void _ZN7PathPtr6FromIDEj(void*, unsigned int);
extern "C" void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void*, void*, int, int, void*, int);
extern "C" unsigned char NumStars(void);
extern "C" void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b, int cc, int d);


int RacingPenguin::InitResources()
{
    unsigned char *c = (unsigned char *)((void *)this);
    int i;

    mModelAnim.SetFile(
        (BMD_File *)Model::LoadFile(*(SharedFilePtr *)&data_ov019_02113498), 1, 1);

    for (i = 0; i < 7; i++)
        Animation::LoadFile(*(SharedFilePtr *)data_ov019_02112788[i]);

    for (int j = 0; j < 3; j++) {
        void *t = data_ov019_0211277c[j];
        TextureSequence::LoadFile(*(SharedFilePtr *)t);
        TextureSequence::Prepare(
            *(BMD_File *)((void **)&data_ov019_02113498)[1],
            *(BTP_File *)((void **)t)[1]);
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    if (NumStars() == 0x96) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x174, c, 0xd0000, 0x12c000, 0x800004, 0);
        mScaleX = 0x1999;
        mScaleY = 0x1000;
        mScaleZ = 0x1999;
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(c + 0x1a8, c, 0xd0000, 0xd0000, 0, 0);
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(c, 0xf0000, 0xf0000, 0x1c20000, 0x1c20000);
    } else {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x174, c, 0x82000, 0x12c000, 0x800004, 0);
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(c + 0x1a8, c, 0x82000, 0x82000, 0, 0);
    }

    unk_396 = (u8)TrackStar((u8)((param1 >> 8) & 0xf), 2);
    func_ov019_021122dc(c, 0);
    _ZN7PathPtr6FromIDEj(&mPath, param1 & 0xff);
    mPathNodeIndex = 0;
    mPath.GetNode(*(Vector3 *)&mPosX, mPathNodeIndex);
    func_ov019_021113b0(c);
    func_ov019_021114ec(c);
    return 1;
}
