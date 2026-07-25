//cpp
typedef short s16;
typedef unsigned short u16;
typedef long long s64;

struct M { int _00; int _01; int _10; int _11; };
extern "C" {
extern int _ZN4cstd4fdivEii(int, int);
extern int func_02053200(int x);
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5);
}

#pragma opt_propagation off
extern "C" void func_ov004_020b3b38(char* c) {
    int m, fd, t, v;
    unsigned short term;
    if (*(int*)(c + 0x24) == 0) return;
    m = (int)(((s64)*(int*)(c + 0x24) * 0xcc + 0x800) >> 12);
    {
        char* e = c + 0x34;
        fd = _ZN4cstd4fdivEii(m, 0x1000);
        t = (int)(((s64)fd * 0x1000 + 0x800) >> 12);
        v = func_02053200(m);
        {
            struct M buf = {0};
            buf._00 = v;
            buf._11 = v;
            t = 0x1000 - t;
            do {
                s16 f = (s16)((((unsigned int)(*(int*)e << 7)) >> 23) & 0xFFFFFFFFFFFFFFFFull);
                int off;
                if (f > 0x100) f -= 0x200;
                off = f * t;
                func_ov004_020b1c68(e,
                    *(s16*)(c + 0x10) - ((off << 4) >> 16),
                    *(s16*)(c + 0x12), *(int*)(c + 0x1c),
                    *(int*)(c + 0x18), &buf);
                term = *(unsigned short*)(e + 6);
                e += 8;
            } while (term != 0xffff);
        }
    }
}
