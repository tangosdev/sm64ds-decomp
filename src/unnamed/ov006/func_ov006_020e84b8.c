extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

#pragma opt_common_subs off
#pragma opt_strength_reduction off

void func_ov006_020e84b8(char *c, int i)
{
    *(int *)(c + 0x52bc + i * 32) += *(int *)(c + i * 32 + 0x52c4);
    *(int *)(c + 0x52c0 + i * 32) += *(int *)(c + i * 32 + 0x52c8);
    if (*(unsigned short *)(c + i * 32 + 0x52ce) != 0) {
        *(unsigned short *)(c + 0x52ce + i * 32) -= 1;
        if (*(short *)(c + i * 32 + 0x52ce) < 0)
            *(short *)(c + i * 32 + 0x52ce) = 0;
        return;
    }
    *(int *)(c + 0x52c4 + i * 32) -= (*(unsigned char *)(c + i * 32 + 0x52d5) << 4) + 0x10;
    if (*(unsigned short *)(c + i * 32 + 0x52d0) != 0) {
        *(unsigned short *)(c + 0x52d0 + i * 32) -= 1;
        if (*(short *)(c + i * 32 + 0x52d0) < 0)
            *(short *)(c + i * 32 + 0x52d0) = 0;
        return;
    }
    {
        int rnd = (int)((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff);
        /* ROM uses lsr (logical), not asr — keep unsigned through the shifts */
        rnd = (int)((((unsigned int)rnd) << 4) >> 0xf);
        rnd = rnd + 0x10;
        *(short *)(c + i * 32 + 0x52ce) = (short)(rnd & 0xff);
    }
    *(unsigned char *)(c + i * 32 + 0x52da) = 3;
}
