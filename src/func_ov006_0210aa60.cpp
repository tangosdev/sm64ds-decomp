//cpp
// @symbol func_ov006_0210aa60
// @emits dScMgSlot3_c_AfterClsn
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScMgSlot3_c::AfterClsn - recovered from vtable slot identity */
struct G2 { static void* GetBG1ScrPtr(); };

extern "C" {
    void SetBg1Offset(int a, int b);
    int func_ov004_020ad674(void);
    void* func_02054ea8(void);
    unsigned int LoadCompressedFileAt(int fileID, void* target);
}

extern unsigned char data_0209d45c;

extern "C" void dScMgSlot3_c_AfterClsn(void)
{
    int idx;

    *(volatile unsigned short*)0x400000a &= ~3;
    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short*)0x400000a &= ~0x40;

    SetBg1Offset(0, 0);

    data_0209d45c &= ~2;

    idx = func_ov004_020ad674();
    LoadCompressedFileAt(data_ov006_0213e614[idx], func_02054ea8());
    LoadCompressedFileAt(0x67, G2::GetBG1ScrPtr());

    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1118;
}
