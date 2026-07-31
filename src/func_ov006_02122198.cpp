//cpp
#include "types.h"
// @symbol func_ov006_02122198
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgTrampoline_c.h"
// @emits dScMgTrampoline_c_InitResources
/* recovered: renamed to Class_Method */
/* dScMgTrampoline_c::InitResources - recovered from vtable slot identity */
extern "C" {

extern s32 _ZN4cstd4fdivEii(s32 a, s32 b);
extern void Camera_UpdateMatrices(void *cam);

extern u8 data_0209d45c;

}

struct Obj {
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
    virtual void m48(int a);
};

extern "C" int dScMgTrampoline_c_InitResources(char *base)
{
    struct dScMgTrampoline_c *self = (struct dScMgTrampoline_c *)(void *)base;
    s32 fov;

    self->unk_5d94 = 0x20;
    self->unk_5d98 = self->unk_5d94;
    func_ov006_0212231c(base);
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
    Camera_UpdateMatrices(base + 0x466c);
    self->unk_47c8 = 0;
    self->unk_47cc = 0x82000;
    self->unk_47d0 = 0;
    self->unk_47d4 = 0;
    self->unk_47d8 = 0;
    self->unk_47dc = fov;
    self->unk_47e0 = 0x400;
    Camera_UpdateMatrices(base + 0x4728);
    if (func_ov006_020cd658((unsigned char *)(base + 0x500c), 4) == 0)
        return 0;
    func_ov006_02120d8c(base + 0x5cd0, 5);
    func_ov006_020d0b2c();
    func_ov004_020b04d0(0x10);
    ((Obj *)base)->m48(-1);
    return 1;
}