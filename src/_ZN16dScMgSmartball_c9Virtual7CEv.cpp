//cpp
// @symbol _ZN16dScMgSmartball_c9Virtual7CEv
// recovered name: dScMgSmartball_c_Kill  -- WRONG, see below
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgSmartball_c.h"
/* recovered: renamed to Class_Method */
/* dScMgSmartball_c::Virtual7C - slot 31, this class's own override.

   Not a Kill: see the slot-31 block in include/dScMgBase_c.h.  Byte-for-byte
   dScMgAmida_c's shape -- BG1CNT base bits 4 -- differing only in the language
   table it indexes. */
extern "C" {
extern int GetGameLanguage(void);
extern unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern unsigned char data_0209d454[];
}

int dScMgSmartball_c::Virtual7C()
{
    int f;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 4;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~0x40;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    SetSubBg1Offset(0, 0);
    data_0209d454[0] &= ~2;
    f = GetGameLanguage();
    LoadCompressedFileAt(data_ov006_0213eccc[f], (void *)_ZN3G2S13GetBG1CharPtrEv());
    LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
}
