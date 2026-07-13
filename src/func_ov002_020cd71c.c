typedef short s16;

extern int data_ov002_020ff1f0[];
extern unsigned char data_020a0e40[];
extern short data_0209f4a2[];
extern void _Z15ApproachLinear2Rsss(short *v, short t, short s);

void func_ov002_020cd71c(char *self)
{
    int ip = data_ov002_020ff1f0[*(int *)(self + 8)];
    int a = (int)(((long long)ip * 0x320 + 0x800) >> 12);
    int c = (int)((((long long)ip << 7) + 0x800) >> 12);
    s16 step = (s16)((((long long)ip << 5) + 0x800) >> 12);
    int b = (int)(((long long)ip * 0x240 + 0x800) >> 12);
    int e = *(short *)((char *)data_0209f4a2 + data_020a0e40[0] * 0x18);
    s16 target = (s16)(((long long)(-b) * e + 0x800) >> 12);

    if (*(int *)(self + 0x640) == 0) {
        step = (s16)((((long long)ip << 6) + 0x800) >> 12);
        target = (s16)(((long long)(-a) * e + 0x800) >> 12);
    }

    if (target > 0) {
        if (*(s16 *)(self + 0x69c) < 0) {
            s16 *q = (s16 *)(((int)self + 0x69c) & 0xFFFFFFFFFFFFFFFFLL);
            *q = *q + 0x40;
            if (*(s16 *)(self + 0x69c) > c)
                *(s16 *)(self + 0x69c) = c;
        } else {
            _Z15ApproachLinear2Rsss((short *)(self + 0x69c), target, step);
        }
    } else if (target < 0) {
        if (*(s16 *)(self + 0x69c) > 0) {
            s16 *q = (s16 *)(((int)self + 0x69c) & 0xFFFFFFFFFFFFFFFFLL);
            *q = *q - 0x40;
            if (*(s16 *)(self + 0x69c) < -c)
                *(s16 *)(self + 0x69c) = -c;
        } else {
            _Z15ApproachLinear2Rsss((short *)(self + 0x69c), target, step);
        }
    } else {
        _Z15ApproachLinear2Rsss((short *)(self + 0x69c), 0, 0x40);
    }

    {
        s16 *q = (s16 *)(((int)self + 0x8e) & 0xFFFFFFFFFFFFFFFFLL);
        *q = *q + *(s16 *)(self + 0x69c);
    }
    *(s16 *)(self + 0x90) = -(*(s16 *)(self + 0x69c) << 3);
    *(s16 *)(self + 0x94) = *(s16 *)(self + 0x8e);
}
