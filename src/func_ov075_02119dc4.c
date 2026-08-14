//cpp
#include "types.h"

extern "C" {
extern u8 data_0209d454;
extern u8 data_0209d45c;
extern u8 data_0209fc58;
extern u8 data_0209fc5c[];
extern u8 data_0209fc5d[];
extern int data_0209fc68;
extern int data_ov075_0211d71c[];
extern int data_ov075_0211d930;

extern void func_ov075_02116028(int *p, int v);
extern void func_02030a78(void);
extern void func_020338b0(int a, int b, int c, char d);
extern int GetOwnerLanguage(void);
extern int func_0203d974(void);
extern int func_0203d9b4(void);
extern void func_ov075_0211a148(char *c, int a, int b);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(vu16 *p, u16 a, s16 b);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int layer);
extern void func_02012790(int id);
}

typedef struct { int a; int b; } Pair;
extern "C" Pair data_02086b58;
extern "C" Pair data_ov075_0211d920;

extern "C" void func_ov075_02119dc4(char *c, int arg)
{
    u8 *p;
    u8 bits;
    u8 flags;
    int i;
    int t;
    volatile Pair tmp;
    Pair *slot;
    Pair *ref;
    int a;
    int b;

    p = data_0209fc5d;
    data_0209d454 &= ~0x11;
    bits = 0;
    i = 1;
    t = i;
    do {
        u8 v = *p;
        if (v != 0)
            bits = (u8)(bits | (t << i));
        p = p + 1;
        i = i + 1;
    } while (i < 4);

    flags = data_0209fc58;
    t = (data_0209fc68 == 6);
    if (t != false) {
        func_ov075_02116028(data_ov075_0211d71c, 5);
        func_02030a78();
        if ((flags & 0x30) == 0) {
            slot = (Pair *)(int)(((long long)(int)(c + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
            ref = &data_ov075_0211d920;
            if (slot->a != ref->a)
                goto play_c5_13;
            if (slot->b != ref->b && ((Pair *)(c + 0x5c))->a != 0)
                goto play_c5_13;
        }
        if (GetOwnerLanguage() == 3)
            func_020338b0(0xa5, 0xb0, 0x15, -1);
        else
            func_020338b0(0xc5, 0xb0, 0x15, -1);
        goto after_sound;
    play_c5_13:
        func_020338b0(0xc5, 0xb0, 0x13, -1);
    after_sound:
        a = data_02086b58.a;
        b = data_02086b58.b;
        tmp.a = a;
        tmp.b = b;
        *(int *)(c + 0x5c) = a;
        *(int *)(c + 0x60) = b;
        data_0209d45c = data_0209d45c & ~*(u8 *)(c + 0x287);
        if (*(u8 *)(c + 0x287) != 0) {
            a = tmp.a;
            b = tmp.b;
            ((Pair *)(c + 0x64))->a = a;
            ((Pair *)(c + 0x64))->b = b;
        }
        _ZN3G2x18SetBlendBrightnessEPVtts((vu16 *)0x4000050, 0x1f, -6);
        *(vu16 *)0x4001050 = 0;
        _ZN5Sound22StopLoadedMusic_Layer1Ej(4);
        func_02012790(0x123);
        (*(u8 **)(c + 0x50))[0xf44] = 1;
    } else {
        func_ov075_02116028(data_ov075_0211d71c, 6);
        if ((func_0203d974() != 0 && data_0209fc5c[0] == 0) ||
            (func_0203d9b4() != 0 && bits == 0)) {
            if (func_0203d974() != 0)
                func_020338b0(0x125, 0xb0, 0x1b, 0);
            else
                func_020338b0(0x125, 0xb0, 0x1a, -1);
            func_02030a78();
            func_ov075_0211a148(c, (int)&data_ov075_0211d930, 0xb4);
            data_0209d45c = data_0209d45c & ~*(u8 *)(c + 0x287);
            if (*(u8 *)(c + 0x287) != 0) {
                a = data_02086b58.a;
                b = data_02086b58.b;
                *(int *)(c + 0x64) = b ? a : a;
                *(int *)(c + 0x68) = b;
            }
            _ZN3G2x18SetBlendBrightnessEPVtts((vu16 *)0x4000050, 0x1f, -6);
            func_02012790(0x12a);
        } else {
            i = 0;
            do {
                int bit = 1 << i;
                if ((flags & bit) != 0) {
                    func_020338b0(0x125, 0xb0, 0x1c, (s8)(u8)i);
                    data_0209fc58 = data_0209fc58 & ~bit;
                    func_ov075_0211a148(c, arg, 0x78);
                    func_02012790(0x12a);
                    break;
                }
                i = i + 1;
            } while (i < 4);
        }
    }
    data_0209d454 |= 5;
    c[0x280] = 0;
}
