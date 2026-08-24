//cpp
// @symbol _ZN17dScMgTrampoline_c13InitResourcesEv
#include "decl_common.h"
#include "dScMgTrampoline_c.h"
/* dScMgTrampoline_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgTrampoline_c, this class's own
 * slot 0 (fBase_c::InitResources). The old file's `recovered name:
 * dScMgTrampoline_c_InitResources` agreed.
 *
 * The final `((Obj *)base)->m48(-1)` is a self-dispatch through this class's own
 * vtable slot 18 -- left as a raw vtable-shim call, same shape the pre-migration
 * file used, just through `this` instead of a `char *base` parameter. */
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

s32 dScMgTrampoline_c::InitResources()
{
    char *base = (char *)this;
    s32 fov;

    this->mScrollY = 0x20;
    this->mScrollTargetY = this->mScrollY;
    func_ov006_0212231c(base);
    data_0209d45c = 0x1d;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    *(u16 *)0x4000060 = (*(u16 *)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();
    *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 1;
    fov = _ZN4cstd4fdivEii(0xc0000, (s32)data_02082414);
    this->unk_470c = 0;
    this->unk_4710 = -0x64000;
    this->unk_4714 = 0;
    this->unk_4718 = 0;
    this->unk_471c = 0;
    this->unk_4720 = fov;
    this->unk_4724 = 0x400;
    Camera_UpdateMatrices(base + 0x466c);
    this->unk_47c8 = 0;
    this->unk_47cc = 0x82000;
    this->unk_47d0 = 0;
    this->unk_47d4 = 0;
    this->unk_47d8 = 0;
    this->unk_47dc = fov;
    this->unk_47e0 = 0x400;
    Camera_UpdateMatrices(base + 0x4728);
    if (func_ov006_020cd658((unsigned char *)(base + 0x500c), 4) == 0)
        return 0;
    func_ov006_02120d8c(base + 0x5cd0, 5);
    func_ov006_020d0b2c();
    func_ov004_020b04d0(0x10);
    ((Obj *)base)->m48(-1);
    return 1;
}
