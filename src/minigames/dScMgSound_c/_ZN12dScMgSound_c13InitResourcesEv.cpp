//cpp
// @symbol _ZN12dScMgSound_c13InitResourcesEv
#include "types.h"
#include "decl_common.h"
#include "dScMgSound_c.h"
/* dScMgSound_c::InitResources -- vtable slot 0, ov006 0x0211c984.
 *
 * Attributed by the vtable: dScMgSound_c's own table is ov006 0x0213f844 and
 * its slot 0 relocates here. The signature is include/fBase_c.h's own slot
 * 0, `virtual s32 InitResources()`.
 *
 * THE SPLIT LITERALS ARE ORIGINAL AND ARE LEFT ALONE. `r7 + 0x5000 + 0xe0` is
 * not the same instruction as `r7 + 0x50e0`: ARM's immediate encoding makes
 * mwccarm build the base once and address off it, which is what the cartridge
 * does. Folding them would be a readability change that has to be re-measured,
 * and they are also the form include/dScMgSound_c.h's banner means when it
 * says these offsets are matched access INSIDE mTable rather than fields of
 * this class -- 0x50e0, 0x5608, 0x5618 and 0x5626 all fall in the 0x6f4 the
 * ctor/dtor pair func_ov006_020c33dc/020c3288 owns.
 *
 * mHudScore is dScMgBase_c's, and reads as an inherited member; the
 * pre-migration file wrote it as `*(int *)(r7 + 0xb4)`.
 *
 * The GX/G2S/GXS entry points keep their mangled spellings INSIDE extern "C",
 * which is the tree's idiom for them (src/_ZN12dScMgLuigi_c13InitResourcesEv.cpp
 * does the same). Inside extern "C" the identifier is emitted verbatim; only a
 * bare namespace-scope `extern` of a mangled name would mangle a SECOND time,
 * which is the defect include/SharedFilePtr.h's banner records. */

extern "C" {
int LoadFile(int handle);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
unsigned _ZN3G2S13GetBG2CharPtrEv(void);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void DecompressLZ16(int src, void *dst);
void func_ov006_020c225c(void *p);
int func_ov006_020c3050(void *p);
void func_ov006_0211c478(void *p);
void func_ov006_0211c080(void *p);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

s32 dScMgSound_c::InitResources()
{
    u8 *r7 = (u8 *)this;
    int r6, r5, r4;

    data_0209d45c |= 0x11;
    r6 = LoadFile(0xff);
    r5 = LoadFile(0x100);
    DecompressLZ16(r6, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)r5, 0, 0x100);

    data_0209d454 |= 4;
    r4 = LoadFile(0x98);
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x214;
    DecompressLZ16(r4, (void *)_ZN3G2S13GetBG2CharPtrEv());
    Deallocate((void *)r4);

    r4 = LoadFile(0x99);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)r4, 0x60, 0x1a0);
    Deallocate((void *)r4);

    r4 = LoadFile(0x9a);
    func_02056374((const void *)r4, 0, 0x800);
    Deallocate((void *)r4);

    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 1;
    DecompressLZ16(r6, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj((const void *)r5, 0, 0x100);
    Deallocate((void *)r6);
    Deallocate((void *)r5);

    func_ov006_020c225c((void *)(r7 + 0x4660));
    if (func_ov006_020c3050((void *)mTable) == 0)
        return 0;

    *(int *)(r7 + 0x5000 + 0xe0) = 1;
    func_ov006_0211c478(r7);
    func_ov006_0211c080(r7);
    *(u8 *)(r7 + 0x5000 + 0x626) = 3;
    *(int *)(r7 + 0x5000 + 0x608) = 1;
    *(u16 *)(r7 + 0x5600 + 0x18) = 0x20;
    func_ov004_020b6808();
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    mHudScore = 0;
    return 1;
}
