extern void _Z14ApproachLinearRiii(int *ref, int a, int b);
extern int func_01ffa344(int a, int b);
extern int AngleDiff(int a, int b);
extern void ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
extern int func_01ffa4bc(int a);
extern int func_01ff99a4(int a, int b);
extern int func_01ff98f4(int a, int b);
extern unsigned char data_020a0e40;
extern char data_0209f4a0[];
extern short data_02082214[];

#define ADDR(p) ((int *)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov002_020dc560(char *self)
{
    _Z14ApproachLinearRiii((int *)(self + 0x98), 0, 0x599);

    {
        int tblval = *(short *)(data_0209f4a0 + data_020a0e40 * 0x18);
        if (tblval != 0) {
            short diff = (short)(*(short *)(self + 0x6d2) - *(short *)(self + 0x94));
            int idx2 = ((unsigned short)diff >> 4) * 2;
            int r1 = func_01ffa344(0x48400000, (unsigned short)diff >> 4);
            int sinv = data_02082214[idx2 + 1];
            int scaled = (int)(((long long)sinv * 0x1800 + 0x800) >> 12);
            int combined = (int)(((long long)scaled * tblval + 0x800) >> 12);
            _Z14ApproachLinearRiii((int *)(self + 0x98), r1, combined);

            {
                short *p94 = (short *)ADDR(self + 0x94);
                int cosv = data_02082214[idx2];
                int tblval2 = *(short *)(data_0209f4a0 + data_020a0e40 * 0x18);
                int scaled2 = (int)(((long long)cosv * 0x200 + 0x800) >> 12);
                int combined2 = (int)(((long long)scaled2 * tblval2 + 0x800) >> 12);
                *p94 = (short)(*p94 + combined2);
            }
        }
    }

    {
        short cur = *(short *)(self + 0x94);
        int ad = AngleDiff(cur, *(short *)(self + 0x8e));
        short target = cur;
        if (ad >= 0x4000) target = (short)(*(short *)(self + 0x94) + 0x8000);
        ApproachAngle((short *)(self + 0x8e), target, 8, 0x4000, 0x10);
    }

    if (func_01ff99a4(func_01ffa4bc(*(int *)(self + 0x98)), 0x48400000) != 0) {
        *ADDR(self + 0x98) -= 0x1000;
    }

    if (func_01ff98f4(func_01ffa4bc(*(int *)(self + 0x98)), -0x3e800000) != 0) {
        *ADDR(self + 0x98) += 0x2000;
    }
}
