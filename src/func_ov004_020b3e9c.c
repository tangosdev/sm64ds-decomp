typedef short s16;
typedef unsigned short u16;
typedef long long s64;

extern s16 data_02082214[];
extern int _ZN4cstd4fdivEii(int a, int b);
extern int func_02053200(int x);
struct M { int _00, _01, _10, _11; };
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5);

void func_ov004_020b3e9c(char* self) {
    u16 flag;
    if (*(int*)(self + 0x24) == 0) return;
    {
        char* el = self + 0x34;
        if (*(int*)(self + 0x24) < 0x18000) {
            int v = 0x2000;
            if (*(int*)(self + 0x24) >= 0x14000) {
                v = 0x1000;
            } else {
                int t = _ZN4cstd4fdivEii(0x1000, 0x18000);
                v -= (int)(((s64)*(int*)(self + 0x24) * t + 0x800) >> 12);
                v = func_02053200(v);
            }
            {
                struct M m = {0};
                m._00 = v;
                m._11 = v;
                do {
                    func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                        *(int*)(self + 0x1c), *(int*)(self + 0x18), &m);
                    flag = *(u16*)(el + 6);
                    el += 8;
                } while (flag != 0xffff);
            }
        } else {
            int angle = (u16)(*(int*)(self + 0x24) - 0x18000);
            struct M m2;
            do {
                int s0 = data_02082214[(angle >> 4) * 2];
                int mm = (int)(((s64)s0 * 0x800 + 0x800) >> 12);
                int idx = (u16)mm >> 4;
                int cos1 = data_02082214[idx * 2 + 1];
                int sin1 = data_02082214[idx * 2];
                int fc = (int)(((s64)cos1 * 0x1000 + 0x800) >> 12);
                int fs = (int)(((s64)sin1 * 0x1000 + 0x800) >> 12);
                m2._00 = fc;
                m2._11 = fc;
                m2._01 = fs;
                m2._10 = -fs;
                func_ov004_020b1c68(el, *(s16*)(self + 0x10), *(s16*)(self + 0x12),
                                    *(int*)(self + 0x1c), *(int*)(self + 0x18), &m2);
                angle = (u16)(angle + 0x4000);
                flag = *(u16*)(el + 6);
                el += 8;
            } while (flag != 0xffff);
        }
    }
}
