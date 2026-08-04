#include "types.h"
extern int data_0209d4b8;
extern int data_ov006_0212e4dc[];

int RandomIntInternal(int* seed);
int LoadFile(int handle);
void func_020563d4(const void *src, u32 offset, u32 count);
void Deallocate(void *ptr);

void func_ov006_020e3250(char* c) {
    int m;
    int r4;
    if (*(int*)(c + 0x4e9c) == 0xff) {
        *(int*)(c + 0x4e9c) = 0;
    } else {
        m = (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3 >> 15;
        if (*(int*)(c + 0x4e9c) == m) {
            m += ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2 >> 15) + 1;
            if (m >= 3) m -= 3;
        }
        *(int*)(c + 0x4e9c) = m;
    }
    *(int*)(c + 0x4e94) = data_ov006_0212e4dc[*(int*)(c + 0x4e9c) * 2] << 12;
    *(int*)(c + 0x4e98) = (data_ov006_0212e4dc[*(int*)(c + 0x4e9c) * 2 + 1] - 0xe0) << 12;
    if (*(int*)(c + 0x4e9c) == 0) {
        r4 = LoadFile(0x30);
    } else if (*(int*)(c + 0x4e9c) == 1) {
        r4 = LoadFile(0x2d);
    } else if (*(int*)(c + 0x4e9c) == 2) {
        r4 = LoadFile(0x31);
    }
    func_020563d4((void*)r4, 0, 0x800);
    Deallocate((void*)r4);
}
