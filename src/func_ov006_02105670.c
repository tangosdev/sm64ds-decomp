void func_ov006_02105670(char *p)
{
    if (((unsigned short *)(p + 0x4e00))[0x62] != 0)
    {
        (*(unsigned short *)(((long long)(int)(p + 0x4ec4)) & 0xFFFFFFFFFFFFFFFFLL))--;
        if (((short *)(p + 0x4e00))[0x62] < 0)
        {
            ((short *)(p + 0x4e00))[0x62] = 0;
        }
        return;
    }
    else
    {
        int k;
        for (k = 0; k < *(int *)(p + 0x4cb8); k++)
        {
            *(unsigned char *)(p + k + 0x4efa) = 1;
            *(unsigned char *)(((long long)(int)(p + k + 0x4f1e)) & 0xFFFFFFFFFFFFFFFFLL) ^= 1;
            *(unsigned char *)(p + k + 0x4f8a) = 1;
        }
        *(unsigned char *)(p + 0x4fe0) = 2;
        ((unsigned short *)(p + 0x4e00))[0x62] = 0x20;
    }
}
