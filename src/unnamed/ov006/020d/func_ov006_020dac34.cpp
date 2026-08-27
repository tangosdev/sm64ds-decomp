//cpp
#include "types.h"
#include "dScMgCard_c.h"
#include "cMgSmartball_object_c.h"
/* dScMgCard_c Behavior @ 0x020dac34 */

extern "C" {
extern int func_ov006_020c0f0c(void *p);
extern int func_ov006_020c1718(void *p);
extern int func_ov006_020da834(void *p);
extern int func_ov006_020da860(void *p, int v);
extern int func_ov006_020da88c(void *p, int v);
extern int func_ov006_020da8b8(void *p, int v);
extern void func_ov006_020c1420(void *c, s16 arg1, void *arg2);
extern void func_ov006_020da420(void *p);
extern int func_ov006_020da174(void *p);
extern void func_ov006_020da154(void *p);
extern int func_ov006_020da4ac(void *p1, void *p2);
extern int func_ov006_020da5e8(void *p1, void *p2);
extern void func_ov006_020c0d68(void *c);
extern void func_ov006_020c0c80(void *c);
extern void func_ov004_020ad79c(int a0, int a1);
extern void func_ov004_020b0cac(int a0, int a1, int a2, int a3, int a4, s16 a5);
extern void func_ov004_020adb1c(int v);
extern void func_ov004_020b0a54(int v);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern u8 data_020a0e40[];
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern int data_ov006_02141768;
extern int data_ov006_0214176c;
extern int data_ov006_02141770;
extern int data_ov004_020bf9ec;
extern int data_ov006_0213bc44;
extern int data_ov004_020bfa18;
extern int data_ov006_0213bd48[];
}

#define S16(off) (*(s16 *)(c + (off)))

