//cpp
// @symbol _ZN21FloatingFloorLllSmall13InitResourcesEv
/* recovered: named members + real C++ method */
/* FloatingFloorLllSmall::InitResources() -- the two spawn IDs 0x4b/0x4c select
 * the variant and its float period; mVariant then indexes the same ov022
 * parameter table CleanupResources uses. */
#include "FloatingFloorLllSmall.h"

extern "C" {
void func_020393a4(int *p, int v);
int func_ov002_020b6584(void *self, void *p, int x);
extern char data_ov022_021140d4[];
}

int FloatingFloorLllSmall::InitResources()
{
    switch (*(unsigned short *)((char *)this + 0xc)) {
    case 0x4b:
        mVariant = 0;
        func_020393a4((int *)((char *)this + 0x124), 0x650000);
        break;
    case 0x4c:
        mVariant = 1;
        func_020393a4((int *)((char *)this + 0x124), 0x1000000);
        break;
    }
    unsigned char idx = mVariant;
    func_ov002_020b6584(this, data_ov022_021140d4 + idx * 0xc, 0x1051);
}
