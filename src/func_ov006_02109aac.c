extern int func_ov006_020c0efc(char *a);
extern void func_ov006_021095ac(int *a, int *b);
extern void func_ov004_020b1b08(int c);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);
extern void func_ov006_020c1764(char *c);
extern short func_ov006_02108650(int a, int b);
extern void func_ov006_02109530(char *out, int *a, int scale);
extern void func_ov006_0210935c(char *a);
extern void func_ov006_021092e8(char *self);
extern void func_ov004_020ad79c(int a, int b);
extern int func_ov006_02108b90(char *a, int b);
extern void func_ov006_02107d20(char *p, char *val);
extern void func_ov006_020c0c80(char *c);
extern void func_ov006_020c0d68(char *c);
extern void func_ov004_020b0a54(int c);
extern void func_ov006_02108f2c(char *o);
extern void func_ov006_02108d28(char *o);
extern void func_ov006_02107db8(char *c);
extern void func_ov006_020c19d0(char *thiz);
extern void func_ov004_020b65e4(void);

extern unsigned char data_ov006_02142ab4[];
extern char *func_020beb68;

#define AT(p,off) ((void*)(int)(((long long)(int)((char*)(p)+(off)))&0xFFFFFFFFFFFFFFFFLL))
#define LNDR(e) ((int)(((long long)(e))&0xFFFFFFFFFFFFFFFFLL))
#define I(o) (*(int*)(c + (o)))
#define H(o) (*(short*)(c + (o)))
#define B(o) (*(unsigned char*)(c + (o)))
#define IA(o) (*(int*)AT(c,(o)))
#define HA(o) (*(short*)AT(c,(o)))
#define PlayBank2 _ZN5Sound12PlayBank2_2DEj

