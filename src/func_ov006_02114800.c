/* ov006 minigame: seeds up to 0x18 particle rows (0x24 bytes each, at c+0x599c)
 * with random headings, first over the free rows in order and then at random
 * rows until the count is reached. The random value is masked only after the
 * first store: the ROM stores the zero, then shifts. Masking before the store
 * lets the scheduler hoist the shift above the store and the second store
 * drifts past the next call's argument setup. */
extern int RandomIntInternal(int *seed);
extern void func_0203d388(int *p, short angle);
extern int data_0209d4b8;

void func_ov006_02114800(char *c, int *pair, int a3)
{
    register int count;
    register int i;
    char *p;
    int *pang;
    unsigned int r;
    char *q;
    int *pf8;
    int *pfc;
    int *pf4;
    int *pf0;
    int ten;
    int one;
    int z_a;
    int z_b;
    int z2;
    int ten2;
    int one2;

    count = 0;
    p = c;
    pang = (int *)(c + 0x59a4);
    ten = 0x10;
    one = 1;
    z_a = count;
    z_b = count;
    for (i = 0; i < 0x40; p += 0x24, pang += 9, i++) {
        if (*(unsigned char *)(p + 0x59bc) == 1)
            continue;
        *(int *)(p + 0x599c) = pair[0];
        *(int *)(p + 0x59a0) = pair[1];
        r = (unsigned int)RandomIntInternal(&data_0209d4b8);
        *(int *)(p + 0x59a4) = z_a;
        r = (r >> 16) & 0x7fff;
        r = r << 16;
        *(int *)(p + 0x59a8) = (int)(r >> 15);
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        func_0203d388(pang, (unsigned short)(r * 2));
        *(int *)(p + 0x59a8) <<= 1;
        *(int *)(p + 0x59a4) += *(int *)(p + 0x599c);
        *(int *)(p + 0x59a8) += *(int *)(p + 0x59a0);
        *(int *)(p + 0x59ac) = z_b;
        *(int *)(p + 0x59b0) = ten;
        *(unsigned char *)(p + 0x59bc) = (unsigned char)one;
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        *(int *)(p + 0x59b4) = (int)((r << 4) >> 15);
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        *(int *)(p + 0x59b8) = (int)((r * 3) >> 15) + 5;
        count++;
        *(unsigned char *)(p + 0x59bd) = (unsigned char)a3;
        if (count >= 0x18)
            return;
    }
    if (count >= 0x18)
        return;

    z2 = 0;
    ten2 = 0x10;
    one2 = 1;
    do {
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        q = c + ((int)((r << 6) >> 15) * 0x24);
        pf0 = (int *)(q + 0x599c);
        *pf0 = pair[0];
        pf4 = (int *)(q + 0x59a0);
        *pf4 = pair[1];
        r = RandomIntInternal(&data_0209d4b8);
        pf8 = (int *)(q + 0x59a4);
        *pf8 = z2;
        pfc = (int *)(q + 0x59a8);
        *pfc = (int)(((((unsigned int)r >> 16) & 0x7fff) * 0xc000) >> 15);
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        func_0203d388(pf8, (unsigned short)(r * 2));
        *pfc <<= 1;
        *pf8 += *pf0;
        *pfc += *pf4;
        *(int *)(q + 0x59ac) = z2;
        *(int *)(q + 0x59b0) = ten2;
        *(unsigned char *)(q + 0x59bc) = (unsigned char)one2;
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        *(int *)(q + 0x59b4) = (int)((r << 4) >> 15);
        r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
        *(int *)(q + 0x59b8) = (int)((r * 3) >> 15) + 5;
        count++;
        *(unsigned char *)(q + 0x59bd) = (unsigned char)a3;
    } while (count < 0x18);
}