extern "C" void func_ov006_020dac34(unsigned char *c)
{
    int cnt3;
    int i;
    int j;
    int k;
    int n;
    unsigned char *p;
    unsigned char *p1;
    unsigned char *p2;
    int ok;
    int idx;
    int v5;
    int v4;
    int r;
    int a;
    int b;
    int t4;
    int t5;
    void *g;
    int a0;
    int a1;
    s16 cnt;

    switch (S16(0x5388)) {
    case 1:
        if (c[0xc4] == 0) {
            c[0xc3] = 1;
            c[0xc4] = 1;
            S16(0xc0) = 0;
        }
        if (func_ov006_020c0f0c(c + 0x4f38) != 0)
            S16(0x5388)++;
        break;
    case 2:
        if (func_ov006_020c1718(c + 0x4f38) != 0) {
            S16(0x538a) = 0x10;
            S16(0x5388)++;
        } else {
            cnt = S16(0x538c);
            if (cnt > 0) {
                idx = cnt - 1;
                if (cnt != 0) {
                    idx = idx * 0x30;
                    p = c + 0x51d3;
                    if ((u8)p[idx] == 0) {
                        p[idx] = 1;
                        (c + idx + 0x5000)[0x2c3] = 1;
                    }
                }
            }
        }
        break;
    case 3:
        S16(0x538a)--;
        if (S16(0x538a) == 0)
            S16(0x5388)++;
        break;
    case 4:
        ok = 0;
        idx = data_020a0e40[0] * 4;
        if (data_020a0de8[idx] != 0 && data_020a0de9[idx] != 0)
            ok = 1;
        if (ok != 0 && func_ov006_020da834(c + 0x51a8) != 0 && data_ov006_02141768 == 0) {
            v5 = data_020a0dea[data_020a0e40[0] * 4];
            v4 = data_020a0deb[data_020a0e40[0] * 4];
            if (func_ov006_020da860(c + 0x51a8, 2) != 0) {
                r = v5 - 0x80;
                if (r < 0)
                    r = -r;
                if (r < 0x48) {
                    r = v4 - 0x58;
                    if (r < 0)
                        r = -r;
                    if (r < 0x14) {
                        data_ov006_0214176c = 1;
                        data_ov006_02141768 = 1;
                        S16(0x5388)++;
                        data_ov004_020bf9ec = 1;
                        _ZN5Sound12PlayBank2_2DEj(0x151);
                        g = data_ov004_020beb68;
                        a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                        a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                        func_ov004_020ad79c(a0, a1);
                    }
                }
            } else {
                r = v5 - 0x80;
                if (r < 0)
                    r = -r;
                if (r < 0x48) {
                    r = v4 - 0x58;
                    if (r < 0)
                        r = -r;
                    if (r < 0x14) {
                        data_ov006_0214176c = 1;
                        data_ov006_02141768 = 1;
                        S16(0x5388)++;
                        data_ov004_020bf9ec = 1;
                        _ZN5Sound12PlayBank2_2DEj(0x151);
                        g = data_ov004_020beb68;
                        a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                        a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                        func_ov004_020ad79c(a0, a1);
                        cnt3 = 0;
                        i = cnt3;
                        p = c;
                        do {
                            r = (u8)(p + 0x5000)[0x1d3];
                            i++;
                            p += 0x30;
                            if (r == 3)
                                cnt3++;
                        } while (i < 5);
                        if (cnt3 < 2)
                            _ZN5Sound12PlayBank2_2DEj(0x144);
                        else
                            _ZN5Sound12PlayBank2_2DEj(0x145);
                    }
                }
            }
        }
        break;
    case 5:
        if (data_ov006_02141768 != 0 && func_ov006_020da860(c + 0x51a8, 2) != 0
            && func_ov006_020c1718(c + 0x4f38) != 0) {
            S16(0x5388)++;
        } else if (func_ov006_020c1718(c + 0x4f38) != 0) {
            r = func_ov006_020da88c(c + 0x51a8, 6);
            if (r != 0) {
                S16(0x538c) = 0;
                S16(0x511e) = 0;
                func_ov006_020c1420(c + 0x4f38, (s16)r, c + 0x538c);
            }
        } else {
            r = func_ov006_020da8b8(c + 0x51a8, 6);
            if (r > -1 && S16(0x538c) != 0) {
                (c + r * 0x30 + 0x5000)[0x1d3] = 5;
                S16(0x538c)--;
            }
        }
        break;
    case 6:
        func_ov006_020da420(c + 0x5298);
        S16(0x5388)++;
        break;
    case 7:
        if (func_ov006_020da860(c + 0x5298, 1) != 0 && func_ov006_020c1718(c + 0x4f38) != 0) {
            r = data_ov006_0213bc44 - 1;
            data_ov006_0213bc44 = r;
            if (r == 0) {
                S16(0x5388)++;
            } else {
                S16(0x5388) = 4;
                data_ov006_0214176c = 0;
                data_ov006_02141768 = 0;
                data_ov006_02141770 = 0;
            }
        } else if (func_ov006_020c1718(c + 0x4f38) != 0) {
            r = func_ov006_020da88c(c + 0x5298, 6);
            if (r != 0) {
                S16(0x538c) = 0;
                S16(0x511e) = 0;
                func_ov006_020c1420(c + 0x4f38, (s16)r, c + 0x538c);
            }
        } else {
            r = func_ov006_020da8b8(c + 0x5298, 6);
            if (r > -1 && S16(0x538c) != 0) {
                (c + r * 0x30 + 0x5000)[0x2c3] = 5;
                S16(0x538c)--;
            }
        }
        break;
    case 8:
        j = 0;
        p = c;
        c[0xc3] = (unsigned char)j;
        do {
            j++;
            (p + 0x5000)[0x2c3] = 2;
            p += 0x30;
        } while (j < 5);
        S16(0x5388)++;
        break;
    case 9:
        a = func_ov006_020da174(c + 0x51a8);
        b = func_ov006_020da174(c + 0x5298);
        if (a != 0 || b != 0)
            _ZN5Sound12PlayBank2_2DEj(0x152);
        S16(0x5388)++;
        break;
    case 10:
        if (func_ov006_020da860(c + 0x51a8, 8) != 0 && func_ov006_020da860(c + 0x5298, 8) != 0) {
            S16(0x538a) = 0x14;
            S16(0x5388)++;
        }
        break;
    case 11:
        S16(0x538a)--;
        if (S16(0x538a) == 0) {
            p = c;
            k = 0;
            do {
                (p + 0x5000)[0x1d4] = 1;
                k++;
                (p + 0x5000)[0x2c4] = 1;
                p += 0x30;
            } while (k < 5);
            func_ov006_020da154(c + 0x51a8);
            func_ov006_020da154(c + 0x5298);
            _ZN5Sound12PlayBank2_2DEj(0x147);
            S16(0x538a) = 0x3c;
            S16(0x5388)++;
        }
        break;
    case 12:
        S16(0x538a)--;
        if (S16(0x538a) == 0) {
            t4 = func_ov006_020da4ac(c + 0x5298, c + 0x5392);
            t5 = func_ov006_020da4ac(c + 0x51a8, c + 0x538e);
            if (data_ov006_02141768 != 0 && data_ov006_02141768 != 0) {
                func_ov004_020b0cac(t4 + 0x16, 0x80, 0x48, 0, -1, 1);
                func_ov004_020b0cac(t5 + 0x16, 0x80, 0x78, 0, -1, 2);
                if (t4 != t5) {
                    S16(0x5392) = 6;
                    S16(0x5394) = 6;
                    S16(0x538e) = 6;
                    S16(0x5390) = 6;
                } else if (S16(0x5392) != S16(0x538e)) {
                    S16(0x5394) = 6;
                    S16(0x5390) = 6;
                }
            }
            r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
            if (r == -1) {
                g = data_ov004_020beb68;
                a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                func_ov004_020ad79c(a0, a1 - 1);
                S16(0x538a) = 0x5a;
            } else if (r == 1) {
                g = data_ov004_020beb68;
                {
                    int *tbl = data_ov006_0213bd48;
                    int scale;
                    int prod;
                    a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                    r = tbl[t5];
                    scale = data_ov004_020bfa18;
                    prod = r * scale + a0;
                    r = g ? *(int *)((char *)g + 0xb4) : 0;
                    func_ov004_020ad79c(prod, r + 1);
                }
                S16(0x538a) = 0x1e;
            } else {
                S16(0x538a) = 0x5a;
            }
            S16(0x5388)++;
        }
        break;
    case 13:
        S16(0x538a)--;
        if (S16(0x538a) == 0) {
            r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
            if (r == -1) {
                func_ov006_020c0d68(c + 0x4f38);
                g = data_ov004_020beb68;
                if (g != 0) {
                    if (*(int *)((char *)g + 0xb4) > 0) {
                        *(int *)((char *)g + 0xb4) = *(int *)((char *)g + 0xb4) - 1;
                    }
                }
                func_ov004_020b0a54(5);
            } else if (r == 1) {
                func_ov006_020c0c80(c + 0x4f38);
                g = data_ov004_020beb68;
                if (g != 0) {
                    if (*(int *)((char *)g + 0xb4) < 0x270f) {
                        int *fld = (int *)((char *)g + 0xb4);
                        *fld = *fld + 1;
                    }
                    a1 = *(int *)((char *)g + 0xb4);
                    if (a1 > *(int *)((char *)g + 0xb8))
                        *(int *)((char *)g + 0xb8) = a1;
                }
                func_ov004_020adb1c(*(int *)(c + 0xb4));
                func_ov004_020b0a54(4);
            } else {
                func_ov004_020b0a54(7);
                _ZN5Sound12PlayBank2_2DEj(0x140);
                _ZN5Sound12PlayBank2_2DEj(0x138);
            }
            c[0xc3] = 0;
            S16(0x538a) = 0;
            S16(0x5388)++;
        }
        break;
    }

    p1 = c + 0x51a8;
    p2 = c + 0x5298;
    n = 0;
    do {
        ((cMgSmartball_object_c *)p1)->Update();
        ((cMgSmartball_object_c *)p2)->Update();
        n++;
        p1 += 0x30;
        p2 += 0x30;
    } while (n < 5);
    data_ov006_0214176c = 0;
}
