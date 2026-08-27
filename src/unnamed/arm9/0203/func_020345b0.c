#include "types.h"
extern void *data_0209fcf8;
extern s16 data_0209fce8;
extern void *data_0209fd08;
extern s32 data_0209fd14;
extern void *data_0209fd00;
extern s32 data_0209fd0c;
extern u8 data_0209fc9c;
extern u8 data_0209fc8c;
extern u8 data_0209fc74;
extern u8 data_0209fcd4;
extern u8 data_0209fce4;
extern u8 data_0209fc78;
extern u8 data_0209fc6c;
extern u8 data_0209fcc0;
extern u8 data_0209fce0;
extern u8 data_0209fcac;
extern u8 data_0209fc7c;
extern u8 data_0209fcc8;
extern u8 data_0209fcb4;
extern u32 data_0209b454;
extern u8 data_0209d45c;

extern void func_02034504(void);
extern int func_02054d88(void);
extern void MultiStore_Int(int val, void *dst, int len);
extern int _ZN2G212GetBG3ScrPtrEv(void);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern void SetBg3Offset(int a, int b);
extern void func_020326ac(void);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16 *p, int a, int b);

void func_020345b0(int arg0)
{
    void *scr;
    s32 i;
    s32 cols;
    s32 last;
    u8 t;
    volatile s32 li;
    volatile u16 ls;

    if (*(u16 *)((char *)data_0209fcf8 + 8) <= (u16)arg0) {
        return;
    }

    data_0209fce8 = (s16)arg0;
    func_02034504();

    data_0209fc9c = 1;
    data_0209fc8c = 0x7f;
    data_0209fc74 = 0x87;
    data_0209fcd4 = 0;
    data_0209fd00 = (char *)data_0209fd08 + data_0209fce8 * 8;
    data_0209fd0c = data_0209fd14 + 0x28 + *(s32 *)((char *)data_0209fcf8 + 4) +
                    *(s32 *)((char *)data_0209fd08 + data_0209fce8 * 8);
    data_0209fc78 = (u8)((*(u16 *)((char *)data_0209fd00 + 4) + 7) / 8);
    data_0209fce4 = 0x7f;
    data_0209fc6c = (u8)(((0x20 - (data_0209fc78 + 2)) / 2) * 8);
    data_0209fcc0 = 0x63;
    data_0209fce0 = (u8)(0x64 - (((*(u8 *)((char *)data_0209fd00 + 6) * 2 + 1) * 8) / 2));
    data_0209fc8c = 0x7f;
    data_0209fc74 = 0x63;
    data_0209fcac = 0;
    data_0209fc7c = 0;
    data_0209b454 |= 0x800000;


    {
        void *p = (char *)func_02054d88() + 0x4000;
        li = 0;
        MultiStore_Int(li, p, 0x3000);
    }

    {
        void *s = (void *)_ZN2G212GetBG3ScrPtrEv();
        ls = 0x37f;
        MultiStore16(ls, s, 0x800);
    }

    scr = (char *)_ZN2G212GetBG3ScrPtrEv() + 0x42;
    SetBg3Offset(-(int)data_0209fc6c, 4 - (int)data_0209fce0);

    i = 0;
    cols = data_0209fc78;
    if ((s32)(cols * (*(u8 *)((char *)data_0209fd00 + 6)) * 2) > 0) {
        last = cols - 1;
        do {
            s32 rem = i % cols;
            *(u16 *)((char *)scr + rem * 2) = (u16)(i + 0x200);
            if (last == rem) {
                scr = (char *)scr + 0x40;
            }
            i++;
        } while (i < (s32)(cols * (*(u8 *)((char *)data_0209fd00 + 6)) * 2));
    }


    t = data_0209fcc8;
    if (t < 4) {
        do {
            func_020326ac();
            t = data_0209fcc8;
        } while (t < 4);
    }
    data_0209fcb4 = t;

    _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x4000050, (data_0209d45c ^ 0x10) | 0x20, -8);

    *(volatile u16 *)0x4000048 = (*(volatile u16 *)0x4000048 & ~0x3f) | (data_0209d45c | 8) | 0x20;
    *(volatile u16 *)0x400004a = (*(volatile u16 *)0x400004a & ~0x3f) | 0x17;
    *(volatile s16 *)0x4000040 = ((data_0209fc8c << 8) & 0xff00) | (u8)(((data_0209fce4 * 2) - data_0209fc8c) + 1);
    *(volatile s16 *)0x4000044 = ((data_0209fc74 << 8) & 0xff00) | (u8)(((data_0209fcc0 * 2) - data_0209fc74) + 1);

    data_0209fcc8 = 0;
    *(volatile s32 *)0x4000000 = (*(volatile s32 *)0x4000000 & ~0xe000) | 0x2000;
    data_0209d45c |= 8;
    *(volatile s32 *)0x4000000 = (*(volatile s32 *)0x4000000 & ~0x1f00) | (data_0209d45c << 8);
}
