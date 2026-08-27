extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int data_ov006_0212e358[];

void func_ov006_020dd334(char *c)
{
    int a[3];
    int b[3];
    int k;
    int r3;
    int r2;
    int i;
    char *p;

    for (k = 0; k < 3; k++) {
        a[k] = (int)((((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) << 3) >> 15);
    }

    for (k = 0; k < 3; k++) {
        unsigned int w = ((((unsigned int)RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 6;
        b[k] = (a[k] + (int)(w >> 15) + 1) & 7;
    }

    r2 = 0;
    r3 = 0;
    i = 0;
    p = c;
    for (; i < 0x18; i++, p += 0x18) {
        *(int *)(p + 0x4ac0) = (((r3 << 5) + 0x10) << 12);
        *(int *)(p + 0x4ac4) = data_ov006_0212e358[r2] << 12;
        r3++;
        *(unsigned char *)(p + 0x4ad1) = 1;
        *(unsigned char *)(p + 0x4ad2) = 1;
        if (r3 >= 8) { r3 = 0; r2++; }
    }

    *(unsigned char *)(c + a[0] * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + b[0] * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (a[1] + 8) * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (b[1] + 8) * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (a[2] + 0x10) * 0x18 + 0x4ad4) = 1;
    *(unsigned char *)(c + (b[2] + 0x10) * 0x18 + 0x4ad4) = 1;
}
