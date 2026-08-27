void func_ov006_020e4520(char *c, int idx)
{
    int off = idx * 0x24;

    *(int *)(c + 0x48c0 + off) += *(int *)(c + 0x48c8 + off);
    *(int *)(c + 0x48c4 + off) += *(int *)(c + 0x48cc + off);

    {
        unsigned short cv = *(unsigned short *)(c + 0x48d4 + off);
        if (cv != 0) {
            *(short *)(c + 0x48d4 + off) = (short)(cv - 1);
            if (*(short *)(c + 0x48d4 + off) < 0) {
                *(short *)(c + 0x48d4 + off) = 0;
            }
            return;
        }
    }

    if (*(int *)(c + 0x48c8 + off) < 0x400) {
        *(int *)(c + 0x48c8 + off) += 0x20;
        if (*(int *)(c + 0x48c8 + off) >= 0x400) {
            *(int *)(c + 0x48c8 + off) = 0x400;
        }
    }

    {
        unsigned short ev = *(unsigned short *)(c + 0x48d6 + off);
        if (ev != 0) {
            *(short *)(c + 0x48d6 + off) = (short)(ev - 1);
            if (*(short *)(c + 0x48d6 + off) < 0) {
                *(short *)(c + 0x48d6 + off) = 0;
            }
            return;
        }
    }

    *(unsigned char *)(c + 0x48de + off) = 3;
}
