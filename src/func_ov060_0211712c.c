extern short data_02082214[];

void func_ov060_0211712c(char *p)
{
    short ang = (short)(((*(int *)(p + 0x370) + *(unsigned short *)(p + 0x376)) & 0x3f) << 10);
    int bi = (unsigned short)ang >> 4;
    int *px = (int *)(((long long)(int)(p + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    int *pz;
    *px = *px + ((data_02082214[((int)*(unsigned short *)(p + 0x94) >> 4) * 2]
                  * data_02082214[bi * 2]) << 2) / 0x1000;
    pz = (int *)(((long long)(int)(p + 0x64)) & 0xFFFFFFFFFFFFFFFFLL);
    *pz = *pz + ((data_02082214[((int)*(unsigned short *)(p + 0x94) >> 4) * 2 + 1]
                  * data_02082214[bi * 2 + 1]) << 2) / 0x1000;
}
