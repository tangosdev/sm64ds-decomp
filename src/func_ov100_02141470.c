extern short data_02082214[];
extern short Vec3_HorzAngle(const void *a, const void *b);
extern short Vec3_VertAngle(const void *a, const void *b);
extern void _Z14ApproachLinearRsss(short *dst, short target, short step);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cyl);
extern int _ZN5Actor15IsPlayerInRangeEi(void *self, int range);

void func_ov100_02141470(char *c)
{
    int *p;

    _Z14ApproachLinearRsss((short *)(c + 0x94),
        Vec3_HorzAngle(c + 0x5c, c + 0x3d4), 0x800);
    _Z14ApproachLinearRsss((short *)(c + 0x92),
        Vec3_VertAngle(c + 0x5c, c + 0x3d4), 0x50);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);

    p = (int *)(((long long)(int)(c + 0x60)) & 0xFFFFFFFFFFFFFFFFLL);
    *p = *p - ((int)(((long long)*(int *)(c + 0x98)
        * data_02082214[(*(unsigned short *)(c + 0x92) >> 4) * 2] + 0x800) >> 12)
        + (short)data_02082214[
        ((unsigned short)(short)((*(int *)(c + 0x3e8) << 16) / 100) >> 4) * 2 + 1]
        * (short)20 / 4);

    {
        int *cnt = (int *)(((long long)(int)(c + 0x3e8)) & 0xFFFFFFFFFFFFFFFFLL);
        *cnt = *cnt + 1;
        if (*(int *)(c + 0x3e8) > 100)
            *(int *)(c + 0x3e8) = 0;
    }

    if (_ZN5Actor15IsPlayerInRangeEi(c, 0xbb8))
        return;

    *(int *)(c + 0x5c) = *(int *)(c + 0x3d4);
    *(int *)(c + 0x60) = *(int *)(c + 0x3d8);
    *(int *)(c + 0x64) = *(int *)(c + 0x3dc);
    *(int *)(c + 0x3e4) = 1;
}
