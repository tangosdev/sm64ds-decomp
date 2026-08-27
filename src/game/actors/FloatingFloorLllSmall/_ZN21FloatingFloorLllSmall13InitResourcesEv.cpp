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

/* NO VALUE IS RETURNED, AND THAT IS FAITHFUL. The header declares `int`; this
 * body sets no return value and the ROM does not either. Adding an explicit
 * `return <v>;` emits a real instruction and breaks the match (measured on
 * PushBlock::OnPushed, 2026-08-22). The legacy .c declared this `void` and never
 * included the header, so nothing checked the disagreement until it became a
 * real method. Harmless for the ROM build; a host-port caller that reads this
 * result gets garbage. Fix upstream in the header's return type if it is wrong --
 * never with a `return` here. */
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
