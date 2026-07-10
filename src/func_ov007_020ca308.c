typedef unsigned short u16;
typedef short s16;
typedef long long s64;
extern short data_02082214[];
extern int func_ov007_020c3ba8(int);
extern int _ZN4cstd3divEii(int, int);
#pragma opt_common_subs off
#pragma opt_strength_reduction off
void func_ov007_020ca308(char *self, char *t, int i)
{
    char *e;
    char *h;
    int cur;
    int amp;
    int cnt;
    int a;
    int b;
    int w;
    int timer;
    int j;
    int ph;

    e = *(char **)(self + i * 4 + 8);
    h = *(char **)(self + 0x18);
    a = *(int *)h;
    cur = *(s16 *)e;
    b = *(int *)(h + 4);
    amp = *(int *)(h + 8);

    if (*(int *)(e + 0xc) == 0)
        *(int *)(*(char **)(self + i * 4 + 8) + 0xc) = func_ov007_020c3ba8(0x3c);
    timer = *(int *)(*(char **)(self + i * 4 + 8) + 0xc);

    if (i == 0 || i == 2) {
        char *q = *(char **)(t + 4);
        cnt = *(int *)(q + 4);
        w = *(int *)q;
    } else if (i == 1 || i == 3) {
        char *q = *(char **)(t + 4);
        cnt = *(int *)q;
        w = *(int *)(q + 4);
    }

    if (cur >= 0 && cur < w) {
        int c = *(u16 *)(h + 0xe);
        if (func_ov007_020c3ba8(10000) <= c * 10 - 1) {
            u16 base;
            u16 step = (u16)_ZN4cstd3divEii(b, cnt);
            j = 0;
            base = (u16)(timer * a);
            if (cnt <= 0)
                return;
            ph = j;
            do {
                s16 s = data_02082214[((u16)(base + ph) >> 4) * 2];
                int val = (int)(((s64)amp * s + 0x800) >> 12);
                if (i == 0) {
                    int p = *(int *)(*(int *)(t + 4) + 8);
                    p = ((int *)p)[j];
                    p = *(int *)p;
                    p = ((int *)p)[cur];
                    *(int *)((p + 0x18) & 0xFFFFFFFFFFFFFFFFLL) += val;
                } else if (i == 1) {
                    int p = *(int *)(*(int *)(t + 4) + 8);
                    p = ((int *)p)[cur];
                    p = *(int *)p;
                    p = ((int *)p)[j];
                    *(int *)((p + 0x18) & 0xFFFFFFFFFFFFFFFFLL) += val;
                }
                j++;
                ph += step;
            } while (j < cnt);
            return;
        }
    }

    if (cur == -1) {
        if (cur != -1)
            return;
        if (timer < *(u16 *)(h + 0xc))
            return;
    }
    {
        int pr = *(u16 *)(h + 0x10);
        if (func_ov007_020c3ba8(10000) <= pr * 10 - 1) {
            *(s16 *)(*(char **)(self + i * 4 + 8) + 2) = -1;
            return;
        }
    }
    *(s16 *)(*(char **)(self + i * 4 + 8) + 2) = func_ov007_020c3ba8(w);
}
