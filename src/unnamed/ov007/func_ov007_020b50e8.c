#include "types.h"
extern char *data_ov007_0210342c;
extern int data_ov007_02103428;

extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020b647c(int r0, int r1, short r2);
extern void func_ov007_020b6208(int s);
extern void func_ov007_020b64a8(int a);
extern void func_ov007_020b7eec(void *t);
extern int _ZN4cstd3divEii(int a, int b);
extern void func_ov007_020aec94(int a);
extern void func_ov007_020b6134(int a);
extern void func_ov007_020b7a44(void);

#define MULFX(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

void func_ov007_020b50e8(char *arg)
{
    char *st;
    u16 state;
    int t;
    int result;
    int is34;
    int thresh;
    short *pv;
    char *gp;
    int v4;
    int angle;
    char *r3p;

    st = *(char **)arg;
    state = *(u16 *)st;
    r3p = *(char **)(st + 0x24);
    gp = *(char **)*(char **)(data_ov007_0210342c + 0x28);
    t = *(int *)(*(char **)(st + 4) + 0xc);
    v4 = *(u16 *)r3p;
    is34 = (state == 3 || state == 4);
    thresh = is34 ? 0x18 : 8;
    pv = *(short **)(data_ov007_0210342c + 8);
    angle = *(s16 *)(r3p + 2) << 12;
    result = 0x1000;

    if (t == 0) {
        func_ov007_020c1d8c(0);
        if (v4 == 0) {
            func_ov007_020b647c(*(u16 *)*(char **)arg, 0, 6);
            func_ov007_020b647c(*(u16 *)*(char **)arg, 1, 6);
            func_ov007_020b647c(*(u16 *)*(char **)arg, 2, 6);
            func_ov007_020b647c(*(u16 *)*(char **)arg, 7, 6);
        } else if ((u32)(state - 0xe) <= 2 && *pv == 0) {
            func_ov007_020b647c(*(u16 *)*(char **)arg, 0xe, 4);
            func_ov007_020b647c(*(u16 *)*(char **)arg, 0xf, 4);
            func_ov007_020b647c(*(u16 *)*(char **)arg, 0x10, 4);
            func_ov007_020b647c(*(u16 *)*(char **)arg, 0x15, 4);
            func_ov007_020b647c(*(u16 *)*(char **)arg, 0x14, 4);
        }
        {
            int flag;
            flag = 1;
            func_ov007_020b6208(state);
            switch (*(u16 *)st) {
            case 9:
                break;
            case 11:
                func_ov007_020b64a8(flag);
                break;
            case 10:
                func_ov007_020b64a8(-1);
                break;
            case 12:
                if (*(u8 *)(gp + 9) == 0) {
                    ((void (*)(int))*(int *)(data_ov007_0210342c + 0x2c))(4);
                }
                break;
            case 13:
                if (*(u8 *)(gp + 9) != 0) {
                    ((void (*)(int))*(int *)(data_ov007_0210342c + 0x2c))(5);
                }
                break;
            default:
                flag = 0;
                break;
            }
            if (flag != 0) {
                func_ov007_020b7eec(*(char **)*(char **)(data_ov007_0210342c + 0x28));
            }
        }
    }

    if (t < thresh) {
        int q;
        if (t <= 0) {
            q = 0;
        } else if (t >= thresh) {
            q = 0x1000;
        } else {
            q = _ZN4cstd3divEii(t << 12, thresh);
        }
        if (is34 != 0) {
            if (q < 0x555) {
                int c;
                int w;
                w = q * 3;
                w <<= 2;
                if (w <= 0x1000) {
                    c = MULFX(w, 0x1000);
                } else if (w <= 0x3000) {
                    c = MULFX(w - 0x1000, 0x800) + 0x1000;
                } else {
                    c = MULFX(w - 0x3000, -0x2000) + 0x2000;
                }
                result = 0x1000 - MULFX(c, 0x4b0);
            } else {
                int c;
                int w;
                w = ((q * 3 - 0x1000) / 2) * 4;
                if (w <= 0x1000) {
                    c = MULFX(w, 0x800);
                } else if (w <= 0x3000) {
                    c = MULFX(w - 0x1000, 0x400) + 0x800;
                } else {
                    c = MULFX(w - 0x3000, -0x1000) + 0x1000;
                }
                result = MULFX(c, 0x640) + 0x1000;
            }
        } else {
            u16 s2;
            s2 = *(u16 *)st;
            if ((u16)(s2 + 0xfff6) <= 1) {
                int dir;
                int w;
                int d8;
                dir = (s2 == 10) ? -1 : 1;
                w = q * 2;
                if (w > 0x1000) {
                    w = -(w - 0x2000);
                }
                d8 = dir * 8;
                angle += d8 * w;
            } else {
                int c;
                int w;
                w = q * 4;
                if (w <= 0x1000) {
                    c = MULFX(w, 0x1000);
                } else if (w <= 0x3000) {
                    c = MULFX(w - 0x1000, -0x2ec) + 0x1000;
                } else {
                    c = MULFX(w - 0x3000, -0xa28) + 0xa28;
                }
                result = 0x1000 - MULFX(c, 0x514);
            }
        }
    } else {
        char *p;
        int next;
        u16 s;
        p = *(char **)(arg + 4);
        next = 0;
        *(int *)(p + 0x40) = 0x1000;
        *(int *)(p + 0x3c) = *(int *)(p + 0x40);
        s = *(u16 *)st;
        switch (s) {
        case 0:
            func_ov007_020aec94(9);
            break;
        case 1:
            func_ov007_020aec94(6);
            break;
        case 2:
            func_ov007_020aec94(7);
            break;
        case 7:
            pv[1] = 8;
            next = 6;
            break;
        case 3:
            next = 1;
            *(s16 *)(*(char **)(data_ov007_0210342c + 4) + 2) = 2;
            break;
        case 4:
            next = 1;
            *(s16 *)(*(char **)(data_ov007_0210342c + 4) + 2) = 3;
            break;
        case 6:
            *(s16 *)(*(char **)(data_ov007_0210342c + 4) + 2) = 4;
            break;
        case 8:
            pv[1] = next;
            break;
        case 14:
        case 15:
        case 16:
            func_ov007_020b6134(s - 0xe);
            break;
        case 21:
            pv[1] = 1;
            break;
        case 17:
            pv[1] = 3;
            break;
        case 18:
            pv[1] = 7;
            break;
        case 19:
            pv[1] = 9;
            break;
        case 20:
            switch (*pv) {
            case 0:
                pv[1] = 0xb;
                break;
            case 1:
                pv[1] = next;
                break;
            case 2:
                pv[1] = 6;
                break;
            case 3:
            case 9:
                pv[1] = 1;
                break;
            case 7: {
                char *d;
                d = data_ov007_0210342c;
                if (*(int *)(d + 0x58) != -1) {
                    *(int *)(d + 0x58) = -1;
                } else {
                    pv[1] = 1;
                }
                break;
            }
            case 12:
                break;
            }
            break;
        case 22:
        case 23:
            if (s == 0x16) {
                data_ov007_02103428 = 2;
                *(s16 *)(*(char **)(*(char **)*(char **)(data_ov007_0210342c + 0x170) + 4) + 2) = 4;
            } else if (s == 0x17) {
                data_ov007_02103428 = 3;
                *(s16 *)(*(char **)(*(char **)*(char **)(data_ov007_0210342c + 0x16c) + 4) + 2) = 4;
            }
            func_ov007_020b7a44();
            next = 4;
            break;
        }
        *(s16 *)(*(char **)(st + 4) + 2) = next;
    }

    *(int *)(*(char **)(arg + 4) + 0x40) = result;
    *(int *)(*(char **)(arg + 4) + 0x3c) = *(int *)(*(char **)(arg + 4) + 0x40);
    *(int *)(*(char **)arg + 8) = angle;
}
