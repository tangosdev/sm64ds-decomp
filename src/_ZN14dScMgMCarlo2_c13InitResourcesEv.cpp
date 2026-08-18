//cpp
// @symbol _ZN14dScMgMCarlo2_c13InitResourcesEv
#include "decl_common.h"
#include "dScMgMCarlo2_c.h"
/* dScMgMCarlo2_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgMCarlo2_c, this class's own
 * slot 0 (fBase_c::InitResources). The old file's `recovered name:
 * dScMgMCarlo2_c_InitResources` agreed.
 *
 * The final `((Base *)c)->m48(-1)` is a self-dispatch through this class's own
 * vtable slot 18 -- left as a raw vtable-shim call, same shape the pre-migration
 * file used, just through `this` instead of a `char *c` parameter. */
extern "C" void func_ov006_0210a534(void *c);
extern "C" int GetGameLanguage(void);
extern "C" int LoadFile(int handle);
extern "C" void DecompressLZ16(int src, int dst);
extern "C" void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, unsigned int a, unsigned int b);
extern "C" void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, unsigned int a, unsigned int b);
extern "C" void func_ov006_020c0aa8(void *p);
extern "C" int func_ov006_020c1a88(void *p);
extern "C" unsigned char data_0209d45c;
extern "C" unsigned char data_0209d454;

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

s32 dScMgMCarlo2_c::InitResources()
{
    char *c = (char *)this;
    int handle;
    int f1, f2;
    func_ov004_020b04d0(0x20);
    func_ov006_0210a534(c);
    handle = GetGameLanguage();
    f1 = LoadFile(data_ov006_0213d744[handle]);
    f2 = LoadFile(0xbb);
    DecompressLZ16(f1, 0x6400000);
    DecompressLZ16(f1, 0x6600000);
    _ZN2GX11LoadOBJPlttEPKvjj((void *)f2, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj((void *)f2, 0, 0x100);
    Deallocate((void *)f1);
    Deallocate((void *)f2);
    data_0209d45c = 0x11;
    data_0209d454 = 0x18;
    *(volatile unsigned short *)0x4000008 = (*(volatile unsigned short *)0x4000008 & ~3) | 1;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    func_ov006_020c0aa8((void *)(c + 0x4660));
    if (func_ov006_020c1a88((void *)(c + 0x4f38)) == 0) return 0;
    func_ov004_020b682c();
    ((Base *)c)->m48(-1);
    this->unk_592a = 0;
    this->unk_0a8 = 0xa;
    this->unk_0ac = this->unk_0a8;
    return 1;
}
