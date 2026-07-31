//cpp
// @symbol _ZN4Bird13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Bird.h"
extern "C" {
struct SharedFilePtr; struct BMD_File; struct BCA_File;
void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
void* _ZN9Animation8LoadFileER13SharedFilePtr(void*);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
void _ZN11ShadowModel12InitCylinderEv(void*);
}

int Bird::InitResources()
{
    void* m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov009_02113c20);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, m, 1, 1);
    void* a = _ZN9Animation8LoadFileER13SharedFilePtr(data_ov009_02113c28);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this)+0xd4, a, 0, 0x1000, 0);
    _ZN11ShadowModel12InitCylinderEv((char*)&mShadowModel);
    {
        int *p60 = (int *)(((int)((char*)this) + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
        int y = *p60;
        int zero = 0;
        *p60 = y + 0xa000;
        unk_09c = zero;
        unk_0a0 = -0x32000;
        unk_180 = 1;
        unk_178 = unk_004;
        unk_160 = mPosX;
        unk_164 = mPosY;
        unk_168 = mPosZ;
        unk_17c = zero;
    }
    return 1;
}
