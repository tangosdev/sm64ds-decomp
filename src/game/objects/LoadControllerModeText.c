#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern short data_0209d6d4;
extern unsigned char data_0209d660;
extern unsigned char data_0209d668;
extern unsigned char data_0209d6a8;
extern unsigned char data_0209d674;
extern unsigned char data_0209d6b0;

extern void func_0201eaac(void);
extern void func_0201d590(void);
extern int _ZN2G212GetBG2ScrPtrEv(void);

void LoadControllerModeText(int param0)
{
    int n, m, i;
    short *scr;

    data_0209d6d4 = (short)param0;
    data_0209d660 = 0;
    func_0201eaac();

    data_0209d668 = 1;
    data_0209d6a8 = 0;
    data_0209d674 = 0;
    func_0201d590();

    n = data_0209d6b0;
    m = (n + 7) / 8;
    {
        int base = _ZN2G212GetBG2ScrPtrEv();
        scr = (short*)(base + 0x40 + (16 - m / 2) * 2);
    }
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

    data_0209d6d4++;
    func_0201d590();
    n = data_0209d6b0;
    m = (n + 7) / 8;
    {
        int base = _ZN2G212GetBG2ScrPtrEv();
        scr = (short*)(base + 0x840 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209d674;
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

    data_0209d6d4++;
    func_0201d590();
    n = data_0209d6b0;
    m = (n + 7) / 8;
    {
        int base = _ZN2G212GetBG2ScrPtrEv();
        scr = (short*)(base + 0x1040 + (16 - m / 2) * 2);
    }
    i = 0;
    if (m > 0) {
        int mm = (n + 7) / 8;
        int d = data_0209d674;
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
}
