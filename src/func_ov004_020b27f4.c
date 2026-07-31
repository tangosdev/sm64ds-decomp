// @symbol func_ov004_020b27f4
// @emits dScMgBase_c_AfterClsn
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgBase_c::AfterClsn - recovered from vtable slot identity */
extern int func_ov004_020ad674(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern unsigned char data_0209d45c[];

void dScMgBase_c_AfterClsn(void)
{
    int f;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~3;
    *(volatile unsigned short *)0x400000a = (*(volatile unsigned short *)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~0x40;
    SetBg1Offset(0, 0);
    data_0209d45c[0] &= ~2;
    f = func_ov004_020ad674();
    LoadCompressedFileAt(data_ov004_020bbff8[f], (void *)func_02054ea8());
    LoadCompressedFileAt(0x67, _ZN2G212GetBG1ScrPtrEv());
}
