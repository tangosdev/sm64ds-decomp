// @symbol func_0206a928
/* recovered: debug printf formatter core. DFA-table % parser (state table data_0209a0a0,
   8 states x 9 character classes; class table data_0209a130), 64-byte chunked output
   through the stream callback at +0x10, conversions b c d i s u o x X f e E g G; %f %e %g
   go through the soft-double helpers func_0206bc8c (digit extract), func_0206bdb4 (/10 and
   normalize), func_0206c244 (x10), func_0206c51c (add). Every byte access is the cluster's
   unaligned-safe u16 read-modify-write pair (READ8/WRITE8). The compiled function is
   0x1364 bytes: its fourth literal-pool word (the data_0208689c address, loaded at +0x12c0)
   sits at +0x1360, one word past the size recorded for the symbol in config/arm9/symbols.txt
   (config/arm9/relocs.txt lists that load at 0x0206bc88). */
#include "types.h"

#define READ8(a) ((((int)(a)) & 1) ? (((*(u16 *)((a) - 1)) & 0xff00) >> 8) : ((*(u16 *)(a)) & 0xff))
#define WRITE8(a, v) \
    if (((int)(a)) & 1) \
        *(u16 *)((a) - 1) = (*(u16 *)((a) - 1) & 0xff) | (((v) & 0xff) << 8); \
    else \
        *(u16 *)(a) = (*(u16 *)(a) & 0xff00) | ((v) & 0xff)

#pragma opt_strength_reduction off
#pragma optimize_for_size on
#pragma opt_common_subs off

typedef void (*WriteFn)(void *self, const char *buf, int len);
struct Stream {
    u8 unk00[0x10];
    WriteFn fn;
};

extern u16 data_0209a0a0[][9];
extern u16 data_0209a130[];
extern char data_02086894[];
extern char data_0208689c[];

extern int func_0206ca44(const char *s);
extern void func_0206c9f4(struct Stream *s, int n);
extern unsigned int func_0206c93c(unsigned int x, unsigned int base);
extern int func_0206c8b4(u64 d);
extern u64 func_0206c244(u64 d);
extern u64 func_0206bdb4(u64 d, int mode);
extern u64 func_0206c51c(u64 a, u64 b);
extern int func_0206bc8c(u64 d);

