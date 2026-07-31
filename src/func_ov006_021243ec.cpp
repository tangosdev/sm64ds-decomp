//cpp
// @symbol func_ov006_021243ec
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgTrampoline2_c.h"
// @emits dScMgTrampoline2_c_InitResources
/* recovered: renamed to Class_Method */
/* dScMgTrampoline2_c::InitResources - recovered from vtable slot identity */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

extern "C" {
extern s32 _ZN4cstd4fdivEii(s32 a, s32 b);
extern void func_ov006_020c0134(void *cam);

extern u8 data_0209d45c;
}

struct Base {
    virtual void m00();
    virtual void m04();
    virtual void m08();
    virtual void m0c();
    virtual void m10();
    virtual void m14();
    virtual void m18();
    virtual void m1c();
    virtual void m20();
    virtual void m24();
    virtual void m28();
    virtual void m2c();
    virtual void m30();
    virtual void m34();
    virtual void m38();
    virtual void m3c();
    virtual void m40();
    virtual void m44();
    virtual void m48(int x);
};

extern "C" int dScMgTrampoline2_c_InitResources(char *base)
{
    struct dScMgTrampoline2_c *self = (struct dScMgTrampoline2_c *)(void *)base;
    s32 fov;

    func_ov006_021245a8(base);
    data_0209d45c = 0x1d;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    *(u16 *)0x4000060 = (*(u16 *)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();
    *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 1;
    fov = _ZN4cstd4fdivEii(0xc0000, (s32)data_02082414);
    self->unk_470c = 0;
    self->unk_4710 = -0x64000;
    self->unk_4714 = 0;
    self->unk_4718 = 0;
    self->unk_471c = 0;
    self->unk_4720 = fov;
    self->unk_4724 = 0x400;
    func_ov006_020c0134(base + 0x466c);
    self->unk_47c8 = 0;
    self->unk_47cc = 0x82000;
    self->unk_47d0 = 0;
    self->unk_47d4 = 0;
    self->unk_47d8 = 0;
    self->unk_47dc = fov;
    self->unk_47e0 = 0x400;
    func_ov006_020c0134(base + 0x4728);
    if (func_ov006_020cae9c(base + 0x500c, 5) == 0)
        return 0;
    func_ov006_02120d8c(base + 0x7ad0, 5);
    func_ov006_020d0b2c();
    func_ov006_020cef14((char *)(base + 0x5ddc), 10);
    func_ov006_020ef0d4((int)(base + 0x6ffc), 10);
    data_ov006_021421b4 = 0;
    func_ov006_02122c04((int)(base + 0x7164), 0x14);
    func_ov004_020b04d0(0x10);
    ((Base *)base)->m48(-1);
    return 1;
}