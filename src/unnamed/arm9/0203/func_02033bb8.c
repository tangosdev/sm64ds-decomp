#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern short data_0209fce8;
extern unsigned char data_0209fc9c;
extern unsigned char data_0209fc94;
extern unsigned char data_0209fc78;
extern unsigned char data_0209fcdc;
extern unsigned char data_0209fc88;

extern void func_02034504(void);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiStore_Int(int val, int *dst, int len);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void func_02034098(void);
extern int _ZN2G212GetBG2ScrPtrEv(void);
extern void func_02033e50(int a, int b);
extern void func_020341a8(int a, int b);

void func_02033bb8(int param0)
{
    volatile int li;
    volatile unsigned short sv;
    int n, m, i;
    short *scr;

    data_0209fce8 = (short)param0;
    data_0209fc9c = 0;
    func_02034504();

    data_0209fc94 = 1;
    data_0209fc78 = 0;
    data_0209fcdc = 0;

    {
        int cp = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
        li = 0;
        MultiStore_Int(li, (int*)cp, 0x2000);
    }

    {
        int sp2 = _ZN3G2S12GetBG0ScrPtrEv();
        sv = 0x2ff;
        MultiStore16(sv, (char*)sp2, 0x800);
    }

    func_02034098();
    n = data_0209fc88;
    m = (n + 7) / 8;
    scr = (short*)(_ZN2G212GetBG2ScrPtrEv() + 0x40 + (16 - m / 2) * 2);
    i = 0;
    if (m > 0) {
        int v2 = m + 0x280;
        do {
            scr[i] = (short)(i + 0x280);
            *(short*)((char*)&scr[i] + 0x40) = (short)v2;
            v2++;
            i++;
        } while (i < m);
    }
    func_02033e50(0xa9, 0x70);

    data_0209fce8++;
    func_02034098();
    n = data_0209fc88;
    m = (n + 7) / 8;
    {
        int base = _ZN2G212GetBG2ScrPtrEv();
        scr = (short*)(base + 0x840 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209fcdc;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = (short)v1;
            *(short*)((char*)&scr[i] + 0x40) = (short)v2;
            v1++;
            v2++;
            i++;
        } while (i < mm);
    }
    func_02033e50(0x149, 0x70);

    data_0209fce8++;
    func_02034098();
    n = data_0209fc88;
    m = (n + 7) / 8;
    {
        int base = _ZN2G212GetBG2ScrPtrEv();
        scr = (short*)(base + 0x1040 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209fcdc;
        int v1 = d - mm * 2 + 0x280;
        int v2 = d - mm + 0x280;
        do {
            scr[i] = (short)v1;
            *(short*)((char*)&scr[i] + 0x40) = (short)v2;
            v1++;
            v2++;
            i++;
        } while (i < mm);
    }
    func_02033e50(0x1e9, 0x70);

    data_0209fc78 = (unsigned char)(data_0209fc78 << 1);
    data_0209fce8++;
    func_020341a8(0x280, 0x20);
}
