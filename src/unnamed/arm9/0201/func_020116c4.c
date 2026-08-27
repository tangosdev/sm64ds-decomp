#include "types.h"
void func_02011ac4(void* slot, int counter, int a1, int a2, int a3, int s0, short s1);
void func_02011698(char* table, char* n);

int func_020116c4(char* c, int a1, int a2, int a3, int s0, short s1)
{
    int i;
    for (i = 0; i < 0x40; i++) {
        if (*(int*)(c + (u8)c[4] * 0x14 + 8) == 0) {
            *(int*)c = *(int*)c + 1;
            if (*(int*)c == 0) *(int*)c = *(int*)c + 1;
            func_02011ac4(c + 8 + (u8)c[4] * 0x14, *(int*)c, a1, a2, a3, s0, s1);
            func_02011698(c, c + 8 + (u8)c[4] * 0x14);
            return *(int*)c;
        }
        c[4] = (u8)(((u8)c[4] + 1) % 0x40);
    }
    return 0;
}
