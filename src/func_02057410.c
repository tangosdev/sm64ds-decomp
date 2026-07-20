typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef long long s64;

typedef struct Ctx {
    int n;
    char *cur;
    char *start;
} Ctx;

extern void func_02057e00(Ctx *ctx, int c);
extern void func_02057d94(Ctx *ctx, int c, int count);
extern void func_02057d30(Ctx *ctx, const char *s, int len);

#pragma opt_strength_reduction off
#pragma opt_lifetimes off
int func_02057410(char *dst, int n, const char *fmt, int *args)
{
    int nsave;
    int pf[3];
    int zero;
    int neg1;
    int ten;
    int lca;
    int eight;
    int uca;
    int spc;
    int zch;
    int sixteen;
    int one;
    int two;
    int minus;
    int plus;
    char prefixbuf[2];
    char numbuf[26];
    Ctx ctx;
    register int cnt;
    int nprefix;
    int flags;
    s64 v;
    int prec;
    int zeros;
    int lj;
    const char *pct;

    ctx.n = n;
    ctx.start = dst;
    ctx.cur = dst;
    nsave = n;
    if (*fmt != 0) {
        spc = 0x20;
        zch = 0x30;
        zero = 0;
        neg1 = -1;
        ten = 0xa;
        lca = 0x57;
        eight = 8;
        uca = 0x37;
        sixteen = 0x10;
        one = 1;
        plus = 0x2b;
        minus = 0x2d;
        two = 2;
        do {
            u8 c = *(u8 *)fmt;
            if ((u32)((c ^ 0x20) - 0xa1) < 0x3c) {
                s8 c2;
                func_02057e00(&ctx, c);
                c2 = *++fmt;
                if (c2 != 0) {
                    fmt++;
                    func_02057e00(&ctx, c2);
                }
            } else if (c != 0x25) {
                fmt++;
                func_02057e00(&ctx, c);
            } else {
                flags = zero;
                prec = neg1;
                pf[0] = zero;
                pf[1] = ten;
                pf[2] = lca;
                pct = fmt;
                for (;;) {
                    s8 fc = *++fmt;
                    switch (fc) {
                    case 0x2b:
                        if (fmt[-1] != 0x20) goto flags_done;
                        flags |= 2;
                        continue;
                    case 0x20:
                        flags |= 1;
                        continue;
                    case 0x2d:
                        flags |= 8;
                        continue;
                    case 0x30:
                        flags |= 0x10;
                        continue;
                    default:
                        goto flags_done;
                    }
                }
            flags_done:
                if (*fmt == 0x2a) {
                    int w;
                    fmt++;
                    args++;
                    w = args[-1];
                    pf[0] = w;
                    if (w < 0) {
                        pf[0] = -w;
                        flags |= 8;
                    }
                } else {
                    while (*fmt >= 0x30 && *fmt <= 0x39) {
                        pf[0] = pf[0] * 10 + *fmt++ - 0x30;
                    }
                }
                if (*fmt == 0x2e) {
                    s8 pc2 = *++fmt;
                    prec = zero;
                    if (pc2 == 0x2a) {
                        fmt++;
                        args++;
                        prec = args[-1];
                        if (prec < 0) prec = neg1;
                    } else {
                        while (*fmt >= 0x30 && *fmt <= 0x39) {
                            prec = prec * 10 + *fmt++ - 0x30;
                        }
                    }
                }
                {
                    s8 mc = *fmt;
                    switch (mc) {
                    case 0x68:
                        if (*++fmt != 0x68) {
                            flags |= 0x40;
                        } else {
                            fmt++;
                            flags |= 0x100;
                        }
                        break;
                    case 0x6c:
                        if (*++fmt != 0x6c) {
                            flags |= 0x20;
                        } else {
                            fmt++;
                            flags |= 0x80;
                        }
                        break;
                    }
                }
                {
                    s8 cc = *fmt;
                    switch (cc) {
                    case 0x6f:
                        pf[1] = eight;
                        flags |= 0x1000;
                        goto num;
                    case 0x75:
                        flags |= 0x1000;
                        goto num;
                    case 0x58:
                        pf[2] = uca;
                        goto xhex;
                    case 0x70:
                        flags |= 4;
                        prec = eight;
                        goto xhex;
                    case 0x63: {
                        int ch;
                        if (prec >= 0) goto dflt;
                        args++;
                        ch = args[-1];
                        if (flags & 8) {
                            func_02057e00(&ctx, (char)ch);
                            func_02057d94(&ctx, spc, pf[0] - 1);
                        } else {
                            char padc = (flags & 0x10) ? zch : spc;
                            func_02057d94(&ctx, padc, pf[0] - 1);
                            func_02057e00(&ctx, (char)ch);
                        }
                        fmt++;
                        break;
                    }
                    case 0x73: {
                        char *s;
                        nprefix = zero;
                        args++;
                        s = (char *)args[-1];
                        if (prec < 0) {
                            while (s[nprefix] != 0) nprefix++;
                        } else {
                            while (nprefix < prec && s[nprefix] != 0) nprefix++;
                        }
                        pf[0] -= nprefix;
                        if (flags & 8) {
                            func_02057d30(&ctx, s, nprefix);
                            func_02057d94(&ctx, spc, pf[0]);
                        } else {
                            char padc = (flags & 0x10) ? zch : spc;
                            func_02057d94(&ctx, padc, pf[0]);
                            func_02057d30(&ctx, s, nprefix);
                        }
                        fmt++;
                        break;
                    }
                    case 0x6e: {
                        int count = ctx.cur - ctx.start;
                        if (!(flags & 0x100)) {
                            if (flags & 0x40) {
                                args++;
                                *(short *)args[-1] = count;
                            } else if (flags & 0x80) {
                                args++;
                                *(s64 *)args[-1] = count;
                            } else {
                                args++;
                                *(int *)args[-1] = count;
                            }
                        }
                        fmt++;
                        break;
                    }
                    case 0x25:
                        if (pct + 1 != fmt) goto dflt;
                        fmt++;
                        func_02057e00(&ctx, cc);
                        break;
                    default:
                    dflt:
                        func_02057d30(&ctx, pct, fmt - pct);
                        break;
                    case 0x78:
                    xhex:
                        pf[1] = sixteen;
                        flags |= 0x1000;
                    case 0x64:
                    case 0x69:
                    num:
                        if (flags & 8) flags &= ~0x10;
                        nprefix = zero;
                        if (prec >= 0) flags &= ~0x10;
                        else prec = one;
                        if (flags & 0x1000) {
                            if (flags & 0x100) {
                                args++;
                                v = *(u8 *)&args[-1];
                            } else if (flags & 0x40) {
                                args++;
                                v = *(u16 *)&args[-1];
                            } else if (flags & 0x80) {
                                args += 2;
                                v = *(s64 *)&args[-2];
                            } else {
                                args++;
                                v = (u32)args[-1];
                            }
                            flags &= ~3;
                            if (flags & 4) {
                                if (pf[1] == 0x10) {
                                    if (v != 0) {
                                        prefixbuf[1] = zch;
                                        nprefix = two;
                                        prefixbuf[0] = pf[2] + 0x21;
                                    }
                                } else if (pf[1] == 8) {
                                    prefixbuf[0] = zch;
                                    nprefix = one;
                                }
                            }
                        } else {
                            if (flags & 0x100) {
                                args++;
                                v = *(s8 *)&args[-1];
                            } else if (flags & 0x40) {
                                args++;
                                v = *(short *)&args[-1];
                            } else if (flags & 0x80) {
                                args += 2;
                                v = *(s64 *)&args[-2];
                            } else {
                                args++;
                                v = args[-1];
                            }
                            if ((((s64)zero << 32 | (u32)(v >> 32)) & 0x80000000) != 0) {
                                prefixbuf[0] = minus;
                                v = ~v + 1;
                                nprefix = one;
                            } else if (v != 0 || prec != 0) {
                                if (flags & 2) {
                                    prefixbuf[0] = plus;
                                    nprefix = one;
                                } else if (flags & 1) {
                                    prefixbuf[0] = spc;
                                    nprefix = one;
                                }
                            }
                        }
                        cnt = zero;
                        if (v != 0) {
                            do {
                                int digit = (int)(v % pf[1]);
                                int dch;
                                v = v / pf[1];
                                if (digit < 0xa) dch = digit + 0x30;
                                else dch = digit + pf[2];
                                numbuf[cnt] = dch;
                                cnt++;
                            } while (v != 0);
                        }
                        if (nprefix > 0) {
                            if (prefixbuf[0] == 0x30) {
                                numbuf[cnt] = zch;
                                nprefix = zero;
                                cnt++;
                            }
                        }
                        zeros = prec - cnt;
                        if (flags & 0x10) {
                            int t = pf[0] - cnt - nprefix;
                            if (zeros < t) zeros = t;
                        }
                        if (zeros > 0) pf[0] -= zeros;
                        pf[0] -= nprefix + cnt;
                        lj = flags & 8;
                        if (!lj) func_02057d94(&ctx, spc, pf[0]);
                        if (nprefix > 0) {
                            char *p = prefixbuf + nprefix;
                            do {
                                s8 pch = *--p;
                                nprefix--;
                                func_02057e00(&ctx, pch);
                            } while (nprefix > 0);
                        }
                        func_02057d94(&ctx, zch, zeros);
                        if (cnt > 0) {
                            char *p2 = numbuf + cnt;
                            do {
                                s8 dch2 = *--p2;
                                cnt--;
                                func_02057e00(&ctx, dch2);
                            } while (cnt > 0);
                        }
                        if (lj) func_02057d94(&ctx, spc, pf[0]);
                        fmt++;
                        break;
                    }
                }
            }
        } while (*fmt != 0);
    }
    if (ctx.n != 0) {
        *ctx.cur = 0;
    } else if (nsave != 0) {
        *((ctx.start + nsave) - 1) = 0;
    }
    return ctx.cur - ctx.start;
}
