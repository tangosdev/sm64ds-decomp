extern void func_ov006_020def80(char *c, int i);
extern int RandomIntInternal(int *seed);
extern void FreeGfxSlotsById(int n);

extern int data_0209e650;
extern unsigned char data_ov006_0213c094[];

void func_ov006_020dfd48(char *c)
{
    int i;
    int r4;
    unsigned int r0;
    unsigned int r3;
    unsigned int r1;
    unsigned int r2;

    (*(int *)(((int)c + 0x541c)))--;
    if (*(int *)(c + 0x541c) > 0) {
        return;
    }

    for (i = 0; i < 3; i++) {
        *(unsigned char *)(c + i + 0x5465) = 0;
        func_ov006_020def80(c, i);
    }

    r4 = *(int *)(c + 0xb4);
    if (r4 < 0xa) {
        *(unsigned char *)(c + 0x5461) = (unsigned char)r4;
    } else {
        r0 = (unsigned int)RandomIntInternal(&data_0209e650);
        r3 = r0 >> 16;
        r1 = (r3 % 5) + 5;
        *(unsigned char *)(c + 0x5461) = (unsigned char)r1;
    }

    *(unsigned char *)(c + 0x5460) =
        data_ov006_0213c094[*(unsigned char *)(c + 0x5461) * 2];

    if (r4 > 3) {
        r0 = (unsigned int)RandomIntInternal(&data_0209e650);
        if (r0 & 1) {
            *(unsigned char *)(c + 0x5468) = 1;
        } else {
            *(unsigned char *)(c + 0x5468) = 2;
        }
    } else if (r4 == 3) {
        *(unsigned char *)(c + 0x5468) = 2;
    }

    *(unsigned char *)(c + 0x546b) = 0;
    *(unsigned char *)(c + 0x5469) = 0;
    *(int *)(c + 0x5418) = 1;
    *(int *)(c + 0x541c) = 0x1e;
    *(unsigned char *)(c + 0x546d) = 0xff;
    *(int *)(c + 0x5458) = 0;
    FreeGfxSlotsById(0x1d);

    r0 = (unsigned int)RandomIntInternal(&data_0209e650);
    r2 = (r0 % 10) + 1;
    *(unsigned char *)(c + 0x546c) = (unsigned char)r2;

    if (*(unsigned char *)(c + 0xc4) == 0) {
        *(unsigned char *)(c + 0xc3) = 1;
        *(unsigned char *)(c + 0xc4) = 1;
        *(short *)(c + 0xc0) = 0;
    }
}
