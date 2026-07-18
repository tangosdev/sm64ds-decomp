extern short data_02082214[];
void func_ov063_0211a810(char *r0, int cond)
{
    int *f = (int *)(((long long)(int)(r0 + 0x19c)) & 0xFFFFFFFFFFFFFFFFLL);
    unsigned short *h = (unsigned short *)(((long long)(int)(r0 + 0x5d4)) & 0xFFFFFFFFFFFFFFFFLL);
    *f |= 1;
    *h &= ~4;
    *(short *)(r0 + 0x5b4) = *(short *)(r0 + 0x94);
    if (cond != 0) {
        *(short *)(r0 + 0x5b6) = *(short *)(*(char **)(r0 + 0x484) + 0x8e);
        return;
    }
    {
        short base = *(short *)(r0 + 0x94);
        int d = (int)base - (int)*(short *)(r0 + 0x5b0);
        d = (short)d;
        d = (unsigned short)d;
        d >>= 4;
        {
            short t = data_02082214[(d << 1) + 1];
            if (t < 0)
                *(short *)(r0 + 0x5b6) = base;
            else
                *(short *)(r0 + 0x5b6) = (short)(base + 0x8000);
        }
    }
}
