//cpp
// @symbol func_ov063_02116e14
/* recovered: shared common types */
#include "common.h"
typedef short s16;

extern "C" {
void func_ov063_02119e38(char* c, int a, int b, int d);
int func_ov063_0211a0dc(char* c);
int func_ov063_0211adb4(char* c);
void func_0201267c(int a, void* p);
int _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, struct Vector3*, void*, int, int);

void func_ov063_02116e14(char* c){
    *(unsigned short*)((int)c + 0x5d4) &= ~0x40;
    if (*(unsigned short*)(c + 0x500 + 0xc0) == 0) {
        func_ov063_02119e38(c, 0x64, 0x200, 0x800);
    }
    {
        int r5 = func_ov063_0211a0dc(c);
        if (func_ov063_0211adb4(c) != 0) {
            *(unsigned char*)(c + 0x5cc) = 0;
        }
        if (r5 == -1) {
            *(unsigned char*)(c + 0x5cc) = 2;
            return;
        }
        if (r5 != 1) return;
    }
    if ((unsigned int)((unsigned short)*(unsigned short*)(c + 0x500 + 0xd4) << 0x19) >> 0x1f) {
        *(unsigned char*)(c + 0x5cc) = 4;
        func_0201267c(0x14a, c + 0x74);
        return;
    }
    *(unsigned char*)(c + 0x5cc) = 3;
    {
        char* r = (char*)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
            *(unsigned short*)(c + 0x4a0), 0, (struct Vector3*)(c + 0x504), 0,
            *(signed char*)(c + 0x5d0), -1);
        if (r != 0) {
            *(int*)(r + 0xa4) = 0;
            *(int*)(r + 0xa8) = 0x32000;
            *(int*)(r + 0xac) = 0;
        }
    }
    *(unsigned short*)(c + 0x5d4) &= ~2;
}
}
