// @symbol func_ov006_02118ae4
// @emits dScMgSmartball_c_Kill
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgSmartball_c::Kill - recovered from vtable slot identity */
extern int func_ov004_020ad674(void);
extern unsigned int func_02054e88(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern unsigned char data_0209d454[];

void dScMgSmartball_c_Kill(void)
{
    int f;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 4;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~0x40;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    SetSubBg1Offset(0, 0);
    data_0209d454[0] &= ~2;
    f = func_ov004_020ad674();
    LoadCompressedFileAt(data_ov006_0213eccc[f], (void *)func_02054e88());
    LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
}
