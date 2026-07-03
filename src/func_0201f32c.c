typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;

extern void *data_0209d70c;
extern s16 data_0209d6d4;
extern u8 data_0209d660;
extern u8 data_0209d658;
extern u8 data_0209d64c;
extern u8 data_0209d6c4;
extern u8 data_0209f2d8;
extern void *data_0209d708;
extern void *data_0209d6f0;
extern s32 data_0209d6f4;
extern s32 data_0209d6fc;
extern u8 data_0209d6a8;
extern u32 data_0209caa0[3];
extern s32 data_0209fc48;
extern u8 data_0209d650;
extern u8 data_0209d6cc;
extern u8 data_0209d6d0;
extern u8 data_0209d6c8;
extern u8 data_0209d698;
extern u8 data_0209d680;
extern u8 data_0209d65c;
extern u8 data_0209d6bc;
extern u8 data_0209d6ac;
extern u8 data_0209d45c;
extern u8 data_0209d454;

extern void func_0201eaac(void);
extern void func_02012790(int a);
extern int func_0201fb4c(void);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32 a, s32 b);
extern void *func_02054ea8(void);
extern void MultiStore_Int(int val, void *dst, int len);
extern void *_ZN2G212GetBG1ScrPtrEv(void);
extern void MultiStore16(u16 val, void *dst, int nbytes);
extern void SetBg1Offset(int a, int b);
extern u32 func_02054e88(void);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern void SetSubBg1Offset(int a, int b);
extern int func_02054d88(void);
extern void *func_02054fb0(void);
extern void SetBg3Offset(int a, int b);
extern void _ZN7Message6UpdateEv(void);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile u16 *p, int a, int b);

