enum { false, true };

extern void func_ov002_020db54c(unsigned char *self, int a, int b, short c);

#pragma opt_common_subs off
#pragma opt_strength_reduction off

void func_ov002_020d9c70(char *o)
{
    short v = *(short *)(o + 0x69c);
    int absv;
    int A, Braw;
    short Bnarrow, Bresult;
    char *p;

    absv = (v < 0) ? (short)(-v) : v;
    A = (int)(((long long)absv * 0x28000 + 0x800) >> 12);

    absv = (v < 0) ? (short)(-v) : v;
    Braw = (int)(((long long)absv * 0x1c71 + 0x800) >> 12);
    Bnarrow = (short)Braw;

    Bresult = (v < 0) ? (short)(*(short *)(o + 0x8e) - Bnarrow) : (short)(*(short *)(o + 0x8e) + Bnarrow);

    p = *(char **)(o + 0x358);
    if (p == 0)
        return;

    {
        int t = *(unsigned short *)(p + 0xc);
        t = t == 0xbf;
        if (t != false) {
            func_ov002_020db54c((unsigned char *)p, A, 0x20000, Bresult);
            return;
        }
    }

    *(short *)(p + 0x92) = 0;
    *(short *)(p + 0x94) = Bresult;
    *(short *)(p + 0x96) = 0;
    {
        char *q = *(char **)(o + 0x358);
        short *r = (short *)(int)(((long long)(int)(q + 0x92)) & 0xFFFFFFFFFFFFFFFFLL);
        short first = r[0];
        *(short *)(q + 0x8c) = first;
        *(short *)(q + 0x8e) = r[1];
        *(short *)(q + 0x90) = r[2];
    }
    *(int *)(*(char **)(o + 0x358) + 0x98) = A;
    {
        char *q = *(char **)(o + 0x358);
        *(int *)(q + 0xa4) = 0;
        *(int *)(q + 0xa8) = 0x20000;
        *(int *)(q + 0xac) = 0;
    }
}
