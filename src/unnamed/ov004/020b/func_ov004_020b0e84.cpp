//cpp
#include "private/ov004_obj_vtbl.h"

extern "C" void func_02012790(int);
extern "C" int GetGameLanguage(void);
extern "C" void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
extern "C" void func_ov004_020b1ea4(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
extern "C" void *data_ov004_020bbfa8[];

static inline int sdiv4(int d) {
    int t = d + (int)((unsigned)(d >> 1) >> 0x1e);
    return t >> 2;
}

static inline int sdiv2(int e) {
    return (e + (int)((unsigned)e >> 31)) >> 1;
}

extern "C" void func_ov004_020b0e84(Obj *sl, int sb) {
    void *r8;
    int r7;
    int i;
    int pin;

    {
        int t = sl->unk50;
        if (t < sb) {
            sl->unk54 = 1;
            sl->unk50 = sb;
            func_02012790(0x1bb);
        } else if (t > sb) {
            sl->unk54 = 0;
            sl->unk50 = sb;
        } else if (sl->unk54 > 0) {
            if (sl->f68() == 2) {
                sl->unk54 += 2;
            } else {
                sl->unk54 += 1;
            }
        }
    }

    r8 = data_ov004_020bbfa8[GetGameLanguage()];
    r7 = 0xa8;

    if (sb < 5) {
        r7 -= 8;
        i = 0;
        if (sb > 0) {
            do {
                if (i == sb - 1 && sl->unk54 > 0 && sl->unk54 % 0x3c >= 0x1e) {
                    break;
                }
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                i += 1;
                r7 -= 0x10;
            } while (i < sb);
        }
        if (sl->unk54 >= sl->unk58) {
            sl->unk54 = 0;
        }
        return;
    }

    if (sb == 5) {
        int u = sl->unk54;
        if (u == 0) {
            func_ov004_020b1ea4(0xf0, r7, 5, 1, 0, 0, 0x14);
            RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
        } else if (u < sl->unk58) {
            r7 -= 8;
            i = 0;
            do {
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                i += 1;
                r7 -= 0x10;
            } while (i < 4);
            if (sl->unk54 % 0x3c < 0x1e) {
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
            }
        } else {
            int d = (sl->unk58 + 0x20) - sl->unk54;
            r7 -= sdiv4(d);
            i = 0;
            do {
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                {
                    unsigned e = (sl->unk58 + 0x20) - sl->unk54;
                    r7 -= sdiv2((int)e);
                }
                i += 1;
            } while (i < 5);
        }
        if (sl->f68() == 2) {
            int b = sl->unk58;
            int c = sl->unk54;
            if (c == b || c == b + 1) {
                func_02012790(0x1ba);
            }
        } else if (sl->unk54 == sl->unk58) {
            func_02012790(0x1ba);
        }
        if (sl->unk54 >= sl->unk58 + 0x20) {
            sl->unk54 = 0;
        }
        return;
    }

    {
        int rem = sb % 5;
        if (rem == 0) {
            int u = sl->unk54;
            if (u == 0) {
                func_ov004_020b1ea4(0xf0, r7, sb, 1, 0, 0, 0x14);
                RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
            } else if (u < sl->unk58) {
                func_ov004_020b1ea4(0xf0, r7, sb - 5, 1, 0, 0, 0x14);
                RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
                r7 -= 0x18;
                i = 0;
                do {
                    if (i == 4 && sl->unk54 > 0 && sl->unk54 % 0x3c >= 0x1e) {
                        break;
                    }
                    RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                    i += 1;
                    r7 -= 0x10;
                } while (i < 5);
            } else {
                int d;
                func_ov004_020b1ea4(0xf0, r7, sb - 5, 1, 0, 0, 0x14);
                RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
                d = (sl->unk58 + 0x20) - sl->unk54;
                r7 -= sdiv4(d);
                i = 0;
                do {
                    unsigned e = (sl->unk58 + 0x20) - sl->unk54;
                    r7 -= sdiv2((int)e);
                    RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                    i += 1;
                } while (i < 5);
            }
            if (sl->f68() == 2) {
                int b = sl->unk58;
                int c = sl->unk54;
                if (c == b || c == b + 1) {
                    func_02012790(0x1ba);
                }
            } else if (sl->unk54 == sl->unk58) {
                func_02012790(0x1ba);
            }
            if (sl->unk54 >= sl->unk58 + 0x20) {
                sl->unk54 = 0;
            }
            return;
        }

        {
            int base5 = (sb / 5) * 5;
            func_ov004_020b1ea4(0xf0, r7, base5, 1, 0, 0, 0x14);
        }
        RenderOamMainScreen(((int *)r8)[0x2c / 4], 0xf0, r7, -1, -1);
        r7 -= 0x18;
        sb = 0;
        if (rem > 0) {
            pin = rem - 1;
            do {
                if (sb == pin && sl->unk54 > 0 && sl->unk54 % 0x3c >= 0x1e) {
                    break;
                }
                RenderOamMainScreen(((int *)r8)[0x24 / 4], 0xf0, r7, -1, -1);
                sb += 1;
                r7 -= 0x10;
            } while (sb < rem);
        }
        if (sl->unk54 >= sl->unk58) {
            sl->unk54 = 0;
        }
    }
}