void func_0201f32c(int arg0)
{
    char *entry;
    u8 disp;
    u8 f2d8_val;
    u8 disp2;
    u8 t;
    void *var_r8;
    s32 var_r7;
    s32 last_rem;
    u8 divisor;
    s32 i;
    volatile u16 sp0;
    volatile u16 sp2;
    volatile u16 sp4;
    volatile s32 sp8;
    volatile s32 spC;
    volatile s32 sp10;

    if (*(u16 *)((char *)data_0209d70c + 8) <= (u16)arg0) {
        return;
    }

    data_0209d6d4 = arg0;
    func_0201eaac();

    data_0209d660 = 1;
    data_0209d658 = 0x7f;
    data_0209d64c = 0x87;
    if (data_0209d6d4 >= 0x2a && data_0209d6d4 < 0x83) {
        data_0209d6c4 = 1;
        func_02012790(0x20);
    } else {
        u32 b0;
        data_0209d6c4 = 0;
        b0 = (data_0209f2d8 == 2);
        if (b0 == 0) {
            func_02012790(4);
        }
    }

    entry = (char *)data_0209d708 + data_0209d6d4 * 8;
    data_0209d6f0 = entry;
    {
        s32 ev = *(s32 *)entry;
        s32 sum = data_0209d6fc + 0x28 + *(s32 *)((char *)data_0209d70c + 4);
        data_0209d6f4 = sum + ev;
    }
    data_0209d6a8 = (u8)((*(u16 *)(entry + 4) + 7) / 8);

    f2d8_val = data_0209f2d8;
    {
        u32 b1 = (f2d8_val == 0);
        if (b1 != 0 && !(data_0209caa0[2] & 0x80)) {
            u32 b2 = (data_0209fc48 != 0);
            if (b2 == 0) {
                data_0209d650 = 0x10;
                data_0209d6cc = 0x64;
                data_0209d6d0 = (u8)((((data_0209d6a8 + 2) / 2) * 8) + 0xf);
                data_0209d6c8 = (u8)((((*(u8 *)(entry + 6) * 2 + 1) * 8) / 2) + 0x63);
                data_0209d658 = data_0209d6d0;
                data_0209d64c = data_0209d6c8;
                goto branchDone;
            }
        }
        {
            u32 b3 = (f2d8_val == 2);
            if (b3 != 0) {
                data_0209d650 = (u8)(((0x20 - (data_0209d6a8 + 2)) / 2) * 8);
                data_0209d6d0 = 0x7f;
                data_0209d6cc = 0xa0;
                data_0209d6c8 = (u8)((((*(u8 *)(entry + 6)) * 16) / 2) + 0x9f);
                data_0209d658 = 0x7f;
                data_0209d64c = data_0209d6c8;
            } else {
                data_0209d650 = (u8)(((0x20 - (data_0209d6a8 + 2)) / 2) * 8);
                data_0209d6d0 = 0x7f;
                data_0209d6c8 = 0x63;
                data_0209d6cc = (u8)(0x64 - (((*(u8 *)(entry + 6) * 2 + 2) * 8) / 2));
                data_0209d658 = 0x7f;
                data_0209d64c = 0x63;
                if (func_0201fb4c() != 0) {
                    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x40, 0xc999);
                }
            }
        }
    branchDone: ;
    }

    data_0209d680 = 0;
    data_0209d65c = 0;
    if (data_0209d698 == 2) {
        {
            void *pa = (char *)func_02054ea8() + 0x4000;
            sp8 = 0;
            MultiStore_Int(sp8, pa, 0x4000);
        }
        {
            void *pb = (char *)_ZN2G212GetBG1ScrPtrEv() + 0x200;
            sp0 = 0x3ff;
            MultiStore16(sp0, pb, 0x400);
        }
        var_r8 = (char *)_ZN2G212GetBG1ScrPtrEv() + 0x24e;
        SetBg1Offset(0, 0);
        var_r7 = 0x200;
        data_0209d660 = 0;
    } else if (data_0209d698 == 1) {
        {
            void *pa = (char *)func_02054e88() + 0x4000;
            spC = 0;
            MultiStore_Int(spC, pa, 0x2000);
        }
        {
            void *pb = _ZN3G2S12GetBG1ScrPtrEv();
            sp2 = 0x22ff;
            MultiStore16(sp2, pb, 0x800);
        }
        var_r8 = (char *)_ZN3G2S12GetBG1ScrPtrEv() + 0x42;
        SetSubBg1Offset(4 - data_0209d650, 4 - data_0209d6cc);
        var_r7 = 0x2200;
    } else {
        {
            void *pa = (char *)func_02054d88() + 0x4000;
            sp10 = 0;
            MultiStore_Int(sp10, pa, 0x3000);
        }
        {
            void *pb = func_02054fb0();
            sp4 = 0x37f;
            MultiStore16(sp4, pb, 0x800);
        }
        var_r8 = (char *)func_02054fb0() + 0x42;
        {
            u32 b4 = (data_0209f2d8 == 2);
            if (b4 != 0) {
                SetBg3Offset(0 - data_0209d650, 8 - data_0209d6cc);
            } else {
                SetBg3Offset(0 - data_0209d650, 4 - data_0209d6cc);
            }
        }
        var_r7 = 0x200;
    }

    {
        divisor = data_0209d6a8;
        i = 0;
        if ((s32)(divisor * (*(u8 *)((char *)data_0209d6f0 + 6)) * 2) > 0) {
            do {
                s32 rem = i % divisor;
                *(u16 *)((char *)var_r8 + rem * 2) = (u16)var_r7;
                if (divisor - 1 == rem) {
                    var_r8 = (char *)var_r8 + 0x40;
                }
                i++;
                var_r7++;
            } while (i < (s32)(divisor * (*(u8 *)((char *)data_0209d6f0 + 6)) * 2));
        }
    }

    t = data_0209d6bc;
    if (t < 4) {
        do {
            _ZN7Message6UpdateEv();
            t = data_0209d6bc;
        } while (t < 4);
    }
    data_0209d6ac = t;

    disp2 = data_0209d698;
    if (disp2 == 2) {
        data_0209d45c |= 2;
        *(volatile s32 *)0x04000000 = (*(volatile s32 *)0x04000000 & ~0x1f00) | (data_0209d45c << 8);
        return;
    }
    if (disp2 == 1) {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x04001050, data_0209d454 | 0x20, -7);
        *(volatile u16 *)0x04001048 = (*(volatile u16 *)0x04001048 & ~0x3f) | (data_0209d454 | 2) | 0x20;
        *(volatile u16 *)0x0400104a = (*(volatile u16 *)0x0400104a & ~0x3f) | 0x1d;
        *(volatile s16 *)0x04001040 = ((data_0209d658 << 8) & 0xff00) | (u8)(((data_0209d6d0 * 2) - data_0209d658) + 1);
        *(volatile s16 *)0x04001044 = ((data_0209d64c << 8) & 0xff00) | (u8)(((data_0209d6c8 * 2) - data_0209d64c) + 1);
        *(volatile s32 *)0x04001000 = (*(volatile s32 *)0x04001000 & ~0xe000) | 0x2000;
        data_0209d454 |= 2;
        *(volatile s32 *)0x04001000 = (*(volatile s32 *)0x04001000 & ~0x1f00) | (data_0209d454 << 8);
        return;
    }

    if (data_0209d6d4 >= 0x2a && data_0209d6d4 < 0x83) {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x04000050, data_0209d45c & ~0x38, 7);
    } else {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile u16 *)0x04000050, data_0209d45c & ~0x38, -8);
    }
    *(volatile u16 *)0x04000048 = (*(volatile u16 *)0x04000048 & ~0x3f) | (data_0209d45c | 8) | 0x20;
    *(volatile u16 *)0x0400004a = (*(volatile u16 *)0x0400004a & ~0x3f) | 0x17;
    *(volatile s16 *)0x04000040 = ((data_0209d658 << 8) & 0xff00) | (u8)(((data_0209d6d0 * 2) - data_0209d658) + 1);
    *(volatile s16 *)0x04000044 = ((data_0209d64c << 8) & 0xff00) | (u8)(((data_0209d6c8 * 2) - data_0209d64c) + 1);
    data_0209d6bc = 0;
    *(volatile s32 *)0x04000000 = (*(volatile s32 *)0x04000000 & ~0xe000) | 0x2000;
    data_0209d45c |= 8;
    *(volatile s32 *)0x04000000 = (*(volatile s32 *)0x04000000 & ~0x1f00) | (data_0209d45c << 8);
}
