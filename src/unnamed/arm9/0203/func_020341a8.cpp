//cpp
#include "types.h"
extern "C" {
extern short data_0209fce8;
extern int data_0209fd08;
extern int *data_0209fcf8;
extern int data_0209fd14;
extern int data_0209fd00;
extern int data_0209fd0c;
extern u8 data_0209fc88;
extern u8 data_0209fc7c;
extern u8 data_0209fc78;
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern void MultiStore_Int(int val, int dst, int len);
extern void func_02031cd4(int a);
extern void func_020318a4(int a);

void func_020341a8(int a0, int a1) {
    volatile int li;
    int n, i, oldc;
    short *scr;
    int idx = data_0209fce8;
    char *base = (char*)data_0209fd08;
    data_0209fd00 = (int)(base + idx * 8);
    data_0209fd0c = data_0209fd14 + 0x28 + data_0209fcf8[1] + *(int*)(base + idx * 8);
    func_02031cd4(1);
    oldc = data_0209fc88;
    data_0209fc7c = (((a1 << 3) - oldc) & 0xf) >> 1;
    data_0209fc88 = oldc + data_0209fc7c;
    n = ((data_0209fc88 + 7) / 8) << 6;
    {
        int p = _ZN3G2S13GetBG0CharPtrEv() + 0x4000 + (data_0209fc78 << 5);
        li = 0;
        MultiStore_Int(li, p, n);
    }
    {
        int sp0 = _ZN3G2S12GetBG0ScrPtrEv() + a0 * 2;
        scr = (short*)(sp0 + ((unsigned short)((a1 - ((oldc + 7) / 8)) >> 1)) * 2);
    }
    for (i = 0; i < (data_0209fc88 + 7) / 8; i++) {
        scr[i] = data_0209fc78 + 0x200 + i;
        (&scr[i])[32] = data_0209fc78 + 0x200 + (data_0209fc88 + 7) / 8 + i;
    }
    func_020318a4(0);
    data_0209fc78 = data_0209fc78 + (u8)(((data_0209fc88 + 7) / 8) << 1);
}
}
