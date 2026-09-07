//cpp
// @symbol _ZN7dBgW_Kc7SetFileEP8KCL_FileR10CLPS_Block
#include "dBgW_Kc.h"
extern "C" {
void func_02039624(dBgW *self);   /* the shared collider init */
}

void dBgW_Kc::SetFile(KCL_File *file, CLPS_Block &clpsBlock)
{
    func_02039624(this);
    kclFile = file;
    clps = clpsBlock;
    unk_28 = 0;
    unk_2c = 0x1000;
    unk_30 = 0;
    unk_34 = 0;
    unk_35 = 0;
    unk_38 = 0x1000;
    unk_3c = 0;
    unk_40 = 0;
    unk_44 = -0x1000;
    unk_48 = 2;
    unk_4c = 1;
    unk_4d = 0;
}
