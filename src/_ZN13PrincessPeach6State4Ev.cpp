//cpp
#include "PrincessPeach.h"

extern "C" {
extern void* data_ov085_021304bc[];
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* self, void* bca, int frame, int speed, unsigned int flags);
}
int PrincessPeach::State4()
{
    char *c = (char *)this;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void*)data_ov085_021304bc[1], 0, 0x1000, 0);
    *(int*)(c + 0x354) = 0;
    return 1;
}
