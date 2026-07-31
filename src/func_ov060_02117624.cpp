//cpp
// @symbol func_ov060_02117624
/* recovered: shared common types */
#include "common.h"
extern "C" {

void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *thisp, void *sm, void *mtx, int rad, int t, unsigned int j);
extern Matrix4x3 data_020a0e68;
void func_ov060_02117624(char *c) {
    if (*(unsigned char*)(c+0x379) == 0) return;
    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c+0x5c)>>3, *(int*)(c+0x364)>>3, *(int*)(c+0x64)>>3);
    *(Matrix4x3*)(c+0x32c) = data_020a0e68;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(c, c+0x304, c+0x32c, *(int*)(c+0x368) * *(int*)(c+0x360), 0x1e000, 0xf);
}
}
