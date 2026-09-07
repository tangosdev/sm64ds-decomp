//cpp
// @symbol _ZN17BowserSkyPlatform13InitResourcesEv
#include "BowserSkyPlatform.h"
// recovered name: daKpa3Bg_c_InitResources
/* recovered: renamed to Class_Method */
/* daKpa3Bg_c::InitResources - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. Model, ModelBase, dBgW_Kc, dBgW_KcMbg and
   dBgW are the real classes, pulled in transitively through
   BowserSkyPlatform.h -> dBgActor_c.h; declaring local shadows with bodies
   for them (as the pre-header-include draft did) redefines the real class
   and fails to compile, so this uses the real types directly. */
struct BMD_File; struct KCL_File; struct dActor_c; struct Matrix4x3;
struct CLPS_Block;

/* Declared by its final name rather than as a shadow method: the ROM's symbol takes
   Fix12<int> where the call site has an int, and Fix12<int> is an aggregate with no
   converting constructor, so a shadow declaration cannot be both callable and mangle
   correctly. Spelling the symbol literally sidesteps the mangling entirely -- the
   parameter types then only decide the call ABI, and a Fix12 passes in one register
   bit-identical to an int. */
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void* self, KCL_File* f, const Matrix4x3& m, int fix, short sh, CLPS_Block& b);

extern "C" void CopyTexPalFromLevelModel(void* p);
extern "C" void func_020393d4(void* p, void* v);
extern "C" void func_020393c4(void* p, void* v);

extern "C" int data_0208e738;
extern "C" SharedFilePtr* data_ov060_02119514[];
extern "C" SharedFilePtr* data_ov060_0211953c[];
extern "C" CLPS_Block* data_ov060_0211a980[];
extern "C" void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
extern "C" void func_ov060_021183f4();

s32 BowserSkyPlatform::InitResources()
{
    char* self = (char*)this;
    int idx = *(int*)(self + 8) & 0xf;
    *(unsigned char*)(self + 0x329) = (unsigned char)idx;
    data_0208e738 = 0;
    ((ModelBase*)(self + 0xd4))->SetFile(
        (BMD_File*)Model::LoadFile(*data_ov060_02119514[*(unsigned char*)(self + 0x329)]), 1, -1);
    CopyTexPalFromLevelModel(self + 0xdc);
    data_0208e738 = 1;
    this->UpdateClsnPosAndRot();
    {
        int i = *(unsigned char*)(self + 0x329);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            self + 0x124, (KCL_File*)dBgW_Kc::LoadFile(*data_ov060_0211953c[i]),
            *(Matrix4x3*)(self + 0x2ec), 0x1000, *(short*)(self + 0x8e),
            *data_ov060_0211a980[i]);
    }
    func_020393d4(self + 0x124, (void*)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(self + 0x124, (void*)&func_ov060_021183f4);
    ((dBgW*)(self + 0x124))->Enable((dActor_c*)self);
    *(int*)(self + 0x320) = 0;
    *(unsigned char*)(self + 0x32b) = 0;
    *(unsigned char*)(self + 0x328) = 0;
    *(unsigned char*)(self + 0x32a) = 0;
    *(unsigned short*)(self + 0x326) = 0;
    *(unsigned short*)(self + 0x324) = *(unsigned short*)(self + 0x326);
    *(int*)(self + 0x9c) = 0;
    *(int*)(self + 0xa0) = -0x1e000;
    return 1;
}
