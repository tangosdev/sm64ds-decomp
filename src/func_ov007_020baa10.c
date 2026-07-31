#include "types.h"
// @symbol func_ov007_020baa10
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern s16 data_02082214[];




void func_ov007_020baa10(void)
{
    char* a;
    s16 state;
    int dx;
    int dy;
    int s;
    char* p;
    int counter;

    a = *(char**)data_ov007_02104ba0;
    p = *(char**)(data_ov007_02104ba0 + 0x10);
    dx = *(int*)(a + 0xc4) - *(int*)(a + 0xb8);
    dy = *(int*)(a + 0xc8) - *(int*)(a + 0xbc);
    func_ov007_020c92d0(p);
    p = *(char**)(data_ov007_02104ba0 + 0x10);
    state = *(s16*)p;
    counter = *(int*)(p + 0xc);

    switch (state) {
    case 0:
        if (counter == 0) {
            *(int*)(a + 0x8c) = 0;
            *(int*)(a + 0x88) = *(int*)(a + 0x8c);
        }
        return;
    case 1:
    case 2: {
        int v;
        int m = _ZN4cstd3modEii(counter, 0x3c);
        if (m <= 0) v = 0;
        else if (m >= 0x3c) v = 0x1000;
        else v = _ZN4cstd3divEii(m << 12, 0x3c);
        s = data_02082214[((u16)((v * 0xffff) >> 12) >> 4) * 2];
        if (state == 2) {
            *(int*)(a + 0x88) = -dx / 48 + func_ov007_020c3ba8(dx / 48 - -dx / 48);
            *(int*)(a + 0x8c) = (int)(((s64)s * (dy / 2 - dy / 8) + 0x800) >> 12);
        } else if (state == 1) {
            *(int*)(a + 0x88) = (int)(((s64)s * (dx / 3) + 0x800) >> 12);
            *(int*)(a + 0x8c) = -dy / 32 + func_ov007_020c3ba8(dy / 32 - -dy / 32);
        }
        return;
    }
    case 3:
    case 4: {
        int v;
        int idx;
        int m = _ZN4cstd3modEii(counter, 0x3c);
        if (m <= 0) v = 0;
        else if (m >= 0x3c) v = 0x1000;
        else v = _ZN4cstd3divEii(m << 12, 0x3c);
        if (*(s16*)*(char**)(data_ov007_02104ba0 + 0x10) == 4)
            v = 0x1000 - v;
        idx = ((u16)((v * 0xffff) >> 12) >> 4) * 2;
        *(int*)(a + 0x88) = (int)(((s64)(dx / 4) * data_02082214[idx + 1] + 0x800) >> 12);
        *(int*)(a + 0x8c) = (int)(((s64)(dy / 4) * data_02082214[idx] + 0x800) >> 12);
        return;
    }
    case 5:
        if (counter == 0) {
            *(struct Vector3*)(a + 0x70) = *(struct Vector3*)(a + 0x7c);
            *(int*)(a + 0x8c) = 0;
            *(int*)(a + 0x88) = *(int*)(a + 0x8c);
        }
        return;
    case 6:
        if (counter == 0) {
            int t;
            t = -dx / 2 + func_ov007_020c3ba8(dx / 2 - -dx / 2);
            *(int*)(a + 0x70) = (int)(((s64)t * 0x5dcLL + 0x800) >> 12);
            t = -dy / 2 + func_ov007_020c3ba8(dy / 2 - -dy / 2);
            *(int*)(a + 0x74) = (int)(((s64)t * 0x5dcLL + 0x800) >> 12);
            *(int*)(a + 0x8c) = 0;
            *(int*)(a + 0x88) = *(int*)(a + 0x8c);
        }
        return;
    }
}
