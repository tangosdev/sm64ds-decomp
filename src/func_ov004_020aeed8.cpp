//cpp
#include "types.h"
// @symbol func_ov004_020aeed8
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_OnAimedAtWithEggReturnVec
/* recovered: renamed to Class_Method */
/* dScMgBase_c::OnAimedAtWithEggReturnVec - recovered from vtable slot identity */
extern "C" u8 data_0209d45c;
extern "C" u8 data_0209d454;
extern "C" u8 data_0209d460;
extern "C" u8 data_0209d458;

extern "C" int _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern "C" int _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);

struct Obj {
    virtual int f00();
    virtual int f04();
    virtual int f08();
    virtual int f0c();
    virtual int f10();
    virtual int f14();
    virtual int f18();
    virtual int f1c();
    virtual int f20();
    virtual int f24();
    virtual int f28();
    virtual int f2c();
    virtual int f30();
    virtual int f34();
    virtual int f38();
    virtual int f3c();
    virtual int f40();
    virtual int f44();
    virtual int f48();
    virtual int f4c();
    virtual int f50();
    virtual int f54();
    virtual int f58();
    virtual int f5c();
    virtual int f60();
    virtual int f64();
    virtual int f68();
};

extern "C" void func_ov004_020aeed8(char* c)
{
    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    Obj* o = (Obj*)c;

    data_0209d45c = 0;
    data_0209d454 = 0;

    *(u32*)0x4000000 &= ~0x1f00;
    *(u32*)0x4001000 &= ~0x1f00;
    *(u16*)0x4000304 = (self->unk_224 << 15) | (*(u16*)0x4000304 & ~0x8000);
    *(u32*)0x4000000 = (*(u32*)0x4000000 & ~0xe000) | ((u32)data_0209d460 << 13);
    *(u32*)0x4001000 = (*(u32*)0x4001000 & ~0xe000) | ((u32)data_0209d458 << 13);

    _ZN2GX10LoadBGPlttEPKvjj(c + 0x4228, 0x20, 0x1e0);
    {
        char *p = c + 0x4228; p += 0x200;
        _ZN3GXS10LoadBGPlttEPKvjj(p, 0, 0x80);
    }

    if (o->f68() != 2)
    {
        char *src = (char*)0x6600000; src += 0x6000;
        MultiCopy_Int((int*)(c + 0x2228), (int*)src, 0x2000);
    }

    data_0209d45c = (u8)self->unk_21c;
    data_0209d454 = (u8)self->unk_220;

    if (o->f68() == 2)
        return;

    *(u32*)0x4000000 = (*(u32*)0x4000000 & ~0x1f00) | ((u32)data_0209d45c << 8);
    *(u32*)0x4001000 = (*(u32*)0x4001000 & ~0x1f00) | ((u32)data_0209d454 << 8);
}
