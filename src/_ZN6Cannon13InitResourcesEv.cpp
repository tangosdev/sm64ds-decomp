//cpp
#include "types.h"
// @symbol _ZN6Cannon13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Cannon.h"
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *file, int a, int b);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor, int r, int h, unsigned int a, unsigned int b);
extern "C" char data_ov098_0213c91c;
extern "C" char data_ov002_0210da38;

int Cannon::InitResources()
{
    char *c = (char*)((void *)this);
    void *f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov098_0213c8e8);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
    *(int*)(c + 0x194) = *(int*)(*(int*)(c + 0xe4) + 0x58);
    *(u8*)(c + 0x184) = *(int*)(c + 8) & 3;
    *(int*)(c + 0x174) = 0;
    *(int*)(c + 0x60) -= 0x50000;
    *(int*)(c + 0x15c) = *(int*)(c + 0x5c);
    *(int*)(c + 0x160) = *(int*)(c + 0x60);
    *(int*)(c + 0x164) = *(int*)(c + 0x64);
    *(short*)(c + 0x17c) = *(short*)(c + 0x94);
    *(u8*)(c + 0x185) = 0;
    u8 k = *(u8*)(c + 0x184);
    if (k != 2) {
        if (k == 1) *(short*)(c + 0x8c) = 0x2000;
        *(short*)(c + 0x178) = 0x2000;
        *(short*)(c + 0x17a) = *(short*)(c + 0x8e);
        *(int*)(c + 0x180) = 0;
        func_ov098_0213b15c(((void *)this));
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x124, ((void *)this), 0xa0000, 0x12c000, 0x800004, 0);
    } else {
        *(int*)(c + 0x60) -= 0x190000;
        *(short*)(c + 0x17a) = *(short*)(c + 0x90);
        *(short*)(c + 0x178) = 0x2000;
        *(int*)(c + 0x180) = 2;
        *(int*)(c + 0xb0) &= ~1;
        _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x124, ((void *)this), 0x50000, 0x12c000, 0x800004, 0);
    }
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov098_0213c91c);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da38);
    return 1;
}
