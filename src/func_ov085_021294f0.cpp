//cpp
// @symbol _ZN4Toad12St_Idle_InitEv
#include "Toad.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov085_02130488;
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    void *, BCA_File *, int, int, u32);
}

void Toad::St_Idle_Init()
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, ((BCA_File **)&data_ov085_02130488)[1], 0, 0x1000, 0);
}
