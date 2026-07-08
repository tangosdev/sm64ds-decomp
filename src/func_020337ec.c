extern unsigned char CONNECTION_ERROR;
extern unsigned char data_0209fc94;
extern unsigned char data_0209fc78;
extern short data_0209fce8;
extern unsigned char data_0209fcdc;
extern void func_02034504(void);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiStore_Int(int val, int *dst, int len);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void func_020341a8(int a, int b);
extern void func_02033464(int a, int b, int c);

void func_020337ec(void)
{
    volatile unsigned short v;
    volatile int z;
    CONNECTION_ERROR = 0;
    func_02034504();
    data_0209fc94 = 1;
    data_0209fc78 = 0;
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
    data_0209fce8 = 0x18;
    func_020341a8(0x280, 0x20);
    data_0209fcdc = data_0209fc78;
    func_02033464(0, 0, 0);
}
