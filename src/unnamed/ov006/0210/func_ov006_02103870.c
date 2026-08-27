extern unsigned char data_ov006_0212ebb4[];

#pragma opt_common_subs off

void func_ov006_02103870(char *c, int i)
{
    *(unsigned short *)(c + 0x4696 + (i << 6)) += 1;

    if (*(unsigned short *)(c + (i << 6) + 0x4600 + 0x96) >= 4) {
        *(unsigned short *)(c + (i << 6) + 0x4600 + 0x96) = 0;
        *(unsigned char *)(c + 0x469d + (i << 6)) += 1;
        if (*(unsigned char *)(c + (i << 6) + 0x4000 + 0x69d) >= 4)
            *(unsigned char *)(c + (i << 6) + 0x4000 + 0x69d) = 0;
    }

    {
        unsigned char idx = *(unsigned char *)(c + (i << 6) + 0x4000 + 0x69d);
        *(unsigned char *)(c + (i << 6) + 0x4000 + 0x69c) = data_ov006_0212ebb4[idx];
        if (*(unsigned char *)(c + (i << 6) + 0x4000 + 0x69e) != 0)
            *(unsigned char *)(c + 0x469c + (i << 6)) += 4;
    }

    *(int *)(c + 0x4660 + (i << 6)) += *(int *)(c + (i << 6) + 0x4000 + 0x668);

    {
        int val = *(int *)(c + (i << 6) + 0x4000 + 0x660);
        int scaled = val >> 12;
        if (scaled >= 0x110 || scaled <= -16) {
            *(unsigned char *)(c + (i << 6) + 0x4000 + 0x699) = 6;
        }
    }
}
