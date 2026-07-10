#pragma opt_strength_reduction off
extern short data_0209fce8;
extern int data_0209fd08;
extern int *data_0209fcf8;
extern int data_0209fd14;
extern int data_0209fd00;
extern int data_0209fd0c;
extern unsigned char data_0209fcc8;
extern unsigned char data_0209fc78;
extern unsigned char data_0209fc7c;
extern unsigned char data_0209fc88;
extern unsigned char data_0209fcb0;

extern void func_02031cd4(void *arg);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiStore_Int(int val, int *dst, int len);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void func_02031e00(void);
extern void func_020319fc(int n);

void func_02033e50(int p0, int p1)
{
    int i;
    int idx = data_0209fce8;
    char *base = (char*)data_0209fd08;
    int n = (p1 >> 3) & 0xff;

    data_0209fd00 = (int)(base + idx * 8);
    data_0209fd0c = data_0209fd14 + 0x28 + data_0209fcf8[1] + *(int*)(base + idx * 8);
    data_0209fcc8 = 0;

    do {
        func_02031cd4(0);
        idx = data_0209fce8;
        if (idx == 0x13 || idx == 0x15) {
            volatile int li;
            data_0209fc7c = 0xc;
            n = ((data_0209fc88 + 0x13) >> 3) & 0xff;
            {
                int cp = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
                li = 0;
                MultiStore_Int(li, (int*)(cp + (data_0209fc78 << 6)), n << 6);
            }
            {
                short *scr = (short*)(_ZN3G2S12GetBG0ScrPtrEv() + p0 * 2);
                i = 0;
                if (n > 0) {
                    int v1 = (data_0209fc78 << 1) + 0x200;
                    int v2 = n + v1;
                    do {
                        scr[i] = v1;
                        *(short*)((char*)&scr[i] + 0x40) = v2;
                        v1++;
                        v2++;
                        i++;
                    } while (i < n);
                }
            }
            func_02031e00();
            func_020319fc(n);
            data_0209fcb0 = 1;
            data_0209fc78 = data_0209fc78 + n;
            p0 = (unsigned short)(p0 + 0x40);
        } else {
            volatile int li;
            data_0209fc7c = (unsigned char)((p1 - data_0209fc88) >> 1);
            {
                int cp = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
                li = 0;
                MultiStore_Int(li, (int*)(cp + (data_0209fc78 << 6)), n << 6);
            }
            {
                int v1, v2;
                short *scr = (short*)(_ZN3G2S12GetBG0ScrPtrEv() + p0 * 2);
                i = 0;
                if (n > 0) {
                    v1 = (data_0209fc78 << 1) + 0x200;
                    v2 = n + v1;
                    do {
                        scr[i] = v1;
                        *(short*)((char*)&scr[i] + 0x40) = v2;
                        v1++;
                        v2++;
                        i++;
                    } while (i < n);
                }
            }
            func_02031e00();
            func_020319fc(n);
            data_0209fcb0 = 1;
            data_0209fc78 = data_0209fc78 + n;
            p0 = (unsigned short)(p0 + 0x40);
        }
    } while (data_0209fcc8 != 8);
    data_0209fcc8 = 0;
}
