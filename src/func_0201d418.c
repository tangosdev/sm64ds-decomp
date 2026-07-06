#pragma opt_strength_reduction off
extern short data_0209d6d4;
extern int data_0209d708;
extern int *data_0209d70c;
extern int data_0209d6fc;
extern int data_0209d6f0;
extern int data_0209d6f4;
extern unsigned char data_0209d6bc;
extern unsigned char data_0209d6a8;
extern unsigned char data_0209d6b0;
extern unsigned char data_0209d65c;
extern unsigned char data_0209d6c0;

extern void func_0201b6f8(int mode);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiStore_Int(int val, int dst, int len);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void func_0201b7cc(void);
extern void func_0201b388(int a);

void func_0201d418(int p0, int p1) {
    volatile int li;
    int i;
    int idx = data_0209d6d4;
    char *base = (char*)data_0209d708;
    int n = (p1 >> 3) & 0xff;
    data_0209d6f0 = (int)(base + idx * 8);
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] + *(int*)(base + idx * 8);
    data_0209d6bc = 0;
    do {
        func_0201b6f8(0);
        data_0209d65c = (p1 - data_0209d6b0) >> 1;
        {
            int cp = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
            li = 0;
            MultiStore_Int(li, cp + (data_0209d6a8 << 6), n << 6);
        }
        {
            short *scr = (short*)(_ZN3G2S12GetBG0ScrPtrEv() + p0 * 2);
            i = 0;
            if (n > 0) {
                int v1 = (data_0209d6a8 << 1) + 0x200;
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
        func_0201b7cc();
        func_0201b388(n);
        data_0209d6c0 = 1;
        data_0209d6a8 = data_0209d6a8 + n;
        p0 = (unsigned short)(p0 + 0x40);
    } while (data_0209d6bc != 8);
    data_0209d6bc = 0;
}
