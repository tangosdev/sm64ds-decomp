#include "types.h"
extern int *data_020a9db8;
int func_02065edc(u32 x);
int func_02066fb0(int a, int b, int c);

int func_02066fec(u32 a, int b, int c) {
    if (func_02065edc(a)) {
        *(int *)((char *)data_020a9db8 + (a - 1) * 4 + 0x14e8) = b;
    }
    return func_02066fb0(a, b, c);
}
