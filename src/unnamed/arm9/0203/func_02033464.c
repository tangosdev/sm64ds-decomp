#include "types.h"
extern int data_0209fd1c;
extern u8 data_0209fcb0;
extern u8 data_0209fc9c;
extern u8 data_0209fc88;
extern u8 data_0209fcd8;
extern u8 data_0209fc84;
extern int data_0209fd5c;
extern u8 data_0209fc94;
extern u8 data_0209fc7c;
extern u8 data_0209fccc;
extern u8 data_0209fca0;
extern u8 data_02092818;
extern u8 data_0209fcdc;
extern volatile u8 data_0209fc78;

extern void MultiStore_Int(int val, int *dst, int len);
extern int _ZN3G2S12GetBG0ScrPtrEv(void);
extern int _ZN3G2S13GetBG0CharPtrEv(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void func_02031028(int a);
extern void func_02031b84(unsigned char c);
extern void func_020319fc(int a);
extern int _ZN2G212GetBG3ScrPtrEv(void);
extern int func_02054d88(void);

void func_02033464(int arg0, int arg1, int arg2)
{
    volatile unsigned short sv0;
    volatile unsigned short sv2;
    volatile int li4;
    volatile int li8;
    volatile int liC;
    u32 t;
    char *d;
    volatile u16 *base;
    u32 x;
    int i;
    int n;
    u8 *p;
    int v1;

    li4 = 0;
    data_0209fd1c = 0;
    data_0209fcb0 = 1;
    data_0209fc9c = 0;
    data_0209fc88 = 0;
    data_0209fcd8 = 0;
    data_0209fc84 = arg1;

    MultiStore_Int(li4, &data_0209fd5c, 0xf00);

    if (arg2 == 0) {
        x = (arg0 * 2 + 8) << 0x15;
        data_0209fc94 = 1;
        t = x >> 16;
        d = (char*)(_ZN3G2S12GetBG0ScrPtrEv() + (((x >> 16) & 0xffe0) << 1));
        sv0 = 0x2ff;
        MultiStore16(sv0, d, 0x80);
        if (arg1 < 0)
            return;
        func_02031028(0);
        data_0209fc7c = (u8)((0x100 - data_0209fc88) >> 1);
        n = data_0209fca0;
        data_0209fccc = data_0209fc7c & 7;
        data_0209fc88 = 0;
        data_0209fcd8 = 0;
        i = 0;
        if (n > 0) {
            p = &data_02092818;
            do {
                func_02031b84(*p);
                i++;
                p++;
            } while (i < data_0209fca0);
        }
        data_0209fc78 = (u8)(arg0 * 0x18 + data_0209fcdc);
        {
            int cp = _ZN3G2S13GetBG0CharPtrEv() + 0x4000;
            int *dst = (int*)(cp + (data_0209fc78 << 5));
            li8 = 0;
            MultiStore_Int(li8, dst, 0x300);
        }
        base = (volatile u16*)(_ZN3G2S12GetBG0ScrPtrEv() + ((t + (data_0209fc7c >> 3)) << 1));
        { u32 ch = data_0209fc78;
        x = ch + 0x200;
        v1 = ch + 0x20c; }
        i = 0;
        do {
            base[i] = (u16)x;
            (base + i)[0x20] = (u16)v1;
            i++;
            x++;
            v1++;
        } while (i < 0xc);
        data_0209fc7c &= 7;
        data_0209fc78 = (u8)(data_0209fc78 >> 1);
        data_0209fcb0 = 1;
        func_020319fc(0xc);
    } else {
        data_0209fc94 = 0;
        d = (char*)(_ZN2G212GetBG3ScrPtrEv() + (arg0 << 1));
        sv2 = 0x2ff;
        MultiStore16(sv2, d, 0x80);
        func_02031028(0);
        n = data_0209fca0;
        data_0209fc7c = 4;
        data_0209fccc = 4;
        data_0209fc88 = 0;
        data_0209fcd8 = 0;
        i = 0;
        if (n > 0) {
            p = &data_02092818;
            do {
                func_02031b84(*p);
                i++;
                p++;
            } while (i < data_0209fca0);
        }
        data_0209fc78 = (u8)(arg1 * 0x18 + data_0209fcdc);
        {
            int cp = func_02054d88() + 0x4000;
            int *dst = (int*)(cp + (data_0209fc78 << 5));
            liC = 0;
            MultiStore_Int(liC, dst, 0x300);
        }
        base = (volatile u16*)(_ZN2G212GetBG3ScrPtrEv() + ((arg0 + (data_0209fc7c >> 3)) << 1));
        { u32 ch = data_0209fc78;
        x = ch + 0x200;
        v1 = ch + 0x20c; }
        i = 0;
        do {
            base[i] = (u16)x;
            (base + i)[0x20] = (u16)v1;
            i++;
            x++;
            v1++;
        } while (i < 0xc);
        data_0209fc7c &= 7;
        data_0209fc78 = (u8)(data_0209fc78 >> 1);
        data_0209fcb0 = 1;
        func_020319fc(0xc);
    }
}
