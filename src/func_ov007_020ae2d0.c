typedef unsigned char u8;
typedef unsigned short u16;
extern u8 data_ov007_0210342c[];
extern short SINE_TABLE[];
extern int _ZN4cstd3modEii(int, int);
extern int _ZN4cstd3divEii(int, int);

void func_ov007_020ae2d0(char *c)
{
    int *p = *(int**)c;
    u16 state = *(u16*)p;
    int v = *(int*)((char*)*(int**)((char*)p + 4) + 0xc);

    switch (state) {
    case 1: {
        int *q = *(int**)data_ov007_0210342c;
        short a = *(short*)(*(int**)((char*)q + 4));
        if (a == 0) {
            short b = *(short*)(*(int**)((char*)q + 8));
            if (b == 0) return;
        }
        if (a == 2) return;
        if (v < 0x28) return;
        if (_ZN4cstd3modEii(v - 0x28, 0x10e) != 0) return;
        {
            int *p2 = *(int**)c;
            *(int*)((char*)p2 + 0x1c) = 0x3c;
            *(int*)((char*)p2 + 0x18) = *(int*)((char*)p2 + 0x1c);
        }
        return;
    }
    case 0: {
        int n = _ZN4cstd3modEii(v, 0x32);
        int m, arrIdx;
        short idx, base;
        int *q;
        if (n <= 0) n = 0;
        else if (n >= 0x32) n = 0x1000;
        else n = _ZN4cstd3divEii(n << 0xc, 0x32);
        m = (-n) * 0xffff;
        m = (int)(((unsigned)(m << 4)) >> 0x10);
        arrIdx = m >> 4;
        idx = SINE_TABLE[arrIdx * 2];
        q = *(int**)c;
        base = *(short*)((char*)*(int**)((char*)q + 0x24) + 4);
        *(int*)((char*)q + 0xc) = idx * 3 + (base << 12);
        return;
    }
    case 7: {
        int *q = *(int**)data_ov007_0210342c;
        int *r = *(int**)((char*)q + 0x134);
        int *s = *(int**)r;
        short t = *(short*)(*(int**)((char*)s + 4));
        if (t == 0) {
            if (*(int*)((char*)p + 0x20) == 0) *(int*)((char*)p + 0x20) = 1;
        } else {
            if (*(int*)((char*)p + 0x20) != 0) *(int*)((char*)p + 0x20) = 0;
        }
        return;
    }
    }
}
