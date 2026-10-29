//cpp
#include "CP15.h"

extern "C" {

extern int func_02058764(void);
extern unsigned int func_02058df4(int a);
extern void func_02058d6c(int idx, unsigned int val);
extern unsigned int OS_GetInitArenaLo(int a);
extern void func_02058d58(int idx, unsigned int val);

void func_02058ec8(void)
{
    if ((func_02058764() & 3) != 2)
        return;
    func_02058d6c(2, func_02058df4(2));
    func_02058d58(2, OS_GetInitArenaLo(2));
    CP15::MPUDataRegion1(0x200002d);
}
}