void func_0206a928(struct Stream *ctx, char *fmt, int **ap)
{
    int conv;
    int isg;
    int npfx;
    int radix;
    int letteroff;
    int nbuf;
    int width;
    u64 v;
    int epos;
    int fwidth;
    int nfrac;
    int sch;
    int fi;
    int i;
    int ch;
    int ech;
    u64 v2;
    int k;
    int nz;
    int strip;
    int pfx0;
    int pfx1;
    int lim;
    int e8;
    int j;
    int state;
    int flags;
    int prec;
    int c;
    char *fp;
    char *p;
    char *b;
    int ndig;
    int exp;
    int len;
    int act;
    int cls;
    int *ip;
    u64 r;
    int cc;
    u64 d;
    char buf[64];
    char frac[64];

    conv = 2;
    flags = 0;
    letteroff = 0;
    width = -1;
    prec = -1;
    state = 0;
    nbuf = 0;

    for (;;) {
        c = READ8(fmt);
        if (c == 0)
            break;
        fmt++;
        if (state == 0) {
            if (c == '%') {
                if (nbuf != 0) {
                    ctx->fn(ctx, buf, nbuf);
                    nbuf = 0;
                }
                flags = 0;
                state = 1;
                width = 0;
                prec = -1;
            } else {
                p = buf + nbuf;
                WRITE8(p, c);
                nbuf++;
                if (nbuf == 64) {
                    ctx->fn(ctx, buf, nbuf);
                    nbuf = 0;
                }
            }
            continue;
        }

        cls = data_0209a130[c] & 0xff;
        act = data_0209a0a0[state - 1][cls] & 0xff;
        switch (act) {
        case 0:
            break;
        case 1:
            p = buf + nbuf;
            WRITE8(p, c);
            nbuf++;
            if (nbuf == 64) {
                ctx->fn(ctx, buf, nbuf);
                nbuf = 0;
            }
            state = 0;
            break;
        case 2:
            state = 2;
            switch (c) {
            case '+':
                flags |= 2;
                break;
            case '-':
                flags |= 1;
                break;
            case ' ':
                flags |= 8;
                break;
            case '#':
                flags |= 0x10;
                break;
            case '0':
                flags |= 4;
                break;
            }
            break;
        case 3:
            state = 3;
            if (width == -1)
                width = c - '0';
            else
                width = width * 10 + (c - '0');
            break;
        case 4:
            state = 4;
            break;
        case 5:
            state = 5;
            if (prec == -1)
                prec = c - '0';
            else
                prec = prec * 10 + (c - '0');
            break;
        case 6:
            state = 6;
            break;
        case 7:
            state = 7;
            switch (c) {
            case 'b':
                conv = 4;
                break;
            case 'c':
                conv = 1;
                break;
            case 's':
                conv = 0;
                break;
            case 'd':
            case 'i':
                conv = 2;
                break;
            case 'u':
                conv = 3;
                break;
            case 'o':
                conv = 5;
                break;
            case 'x':
                conv = 6;
                break;
            case 'X':
                conv = 7;
                break;
            case 'f':
                conv = 10;
                break;
            case 'e':
                conv = 8;
                break;
            case 'E':
                conv = 9;
                break;
            case 'g':
                conv = 11;
                break;
            case 'G':
                conv = 12;
                break;
            }
            break;
        }

        if (state != 7)
            continue;

        switch (conv) {
        case 1:
            *ap += 1;
            cc = (*ap)[-1];
            ctx->fn(ctx, (char *)&cc, 1);
            break;
        case 0: {
            char *s;
            int len;
            int left;
            *ap += 1;
            s = (char *)(*ap)[-1];
            if (s == 0)
                s = data_02086894;
            len = func_0206ca44(s);
            left = flags & 1;
            if (!left) {
                if (len < width)
                    func_0206c9f4(ctx, width - len);
            }
            ctx->fn(ctx, s, len);
            if (left) {
                if (len < width)
                    func_0206c9f4(ctx, width - len);
            }
            break;
        }
        {
            int w;
            char *p;
            unsigned int q;
            unsigned int dig;
            int n;
            int len;
        case 2:
            *ap += 1;
            n = (*ap)[-1];
            if (n < 0) {
                flags |= 0x4000;
                n = -n;
            }
            flags |= 0x1000;
            radix = 10;
            goto number;
        case 3:
            *ap += 1;
            n = (*ap)[-1];
            flags |= 0x2000;
            radix = 10;
            goto number;
        case 4:
            *ap += 1;
            n = (*ap)[-1];
            flags |= 0x2000;
            radix = 2;
            goto number;
        case 5:
            *ap += 1;
            n = (*ap)[-1];
            flags |= 0x2000;
            radix = 8;
            goto number;
        case 6:
            *ap += 1;
            n = (*ap)[-1];
            flags |= 0x2000;
            letteroff = 39;
            radix = 16;
            goto number;
        case 7:
            *ap += 1;
            n = (*ap)[-1];
            flags |= 0x2000;
            radix = 16;
            letteroff = 7;
        number:
            w = width;
            if (width > 60)
                w = 60;
            p = buf + 63;
            if (prec == -1)
                prec = 1;
            else if (prec > 58)
                prec = 58;
            while (prec-- > 0 || n != 0) {
                q = func_0206c93c(n, radix);
                dig = n - q * radix + '0';
                if (dig > '9')
                    dig += letteroff;
                n = q;
                WRITE8(p, dig);
                p--;
            }
            npfx = 0;
            if (radix != 10) {
                if (flags & 0x10) {
                    if (conv == 6) {
                        pfx0 = '0';
                        pfx1 = 'x';
                        npfx = 2;
                    } else if (conv == 7) {
                        pfx0 = '0';
                        pfx1 = 'X';
                        npfx = 2;
                    } else if (conv == 5) {
                        pfx0 = '0';
                        npfx = 1;
                    } else if (conv == 4) {
                        pfx0 = '0';
                        pfx1 = 'b';
                        npfx = 2;
                    }
                }
            } else {
                if (flags & 0x4000) {
                    pfx0 = '-';
                    npfx = 1;
                } else if (flags & 2) {
                    pfx0 = '+';
                    npfx = 1;
                } else if (flags & 8) {
                    pfx0 = ' ';
                    npfx = 1;
                }
            }
            w -= (buf + 63 - p) - npfx;
            if (w > 0 && (flags & 4)) {
                i = 0;
                if (w > 0) {
                    for (; i < w; i++) {
                        WRITE8(p, '0');
                        p--;
                    }
                }
            }
            if (npfx != 0) {
                if (npfx == 2) {
                    WRITE8(p, pfx1);
                    p--;
                }
                WRITE8(p, pfx0);
                p--;
            }
            len = buf + 63 - p;
            if (w > 0 && !(flags & 5))
                func_0206c9f4(ctx, w);
            ctx->fn(ctx, p + 1, len);
            if (w > 0 && (flags & 1))
                func_0206c9f4(ctx, w);
            break;
        }
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            *ap += 2;
            fp = buf + 63;
            ip = *ap;
            c = ip[-2];
            fwidth = width;
            ((u32 *)&d)[1] = ip[-1];
            ndig = 0;
            ((u32 *)&d)[0] = c;
            sch = 0;
            nz = 0;
            strip = 1;
            flags |= 0x2000;
            b = (char *)&d + 7;
            if (READ8(b) & 0x80) {
                flags |= 0x4000;
                b = (char *)&d + 7;
                ch = READ8(b) & 0x7f;
                b = (char *)&d + 7;
                if (((int)b) & 1)
                    *(u16 *)(b - 1) = (*(u16 *)(b - 1) & 0xff) | (ch << 8);
                else
                    *(u16 *)b = (*(u16 *)b & 0xff00) | ch;
            }
            v = d;
            if (width > 60)
                fwidth = 60;
            if (func_0206c8b4(v) < 0x3ff) {
                exp = 0;
                while (func_0206c8b4(v) < 0x3ff) {
                    v = func_0206c244(v);
                    exp--;
                }
            } else if (func_0206c8b4(func_0206bdb4(v, 0)) >= 0x3ff) {
                exp = 0;
                while (func_0206c8b4(func_0206bdb4(v, 0)) >= 0x3ff) {
                    v = func_0206bdb4(v, 1);
                    exp++;
                }
            } else {
                exp = 0;
            }
            if (conv == 10) {
                if (exp > 58 - prec)
                    conv = 8;
            }
            cls = conv - 11;
            if ((unsigned int)cls <= 1)
                isg = 1;
            else
                isg = 0;
            if (prec == -1)
                prec = 6;
            if (prec > 58)
                prec = 58;
            if ((unsigned int)cls <= 1) {
                if (exp < -4 || exp >= prec) {
                    conv = (conv == 11) ? 8 : 9;
                } else {
                    conv = 10;
                }
            }
            nfrac = prec;
            if (isg == 1) {
                if (prec == 0)
                    nfrac = 1;
                nfrac -= (conv == 10) ? exp + 1 : 1;
            }
            e8 = conv - 8;
            if ((unsigned)e8 <= 1)
                d = v;
            r = 0x4014000000000000ULL;
            for (fi = 0; fi <= nfrac; fi++)
                r = func_0206bdb4(r, 1);
            d = func_0206c51c(d, r);
            if ((unsigned)e8 <= 1) {
                if (isg == 0 || exp != 0) {
                    if (exp >= 0) {
                        epos = 1;
                    } else {
                        exp *= -1;
                        epos = 0;
                    }
                    for (j = 0; j < 3; j++) {
                        ech = exp - func_0206c93c(exp, 10) * 10 + '0';
                        exp = func_0206c93c(exp, 10);
                        WRITE8(fp, ech);
                        fp--;
                        ndig++;
                    }
                    ch = epos ? '+' : '-';
                    WRITE8(fp, ch);
                    b = fp - 1;
                    ch = (conv == 8) ? 'e' : 'E';
                    WRITE8(b, ch);
                    fp -= 2;
                    ndig += 2;
                }
            }
            v2 = d;
            k = 0;
            while (nfrac > 0) {
                v2 = func_0206c244(v2);
                ch = (u8)(func_0206bc8c(v2) + '0');
                b = frac + k;
                WRITE8(b, ch);
                if (READ8(b) < '0') {
                    WRITE8(b, '0');
                }
                k++;
                nfrac--;
            }
            while (k > 0) {
                k--;
                if (isg == 1 && strip != 0) {
                    b = frac + k;
                    if (READ8(b) == '0')
                        continue;
                }
                strip = 0;
                b = frac + k;
                ch = READ8(b);
                WRITE8(fp, ch);
                fp--;
                ndig++;
            }
            if (strip == 0 || (flags & 0x10)) {
                WRITE8(fp, '.');
                fp--;
                ndig++;
            }
            lim = 58 - prec;
            while (exp > lim) {
                nz++;
                d = func_0206bdb4(d, 1);
                exp--;
            }
            ch = (u8)(func_0206bc8c(d) + '0');
            if (ch < '0')
                ch = '0';
            d = func_0206bdb4(d, 0);
            c = (u8)ch;
            WRITE8(fp, c);
            fp--;
            ndig++;
            while (func_0206c8b4(d) >= 0x3ff) {
                ch = (u8)(func_0206bc8c(d) + '0');
                if (ch < '0')
                    ch = '0';
                d = func_0206bdb4(d, 0);
                c = (u8)ch;
                WRITE8(fp, c);
                fp--;
                ndig++;
            }
            npfx = 0;
            if (flags & 0x4000) {
                sch = '-';
                npfx = 1;
                ndig++;
            } else if (flags & 2) {
                sch = '+';
                npfx = 1;
                ndig++;
            } else if (flags & 8) {
                sch = ' ';
                npfx = 1;
                ndig++;
            }
            fwidth -= ndig;
            if (fwidth > 0 && (flags & 4)) {
                i = 0;
                if (fwidth > 0) {
                    for (; i < fwidth; i++) {
                        WRITE8(fp, '0');
                        fp--;
                    }
                }
            }
            if (npfx != 0) {
                WRITE8(fp, sch);
                fp--;
            }
            len = buf + 63 - fp;
            if (fwidth > 0 && !(flags & 5))
                func_0206c9f4(ctx, fwidth);
            while (len > 0) {
                fp++;
                if (READ8(fp) == '.') {
                    while (nz > 0) {
                        ctx->fn(ctx, data_0208689c, 1);
                        nz--;
                    }
                }
                ctx->fn(ctx, fp, 1);
                len--;
            }
            if (fwidth > 0 && (flags & 1))
                func_0206c9f4(ctx, fwidth);
            break;
        }
        state = 0;
    }
    if (nbuf != 0)
        ctx->fn(ctx, buf, nbuf);
}
