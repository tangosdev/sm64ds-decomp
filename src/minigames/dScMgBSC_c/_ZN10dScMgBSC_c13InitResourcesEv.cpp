//cpp
// @symbol _ZN10dScMgBSC_c13InitResourcesEv
#include "decl_common.h"
#include "dScMgBSC_c.h"
/* dScMgBSC_c::InitResources -- vtable slot 0.
 *
 * Attributed by tools/rtti_vtables.py --own dScMgBSC_c, which lists this address
 * as this class's own slot 0 (fBase_c::InitResources), distinct from
 * dScMgSingle3DBase_c's. The old file's `recovered name: dScMgBSC_c_InitResources`
 * agreed.
 *
 * The final call is a self-dispatch through this class's own vtable slot 18
 * (func_ov006_02125364, per the same rtti dump) -- neither dScMgBase_c.h nor this
 * header names that slot yet (both leave 18-35 undeclared), so it stays a raw
 * vtable-shim call exactly as the pre-migration file spelled it, just through
 * `this` instead of a `void *self` parameter. */
extern "C" {
extern unsigned char data_0209d45c[];
extern int data_0208ee44[];
extern void func_ov006_0210a534(void);
extern s32 GetGameLanguage(void);
extern u32 LoadCompressedFileAt(int fileID, void *target);
extern int LoadFile(int handle);
extern void func_ov006_020c0aa8(void *p);
extern int func_ov006_020c1a88(void *p);
}
extern "C" void _ZN3GXS11LoadOBJPlttEPKvjj(void const *a, unsigned int b, unsigned int c);

struct Obj {
    virtual void v0();virtual void v1();virtual void v2();virtual void v3();
    virtual void v4();virtual void v5();virtual void v6();virtual void v7();
    virtual void v8();virtual void v9();virtual void v10();virtual void v11();
    virtual void v12();virtual void v13();virtual void v14();virtual void v15();
    virtual void v16();virtual void v17();virtual void v18(int x);
    char pad[0x10000];
};

s32 dScMgBSC_c::InitResources()
{
    Obj *o = (Obj *)this;
    char *c = (char *)this;
    int fh;
    data_0209d45c[0] = 0x11;
    func_ov006_0210a534();
    LoadCompressedFileAt(data_ov006_0213fe78[GetGameLanguage()], (void *)0x6600000);
    fh = LoadFile(0xb3);
    _ZN3GXS11LoadOBJPlttEPKvjj((void *)fh, 0, 0x100);
    Deallocate((void *)fh);
    func_ov006_020c0aa8(c + 0x4660);
    if (func_ov006_020c1a88(c + 0x4f38) == 0)
        return 0;
    func_ov004_020b04d0(0x20);
    func_ov004_020b682c();
    data_0208ee44[0] = 1;
    *(int *)(c + 0xa8) = func_ov004_020ad8b8();
    *(int *)(c + 0xac) = *(int *)(c + 0xa8);
    o->v18(3);
    return 1;
}
