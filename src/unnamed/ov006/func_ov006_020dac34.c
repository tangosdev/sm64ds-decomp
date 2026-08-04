//cpp
// NONMATCHING: extra logic (you do more) (div=2). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
    extern int func_ov006_020c0f0c(void *p);
    extern int func_ov006_020c1718(void *r0);
    extern int func_ov006_020da834(void *p);
    extern int func_ov006_020da860(void *p, int v);
    extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
    extern void func_ov004_020ad79c(int r0arg, int r1arg);
    extern int func_ov006_020da88c(void *p, int v);
    extern void func_ov006_020c1420(void *c, short arg1, void *arg2);
    extern int func_ov006_020da8b8(void *p, int v);
    extern void func_ov006_020da420(void *p);
    extern int func_ov006_020da174(void *p);
    extern void func_ov006_020da154(void *p);
    extern int func_ov006_020da4ac(void *p1, void *p2);
    extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
    extern int func_ov006_020da5e8(void *p1, void *p2);
    extern void func_ov006_020c0d68(void *c);
    extern void func_ov004_020b0a54(int c);
    extern void func_ov006_020c0c80(void *c);
    extern void func_ov004_020adb1c(int self);
    
    extern unsigned char data_020a0e40[];
    extern unsigned char data_020a0de8[];
    extern unsigned char data_020a0de9[];
    extern unsigned char data_020a0dea[];
    extern unsigned char data_020a0deb[];
    extern int data_ov006_02141768;
    extern int data_ov006_0214176c;
    extern int data_ov006_02141770;
    extern int data_ov004_020bf9ec;
    extern void *func_020beb68;
    extern int data_ov006_0213bc44;
    extern int data_ov004_020bfa18;
    extern int data_ov006_0213bd48[];
}

