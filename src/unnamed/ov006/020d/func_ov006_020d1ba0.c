// NONMATCHING: register allocation (div=36). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_02012718(void *a, int b);
extern int func_ov004_020ae5c4(void *a, int b, int c, int d, int e, int f, int g);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
extern int func_ov006_020d2580(void *this, int *p1, int *p2);
extern void func_02012790(int a);
extern void func_ov006_020d1450(void *a);
extern void func_ov006_020d3624(void *a);
extern void func_ov006_020d3668(void *a);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);

extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];

void func_ov006_020d1ba0(void *arg0)
{
    char *p = (char*)arg0;
    volatile int sp10, sp14, sp18, sp1c;
    int oldX, oldY;
    int dx;

    if ((unsigned)*(unsigned char*)(p+0x470a) >= 0xff) return;
    if (*(unsigned char*)(p+0x53dd) == 1) return;

    oldX = *(int*)(p+0x46f0);
    oldY = *(int*)(p+0x46f4);

    if (data_020a0de8[data_020a0e40[0]*4] != 0) {
        int newX = data_020a0dea[data_020a0e40[0]*4];
        int newY = data_020a0deb[data_020a0e40[0]*4];
        if (newX < 0) return;
        if (newX >= 0x100) return;
        if (newY < 0) return;
        if (newY >= *(int*)(p+0x4700)) return;
        if (*(unsigned char*)(p+0x53de) != 0) return;
        *(int*)(p+0x46f0) = newX;
        *(int*)(p+0x46f4) = newY;
        if (*(int*)(p+0x46e8) == -1 || *(int*)(p+0x46ec) == -1) {
            *(int*)(p+0x46e8) = newX;
            *(int*)(p+0x46ec) = newY;
            func_02012718((void*)0xdc, *(int*)(p+0x46e8) << 0xc);
        }
        if (oldX == -1 && oldY == -1) return;
        if (oldX == *(int*)(p+0x46f0) && oldY == *(int*)(p+0x46f4)) {
            *(unsigned char*)(p+0x53c8) = 1;
            return;
        }
        func_ov004_020ae5c4(arg0, oldX, oldY, *(int*)(p+0x46f0), *(int*)(p+0x46f4), *(int*)(p+0x53e4), 1);
        if (*(unsigned char*)(p+0x53c8) == 1) {
            *(int*)(p+0x53cc) = func_02012468(*(int*)(p+0x53cc), 2, 0xde, 4, 0, 0, func_020126e8(*(int*)(p+0x46f0) << 0xc), 0);
            return;
        }
        *(int*)(p+0x53cc) = func_02012468(*(int*)(p+0x53cc), 2, 0xdd, 4, 0, 0, func_020126e8(*(int*)(p+0x46f0) << 0xc), 0);
        return;
    } else {
      int cond;
      if (data_020a0de8[data_020a0e40[0]*4] == 0 && data_020a0de9[data_020a0e40[0]*4] != 0) {
          cond = 1;
      } else {
          cond = 0;
      }
      if (cond != 0) {
        {
            if (*(int*)(p+0x46e8) != -1 && *(int*)(p+0x46ec) != -1 && oldX != -1 && oldY != -1) {
                if (*(unsigned char*)(p+0x4704) == 0) {
                    int d1 = oldX - *(int*)(p+0x46e8); if (d1 < 0) d1 = -d1;
                    if (d1 < 0x10) {
                        int d2 = oldY - *(int*)(p+0x46ec); if (d2 < 0) d2 = -d2;
                        if (d2 >= 0x10) goto classify;
                    } else {
                    classify:
                        if (oldX >= 0x21 && oldX <= 0x3f) {
                            if (*(int*)(p+0x46e8) >= 0x41 && *(int*)(p+0x46e8) <= 0x5f) {
                                *(int*)(p+0x46d8) = 0x60;
                                *(int*)(p+0x46dc) = *(int*)(p+0x46ec);
                                *(int*)(p+0x46e0) = 0x20;
                                *(int*)(p+0x46e4) = *(int*)(p+0x46f4);
                            }
                        } else if (oldX >= 0x41 && oldX <= 0x5f) {
                            if ((*(int*)(p+0x46e8)) >= 0x21 && (*(int*)(p+0x46e8)) <= 0x3f) {
                                *(int*)(p+0x46d8) = 0x20;
                                *(int*)(p+0x46dc) = *(int*)(p+0x46ec);
                                *(int*)(p+0x46e0) = 0x60;
                                *(int*)(p+0x46e4) = *(int*)(p+0x46f4);
                            }
                        } else if (oldX >= 0x61 && oldX <= 0x7f) {
                            if ((*(int*)(p+0x46e8)) >= 0x81 && (*(int*)(p+0x46e8)) <= 0x9f) {
                                *(int*)(p+0x46d8) = 0xa0;
                                *(int*)(p+0x46dc) = *(int*)(p+0x46ec);
                                *(int*)(p+0x46e0) = 0x60;
                                *(int*)(p+0x46e4) = *(int*)(p+0x46f4);
                            }
                        } else if (oldX >= 0x81 && oldX <= 0x9f) {
                            if ((*(int*)(p+0x46e8)) >= 0x61 && (*(int*)(p+0x46e8)) <= 0x7f) {
                                *(int*)(p+0x46d8) = 0x60;
                                *(int*)(p+0x46dc) = *(int*)(p+0x46ec);
                                *(int*)(p+0x46e0) = 0xa0;
                                *(int*)(p+0x46e4) = *(int*)(p+0x46f4);
                            }
                        } else if (oldX >= 0xa1 && oldX <= 0xbf) {
                            if ((*(int*)(p+0x46e8)) >= 0xc1 && (*(int*)(p+0x46e8)) <= 0xdf) {
                                *(int*)(p+0x46d8) = 0xe0;
                                *(int*)(p+0x46dc) = *(int*)(p+0x46ec);
                                *(int*)(p+0x46e0) = 0xa0;
                                *(int*)(p+0x46e4) = *(int*)(p+0x46f4);
                            }
                        } else if (oldX >= 0xc1 && oldX <= 0xdf && (*(int*)(p+0x46e8)) >= 0xa1 && (*(int*)(p+0x46e8)) <= 0xbf) {
                            *(int*)(p+0x46d8) = 0xa0;
                            *(int*)(p+0x46dc) = *(int*)(p+0x46ec);
                            *(int*)(p+0x46e0) = 0xe0;
                            *(int*)(p+0x46e4) = *(int*)(p+0x46f4);
                        }
                        if (*(volatile int*)(p+0x46d8) >= 0 && *(volatile int*)(p+0x46e0) >= 0) {
                            int pp1[2], pp2[2];
                            pp1[0] = *(int*)(p+0x46d8);
                            pp1[1] = *(int*)(p+0x46dc);
                            pp2[0] = *(int*)(p+0x46e0);
                            pp2[1] = *(int*)(p+0x46e4);
                            {
                                int ok;
                                if (func_ov006_020d2580(arg0, pp1, pp2) == 0) { ok = 1; } else { ok = 0; }
                                if (ok != 0) {
                                    *(unsigned char*)(p+0x4704) = 1;
                                    *(unsigned char*)(p+0x4705) = 1;
                                }
                            }
                        }
                    }
                } else if (*(unsigned char*)(p+0x4705) == 0) {
                    int va1, va2, vip;
                    dx = oldX - *(int*)(p+0x46d8);
                    if (dx < 0) va1 = -dx; else va1 = dx;
                    vip = (*(int*)(p+0x46e8)) - *(int*)(p+0x46d8);
                    if (vip < 0) va2 = -vip; else va2 = vip;
                    if (va1 >= va2) {
                        if (dx < 0) dx = -dx;
                        if (dx >= 0x20) {
                            if (oldX < *(int*)(p+0x46d8)) *(int*)(p+0x46e0) = *(int*)(p+0x46d8) - 0x40;
                            else *(int*)(p+0x46e0) = *(int*)(p+0x46d8) + 0x40;
                            *(int*)(p+0x46dc) = *(int*)(p+0x46e4);
                            *(int*)(p+0x46e4) = *(int*)(p+0x46f4);
                        }
                    } else {
                        int t = (*(int*)(p+0x46e8)) - *(int*)(p+0x46d8);
                        if (t < 0) t = -t;
                        if (t >= 0x20) {
                            if ((*(int*)(p+0x46e8)) < *(int*)(p+0x46d8)) *(int*)(p+0x46e0) = *(int*)(p+0x46d8) - 0x40;
                            else *(int*)(p+0x46e0) = *(int*)(p+0x46d8) + 0x40;
                            *(int*)(p+0x46e4) = *(int*)(p+0x46ec);
                        }
                    }
                    if (*(volatile int*)(p+0x46d8) >= 0 && *(volatile int*)(p+0x46e0) >= 0) {
                        int pp1[2], pp2[2];
                        pp1[0] = *(int*)(p+0x46d8);
                        pp1[1] = *(int*)(p+0x46dc);
                        pp2[0] = *(int*)(p+0x46e0);
                        pp2[1] = *(int*)(p+0x46e4);
                        {
                            int ok;
                            if (func_ov006_020d2580(arg0, pp1, pp2) == 0) { ok = 1; } else { ok = 0; }
                            if (ok != 0) {
                                *(unsigned char*)(p+0x4705) = 1;
                            }
                        }
                    }
                }
            }
        }

        if (*(unsigned char*)(p+0x4704) == 0 || *(unsigned char*)(p+0x4705) == 0) {
            if (*(unsigned char*)(p+0x53de) == 0 && *(unsigned char*)(p+0x53dd) == 0) func_02012790(0xe);
            func_ov006_020d1450(arg0);
            *(int*)(p+0x46d8) = -1;
            *(int*)(p+0x46dc) = -1;
            *(int*)(p+0x46e0) = -1;
            *(int*)(p+0x46e4) = -1;
            return;
        }
        if (*(volatile int*)(p+0x46e0) == *(volatile int*)(p+0x46d8)) {
            int i, j;
            func_ov006_020d3624(arg0);
            for (i = 0; i < 0x100; i++) {
                for (j = 0; j < 0x158; j++) {
                    *(*(unsigned char**)(p+0x470c) + i*0x158 + j) = 0;
                }
            }
            goto Lreset990;
        } else {
            int b8, bc, e0, e4;
            b8 = *(int*)(p+0x46d8);
            sp10 = b8;
            bc = *(int*)(p+0x46dc);
            sp14 = bc;
            bc = sp14;
            e0 = *(int*)(p+0x46e0);
            sp18 = e0;
            e4 = *(int*)(p+0x46e4);
            sp1c = e4;
            if (b8 < e0) { sp10 = b8+2; sp18 = e0-2; }
            else { sp10 = b8-2; sp18 = e0+2; }
            {
                int pp1[2], pp2[2];
                int ok;
                pp1[0] = sp10; pp1[1] = bc; pp2[0] = sp18; pp2[1] = sp1c;
                if (func_ov006_020d2580(arg0, pp1, pp2) == 1) { ok = 1; } else { ok = 0; }
                if (ok != 0) {
                    if (*(unsigned char*)(p+0x53de) == 0 && *(unsigned char*)(p+0x53dd) == 0) func_02012790(0xe);
                    func_ov006_020d1450(arg0);
                    *(int*)(p+0x46d8) = -1;
                    *(int*)(p+0x46dc) = -1;
                    *(int*)(p+0x46e0) = -1;
                    *(int*)(p+0x46e4) = -1;
                    return;
                }
            }
            {
                int match;
                int boxE0v = *(int*)(p+0x46e0);
                int boxD8v = *(int*)(p+0x46d8);
                if (boxD8v < boxE0v) {
                    match = *(unsigned char*)(*(unsigned char**)(p+0x4710) + boxE0v*0x158 + *(int*)(p+0x46e4) + 0x218);
                    if (match == 0) {
                        match = *(unsigned char*)(*(unsigned char**)(p+0x4710) + boxD8v*0x158 + *(int*)(p+0x46dc) - 0x98);
                    }
                } else {
                    match = *(unsigned char*)(*(unsigned char**)(p+0x4710) + boxE0v*0x158 + *(int*)(p+0x46e4) - 0x98);
                    if (match == 0) {
                        match = *(unsigned char*)(*(unsigned char**)(p+0x4710) + boxD8v*0x158 + *(int*)(p+0x46dc) + 0x218);
                    }
                }
                if (match == 0) {
                    int t1, a, b, c;
                    func_ov006_020d1450(arg0);
                    *(unsigned char*)(p+0x4706) = 1;
                    func_ov004_020ae5c4(arg0, sp10, sp14, sp18, sp1c, *(int*)(p+0x53e4), 1);
                    a = sp10;
                    b = sp18;
                    if (a < b) {
                        t1 = a - 1;
                        c = sp14;
                        func_ov004_020ae5c4(arg0, t1, c, t1, c, *(int*)(p+0x53e4), 1);
                        t1 = sp18 + 1;
                        c = sp1c;
                        func_ov004_020ae5c4(arg0, t1, c, t1, c, *(int*)(p+0x53e4), 1);
                    } else {
                        t1 = a + 1;
                        c = sp14;
                        func_ov004_020ae5c4(arg0, t1, c, t1, c, *(int*)(p+0x53e4), 1);
                        t1 = sp18 - 1;
                        c = sp1c;
                        func_ov004_020ae5c4(arg0, t1, c, t1, c, *(int*)(p+0x53e4), 1);
                    }
                    *(unsigned char*)(p+0x4706) = 0;
                    func_ov006_020d3668(arg0);
                    {
                        unsigned char *q = (unsigned char*)(((int)p + 0x470a) & 0xFFFFFFFFFFFFFFFF);
                        *q = *q + 1;
                    }
                    _ZN5Sound12PlayBank2_2DEj(0xdf);
                    goto Lreset990;
                } else {
                    if (*(unsigned char*)(p+0x53de) == 0 && *(unsigned char*)(p+0x53dd) == 0) func_02012790(0xe);
                    func_ov006_020d1450(arg0);
                    *(int*)(p+0x46d8) = -1;
                    *(int*)(p+0x46dc) = -1;
                    *(int*)(p+0x46e0) = -1;
                    *(int*)(p+0x46e4) = -1;
                    return;
                }
            }
        }
      }
    }

Lreset990:
    *(int*)(p+0x46f0) = -1;
    *(int*)(p+0x46f4) = -1;
    *(int*)(p+0x46e8) = -1;
    *(int*)(p+0x46ec) = -1;
    *(unsigned char*)(p+0x4704) = 0;
    *(unsigned char*)(p+0x4705) = 0;
    *(unsigned char*)(p+0x53c8) = 0;
}
