//cpp
extern "C" {
extern int __aeabi_idiv(int a, int b);
extern int func_ov080_02125d08(void *self, int frames);
extern int func_ov080_02125cd4(void *c, int n);
extern int _ZN4cstd4fdivEii(int a, int b);
extern short data_02082214[];
int func_ov080_02125bb0(void *self, int frames);
}

int func_ov080_02125bb0(void *self, int frames)
{
    char *c = (char*)self;
    int f1b6;
    int r5;
    int r4;
    int t;
    short tv;

    f1b6 = *(unsigned short*)(c + 0x1b6);
    if (f1b6 == 0) return 0;

    unsigned char bits = ((unsigned int)*(int*)(c + 8) >> 0xd) & 3;
    if (bits == 0) {
        int *p = *(int**)(c + 0x1ac);
        int q = __aeabi_idiv(0xffff, p[1]);
        q = __aeabi_idiv(p[2], q);
        if (frames > q * (p[4] - f1b6)) return 0;
    }

    r5 = func_ov080_02125d08(c, frames);
    r4 = func_ov080_02125cd4(c, (*(int**)(c + 0x1ac))[4] - *(unsigned short*)(c + 0x1b6));
    {
        int d = _ZN4cstd4fdivEii(r4, (*(int**)(c + 0x1ac))[3]);
        t = r4 + (int)(((long long)(-d) * frames + 0x800) >> 12);
        if (t < 0) t = 0;
    }
    tv = data_02082214[((unsigned short)r5 >> 4) << 1];
    return (int)(((long long)tv * t + 0x800) >> 12);
}
