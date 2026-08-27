//cpp
// @symbol _ZN18dScMgTrampoline2_c13InitResourcesEv
#include "decl_common.h"
#include "dScMgTrampoline2_c.h"
/* dScMgTrampoline2_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgTrampoline2_c, this class's own
 * slot 0 (fBase_c::InitResources). The old file's `recovered name:
 * dScMgTrampoline2_c_InitResources` agreed.
 *
 * The final `((Base *)base)->m48(-1)` is a self-dispatch through this class's own
 * vtable slot 18 -- left as a raw vtable-shim call, same shape the pre-migration
 * file used, just through `this` instead of a `char *base` parameter. */
extern "C" {
extern s32 _ZN4cstd4fdivEii(s32 a, s32 b);
extern void Camera_UpdateMatrices(void *cam);

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

s32 dScMgTrampoline2_c::InitResources()
{
    char *base = (char *)this;
    s32 fov;

    func_ov006_021245a8(base);
    data_0209d45c = 0x1d;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    *(u16 *)0x4000060 = (*(u16 *)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();
    *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 1;
    fov = _ZN4cstd4fdivEii(0xc0000, (s32)data_02082414);
    this->mCamera0EyeX = 0;
    this->mCamera0EyeY = -0x64000;
    this->mCamera0EyeZ = 0;
    this->mCamera0TargetX = 0;
    this->mCamera0TargetY = 0;
    this->mCamera0TargetZ = fov;
    this->mCamera0Angle = 0x400;
    Camera_UpdateMatrices(base + 0x466c);
    this->mCamera1EyeX = 0;
    this->mCamera1EyeY = 0x82000;
    this->mCamera1EyeZ = 0;
    this->mCamera1TargetX = 0;
    this->mCamera1TargetY = 0;
    this->mCamera1TargetZ = fov;
    this->mCamera1Angle = 0x400;
    Camera_UpdateMatrices(base + 0x4728);
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