struct VObj {
    virtual void v0();
    virtual void v1();
};
    
    #define S16(off) (*(short *)(c + (off)))
    #define S16L(off) (*(short *)((int)(((long long)(c + (off))) & 0xFFFFFFFFFFFFFFFFLL)))
    #define U8(off) (*(unsigned char *)(c + (off)))
    
    extern "C" void func_ov006_020dac34(unsigned char *c)
    {
        switch (S16(0x5388)) {
        case 1:
            if (U8(0xc4) == 0) {
                c[0xc3] = 1;
                c[0xc4] = 1;
                S16(0xc0) = 0;
            }
            if (func_ov006_020c0f0c(c + 0x4f38) != 0) {
                S16L(0x5388) += 1;
            }
            break;
        case 2:
            if (func_ov006_020c1718((int *)(c + 0x4f38)) != 0) {
                S16(0x538a) = 0x10;
                S16L(0x5388) += 1;
            } else {
                short cnt = S16(0x538c);
                if (cnt > 0) {
                    int t = cnt - 1;
                    if (cnt != 0) {
                        int idx = t * 0x30;
                        unsigned char *p = c + 0x51d3;
                        if (p[idx] == 0) {
                            p[idx] = 1;
                            *(c + idx + 0x52c3) = 1;
                        }
                    }
                }
            }
            break;
        case 3:
            S16L(0x538a) -= 1;
            if (S16(0x538a) == 0) {
                S16L(0x5388) += 1;
            }
            break;
        case 4: {
            int ok = 0;
            int idx0 = data_020a0e40[0] * 4;
            if (data_020a0de8[idx0] != 0 && data_020a0de9[idx0] != 0) {
                ok = 1;
            }
            if (ok != 0 && func_ov006_020da834(c + 0x51a8) != 0 && data_ov006_02141768 == 0) {
                unsigned char idx1 = data_020a0e40[0];
                unsigned char v5 = data_020a0dea[idx1 * 4];
                unsigned char v4 = data_020a0deb[idx1 * 4];
                if (func_ov006_020da860(c + 0x51a8, 2) != 0) {
                    int d0 = v5 - 0x80;
                    if (d0 < 0) d0 = -d0;
                    if (d0 < 0x48) {
                        int d1 = v4 - 0x58;
                        if (d1 < 0) d1 = -d1;
                        if (d1 < 0x14) {
                            data_ov006_0214176c = 1;
                            data_ov006_02141768 = 1;
                            S16L(0x5388) += 1;
                            data_ov004_020bf9ec = 1;
                            _ZN5Sound12PlayBank2_2DEj(0x151);
                            {
                                void *g = func_020beb68;
                                int a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                                int a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                                func_ov004_020ad79c(a0, a1);
                            }
                        }
                    }
                } else {
                    int d0 = v5 - 0x80;
                    if (d0 < 0) d0 = -d0;
                    if (d0 < 0x48) {
                        int d1 = v4 - 0x58;
                        if (d1 < 0) d1 = -d1;
                        if (d1 < 0x14) {
                            int cnt3, i;
                            unsigned char *p;
                            data_ov006_0214176c = 1;
                            data_ov006_02141768 = 1;
                            S16L(0x5388) += 1;
                            data_ov004_020bf9ec = 1;
                            _ZN5Sound12PlayBank2_2DEj(0x151);
                            {
                                void *g = func_020beb68;
                                int a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                                int a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                                func_ov004_020ad79c(a0, a1);
                            }
                            cnt3 = 0;
                            i = 0;
                            p = c;
                            do {
                                unsigned char v = p[0x51d3];
                                i++;
                                p += 0x30;
                                if (v == 3) cnt3++;
                            } while (i < 5);
                            if (cnt3 < 2) _ZN5Sound12PlayBank2_2DEj(0x144);
                            else _ZN5Sound12PlayBank2_2DEj(0x145);
                        }
                    }
                }
            }
            break;
        }
        case 5:
            if (data_ov006_02141768 != 0 && func_ov006_020da860(c + 0x51a8, 2) != 0 && func_ov006_020c1718((int *)(c + 0x4f38)) != 0) {
                S16L(0x5388) += 1;
            } else if (func_ov006_020c1718((int *)(c + 0x4f38)) != 0) {
                int r = func_ov006_020da88c(c + 0x51a8, 6);
                if (r != 0) {
                    S16(0x538c) = 0;
                    S16(0x511e) = 0;
                    func_ov006_020c1420(c + 0x4f38, (short)r, c + 0x538c);
                }
            } else {
                int r = func_ov006_020da8b8(c + 0x51a8, 6);
                if (r > -1 && S16(0x538c) != 0) {
                    (c + r * 0x30)[0x51d3] = 5;
                    S16L(0x538c) -= 1;
                }
            }
            break;
        case 6:
            func_ov006_020da420(c + 0x5298);
            S16L(0x5388) += 1;
            break;
        case 7:
            if (func_ov006_020da860(c + 0x5298, 1) != 0 && func_ov006_020c1718((int *)(c + 0x4f38)) != 0) {
                int t = data_ov006_0213bc44 - 1;
                data_ov006_0213bc44 = t;
                if (t == 0) {
                    S16L(0x5388) += 1;
                } else {
                    S16(0x5388) = 4;
                    data_ov006_0214176c = 0;
                    data_ov006_02141768 = 0;
                    data_ov006_02141770 = 0;
                }
            } else if (func_ov006_020c1718((int *)(c + 0x4f38)) != 0) {
                int r = func_ov006_020da88c(c + 0x5298, 6);
                if (r != 0) {
                    S16(0x538c) = 0;
                    S16(0x511e) = 0;
                    func_ov006_020c1420(c + 0x4f38, (short)r, c + 0x538c);
                }
            } else {
                int r = func_ov006_020da8b8(c + 0x5298, 6);
                if (r > -1 && S16(0x538c) != 0) {
                    (c + r * 0x30)[0x52c3] = 5;
                    S16L(0x538c) -= 1;
                }
            }
            break;
        case 8: {
            int cnt = 0;
            unsigned char *p = c;
            c[0xc3] = 0;
            do {
                cnt++;
                p[0x52c3] = 2;
                p += 0x30;
            } while (cnt < 5);
            S16L(0x5388) += 1;
            break;
        }
        case 9: {
            int a = func_ov006_020da174(c + 0x51a8);
            int b = func_ov006_020da174(c + 0x5298);
            if (a != 0 || b != 0) {
                _ZN5Sound12PlayBank2_2DEj(0x152);
            }
            S16L(0x5388) += 1;
            break;
        }
        case 10:
            if (func_ov006_020da860(c + 0x51a8, 8) != 0 && func_ov006_020da860(c + 0x5298, 8) != 0) {
                S16(0x538a) = 0x14;
                S16L(0x5388) += 1;
            }
            break;
        case 11:
            S16L(0x538a) -= 1;
            if (S16(0x538a) != 0) {
                break;
            }
            {
                int i = 0;
                unsigned char *p = c;
                do {
                    p[0x51d4] = 1;
                    i++;
                    p[0x52c4] = 1;
                    p += 0x30;
                } while (i < 5);
                func_ov006_020da154(c + 0x51a8);
                func_ov006_020da154(c + 0x5298);
                _ZN5Sound12PlayBank2_2DEj(0x147);
                S16(0x538a) = 0x3c;
                S16L(0x5388) += 1;
            }
            break;
        case 12:
            S16L(0x538a) -= 1;
            if (S16(0x538a) == 0) {
                int t4 = func_ov006_020da4ac(c + 0x5298, c + 0x5392);
                int t5 = func_ov006_020da4ac(c + 0x51a8, c + 0x538e);
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
                {
                    int r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
                    if (r == -1) {
                        void *g = func_020beb68;
                        int a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                        int a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                        func_ov004_020ad79c(a0, a1 - 1);
                        S16(0x538a) = 0x5a;
                    } else if (r == 1) {
                        void *g = func_020beb68;
                        int a0 = g ? *(int *)((char *)g + 0xa8) : 0;
                        int arrv = data_ov006_0213bd48[t5];
                        int bfa = data_ov004_020bfa18;
                        int arg0v = arrv * bfa + a0;
                        int a1 = g ? *(int *)((char *)g + 0xb4) : 0;
                        func_ov004_020ad79c(arg0v, a1 + 1);
                        S16(0x538a) = 0x1e;
                    } else {
                        S16(0x538a) = 0x5a;
                    }
                }
                S16L(0x5388) += 1;
            }
            break;
        case 13:
            S16L(0x538a) -= 1;
            if (S16(0x538a) == 0) {
                int r = func_ov006_020da5e8(c + 0x51a8, c + 0x5298);
                if (r == -1) {
                    func_ov006_020c0d68(c + 0x4f38);
                    {
                        char *g = (char *)func_020beb68;
                        if (g != 0) {
                            if (*(volatile int *)(g + 0xb4) > 0) {
                                int *gi = (int *)((int)(((long long)(int)(g + 0xb4)) & 0xFFFFFFFFFFFFFFFFLL));
                                *gi = *gi - 1;
                            }
                        }
                    }
                    func_ov004_020b0a54(5);
                } else if (r == 1) {
                    func_ov006_020c0c80(c + 0x4f38);
                    {
                        void *g = func_020beb68;
                        if (g != 0) {
                            char *gb = (char *)g;
                            if (*(volatile int *)(gb + 0xb4) < 0x270f) {
                                int *fld = (int *)((int)(((long long)(int)(gb + 0xb4)) & 0xFFFFFFFFFFFFFFFFLL));
                                *fld = *fld + 1;
                            }
                            {
                                int t = *(int *)(gb + 0xb4);
                                if (t > *(int *)(gb + 0xb8)) {
                                    *(int *)(gb + 0xb8) = t;
                                }
                            }
                        }
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
                S16L(0x5388) += 1;
            }
            break;
        }
    
        {
            int i = 0;
            unsigned char *p1 = c + 0x51a8;
            unsigned char *p2 = c + 0x5298;
            do {
                ((VObj *)p1)->v1();
                ((VObj *)p2)->v1();
                i++;
                p1 += 0x30;
                p2 += 0x30;
            } while (i < 5);
        }
        data_ov006_0214176c = 0;
    }
