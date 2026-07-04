//cpp
extern "C" {
typedef unsigned char u8;
extern short data_0209fce8;
extern int data_0209fd08;
extern int *data_0209fcf8;
extern int data_0209fd14;
extern int data_0209fd00;
extern int data_0209fd0c;
extern u8 data_0209fc88;
extern u8 data_0209fc7c;
extern u8 data_0209fcdc;
extern int _ZN2G213GetBG2CharPtrEv(void);
extern void MultiStore_Int(int val, int dst, int len);
extern void func_02031cd4(int a);
extern void func_020318a4(int a);

void func_02034098(void) {
    volatile int li;
    int n;
    int idx = data_0209fce8;
    char *base = (char*)data_0209fd08;
    data_0209fd00 = (int)(base + idx * 8);
    data_0209fd0c = data_0209fd14 + 0x28 + data_0209fcf8[1] + *(int*)(base + idx * 8);
    func_02031cd4(1);
    data_0209fc7c = 0;
    n = ((data_0209fc88 + 7) / 8) << 6;
    {
        int p = _ZN2G213GetBG2CharPtrEv() + ((data_0209fcdc + 0x280) << 5);
        li = 0;
        MultiStore_Int(li, p, n);
    }
    func_020318a4(1);
    data_0209fcdc = data_0209fcdc + (u8)(((data_0209fc88 + 7) / 8) << 1);
}
}
