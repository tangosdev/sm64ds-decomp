//cpp
#include "types.h"
#include "decl_common.h"
#include "dScMgBase_c.h"
// @symbol _ZN11dScMgBase_c16OnAimedAtWithEggEv
// recovered name: dScMgBase_c_OnAimedAtWithEgg
/* recovered: renamed to Class_Method, RTTI class fields named, declarations
   from a shared header */
/* dScMgBase_c::OnAimedAtWithEgg - recovered from vtable slot identity.

   THE 27-METHOD SHADOW SCAFFOLD IS GONE.  This file used to carry a local
   `struct Base` of twenty-six placeholder virtuals plus `struct Obj : Base`
   with a `char pad[0x4700]`, existing only so that `self->Init()` would compile
   to a load of vtable+0x68.  Slot 26 is OnHitByCannonBlastedChar and
   dScMgBase_c declares it now, so the class does that job and both calls can be
   written for what they are.  Every field this body touches was already named
   in the header before this change -- mSavedMainBgBits's comment there names
   this function -- so those offsets go too.  The two buffers at +0x2228 and
   +0x4228 stay as raw offsets: they are inside pad_228, and this body is the
   only evidence for their extent.

   WHAT IT DOES, worth writing down because the NAME DOES NOT SAY IT.
   `OnAimedAtWithEgg` is transplanted from include/dActor_c.h at the same slot
   index; nothing in the cartridge carries a method name for this class.  The
   body saves the screen and then draws over it.  It stashes POWCNT1's
   screen-swap bit and the two BG-enable bytes into the object, clears every
   layer- and window-enable bit in both DISPCNTs, saves 0x400 bytes of BG
   palette out of palette RAM into +0x4228 and loads the menu's palettes over
   the top, then saves 0x2000 bytes of sub-screen OBJ VRAM at 0x06606000 into
   +0x2228 and decompresses a per-language image over that.  Its one caller in
   this module is dScMgBase_c::OnKicked, which reaches it only on a rising edge
   of mMenuOpen.  That is the three-item overlay menu going up, not a Yoshi egg.
   See the slot 29 block in include/dScMgBase_c.h for why the name is kept. */
extern "C" void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *addr, unsigned int size);
extern "C" void _ZN2GX10LoadBGPlttEPKvjj(const void *src, unsigned int offset, unsigned int size);
extern "C" void _ZN3GXS10LoadBGPlttEPKvjj(const void *src, unsigned int offset, unsigned int size);
extern "C" void MultiStore16(unsigned short val, char *dst, int nbytes);
extern "C" void func_0201f32c(int arg0);
extern "C" int GetGameLanguage(void);
extern "C" void DecompressLZ16(void *src, void *dst);

extern "C" unsigned char data_0209d45c;
extern "C" unsigned char data_0209d454;

/* The parameter is spelled void* to agree with decl_common.h, which the two
   overrides reach this symbol through; `this` is recovered on the first line. */
int dScMgBase_c::OnAimedAtWithEgg()
{
    void *cv = (void *)this;

    struct dScMgBase_c *self = (struct dScMgBase_c *)cv;
    char *c = (char *)cv;

    if (self->OnHitByCannonBlastedChar() != 0)
        func_02019028();

    self->mSavedScreenSwap = (*(volatile u16 *)0x4000304 & 0x8000) >> 15;

    self->mSavedMainBgBits = data_0209d45c;
    self->mSavedSubBgBits = data_0209d454;

    *(volatile u16 *)0x4000304 |= 0x8000;
    data_0209d45c = 0;
    data_0209d454 = 0;

    *(volatile int *)0x4000000 &= ~0x1f00;
    *(volatile int *)0x4001000 &= ~0x1f00;
    *(volatile int *)0x4000000 &= ~0xe000;
    *(volatile int *)0x4001000 &= ~0xe000;

    MultiCopy_Int((int *)0x5000020, (int *)(c + 0x4228), 0x1e0);
    {
        int *ip4228 = (int *)(c + 0x4228);
        MultiCopy_Int((int *)0x5000400, ip4228 + 0x80, 0x80);
    }
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((void *)(c + 0x4228), 0x400);

    _ZN2GX10LoadBGPlttEPKvjj((const void *)data_ov004_020bea28, 0x20, 0xa0);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)data_ov004_020beac8, 0, 0x80);

    {
        volatile unsigned short tmp = 0;
        MultiStore16(tmp, (char *)0x5000000, 2);
    }
    {
        volatile unsigned short tmp = 0;
        MultiStore16(tmp, (char *)0x5000400, 2);
    }

    func_0201f32c(self->mSceneKind);

    data_0209d45c = 0x12;
    data_0209d454 = 0x10;

    if (self->OnHitByCannonBlastedChar() == 2)
        return;

    int *vbase = (int *)0x6600000;
    int *vram = vbase + 0x1800;
    MultiCopy_Int(vram, (int *)(c + 0x2228), 0x2000);
    int idx = GetGameLanguage();
    DecompressLZ16(data_ov004_020bbf94[idx], vram);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((void *)(c + 0x2228), 0x2000);
}
