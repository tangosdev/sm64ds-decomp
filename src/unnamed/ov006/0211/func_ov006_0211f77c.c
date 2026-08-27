extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern short data_02082214[];
extern int data_ov006_0212f014[];
extern int data_ov006_0212f050[];
extern int data_ov006_0212f08c[];

void func_ov006_0211f77c(char *c)
{
    int sel;
    int count;
    unsigned short phase;
    int i;
    int valA;
    int flagA;
    int valB;
    int cval;
    int full;
    unsigned int r;

    sel = *(int *)(c + 0xbc);
    if (sel >= 0xf) {
        int k;
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        k = (r << 2) >> 15;
        if (k + 0xa == *(unsigned char *)(c + 0x4c23)) {
            k += (((((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 3) >> 15) + 1;
            if (k >= 4) k -= 4;
        }
        sel = k + 0xa;
    }
    *(unsigned char *)(c + 0x4c23) = sel;
    count = data_ov006_0212f014[sel];
    *(unsigned char *)(c + 0x4c25) = count;

    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    phase = ((r << 4) >> 15) << 12;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    valA = ((((r << 3) >> 15) << 3) + 0x60) << 12;
    r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
    valB = ((((r << 3) >> 15) << 3) + 0x60) << 12;

    i = 0;
    if (count <= 0) {
        return;
    }
    flagA = data_ov006_0212f08c[sel];
    cval = data_ov006_0212f050[sel];
    full = 0x10000;
    do {
        if (flagA == 0) {
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(int *)(c + 0x4660) = ((((r * 0xf) >> 15) << 4) + 0x10) << 12;
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(int *)(c + 0x4664) = ((((r * 0xb) >> 15) << 4) + 0x10) << 12;
            r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(unsigned short *)(c + 0x466c) = ((r << 5) >> 15) << 11;
        } else {
            *(int *)(c + 0x4660) = valA;
            *(int *)(c + 0x4664) = valB;
            int q = full / count;
            *(unsigned short *)(c + 0x466c) = q * i + phase;
        }
        *(unsigned char *)(c + 0x467f) = cval;
        *(unsigned short *)(c + 0x466e) = 0;
        *(unsigned short *)(c + 0x4670) = 0;
        *(unsigned short *)(c + 0x4672) = 0;
        *(unsigned short *)(c + 0x4674) = 0;
        *(unsigned char *)(c + 0x4677) = 1;
        *(unsigned char *)(c + 0x4678) = 0;
        *(unsigned char *)(c + 0x4679) = 0;
        *(unsigned char *)(c + 0x467b) = 0;
        *(unsigned char *)(c + 0x467c) = 0;
        *(unsigned char *)(c + 0x467d) = 0;
        {
            int ang = *(unsigned short *)(c + 0x466c);
            if (data_02082214[(ang >> 4) * 2 + 1] >= 0) {
                *(unsigned char *)(c + 0x467e) = 1;
            } else {
                *(unsigned char *)(c + 0x467e) = 0;
            }
        }
        i++;
        c += 0x24;
    } while (i < count);
}
