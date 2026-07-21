typedef short s16;
typedef unsigned short u16;
typedef long long s64;

extern int _ZN4cstd4fdivEii(int a, int b);
extern int func_02053200(int x);
struct M { int _00, _01, _10, _11; };
struct B { unsigned int pad : 16; unsigned int f : 9; unsigned int hi : 7; };
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5);

#pragma opt_propagation off
void func_ov004_020b4360(char* self) {
    u16 flag;
    int val = *(int*)(self + 0x24);
    if (val == 0) return;
    {
        char* el = self + 0x34;
        if (val < 0x1e000) {
            int d;
            int scale;
            if (val < 0x14000) {
                int t;
                d = (int)(((s64)val * 0x100 + 0x800) >> 12);
                t = _ZN4cstd4fdivEii(d, 0x1400);
                scale = (int)(((s64)t * 0x1400 + 0x800) >> 12);
            } else {
                int a = (int)(((s64)(val - 0x14000) * 0x66 + 0x800) >> 12);
                int b = 0x1400 - a;
                int t = _ZN4cstd4fdivEii(b - 0x1000, 0x400);
                scale = (0x1000 - t) + (int)(((s64)t * 0x1400 + 0x800) >> 12);
                d = b;
            }
            d = func_02053200(d);
            {
                struct M m = {0};
                m._00 = d;
                m._11 = d;
                scale = 0x1000 - scale;
                do {
                    s16 f = (s16)((((unsigned int)(*(int*)el << 7)) >> 23) & 0xFFFFFFFFFFFFFFFFull);
                    int off;
                    if (f > 0x100) f -= 0x200;
                    off = f * scale;
                    func_ov004_020b1c68(el,
                                        *(s16*)(self + 0x10) - ((off << 4) >> 16),
                                        *(s16*)(self + 0x12), *(int*)(self + 0x1c),
                                        *(int*)(self + 0x18), &m);
                    flag = *(u16*)(el + 6);
                    el += 8;
                } while (flag != 0xffff);
            }
        } else {
            int idx = (val - 0x1e000) >> 2;
            int i = 0x1000;
            do {
                int diff = idx - i;
                int r = 0x1000;
                if (diff < 0) diff = -diff;
                if (diff < 0x1000) {
                    r += (int)(((s64)(0x1000 - diff) * 0x400 + 0x800) >> 12);
                }
                r = func_02053200(r);
                {
                    struct M m2 = {0};
                    m2._00 = r;
                    m2._11 = r;
                    func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                        *(int*)(self + 0x1c), *(int*)(self + 0x18), &m2);
                }
                i += 0x1000;
                flag = *(u16*)(el + 6);
                el += 8;
            } while (flag != 0xffff);
        }
    }
}