int func_ov006_02109aac(char *c)
{
    switch (H(0x53e6)) {
    case 0:
        break;
    case 1:
        if (func_ov006_020c0efc(c + 0x4f38) != 0) {
            if (H(0x53d8) == 0) {
                H(0x53d8) = 1;
            }
        }
        if (B(0xc4) == 0) {
            B(0xc3) = 1;
            B(0xc4) = 1;
            H(0xc0) = 0;
        }
        if (func_ov006_020c0efc(c + 0x4f38) != 0) {
            HA(0x53e8) -= 1;
            if (H(0x53e8) == 0) {
                H(0x53e4) = 1;
                if (I(0x53f8) < I(0x53fc)) {
                    int v[2];
                    int w;
                    w = LNDR(0 - (I(0x53f8) << 16)) + 0x80000;
                    v[0] = 0x20000;
                    v[1] = w;
                    func_ov006_021095ac((int *)(c + 0x51a8 + I(0x53f8) * 0x34), v);
                    IA(0x53f8) += 1;
                    H(0x53e8) = 8;
                    func_ov004_020b1b08(1);
                    PlayBank2(0x14d);
                } else {
                    int i = 0;
                    if (I(0x53fc) > 0) {
                        char *p = c;
                        do {
                            *(unsigned char *)(p + 0x51da) = 1;
                            i++;
                            p += 0x34;
                        } while (i < I(0x53fc));
                    }
                    H(0x53e8) = 0x258;
                    func_ov006_020c1764(c + 0x4f38);
                    HA(0x53e6) += 1;
                    I(0x53f8) = 0;
                    PlayBank2(0x15b);
                }
            }
        }
        break;
    case 2: {
        int h = H(0x53e8);
        if (h % 60 == 0 && h > 0 && h <= 0xb4) {
            PlayBank2(0xa6);
        }
        HA(0x53e8) -= 1;
        if (H(0x53e8) == 0) {
            {
                short type;
                int b;
                int i = 0;
                if (I(0x53fc) > 0) {
                    char *p1 = c;
                    char *p2 = c + 0x51a8;
                    do {
                        *(short *)(p1 + 0x51d4) = func_ov006_02108650(*(int *)(p1 + 0x51c0) >> 12, *(int *)(p1 + 0x51c4) >> 12);
                        b = 0;
                        type = *(short *)(p1 + 0x51d4);
                        if (type >= 0xc && type <= 0x1c)
                            b = 1;
                        if (b != 0)
                            goto lab530;
                        if (type >= 0x1d && type <= 0x22)
                            b = 1;
                        else
                            b = 0;
                        if (b == 0)
                            goto lab35c;
                    lab530:
                        func_ov006_02109530(p2, (int *)&data_ov006_02142ab4[type * 8], 0x100);
                        goto labnext;
                    lab35c:
                        func_ov006_0210935c(p2);
                    labnext:
                        i++;
                        p1 += 0x34;
                        p2 += 0x34;
                    } while (i < I(0x53fc));
                }
            }
            I(0x53f8) = 0;
            HA(0x53e4) += 1;
            HA(0x53e6) += 1;
            I(0x53dc) = 1;
            H(0x53e8) = 1;
            PlayBank2(0x15a);
            {
                int off6 = 0x53f6;
                int found = 0;
                int j = 0;
                if (I(0x53fc) > 0) {
                    char *q1 = c;
                    char *q2 = c + 0x51a8;
                    do {
                        if (*(short *)(q1 + 0x51d4) == 0x25) {
                            func_ov006_021092e8(q2);
                            if (found == 0) {
                                PlayBank2(0x15c);
                                found = 1;
                            }
                        } else {
                            *(unsigned char *)(q1 + 0x51da) = 3;
                            *(short *)(c + off6) += 1;
                        }
                        j++;
                        q1 += 0x34;
                        q2 += 0x34;
                    } while (j < I(0x53fc));
                }
                func_ov004_020ad79c(I(0x53fc) - H(0x53f6) + ((func_020beb68 != 0) ? *(int *)(func_020beb68 + 0xa8) : 0), 0);
            }
        }
        break;
    }
    case 3: {
        int b;
        short t;
        int off2;
        int d;
        int j;
        int r;
        char *p;
        b = (I(0x53c4) == 0) ? 1 : 0;
        if (LNDR(b) != 0) {
            j = 0;
            B(0xc3) = 0;
            t = H(0x53e8);
            HA(0x53e8) -= 1;
            if (t == 0) {
                d = H(0x53d6);
                H(0x53f2) = 0;
                if (I(0x53fc) > 0) {
                    p = c + 0x51a8;
                    off2 = 0x53f2;
                    do {
                        r = func_ov006_02108b90(p, d);
                        if (r == 2) {
                            func_ov006_02107d20(c + 0x52f4, p);
                        } else if (r == 3) {
                            func_ov006_02107d20(c + 0x52dc, p);
                        } else if (r == 6) {
                            func_ov006_02107d20(c + 0x52c4, p);
                        } else if (r == 0xc) {
                            func_ov006_02107d20(c + 0x52ac, p);
                        }
                        *(short *)(c + off2) += r;
                        j++;
                        p += 0x34;
                    } while (j < I(0x53fc));
                }
                func_ov004_020ad79c(H(0x53f2) + ((func_020beb68 != 0) ? *(int *)(func_020beb68 + 0xa8) : 0), 0);
                H(0x53f4) = 0;
                I(0x53f8) = 0;
                H(0x53e8) = 0x5a;
                HA(0x53e6) += 1;
            }
        }
        break;
    }
    case 4: {
        HA(0x53e8) -= 1;
        if (H(0x53e8) == 0) {
            short f6 = H(0x53f6);
            short f2 = H(0x53f2);
            if (f2 > f6) {
                func_ov006_020c0c80(c + 0x4f38);
                func_ov004_020b0a54(4);
            } else if (f2 < f6) {
                func_ov006_020c0d68(c + 0x4f38);
                func_ov004_020b0a54(5);
            } else {
                func_ov004_020b0a54(8);
            }
            H(0x53e4) = 0;
            HA(0x53e6) += 1;
        }
        break;
    }
    }

    {
        int k = 0;
        if (I(0x53fc) > 0) {
            char *p = c + 0x51a8;
            do {
                func_ov006_02108f2c(p);
                k++;
                p += 0x34;
            } while (k < I(0x53fc));
        }
    }
    {
        int k = 0;
        if (I(0x53fc) > 0) {
            char *p = c + 0x51a8;
            do {
                func_ov006_02108d28(p);
                k++;
                p += 0x34;
            } while (k < I(0x53fc));
        }
    }
    func_ov006_02107db8(c + 0x530c);
    func_ov006_020c19d0(c + 0x4f38);
    func_ov004_020b65e4();
    return 1;
}
