#pragma opt_strength_reduction off
#include "common.h"

extern int func_0203da9c(void);
extern int TouchArea_Update(void *p, int x);
extern int func_ov075_02116d9c(void *self, int id);
extern void func_ov001_020ab41c(void *p);
extern void func_ov075_02116c38(void *self);
extern void func_02012790(int a);
extern int GetSoundMode(void);
extern void SetSoundMode(int mode);
extern void func_ov001_020ab3f0(void *p);
extern int func_02033ae0(int a);
extern void TurnBacklightOn(void);
extern void TurnBacklightOff(void);
extern u16 *_ZN3G2S12GetBG2ScrPtrEv(void);
extern u16 data_020a0e5a[];

void func_ov075_0211705c(void *thiz)
{
    unsigned char *s = (unsigned char *)thiz;
    int w = func_0203da9c();
    int dir;
    u16 flags;

    if (TouchArea_Update(s + 0x124, -1) != 0
        || TouchArea_Update(s + 0x190, -1) != 0
        || ((flags = data_020a0e5a[w << 1]) & 8) != 0
        || (flags & 4) != 0
        || (func_ov075_02116d9c(s, -1) != 0 && s[0x281] == 2)) {
        func_ov001_020ab41c(s + 0x124);
        s[0x281] = 2;
        func_ov075_02116c38(s);
        func_02012790(0x11b);
    } else {
        dir = 0;
        if (s[0xc9] != 0) {
            if (TouchArea_Update(s + 0xb8, -1) != 0
                || TouchArea_Update(s + 0x94, -1) != 0
                || TouchArea_Update(s + 0x148, -1) != 0) {
                s[0x281] = 0;
                dir = 1;
            } else if (TouchArea_Update(s + 0x70, -1) != 0) {
                s[0x281] = dir;
                dir = -1;
            } else if (func_ov075_02116d9c(s, -1) != 0) {
                if (s[0x281] == 0)
                    dir = 1;
            }
            if (dir != 0 && s[0x281] == 0) {
                int mode = GetSoundMode();
                unsigned newmode;
                if (dir > 0) {
                    if (mode == 2)
                        newmode = 0;
                    else
                        newmode = (mode + 1) & 0xff;
                    s[0xa4] = 0;
                    func_ov001_020ab3f0(s + 0x94);
                } else {
                    if (mode == 0)
                        newmode = 2;
                    else
                        newmode = (mode - 1) & 0xff;
                    s[0x80] = 0;
                    func_ov001_020ab3f0(s + 0x70);
                }
                SetSoundMode(newmode);
                func_02033ae0((s16)(GetSoundMode() + 9));
                func_ov001_020ab41c(s + 0xb8);
                func_02012790(100);
            }
        }
        if (s[0xed] != 0 && s[0x111] != 0) {
            if ((func_ov075_02116d9c(s, -1) != 0 && s[0x281] == 1)
                || TouchArea_Update(s + 0x16c, -1) != 0) {
                dir = 1;
                s[0x281] = 1;
                if (s[0xec] == 0)
                    dir = -1;
            }
            if (TouchArea_Update(s + 0xdc, -1) != 0
                || (dir < 0 && s[0x281] == 1)) {
                TurnBacklightOn();
                func_ov001_020ab41c(s + 0xdc);
                s[0x110] = 0;
                s[0x281] = 1;
                func_02012790(0x66);
            } else if (TouchArea_Update(s + 0x100, -1) != 0
                       || (dir > 0 && s[0x281] == 1)) {
                TurnBacklightOff();
                s[0xec] = 0;
                func_ov001_020ab41c(s + 0x100);
                s[0x281] = 1;
                func_02012790(0x66);
            }
        }
    }

    {
        int i;
        int zero;
        u16 palOff;
        u16 palOn;
        u16 mask;
        i = 0;
        dir = i;
        zero = i;
        palOff = 0x3000;
        palOn = 0x4000;
        mask = 0xfff;
        do {
            u16 *p = (u16 *)((char *)_ZN3G2S12GetBG2ScrPtrEv() + 0x184);
            p += dir;
            u16 pal = (i == s[0x281]) ? palOn : palOff;
            int j = zero;
            do {
                unsigned v = *p;
                j++;
                v = v & mask;
                *p = v | pal;
                v = p[0x20];
                v = v & mask;
                p[0x20] = v | pal;
                p++;
            } while (j < 11);
            i++;
            dir += 0xc0;
        } while (i < 2);
    }
}
