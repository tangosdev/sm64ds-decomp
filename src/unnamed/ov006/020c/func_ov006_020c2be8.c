#include "types.h"
extern int func_ov006_020c2994(char* c);
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];

#define COMPUTE(c, s, t) \
    { \
        int q = ((s) << 12) / 255; \
        int r = ((t) << 12) / 192; \
        int w = 0x1000 - r; \
        int a; \
        int b; \
        if (q > 0x800) { \
            int d = (q - 0x800) << 1; \
            a = (int)(((s64)d * -0x100 + 0x800) >> 12); \
            b = (int)(((s64)d * -0x260 + 0x800) >> 12); \
        } else { \
            int d = (0x800 - q) << 1; \
            a = (int)(((s64)d * 0x260 + 0x800) >> 12); \
            b = (int)(((s64)d * 0x100 + 0x800) >> 12); \
        } \
        { \
            int m = (int)(((s64)r * 0x20 + 0x800) >> 12) + (int)(((s64)w * 0x500 + 0x800) >> 12); \
            int o1 = *(int*)((c) + 0x18c); \
            int o2 = *(int*)((c) + 0x194); \
            b -= o2; \
            int o3 = *(int*)((c) + 0x190); \
            int o4 = *(int*)((c) + 0x198); \
            *(int*)((c) + 0x18c) += (a - o1) >> 2; \
            *(int*)((c) + 0x194) += b >> 2; \
            o3 = m - o3; \
            *(int*)((c) + 0x190) += o3 >> 2; \
            o4 = m - o4; \
            *(int*)((c) + 0x198) += o4 >> 2; \
        } \
    }

void func_ov006_020c2be8(char* c)
{
    if (*(int*)(c + 0x1a4) != 0) {
        int s = *(int*)(c + 0x19c);
        int t = *(int*)(c + 0x1a0);
        COMPUTE(c, s, t);
        return;
    }
    if (func_ov006_020c2994(c) != 0) {
        int i = data_020a0e40;
        if (data_020a0de8[i * 4] != 0) {
            int s = data_020a0dea[i * 4];
            int t = data_020a0deb[i * 4];
            COMPUTE(c, s, t);
            return;
        }
    }
    *(int*)(c + 0x18c) = (int)(((s64)*(int*)(c + 0x18c) * 0xE00 + 0x800) >> 12);
    *(int*)(c + 0x190) = (int)(((s64)*(int*)(c + 0x190) * 0xE00 + 0x800) >> 12);
    *(int*)(c + 0x194) = (int)(((s64)*(int*)(c + 0x194) * 0xE00 + 0x800) >> 12);
    *(int*)(c + 0x198) = (int)(((s64)*(int*)(c + 0x198) * 0xE00 + 0x800) >> 12);
}
