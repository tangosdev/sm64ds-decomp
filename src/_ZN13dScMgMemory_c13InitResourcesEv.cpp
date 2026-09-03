//cpp
// @symbol _ZN13dScMgMemory_c13InitResourcesEv
#include "types.h"
#include "decl_common.h"
#include "dScMgMemory_c.h"
/* dScMgMemory_c::InitResources -- vtable slot 0, ov006 0x020f53e4.
 *
 * Attributed by the vtable: dScMgMemory_c's own table is ov006 0x0213d1b8 and
 * its slot 0 relocates here. The signature is include/fBase_c.h's own slot
 * 0, `virtual s32 InitResources()`.
 *
 * unk_0a8, unk_0ac and mHudScore ARE dScMgBase_c'S, not this class's, which is
 * what include/dScMgMemory_c.h's banner says about the three fields the old
 * auto-generated header declared below 0x4f38. They read as inherited members
 * here, which is the point of the migration: the pre-migration file wrote all
 * three through `*(int *)(self + 0xa8)` and could not have said whose they
 * were.
 *
 * GX::LoadOBJPltt and GXS::LoadOBJPltt are declared as namespaced functions
 * rather than as extern "C" of their mangled names, so the compiler produces
 * _ZN2GX11LoadOBJPlttEPKvjj and _ZN3GXS11LoadOBJPlttEPKvjj the ordinary way.
 * A bare extern "C" of the mangled spelling would mangle a SECOND time in a
 * //cpp translation unit -- the defect include/SharedFilePtr.h's banner
 * records -- and reference a symbol nothing defines. */

extern "C" {
void func_ov006_0210a534(void);
s32 GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
void _ZN13dScMgMemory_c9ResetGameEv(void *c);
void func_ov006_020c0aa8(void *c);
int func_ov006_020c1a88(void *c);
int func_ov004_020ad8b8(void);
int func_ov004_020ad878(void);
void func_ov004_020b66d4(void);
void func_ov004_020b04d0(int v);
void func_ov004_020b682c(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

namespace GX { void LoadOBJPltt(void const *, unsigned int, unsigned int); }
namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

s32 dScMgMemory_c::InitResources()
{
    char *self = (char *)this;
    void *a;
    void *b;

    data_0209d45c = 0x11;
    func_ov006_0210a534();
    a = LoadFile(data_ov006_0213d0c4[GetGameLanguage()]);
    b = LoadFile(0xbb);
    DecompressLZ16(a, (void *)0x6400000);
    GX::LoadOBJPltt(b, 0, 0x100);
    DecompressLZ16(a, (void *)0x6600000);
    GXS::LoadOBJPltt(b, 0, 0x100);
    Deallocate(a);
    Deallocate(b);
    data_0209d454 = 0x18;
    _ZN13dScMgMemory_c9ResetGameEv(self);
    func_ov006_020c0aa8(self + 0x4660);
    if (func_ov006_020c1a88(self + 0x4f38) == 0)
        return 0;
    unk_0a8 = func_ov004_020ad8b8();
    unk_0ac = unk_0a8;
    mHudScore = func_ov004_020ad878();
    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
    unk_533b = 3;
    func_ov004_020b04d0(0x20);
    func_ov004_020b682c();
    return 1;
}
