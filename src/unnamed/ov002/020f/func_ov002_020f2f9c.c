#include "types.h"
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32 c);
extern int func_ov002_020f5a94(void);

extern int* data_ov002_0210b97c[];

void func_ov002_020f2f9c(void* self, int count)
{
    char* sl = (char*)self;
    int i;
    int idx = func_ov002_020f5a94() - 2;
    if (idx < 0) idx = 0;
    for (i = 0; i < count + 1; i++) {
        int ok = 0;
        if (i == 0 || i == 1 || i == 4) {
            ok++;
        } else if (i == 2) {
            if (_ZN8SaveData19IsCharacterUnlockedEj(2)) ok++;
        } else {
            if (_ZN8SaveData19IsCharacterUnlockedEj(1)) ok++;
        }
        if (ok) {
            int* p;
            *(u8*)(sl + 0x170) = 1;
            *(u8*)(sl + 0x171) = 1;
            p = data_ov002_0210b97c[idx];
            *(int*)(sl + 0x160) = (p[i * 4 + 0] + 0xe0) << 12;
            *(int*)(sl + 0x164) = (p[i * 4 + 1] + 0x66) << 12;
            *(u8*)(sl + 0x173) = (u8)p[i * 4 + 2];
            *(u8*)(sl + 0x172) = (u8)p[i * 4 + 3];
        }
        sl += 0x14;
    }
}
