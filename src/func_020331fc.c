#pragma opt_strength_reduction off
extern int func_02054d88(void);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int val, int dst, int len);
extern void func_02031028(int a);
extern void func_020316d8(int a, int b, int c, int d);

extern unsigned char data_0209fc78;
extern unsigned char data_0209fc7c;
extern unsigned char data_0209fc84;
extern unsigned char data_0209fc88;
extern unsigned char data_0209fca0;
extern unsigned char data_02092818[];
extern unsigned char data_0208f174[];

void func_020331fc(int a0, int a1, int a2)
{
    volatile int li;
    short *scr;
    int base;
    int cnt;
    int acc;
    int i;
    int old;
    unsigned char *p;

    base = func_02054d88() + 0x8000;
    data_0209fc78 = 0;
    data_0209fc88 = 0;
    data_0209fc84 = a2;

    {
        int cp = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
        li = 0;
        MultiStore_Int(li, cp + (data_0209fc78 << 5), 0x800);
    }
    scr = (short *)(_ZN3G2S12GetBG0ScrPtrEv() + a0 * 2);
    func_02031028(1);

    old = data_0209fc88;
    data_0209fc7c = (((a1 << 3) - old) >> 1) & 7;
    acc = data_0209fc7c;
    cnt = (unsigned char)((a1 - (old + 7) / 8) >> 1);
    data_0209fc88 = old + acc;
    i = 0;
    if (i < data_0209fca0) {
        p = data_02092818;
        do {
            func_020316d8(0, *p, acc, base);
            acc = (acc + data_0208f174[*p]) & 0xff;
            p++;
            i++;
        } while (i < data_0209fca0);
    }

    scr += cnt;
    {
        int j = 0;
        int v2;
        int n = (data_0209fc88 + 7) / 8;
        if (n > 0) {
            v2 = n + 0x200;
            do {
                scr[j] = j + 0x200;
                *(short *)((char *)&scr[j] + 0x40) = v2;
                v2++;
                j++;
            } while (j < n);
        }
    }
}
