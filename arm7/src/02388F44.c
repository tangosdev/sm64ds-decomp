extern void fn_02388CD8(int a1);
extern long long fn_0238D594(int a1, int a2);

int fn_02388F44(unsigned short *out, int tol, int count, int flag) {
    int i;
    int mode;
    int cmd8;
    int neg;
    int cmd;
    int sample;
    int zero;
    int one;
    int two;
    int s[2];
    volatile unsigned short *ctrl;
    volatile unsigned short *data;

    if (flag == 0) {
        cmd = 0xD1;
        mode = 1;
    } else {
        cmd = 0x91;
        mode = 2;
    }

    ctrl = (volatile unsigned short *)0x40001C0;
    while ((*ctrl & 0x80) != 0)
        fn_02388CD8(1);
    *ctrl = 0x8A01;
    cmd8 = cmd & 0xFF;
    data = (volatile unsigned short *)0x40001C2;
    *data = cmd8;

    one = 1;
    while ((*ctrl & 0x80) != 0)
        fn_02388CD8(one);
    *data = 0;

    one = 1;
    ctrl = (volatile unsigned short *)0x40001C0;
    while ((*ctrl & 0x80) != 0)
        fn_02388CD8(one);

    data = (volatile unsigned short *)0x40001C2;
    s[0] = (int)(((*data & 0xFFu) << 16) >> 8);
    *data = cmd8;

    one = 1;
    ctrl = (volatile unsigned short *)0x40001C0;
    while ((*ctrl & 0x80) != 0)
        fn_02388CD8(one);

    {
        unsigned short b = (unsigned short)(*data & 0xFF);
        s[0] = s[0] | b;
        s[0] = (s[0] & 0x7FF8) >> 3;
        *data = 0;
    }

    one = 1;
    ctrl = (volatile unsigned short *)0x40001C0;
    while ((*ctrl & 0x80) != 0)
        fn_02388CD8(one);

    data = (volatile unsigned short *)0x40001C2;
    s[1] = (int)(((*data & 0xFFu) << 16) >> 8);
    *data = cmd8;

    one = 1;
    ctrl = (volatile unsigned short *)0x40001C0;
    while ((*ctrl & 0x80) != 0)
        fn_02388CD8(one);

    {
        unsigned short b = (unsigned short)(*data & 0xFF);
        s[1] = s[1] | b;
        s[1] = (s[1] & 0x7FF8) >> 3;
    }

    if (count == 0) {
        *out = (unsigned short)((s[0] + s[1]) >> 1);
        mode = 0;
    }

    i = 0;
    neg = -tol;
    zero = 0;
    one = 1;
    two = 2;
    while (i < count) {
        data = (volatile unsigned short *)0x40001C2;
        *data = zero;
        ctrl = (volatile unsigned short *)0x40001C0;
        while ((*ctrl & 0x80) != 0)
            fn_02388CD8(one);
        {
            int hi = (int)(((*data & 0xFFu) << 16) >> 8);
            *data = cmd8;
            while ((*ctrl & 0x80) != 0)
                fn_02388CD8(one);
            {
                unsigned shifted = (*data & 0xFFu) << 16;
                int merged = hi | (shifted >> 16);
                merged &= 0x7FF8;
                sample = merged >> 3;
                if (s[0] - (merged >> 3) >= neg && s[0] - (merged >> 3) <= tol) {
                    if (s[1] - sample >= neg && s[1] - sample <= tol) {
                        long long q = fn_0238D594(i + 1, 2);
                        *out = (unsigned short)((sample + s[(int)(q >> 32)]) >> 1);
                        mode = 0;
                        break;
                    }
                }
            }
        }
        {
            long long q = fn_0238D594(i, two);
            s[(int)(q >> 32)] = sample;
        }
        i++;
    }
    if (mode != 0)
        *out = (unsigned short)((s[0] + s[1]) >> 1);

    ctrl = (volatile unsigned short *)0x40001C0;
    *ctrl = 0x8201;
    data = (volatile unsigned short *)0x40001C2;
    *data = 0;
    one = 1;
    while ((*ctrl & 0x80) != 0)
        fn_02388CD8(one);
    *out = (unsigned short)(*out & ~7);
    return mode;
}
