#include "types.h"
extern char* data_ov007_0210342c;

int func_ov007_020be850(void) {
    char* g = data_ov007_0210342c;
    char* r2;
    int res = 0;
    if (*(int*)(g + 0xe8) == 0) return res;
    r2 = *(char**)(*(char**)(g + 0xf4) + 0x18);
    if (*(s16*)(r2 + 0xc) == 0) {
        if (*(u16*)(r2 + 0x10) >= 0x7c) {
            if (*(int*)(g + 0xec) == 1) return *(int*)(g + 0x104);
        }
        return *(int*)(g + 0x100);
    }
    if (*(s16*)(*(char**)(g + 8)) == 0xb) res = *(int*)(g + 0x100);
    return res;
}
