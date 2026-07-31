// @symbol func_ov064_02119010
/* recovered: shared common types */
#include "common.h"
extern void Matrix4x3_FromTranslation(struct Matrix4x3* m, int x, int y, int z);
void func_ov064_02119010(char* c) {
    int x = *(int*)((char*)c + 0x5c) >> 3;
    int y = (*(int*)((char*)c + 0x60) + *(int*)((char*)c + 0x330)) >> 3;
    int z = *(int*)((char*)c + 0x64) >> 3;
    Matrix4x3_FromTranslation((struct Matrix4x3*)((char*)c + 0xf0), x, y, z);
}
