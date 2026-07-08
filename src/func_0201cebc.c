    extern short CURR_MSG_ID;
    extern unsigned char PLAYER_TALKING;
    extern unsigned char data_0209d668;
    extern unsigned char MSG_LINE_HEIGHT;
    extern unsigned char data_0209d674;
    extern void func_0201eaac(void);
    extern int _ZN3G2S13GetBG0CharPtrEv(void);
    extern void MultiStore_Int(int val, int *dst, int len);
    extern int _ZN3G2S12GetBG0ScrPtrEv(void);
    extern void MultiStore16(unsigned short val, char *dst, int nbytes);
    extern void func_0201d6a0(int a, int b);

    void func_0201cebc(short a0)
    {
        volatile unsigned short v;
        volatile int z;
        CURR_MSG_ID = a0;
        PLAYER_TALKING = 0;
        func_0201eaac();
        data_0209d668 = 1;
        MSG_LINE_HEIGHT = 0;
        data_0209d674 = 0;
        {
            int *dst = (int *)(_ZN3G2S13GetBG0CharPtrEv() + 0x4000);
            z = 0;
            MultiStore_Int(z, dst, 0x2000);
        }
        {
            char *dst = (char *)_ZN3G2S12GetBG0ScrPtrEv();
            v = 0x2ff;
            MultiStore16(v, dst, 0x800);
        }
        func_0201d6a0(0x100, 0x20);
        CURR_MSG_ID = 0x27e;
        func_0201d6a0(0x1c0, 0x20);
    }
