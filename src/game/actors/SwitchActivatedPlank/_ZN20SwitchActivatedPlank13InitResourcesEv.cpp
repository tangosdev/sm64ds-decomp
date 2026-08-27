//cpp
// @symbol _ZN20SwitchActivatedPlank13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "SwitchActivatedPlank.h"
typedef int Fix12i;
struct SharedFilePtr; struct BMD_File; struct KCL_File; struct Matrix4x3; struct CLPS_Block;
/* Model, ModelBase and dBgW_KcMbg are the real classes now, through this
   actor's header. The placeholder `struct dBgW_KcMbg { int d; };` was only
   ever a cast target and redefining the real one ICEd mwccarm; dBgW_Kc was
   never used as a type at all. */

extern "C" BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(ModelBase*, BMD_File*, int, int);
extern "C" KCL_File* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr&);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg*, KCL_File*, const Matrix4x3&, Fix12i, short, CLPS_Block&);
extern "C" void func_020393d4(int* p, int v);

extern SharedFilePtr data_ov029_0211432c;
extern SharedFilePtr data_ov029_02114324;
extern CLPS_Block data_ov029_0211304c;
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

int SwitchActivatedPlank::InitResources()
{
    char* c = ((char*)this);
    BMD_File* bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_0211432c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0x320), bmd, 1, -1);
    func_ov029_02112710(c);
    func_ov029_021126dc(c);
    {
        KCL_File* kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov029_02114324);
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            (dBgW_KcMbg*)(c + 0x124), kcl, *(const Matrix4x3*)(c + 0x370),
            0x199, *(short*)(c + 0x8e), data_ov029_0211304c);
    }
    func_020393d4((int*)(c + 0x124), (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    *(unsigned char*)(c + 0x3a2) = 0;
    *(short*)(c + 0x3a0) = 0;
    *(unsigned char*)(c + 0x3a3) = 0;
    *(unsigned char*)(c + 0x3a4) = (unsigned char)(*(int*)(c + 8) & 0x1f);
    return 1;
}
