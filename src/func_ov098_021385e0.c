extern short data_02082214[];

#define ADDR(p) ((int *)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov098_021385e0(char *self)
{
    char *obj;
    short angle;
    int idx;
    int cosv, sinv;

    *(int *)(self + 0x5f0) = 0;
    *(int *)(self + 0x5f4) = 0;

    obj = *(char **)(self + 0xd0);
    angle = *(short *)(obj + 0x8e);
    *(short *)(self + 0x8e) = angle;
    *(short *)(self + 0x94) = *(short *)(self + 0x8e);

    obj = *(char **)(self + 0xd0);
    {
        int *osrc = ADDR(obj + 0x5c);
        *(int *)(self + 0x5c) = osrc[0];
        *(int *)(self + 0x60) = osrc[1];
        *(int *)(self + 0x64) = osrc[2];
    }

    idx = (unsigned short)*(short *)(self + 0x8e) >> 4;
    cosv = data_02082214[idx * 2];
    *ADDR(self + 0x5c) = *ADDR(self + 0x5c) + (int)(((long long)cosv * 0x50000 + 0x800) >> 12);

    *ADDR(self + 0x60) = *ADDR(self + 0x60) + 0x50000;

    idx = (unsigned short)*(short *)(self + 0x8e) >> 4;
    sinv = data_02082214[idx * 2 + 1];
    *ADDR(self + 0x64) = *ADDR(self + 0x64) + (int)(((long long)sinv * 0x50000 + 0x800) >> 12);

    *ADDR(self + 0xb0) &= ~0x80000;

    *(int *)(self + 0x98) = 0x1e000;
    *(int *)(self + 0xa8) = 0xf000;
    *(char **)(self + 0x5e8) = *(char **)(self + 0xd0);
    *(char **)(self + 0xd0) = 0;
    *ADDR(self + 0x57c) |= 0x2000;
}
