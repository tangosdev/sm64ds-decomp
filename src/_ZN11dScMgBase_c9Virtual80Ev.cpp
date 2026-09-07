//cpp
// @symbol _ZN11dScMgBase_c9Virtual80Ev
// recovered name: dScMgBase_c_AfterClsn  -- WRONG, see below
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgBase_c.h"
/* recovered: renamed to Class_Method */
/* dScMgBase_c::Virtual80 - slot 32.

   The `recovered name:` line above is kept visible because it is wrong.  This
   time the borrowed name is a REAL ROM name -- _ZN16dPathLiftActor_c9AfterClsnEi,
   declared at include/PathLift.h:58 -- which makes it the more misleading of the
   two.  dPathLiftActor_c derives from dBgActor_c, which derives from dActor_c;
   dScMgBase_c is a dScene_c.  The chains share only dBase_c, which adds no
   virtual, so the two slot 32s have fBase_c's first eighteen entries in common
   and nothing else.  That AfterClsn also takes an int.  See the slot-32 block in
   include/dScMgBase_c.h.

   It touches no collision.  It is slot 31 with the other display engine: three
   read-modify-writes leave the MAIN BG1CNT at 0x0400000a holding exactly 0x1000,
   the layer's scroll is reset, BG1's bit is cleared from data_0209d45c -- the
   MAIN BG-enable shadow slot 30 restores the main DISPCNT from, where 31 cleared
   the sub's data_0209d454 -- and a language-indexed character file plus the
   shared screen map (file 0x67 here, 0x5b there) are installed.  It builds this
   minigame's TOP-screen background, from AfterInitResources. */
extern "C" {
extern int GetGameLanguage(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern unsigned char data_0209d45c[];
}

void dScMgBase_c::Virtual80()
{
    int f;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~3;
    *(volatile unsigned short *)0x400000a = (*(volatile unsigned short *)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~0x40;
    SetBg1Offset(0, 0);
    data_0209d45c[0] &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(data_ov004_020bbff8[f], (void *)func_02054ea8());
    LoadCompressedFileAt(0x67, _ZN2G212GetBG1ScrPtrEv());
}
