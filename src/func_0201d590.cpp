//cpp
extern "C" {
typedef unsigned char u8;
extern short data_0209d6d4;
extern int data_0209d708;
extern int *data_0209d70c;
extern int data_0209d6fc;
extern int data_0209d6f0;
extern int data_0209d6f4;
extern u8 data_0209d6b0;
extern u8 data_0209d65c;
extern u8 data_0209d674;
extern int _ZN2G213GetBG2CharPtrEv(void);
extern void MultiStore_Int(int val, int dst, int len);
extern void func_0201b6f8(int a);
extern void func_0201b100(int a);

void func_0201d590(void) {
    volatile int li;
    int n;
    int idx = data_0209d6d4;
    char *base = (char*)data_0209d708;
    data_0209d6f0 = (int)(base + idx * 8);
    data_0209d6f4 = data_0209d6fc + 0x28 + data_0209d70c[1] + *(int*)(base + idx * 8);
    func_0201b6f8(1);
    data_0209d65c = 0;
    n = ((data_0209d6b0 + 7) / 8) << 6;
    {
        int p = _ZN2G213GetBG2CharPtrEv() + ((data_0209d674 + 0x280) << 5);
        li = 0;
        MultiStore_Int(li, p, n);
    }
    func_0201b100(1);
    data_0209d674 = data_0209d674 + (u8)(((data_0209d6b0 + 7) / 8) << 1);
}
}
