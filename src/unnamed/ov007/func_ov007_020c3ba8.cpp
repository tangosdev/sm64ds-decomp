//cpp
#include "cstd.h"

extern "C" {
extern int func_02070348(int);

int func_ov007_020c3ba8(int r4) {
    int v = func_02070348(r4);
    return cstd::div(r4 * v, 0x8000);
}
}
