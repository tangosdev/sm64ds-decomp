#pragma opt_common_subs off

int func_ov006_02125cdc(int c, int *p)
{
    int z = p[1];
    int x;
    unsigned short t;
    int sum, diff, xf, zf;

    if (z < -0x20000)
        return 1;
    if (z >= *(int *)(c + 0xba08) << 16)
        return 1;
    if (z >= *(int *)(c + 0xab6c) + 0x1d0000)
        return 1;
    x = p[0];
    if (x < 0)
        return 1;
    if (x >= 0x100000)
        return 1;
    if (z < 0)
        t = *(unsigned short *)(c + ((x >> 12) / 16) * 0x5c0 + 0x4f38);
    else
    {
        int xq = (x >> 12) / 16;
        xq = xq ? xq : xq;
        t = *(unsigned short *)(c + xq * 0x5c0 + ((z >> 12) / 16) * 2 + 0x4f38);
    }
    zf = (z >> 12) & 0xf;
    xf = (x >> 12) & 0xf;
    sum = xf + zf;
    diff = xf - zf;
    switch (t) {
    case 0: case 13: return 0;
    case 1: return 1;
    case 2: case 14: return ((x >> 12) & 8) == 0;
    case 3: return sum < 0x18;
    case 4: return sum < 8;
    case 5: return diff < 8;
    case 6: return diff < -8;
    case 7: case 15: return ((x >> 12) & 8) != 0;
    case 8: return diff >= 8;
    case 9: return diff >= -8;
    case 10: return sum >= 0x18;
    case 11: return sum >= 8;
    case 12: return 0;
    default: return 0;
    }
}
