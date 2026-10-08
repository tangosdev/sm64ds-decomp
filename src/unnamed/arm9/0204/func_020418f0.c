#include "types.h"
extern u32 data_020a1fc0[];
extern void func_02041e14(void);
extern void func_02041930(void);

void func_020418f0(void) {
    u32* g = data_020a1fc0;
    if (g[3] == 0) return;
    g[1] = 1;
    func_02041e14();
    func_02041930();
}
