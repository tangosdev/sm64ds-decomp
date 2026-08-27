extern void func_ov002_020f2f18(char *c, int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void SetSubBg2Offset(int a, int b);

void func_ov002_020f30f8(char *o, int i)
{
    int idx = i * 0x30;
    unsigned short t;
    int oldA, oldB;
    int cc, dd, ee, ff;

    t = *(unsigned short *)(o + 0x158 + idx);
    if (t != 0) {
        *(unsigned short *)(o + 0x158 + idx) = t - 1;
        return;
    }

    *(int *)(o + 0x140 + idx) -= *(int *)(o + 0x150 + idx);
    *(int *)(o + 0x144 + idx) -= *(int *)(o + 0x154 + idx);
    *(int *)(o + 0x148 + idx) += *(int *)(o + 0x150 + idx);
    *(int *)(o + 0x14c + idx) += *(int *)(o + 0x154 + idx);

    oldA = *(int *)(o + 0x130 + idx);
    oldB = *(int *)(o + 0x134 + idx);
    *(int *)(o + 0x130 + idx) = oldA + *(int *)(o + 0x150 + idx);
    *(int *)(o + 0x134 + idx) += *(int *)(o + 0x154 + idx);

    if (*(int *)(o + 0x130 + idx) >= 0) *(int *)(o + 0x130 + idx) = 0;
    if (*(int *)(o + 0x134 + idx) >= 0) *(int *)(o + 0x134 + idx) = 0;
    if (*(int *)(o + 0x140 + idx) < 0) *(int *)(o + 0x140 + idx) = 0;
    if (*(int *)(o + 0x144 + idx) < 0) *(int *)(o + 0x144 + idx) = 0;
    if (*(int *)(o + 0x148 + idx) >= 0xff000) *(int *)(o + 0x148 + idx) = 0xff000;
    if (*(int *)(o + 0x14c + idx) >= 0xc0000) *(int *)(o + 0x14c + idx) = 0xc0000;
    if (*(int *)(o + 0x130 + idx) > 0) *(int *)(o + 0x130 + idx) = 0;
    if (*(int *)(o + 0x134 + idx) > 0) *(int *)(o + 0x134 + idx) = 0;

    cc = *(int *)(o + 0x130 + idx) - oldA;
    func_ov002_020f2f18(o, cc, *(int *)(o + 0x134 + idx) - oldB);

    ee = *(int *)(o + 0x148 + idx) >> 12;
    ff = *(int *)(o + 0x14c + idx) >> 12;
    cc = *(int *)(o + 0x140 + idx) >> 12;
    dd = *(int *)(o + 0x144 + idx) >> 12;
    *(unsigned short *)0x4001042 = ((cc << 8) & 0xff00) | (ee & 0xff);
    *(unsigned short *)0x4001046 = ((dd << 8) & 0xff00) | (ff & 0xff);
    SetSubBg1Offset(*(int *)(o + 0x130 + idx) >> 12, *(int *)(o + 0x134 + idx) >> 12);
    SetSubBg2Offset(*(int *)(o + 0x130 + idx) >> 12, *(int *)(o + 0x134 + idx) >> 12);
}
