//cpp
// @symbol _ZN14dScMgMemory2_c13InitResourcesEv
#include "types.h"
#include "decl_common.h"
#include "dScMgMemory2_c.h"
/* dScMgMemory2_c::InitResources -- vtable slot 0.
 *
 * Attributed by the ROM's vtable: one of the three slots where this class's table
 * differs from dScMgSingle3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * Loads the language-specific card art into both OBJ VRAM banks with the shared
 * palette, resets the class's own state, then arms the timer and the HUD. The bail
 * on func_ov006_020c1a88 returning 0 is the only failure path, and it returns 0
 * from InitResources -- which is what the s32 return of fBase_c::InitResources
 * is for.
 *
 * func_ov006_0210a534 is dScMgSingle3DBase_c's slot 26, still unnamed at that
 * level, so it stays a mangled-free `extern "C"` call rather than being spelled as
 * an inherited method. */

extern "C" {
void func_ov006_0210a534(void);
s32 GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
void _ZN14dScMgMemory2_c9ResetGameEv(void *c);
void func_ov006_020c0aa8(void *c);
int func_ov006_020c1a88(void *c);
int func_ov004_020ad8b8(void);
int func_ov004_020ad878(void);
void func_ov004_020b66d4(void);
void func_ov004_020b04d0(int v);
void func_ov004_020b682c(void);
}
namespace GX { void LoadOBJPltt(void const *, unsigned int, unsigned int); }
namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

extern "C" {
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

s32 dScMgMemory2_c::InitResources()
{
    char *self = (char *)this;
    void *a;
    void *b;

    data_0209d45c = 0x11;
    func_ov006_0210a534();
    a = LoadFile(data_ov006_0213d370[GetGameLanguage()]);
    b = LoadFile(0xbb);
    DecompressLZ16(a, (void *)0x6400000);
    GX::LoadOBJPltt(b, 0, 0x100);
    DecompressLZ16(a, (void *)0x6600000);
    GXS::LoadOBJPltt(b, 0, 0x100);
    Deallocate(a);
    Deallocate(b);
    data_0209d454 = 0x18;
    _ZN14dScMgMemory2_c9ResetGameEv(self);
    func_ov006_020c0aa8(self + 0x4660);
    if (func_ov006_020c1a88(self + 0x4f38) == 0) return 0;
    *(int *)(self + 0xa8) = func_ov004_020ad8b8();
    *(int *)(self + 0xac) = *(int *)(self + 0xa8);
    *(int *)(self + 0xb4) = func_ov004_020ad878();
    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
    *(u8 *)(self + 0x5409) = 5;
    func_ov004_020b04d0(0x20);
    func_ov004_020b682c();
    return 1;
}
