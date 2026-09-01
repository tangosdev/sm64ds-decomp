//cpp
// @symbol _ZN11dScMgBase_c9Virtual7CEv
// recovered name: dScMgBase_c_Kill  -- WRONG, see below
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgBase_c.h"
/* recovered: renamed to Class_Method */
/* dScMgBase_c::Virtual7C - slot 31.

   The `recovered name:` line above is kept visible because it is wrong.  The
   ROM names nothing here: dScMgBase_c is a SCENE (fBase_c -> dBase_c ->
   dScene_c -> dScMgBase_c), not an actor, and dActor_c -- whose names slots
   18-30 borrowed by index -- has no slot 31 at all.  `Kill` was carried across
   from dBgActor_c, dActor_c's own child, where _ZN10dBgActor_c4KillEv
   (ov002:0x020ee55c) is that class's new slot 31.  Different branch, same
   index, no relationship.  See the slot-31 block in include/dScMgBase_c.h.

   It also does not kill anything.  Three read-modify-writes leave the sub
   engine's BG1CNT holding exactly 0x10 -- priority 0, no mosaic -- then the
   layer's scroll is reset, BG1's bit is cleared from the sub BG-enable shadow
   that slot 30 restores the sub DISPCNT from, and a language-indexed character
   file plus the shared screen map (file 0x5b) are installed.  It builds this
   minigame's touch-screen background, from BeforeInitResources. */
extern "C" {
extern int GetGameLanguage(void);
extern unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern unsigned char data_0209d454[];
}

int dScMgBase_c::Virtual7C()
{
    int f;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x10;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~0x40;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    SetSubBg1Offset(0, 0);
    data_0209d454[0] &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(data_ov004_020bc00c[f], (void *)_ZN3G2S13GetBG1CharPtrEv());
    LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
}
