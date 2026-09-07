//cpp
#include "TextureSequence.h"
#include "dMg3DEspModel_c.h"
struct BMD_File;
struct BTP_File;
extern "C" {
int func_020179b4(void* r0, void* r1, int r2);
void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* r);
void _ZN9Animation8LoadFileER13SharedFilePtr(void* r);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void* self, void* btp, int a, int b, unsigned int d);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* bca, int a, int b, unsigned int d);
}

extern void* data_ov006_02141e54;
extern void* data_ov006_02141e64;
extern void* data_ov006_02141e5c;
extern void* data_ov006_02141e7c;
extern void* data_ov006_02141e8c;
extern void* data_ov006_02141e84;

// @symbol _ZN15dMg3DEspModel_c13InitResourcesEv
int dMg3DEspModel_c::InitResources()
{
    char* c = (char*)this;
    if (func_020179b4(&data_ov006_02141e54, c + 0xc, 1) == 0)
        return 0;
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov006_02141e64);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e5c);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e7c);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e8c);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov006_02141e84);
    TextureSequence::Prepare(**(BMD_File**)((void**)&data_ov006_02141e54 + 1),
                             **(BTP_File**)((void**)&data_ov006_02141e64 + 1));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x70, *((void**)&data_ov006_02141e64 + 1), 0, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xc, *((void**)&data_ov006_02141e5c + 1), 0x40000000, 0x800, 0);
    mAnimSet.InitResources();
    ResetTransform();
    Reset();
    unk_208 = 0;
    return 1;
}
