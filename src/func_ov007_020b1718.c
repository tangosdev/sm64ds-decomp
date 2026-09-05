#include "types.h"
// @symbol func_ov007_020b1718
/* recovered: per-frame update of a two-mode screen transition driven by the
 * manager at data_ov007_0210342c. Mode 1 eases a scale toward 0x1000 over
 * 12 frames and flags completion when the slot reaches its end; mode 0 copies
 * the start positions on frame 0, then lerps three Fix12 fields of the child
 * object along a smoothstep-squared curve and flags completion at frame 0x28.
 * Both modes leave a (remaining, total) pair for the shared tail, which
 * advances the counter and fades the target through func_ov007_020b2764.
 *
 * Codegen note: `r7val = v - 2; divisor = 0x28;` must be assigned BEFORE the
 * Fix12 store block (the compiler sinks both defs to the block exit, ahead of
 * the predicated flag write); assigning them after the block instead lands the
 * movge first and rotates the block's coloring.
 */
extern int _ZN4cstd3divEii(int a, int b);
extern void func_ov007_020bdeb0(int a);
extern void func_ov007_020b257c(void);
extern void func_ov007_020b2454(void* a, void* b);
extern void func_ov007_020bce70(int a, int b);
extern int func_ov007_020c3be0(int a, int b, int c);
extern void func_ov007_020b2764(int a);

extern char* data_ov007_0210342c;

struct S12 { int a, b, c; };

void func_ov007_020b1718(void)
{
    char* F = *(char**)(data_ov007_0210342c + 0xf4);
    int r7val;
    int v;
    int flag = 0;
    int divisor;
    int mode;

    v = *(int*)(*(char**)(data_ov007_0210342c + 8) + 0xc);

    if (v == 0)
        *(s16*)(*(char**)(data_ov007_0210342c + 0xc) + 2) = 2;

    if (v == 0 && *(int*)(data_ov007_0210342c + 0xec) == 0)
        func_ov007_020bdeb0(0x1c);
    else if (v == 0 && *(int*)(data_ov007_0210342c + 0xec) == 1)
        func_ov007_020bdeb0(0x1d);

    mode = *(int*)(data_ov007_0210342c + 0x20);
    switch (mode) {
    case 1: {
        char* F18 = *(char**)(F + 0x18);
        int q, r3, s;
        char* F18b;

        divisor = 0x1e;
        r7val = (int)*(u16*)(F18 + 0x10) - ((int)*(u16*)(F18 + 0x12) - 0x12);

        if (v < 0xc) {
            if (v <= 0) q = 0;
            else if (v >= 0xc) q = 0x1000;
            else q = _ZN4cstd3divEii(v << 0xc, 0xc);
            r3 = q << 2;

            if (r3 <= 0x1000)
                s = (int)(((s64)r3 * 0x1000 + 0x800) >> 12);
            else if (r3 <= 0x3000)
                s = (int)(((s64)(r3 - 0x1000) * (-0x400) + 0x800) >> 12) + 0x1000;
            else
                s = (int)((((s64)1 - (r3 - 0x3000)) * 0x800) >> 12) + 0x800;

            *(int*)(data_ov007_0210342c + 0xf8) = 0x1000 - (s >> 2);
        } else {
            *(int*)(data_ov007_0210342c + 0xf8) = 0x1000;
        }

        F18b = *(char**)(F + 0x18);
        if (*(s16*)(F18b + 0xc) != -1) {
            if ((int)*(u16*)(F18b + 0x10) == (int)*(u16*)(F18b + 0x12) - 2) {
                if (*(int*)F18b == 0)
                    *(int*)F18b = 1;
                flag = 1;
            }
        }
        break;
    }
    case 0: {
        char* m = *(char**)(data_ov007_0210342c + 0x30);
        int q, t, w, r3, u;

        if (v == 0) {
            char* d;
            func_ov007_020b257c();
            d = *(char**)(m + 0x3c);
            *(struct S12*)(d + 0x20) = *(struct S12*)(m + 0x14);
            d = *(char**)(m + 0x3c);
            *(struct S12*)(d + 0x14) = *(struct S12*)(d + 0x20);
            *(int*)(*(char**)(m + 0x40) + 0x18) = *(int*)(m + 0x2c);
            d = *(char**)(m + 0x40);
            *(int*)(d + 0x14) = *(int*)(d + 0x18);
            func_ov007_020b2454(*(char**)(m + 0x3c) + 0x20, *(char**)(m + 0x40) + 0x18);
        }

        if (v <= 0) q = 0;
        else if (v >= 0x1e) q = 0x1000;
        else q = _ZN4cstd3divEii(v << 0xc, 0x1e);

        t = (int)(((s64)q * (0x2000 - q)) >> 12);
        u = (int)(((s64)t * (0x2000 - t)) >> 12);
        w = (int)(((s64)u * u) >> 12);
        if (w >= 0x800)
            r3 = 0x1000 - ((w - 0x800) << 1);
        else
            r3 = w << 1;

        r7val = v - 2;
        divisor = 0x28;
        {
            *(int*)(m + 0x14) = (int)(((s64)(0x1000 - u) * *(int*)(*(char**)(m + 0x3c) + 0x14) + (s64)(int)(unsigned int)u * *(int*)(*(char**)(m + 0x3c) + 0x20)) >> 12);
            {
                char* d2 = *(char**)(m + 0x3c);
                r3 = u - (int)(((s64)r3 * (0x2000 - r3)) >> 12);
                *(int*)(m + 0x18) = (int)(((s64)(0x1000 - r3) * *(int*)(d2 + 0x18) + (s64)r3 * *(int*)(d2 + 0x24)) >> 12);
            }
            q = (int)(((s64)u * (0x2000 - u)) >> 12);
            *(int*)(m + 0x2c) = (int)(((s64)(0x1000 - q) * *(int*)(*(char**)(m + 0x40) + 0x14) + (s64)q * *(int*)(*(char**)(m + 0x40) + 0x18)) >> 12);
        }
        if (v >= 0x28) flag = 1;
        break;
    }
    }

    if (flag != 0) {
        *(int*)(data_ov007_0210342c + 0x1c) = 2;
        *(s16*)(*(char**)(data_ov007_0210342c + 4) + 2) = 1;
    }

    if (v >= 2 && r7val >= 0) {
        int w;
        int qq;
        func_ov007_020bce70(2, 0xf);
        w = *(int*)(*(char**)(data_ov007_0210342c + 0x40) + 0xc);
        if (w <= 0) qq = 0;
        else if (w >= 0xf) qq = 0x1000;
        else qq = _ZN4cstd3divEii(w << 0xc, 0xf);
        *(u16*)(*(char**)(data_ov007_0210342c + 0x3c) + 0x22) =
            func_ov007_020c3be0(qq, *(u16*)(*(char**)(data_ov007_0210342c + 0x3c) + 0x22), 0);
    }

    {
        int qq2;
        if (v <= 0) qq2 = 0;
        else if (v >= divisor) qq2 = 0x1000;
        else qq2 = _ZN4cstd3divEii(v << 0xc, divisor);
        func_ov007_020b2764(0x1000 - qq2);
    }
}
