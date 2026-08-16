//cpp
// @symbol _ZN13RacingPenguin13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RacingPenguin.h"
extern "C" unsigned char _ZN5Actor9TrackStarEjj(void*, unsigned int, unsigned int);
extern "C" void* _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void*, void*, int, int, unsigned int, unsigned int);
extern "C" void _ZN7PathPtr6FromIDEj(void*, unsigned int);
extern "C" void _ZNK7PathPtr7GetNodeER7Vector3j(void*, void*, unsigned int);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void*);
extern "C" void* _ZN15TextureSequence8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void*, void*);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void*, void*, int, int, void*, int);
extern "C" unsigned char NumStars(void);
extern "C" void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b, int cc, int d);


int RacingPenguin::InitResources()
{
    unsigned char *c = (unsigned char *)((void *)this);
    int i;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov019_02113498), 1, 1);

    for (i = 0; i < 7; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov019_02112788[i]);

    for (int j = 0; j < 3; j++) {
        void *t = data_ov019_0211277c[j];
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(t);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File((&data_ov019_02113498)[1], *(void **)((char *)t + 4));
    }

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x14c) == 0)
        return 0;

    if (NumStars() == 0x96) {
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x174, c, 0xd0000, 0x12c000, 0x800004, 0);
        *(int *)(c + 0x80) = 0x1999;
        *(int *)(c + 0x84) = 0x1000;
        *(int *)(c + 0x88) = 0x1999;
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1a8, c, 0xd0000, 0xd0000, 0, 0);
        _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(c, 0xf0000, 0xf0000, 0x1c20000, 0x1c20000);
    } else {
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x174, c, 0x82000, 0x12c000, 0x800004, 0);
        *(int *)(c + 0x80) = 0x1000;
        *(int *)(c + 0x84) = 0x1000;
        *(int *)(c + 0x88) = 0x1000;
        _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1a8, c, 0x82000, 0x82000, 0, 0);
    }

    *(unsigned char *)(c + 0x396) =
        _ZN5Actor9TrackStarEjj(c, (unsigned char)((*(unsigned int *)(c + 8) >> 8) & 0xf), 2);
    func_ov019_021122dc(c, 0);
    _ZN7PathPtr6FromIDEj(c + 0x364, *(unsigned int *)(c + 8) & 0xff);
    *(int *)(c + 0x36c) = 0;
    _ZNK7PathPtr7GetNodeER7Vector3j(c + 0x364, c + 0x5c, *(unsigned int *)(c + 0x36c));
    func_ov019_021113b0(c);
    func_ov019_021114ec(c);
    return 1;
}
