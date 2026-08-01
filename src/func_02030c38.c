#include "types.h"
extern int func_0203d950(int i);
extern void func_02030790(void);

extern unsigned char data_0209fc5d[];
extern unsigned char data_0209fc58;

void func_02030c38(void)
{
    int i;
    unsigned char *p = data_0209fc5d;
    for (i = 1; i < 4; i++) {
        if (*p != 0 && func_0203d950(i) == 0) {
            data_0209fc58 |= 1 << i;
            func_02030790();
        }
        p++;
    }
}
