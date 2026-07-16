#pragma opt_common_subs off
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed char s8;
typedef unsigned char u8;

extern u8 data_0209fcc8;
extern u8 data_0209fc7c;
extern u8 data_0209fc78;
extern u8 data_0209fc70;
extern u8 data_0209fcd0;
extern u8 data_0209fc98;
extern u8 data_0209fcb8;
extern u8 data_020a0e40;
extern u8 data_0209f4a2;
extern u8 data_0209f4a4;
extern int data_0209caa0[];
extern u8 data_020a0e5a;
extern u8 data_0209fc6c;
extern u8 data_0209fcf4[];
extern u8 data_0209fce0;
extern u8 data_0209fc80;
extern s32 data_0208ee44;
extern s32 data_020a0db0;
extern u8 data_ov002_0210c3a8;
extern u8 data_ov002_0210c3a0;
extern u8 data_ov002_0210c390;
extern u8 data_ov002_0210c398;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0deb[];
extern s16 data_0209fcec;
extern s16 data_0209fcf0;
extern s32 data_0209fd0c;
extern u8 data_0209fcc4;
extern u8 data_0209fcbc;
extern u8 data_0209fce4;
extern u8 data_0209fcc0;
extern u8 data_0209fcd4;

extern void func_02031e00(void);
extern void func_020319fc(int n);
extern void func_02031428(void);
extern int IsButtonInputValid(void);
extern void* _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int a, void* b, int c, int d, int e, int f, int g, int h, int i, int j);

void func_020326ac(void)
{
    switch (data_0209fcc8) {
    case 0:
    case 8:
        break;
    case 1: {
        data_0209fc7c = 0;
        func_02031e00();
        return;
    }
    case 2: {
        func_020319fc(data_0209fc78);
        data_0209fcc8 = 1;
        return;
    }
    case 3: {
        data_0209fc7c = 0;
        func_02031428();
        data_0209fc70 += 1;
        if (data_0209fc70 != 0x10) {
            return;
        }
        data_0209fcc8 = 1;
        data_0209fc7c = 0;
        func_02031e00();
        return;
    }
    case 4: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209fc98 = data_0209fcd0 + 1;
            data_0209fcb8 = 1;
            return;
        }
        t = *(s16*)(&data_0209f4a2 + data_020a0e40 * 0x18);
        if (t > 0x80) {
            data_0209fcd0 = 1;
        } else if (t < -0x80) {
            data_0209fcd0 = 0;
        } else if (*((u8*)data_0209caa0 + 0x42) == 0 &&
                   (*(u16*)(&data_020a0e5a + data_020a0e40 * 4) & 0x30)) {
            data_0209fcd0 ^= 1;
        }
        {
            int r5 = data_0209fcf4[data_0209fcd0] * 9 + data_0209fc6c + 0xC;
            int r4 = data_0209fce0 + (data_0209fc80 * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 5: {
        s16 t;
        if (IsButtonInputValid() != 0) {
            data_0209fc98 = data_0209fcd0 + 1;
            data_0209fcb8 = 1;
            return;
        }
        t = *(s16*)(&data_0209f4a4 + data_020a0e40 * 0x18);
        if (t > 0x80) {
            data_0209fcd0 = 1;
        } else if (t < -0x80) {
            data_0209fcd0 = 0;
        } else if (*((u8*)data_0209caa0 + 0x42) == 0 &&
                   (*(u16*)(&data_020a0e5a + data_020a0e40 * 4) & 0xC0)) {
            data_0209fcd0 ^= 1;
        }
        {
            int r5 = data_0209fc6c + 0xC;
            int r4 = data_0209fce0 + (data_0209fcf4[data_0209fcd0] * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 6: {
        int hasf1;
        u8 idx;
        u8 f0;
        if (IsButtonInputValid() != 0) {
            data_0209fc98 = data_0209fcd0 + 1;
            data_0209fcb8 = 1;
            return;
        }
        idx = data_020a0e40;
        hasf1 = 0;
        f0 = data_020a0de8[idx * 4];
        if (f0 != 0) {
            if (data_020a0de9[idx * 4] != 0) {
                hasf1 = 1;
            }
        }
        if (hasf1 != 0) {
            data_0209fcec = data_020a0deb[idx * 4];
            data_0209fcec -= (s16)(data_0209fcd0 * 8);
        } else if (f0 != 0) {
            s16 debval = data_020a0deb[idx * 4];
            data_0209fcf0 = debval - data_0209fcec;
            if (data_0209fcf0 > 0x10) {
                data_0209fcf0 = 0x10;
                data_0209fcec = debval - 0x10;
            } else if (data_0209fcf0 < 0) {
                data_0209fcf0 = 0;
                data_0209fcec = debval;
            }
            switch (data_0209fcd0) {
            case 0:
                if (data_0209fcf0 > 0 && data_0209fcf0 >= 8) {
                    data_0209fcd0 += 1;
                }
                break;
            case 1:
                if (data_0209fcf0 > 0) {
                    if (data_0209fcf0 >= 0x10) {
                        data_0209fcd0 += 1;
                    } else if (data_0209fcf0 <= 6) {
                        data_0209fcd0 -= 1;
                    }
                }
                break;
            case 2:
                if (data_0209fcf0 > 0 && data_0209fcf0 <= 0xE) {
                    data_0209fcd0 -= 1;
                }
                break;
            }
        } else if (*((u8*)data_0209caa0 + 0x42) == 0) {
            u16 mask = *(u16*)(&data_020a0e5a + idx * 4);
            if (mask & 0xC0) {
                u8 target = (mask & 0x40) ? 0 : 2;
                if (data_0209fcd0 != target) {
                    if (target != 0) {
                        data_0209fcd0 += 1;
                    } else {
                        data_0209fcd0 -= 1;
                    }
                }
            }
        }
        {
            int r5 = data_0209fc6c + 0xC;
            int r4 = data_0209fce0 + (data_0209fcf4[data_0209fcd0] * 0x10) + 0xE;
            if (data_020a0db0 & (0x10 / data_0208ee44)) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a8, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c3a0, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    case 7: {
        if ((data_0209caa0[2] & 0x80) && IsButtonInputValid() != 0) {
            data_0209fd0c += 1;
            data_0209fcc4 = 1;
            data_0209fcbc = 0;
            data_0209fcc8 = 1;
        }
        if (!(data_0209caa0[2] & 0x80)) {
            return;
        }
        {
            int r5 = ((data_0209fce4 * 2) - 6) - data_0209fc6c;
            int r4 = ((data_0209fcc0 * 2) + 2) - data_0209fce0;
            if (!(data_020a0db0 & (0x10 / data_0208ee44))) {
                return;
            }
            if (data_0209fcd4 != 0) {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c390, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            } else {
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, &data_ov002_0210c398, r5, r4, -1, -1, 0x1000, 0x1000, 0, -1);
            }
            return;
        }
    }
    }
}
