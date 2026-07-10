extern unsigned char DecIfAbove0_Byte(unsigned char* p);

int func_ov071_0212070c(char* c)
{
    int delta;
    int eq;
    int hi;
    int *p;
    int v;
    short *b200;

    b200 = (short *)(c + 0x200);
    delta = (short)(*(short *)(c + 0x8e) - b200[6]);
    eq = (int)(*(unsigned short *)(c + 0xc) == 0x107);
    if (eq != 0)
        hi = 0x190;
    else
        hi = 0x320;

    if (delta > hi) {
        v = *(int *)(c + 0x1f4);
        if (v >= 0) {
            p = (int *)(((int)c + 0x1f4) & 0xFFFFFFFFFFFFFFFF);
            *p = *p + delta;
            *(unsigned char *)(c + 0x216) = 0x2e;
        } else {
            if (*(unsigned char *)(c + 0x216) == 0)
                *(int *)(c + 0x1f4) = 0;
            DecIfAbove0_Byte((unsigned char *)(((int)c + 0x216) & 0xFFFFFFFFFFFFFFFF));
        }
    } else if (delta < -hi) {
        v = *(int *)(c + 0x1f4);
        /* fallthrough ADD when v <= 0 (target: bgt to Dec) */
        if (v <= 0) {
            p = (int *)(((int)c + 0x1f4) & 0xFFFFFFFFFFFFFFFF);
            *p = *p + delta;
            *(unsigned char *)(c + 0x216) = 0x2e;
        } else {
            if (*(unsigned char *)(c + 0x216) == 0)
                *(int *)(c + 0x1f4) = 0;
            DecIfAbove0_Byte((unsigned char *)(((int)c + 0x216) & 0xFFFFFFFFFFFFFFFF));
        }
    } else {
        if (*(unsigned char *)(c + 0x216) == 0)
            *(int *)(c + 0x1f4) = 0;
        DecIfAbove0_Byte((unsigned char *)(((int)c + 0x216) & 0xFFFFFFFFFFFFFFFF));
    }

    v = *(int *)(c + 0x1f4);
    if (v > 0x17fff || v < -0x17fff) {
        *(int *)(c + 0x1f4) = 0;
        *(unsigned char *)(c + 0x216) = 0x2e;
        return 1;
    }
    return 0;
}
