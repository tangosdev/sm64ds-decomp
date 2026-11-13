/* String formatting (the Nitro SDK's os_printf.c), arm9 .text
 * 0x02057410..0x02057e30.
 *
 * OS_VSNPrintf is a small vsnprintf: flags, width and precision (each may be
 * '*'), the h/hh/l/ll size modifiers and the d i o u x X p c s n % specs. A
 * Shift-JIS lead byte is copied with the byte after it, so a two-byte
 * character is never split. The three string_ helpers write into a printfStr,
 * which counts every character but stores only those that still fit.
 *
 * pret's pokediamond OS_printf.c defines string_put_char, string_fill_char,
 * string_put_string, OS_SPrintf, OS_VSPrintf, OS_SNPrintf and OS_VSNPrintf
 * in this order. mwccarm emits the bodies in reverse, so the ROM runs from
 * OS_VSNPrintf at 0x02057410 up to string_put_char at 0x02057e00.
 * OS_SNPrintf is not in this ROM: nothing calls it, so the linker dropped it.
 */
#include "types.h"
#include "nitro/os/printf.h"

typedef struct printfStr {
    s32 spaceLeft;
    char *stringEnd;
    char *stringStart;
} printfStr;

int OS_VSPrintf(char *buffer, const char *format, void *args);
int OS_VSNPrintf(char *buffer, int bufsz, const char *format, int *args);

void string_put_char(printfStr *dest, int value);
void string_fill_char(printfStr *dest, s8 value, int count);
void string_put_string(printfStr *dest, const char *src, int count);

/* value is an int, not pret's s8: OS_VSNPrintf passes it both a u8 and an
 * s8, and with a char parameter those calls would narrow them first. */
void string_put_char(printfStr *dest, int value) {
    if (dest->spaceLeft != 0) {
        dest->stringEnd[0] = value;
        dest->spaceLeft--;
    }

    dest->stringEnd++;
}

/* Both fill and put advance stringEnd by the whole count, even past the
 * space left, so OS_VSNPrintf returns the length the output would have. */
void string_fill_char(printfStr *dest, s8 value, int count) {
    u32 written;
    u32 toWrite;

    if (count <= 0)
        return;

    toWrite = dest->spaceLeft;
    if (toWrite > (u32)count)
        toWrite = count;

    for (written = 0; written < toWrite; written++)
        dest->stringEnd[written] = value;

    dest->spaceLeft -= toWrite;
    dest->stringEnd += count;
}

void string_put_string(printfStr *dest, const char *src, int count) {
    u32 written;
    u32 toWrite;

    if (count <= 0)
        return;

    toWrite = dest->spaceLeft;
    if (toWrite > (u32)count)
        toWrite = count;

    for (written = 0; written < toWrite; written++)
        dest->stringEnd[written] = src[written];

    dest->spaceLeft -= toWrite;
    dest->stringEnd += count;
}

/* The variable arguments start on the stack word after format, which is
 * how pret's file spells va_start. */
int OS_SPrintf(char *buffer, const char *format, ...) {
    void *args = (void *)((u32 *)((uintptr_t)&format & ~3) + 1);
    return OS_VSPrintf(buffer, format, args);
}

int OS_VSPrintf(char *buffer, const char *format, void *args) {
    return OS_VSNPrintf(buffer, 0x7fffffff, format, args);
}

/* The pragmas reach to the end of the file, so they cover OS_VSNPrintf alone.
 * The named constant locals (zero, neg1, ten, ...) are a matching device,
 * not SDK source: the shard reached the ROM's register use with them. */
#pragma opt_strength_reduction off
#pragma opt_lifetimes off
int OS_VSNPrintf(char *buffer, int bufsz, const char *fmt, int *args) {
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
    printfStr str;
    register int cnt;
    int nprefix;
    int flags;
    s64 v;
    int prec;
    int zeros;
    int lj;
    const char *pct;

    str.spaceLeft = bufsz;
    str.stringStart = buffer;
    str.stringEnd = buffer;
    nsave = bufsz;
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
                string_put_char(&str, c);
                c2 = *++fmt;
                if (c2 != 0) {
                    fmt++;
                    string_put_char(&str, c2);
                }
            } else if (c != 0x25) {
                fmt++;
                string_put_char(&str, c);
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
                        if (fmt[-1] != 0x20)
                            break;
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
                    }
                    break;
                }
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
                        if (prec < 0)
                            prec = neg1;
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
                    /* num/xhex/dflt gotos kept: they are the shared integer, hex and
                       invalid-spec tails, and the case bodies sit in ROM order */
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
                        if (prec >= 0)
                            goto dflt;
                        args++;
                        ch = args[-1];
                        if (flags & 8) {
                            string_put_char(&str, (char)ch);
                            string_fill_char(&str, spc, pf[0] - 1);
                        } else {
                            char padc = (flags & 0x10) ? zch : spc;
                            string_fill_char(&str, padc, pf[0] - 1);
                            string_put_char(&str, (char)ch);
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
                            while (s[nprefix] != 0)
                                nprefix++;
                        } else {
                            while (nprefix < prec && s[nprefix] != 0)
                                nprefix++;
                        }
                        pf[0] -= nprefix;
                        if (flags & 8) {
                            string_put_string(&str, s, nprefix);
                            string_fill_char(&str, spc, pf[0]);
                        } else {
                            char padc = (flags & 0x10) ? zch : spc;
                            string_fill_char(&str, padc, pf[0]);
                            string_put_string(&str, s, nprefix);
                        }
                        fmt++;
                        break;
                    }
                    case 0x6e: {
                        int count = str.stringEnd - str.stringStart;
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
                        if (pct + 1 != fmt)
                            goto dflt;
                        fmt++;
                        string_put_char(&str, cc);
                        break;
                    default:
                    dflt:
                        string_put_string(&str, pct, fmt - pct);
                        break;
                    case 0x78:
                    xhex:
                        pf[1] = sixteen;
                        flags |= 0x1000;
                    case 0x64:
                    case 0x69:
                    num:
                        if (flags & 8)
                            flags &= ~0x10;
                        nprefix = zero;
                        if (prec >= 0)
                            flags &= ~0x10;
                        else
                            prec = one;
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
                                int digit = (int)((u64)v % (u64)pf[1]);
                                int dch;
                                v = (u64)v / (u64)pf[1];
                                if (digit < 0xa)
                                    dch = digit + 0x30;
                                else
                                    dch = digit + pf[2];
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
                            if (zeros < t)
                                zeros = t;
                        }
                        if (zeros > 0)
                            pf[0] -= zeros;
                        pf[0] -= nprefix + cnt;
                        lj = flags & 8;
                        if (!lj)
                            string_fill_char(&str, spc, pf[0]);
                        if (nprefix > 0) {
                            char *p = prefixbuf + nprefix;
                            do {
                                s8 pch = *--p;
                                nprefix--;
                                string_put_char(&str, pch);
                            } while (nprefix > 0);
                        }
                        string_fill_char(&str, zch, zeros);
                        if (cnt > 0) {
                            char *p2 = numbuf + cnt;
                            do {
                                s8 dch2 = *--p2;
                                cnt--;
                                string_put_char(&str, dch2);
                            } while (cnt > 0);
                        }
                        if (lj)
                            string_fill_char(&str, spc, pf[0]);
                        fmt++;
                        break;
                    }
                }
            }
        } while (*fmt != 0);
    }
    if (str.spaceLeft != 0) {
        *str.stringEnd = 0;
    } else if (nsave != 0) {
        *((str.stringStart + nsave) - 1) = 0;
    }
    return str.stringEnd - str.stringStart;
}
