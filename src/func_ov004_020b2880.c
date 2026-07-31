// @symbol func_ov004_020b2880
// @emits dScMgBase_c_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgBase_c::Kill - recovered from vtable slot identity */
extern int GetGameLanguage(void);
extern unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern unsigned char data_0209d454[];

void dScMgBase_c_Kill(void)
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
