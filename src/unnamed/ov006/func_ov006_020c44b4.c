extern void *data_ov006_021402fc;
extern int data_ov006_02140304;
extern int data_ov006_02140308;
extern int data_ov006_0214031c;
extern char *data_ov006_02140324;
extern int data_ov006_02140328;
extern int data_ov006_021403b8[];
extern int data_0209e650;
extern char data_ov006_0212d560[];
extern char data_ov006_0212c9f0[];

extern void func_ov006_020c6ca4(char *dst, char *src);
extern void func_ov006_020c4c00(char *c);
extern int RandomIntInternal(int *seed);

void func_ov006_020c44b4(void *arg0, int arg1)
{
    int count;
    int sl;
    int sb;
    int r8;
    int r4;
    int *seed;
    int r6;
    int sp0;
    int sp4;
    int sp8;
    int sp10;
    int spc;
    int r1;
    char *base;
    int t;

    count = data_ov006_02140328;
    data_ov006_0214031c = arg1;
    data_ov006_021402fc = arg0;
    data_ov006_02140304 = count;
    data_ov006_02140308 = 0;
    if (count <= 0)
        return;

    seed = &data_0209e650;
    sl = 0;
    r8 = sl;
    sp4 = sl;
    r4 = sl;
    spc = sl;
    sp10 = sl;
    sp8 = sl;

    do {
        if (sl < 3) {
            base = data_ov006_02140324 + r8;
            *(short *)(base + 0xea) = (short)sp8;
            func_ov006_020c6ca4(
                data_ov006_02140324 + r8,
                data_ov006_0212d560 + data_ov006_0214031c * 0x90 + sp4);
            func_ov006_020c4c00(data_ov006_02140324 + r8);
        } else {
            t = RandomIntInternal(seed) & 0x7fffffff;
            t = (int)((unsigned)t >> 0x13);
            t = t << 2;
            sb = t >> 0xc;
            t = RandomIntInternal(seed) & 0x7fffffff;
            t = (int)((unsigned)t >> 0x13);
            t = t << 2;
            sp0 = t >> 0xc;
            r1 = spc;
            r6 = (sl - 3) | (r1 - r1);
            if (r6 > 0) {
                do {
                    if (sb == data_ov006_021403b8[r1]) {
                        t = RandomIntInternal(seed) & 0x7fffffff;
                        t = (int)((unsigned)t >> 0x13);
                        t = t << 2;
                        sb = t >> 0xc;
                        r1 = r4;
                    }
                    r1 = r1 + 1;
                } while (r1 < r6);
            }
            data_ov006_021403b8[sl - 3] = sb;
            base = data_ov006_02140324 + r8;
            *(short *)(base + 0xea) = (short)sp10;
            func_ov006_020c6ca4(
                data_ov006_02140324 + r8,
                data_ov006_0212c9f0 + sb * 0xc0 + sp0 * 0x30);
            func_ov006_020c4c00(data_ov006_02140324 + r8);
        }
        sp4 = sp4 + 0x30;
        r8 = r8 + 0xf0;
        sl = sl + 1;
    } while (sl < data_ov006_02140328);
}
