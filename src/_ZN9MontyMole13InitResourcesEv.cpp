//cpp
// @symbol _ZN9MontyMole13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MontyMole.h"
/* SharedFilePtr stays incomplete: Model.h forward-declares it and its layout is
   deliberately not recovered (include/SharedFilePtr.h). Used only by address here. */
typedef struct BMD_File BMD_File;
typedef struct BCA_File BCA_File;
typedef struct Actor Actor;
extern SharedFilePtr* data_ov080_0212766c[];
extern SharedFilePtr data_ov002_0210d9d8;
extern SharedFilePtr data_ov080_021283c8;
extern SharedFilePtr data_ov080_021283c0;
extern "C" {
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, BCA_File* f, int a, Fix12i b, unsigned int e);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, Actor* a, Fix12i r, Fix12i h, unsigned int e, unsigned int g);
}

int MontyMole::InitResources()
{
    int i;
    for (i = 0; i < 4; i++) _ZN9Animation8LoadFileER13SharedFilePtr(data_ov080_0212766c[i]);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9d8);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov080_021283c8);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this)+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov080_021283c0), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this)+0xd4, (BCA_File*)data_ov080_021283d8[1], 0, 0x1000, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(((char*)this)+0x138, (Actor*)((char*)this), 0x50000, 0x64000, 0x200000, 0x66fe0);
    unk_180 = unk_008 & 0xf;
    unk_17c = 0;
    if (unk_180 == 0) unk_181 = 1;
    else unk_181 = (unk_008 >> 8) & 1;
    unk_182 = (unk_008 >> 4) & 0xf;
    unk_184 = 0;
    unk_183 = 0;
    for (i = 0; i < 4; i++) ((int*)((char*)&unk_16c))[i] = 0;
    unk_188 = 0;
    return 1;
}
