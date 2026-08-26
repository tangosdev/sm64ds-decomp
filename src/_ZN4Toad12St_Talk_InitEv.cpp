//cpp
// @symbol _ZN4Toad12St_Talk_InitEv
#include "Toad.h"

struct BCA_File;

extern "C" {
extern int data_ov085_02130490;
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *, BCA_File *, int, int, u32);
}

void Toad::St_Talk_Init()
{
    u32 flags = 0;
    BCA_File *file = (BCA_File *)(((int *)&data_ov085_02130490)[1]);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, file, 0, 0x1000, flags);
}
